#include "./spotify-api.h"

#include <QtNetworkAuth/qabstractoauth.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "./playlist.h"

SpotifyApi::SpotifyApi(QObject *parent)
    : QObject(parent), authorization_(new Authorization(this)) {
  connect(authorization_, &Authorization::granted, this,
          &SpotifyApi::onAuthGranted);
  connect(authorization_, &Authorization::failed, this,
          &SpotifyApi::onAuthFailed);
  api_.setBaseUrl(QUrl("https://api.spotify.com/v1"));
}

QSet<QByteArray> getScopeSet(const QStringList &scope) {
  QSet<QByteArray> scopeSet;
  for (const QString &s : scope) {
    scopeSet.insert(s.toUtf8());
  }
  return scopeSet;
}
void SpotifyApi::authorize() {
  authorization_->authorize(clientId_, getScopeSet(scope_));
}

void SpotifyApi::init() {
  qInfo() << "client: " << clientId_ << "scope: " << scope_[0];
  authorization_->refreshTokens(clientId_, getScopeSet(scope_));
}

bool SpotifyApi::isAuthorized() const { return authorized_; }

void SpotifyApi::onAuthGranted(const QString &token) {
  qInfo() << "On auth granted: " << token;
  accessToken_ = token;
  api_.setBearerToken(token.toUtf8());
  if (!authorized_) {
    authorized_ = true;
    emit authorizedChanged();
  }
}

QVariantMap SpotifyApi::currentUser() const { return currentUser_; }

void SpotifyApi::onAuthFailed(const QAbstractOAuth::Error &error) {
  if (authorized_) {
    authorized_ = false;
    emit authorizedChanged();
  }
  QString errorStr;
  switch (error) {
    case QAbstractOAuth::Error::NoError:
      errorStr = "NoError";
      break;
    case QAbstractOAuth::Error::NetworkError:
      errorStr = "NetworkError";
      break;
    case QAbstractOAuth::Error::ServerError:
      errorStr = "ServerError";
      break;
    case QAbstractOAuth::Error::OAuthTokenNotFoundError:
      errorStr = "OAuthTokenNotFoundError";
      break;
    case QAbstractOAuth::Error::OAuthTokenSecretNotFoundError:
      errorStr = "OAuthTokenSecretNotFoundError";
      break;
    case QAbstractOAuth::Error::OAuthCallbackNotVerified:
      errorStr = "OAuthCallbackNotVerified";
      break;

    case QAbstractOAuth::Error::ClientError:
      errorStr = "ClientError";
      break;
    case QAbstractOAuth::Error::ExpiredError:
      errorStr = "ExpiredError";
      break;

    default:
      errorStr = "UnknownError";
  }
  qInfo() << errorStr;
  emit authorizationFailed(errorStr);
}

void SpotifyApi::updateCurrentUser() {
  if (accessToken_.isEmpty()) return;
  QNetworkReply *reply = netw_.get(api_.createRequest("me"));
  connect(reply, &QNetworkReply::finished, this,
          [this, reply]() { onCurrentUserReply(reply); });
}

void SpotifyApi::onCurrentUserReply(QNetworkReply *reply) {
  QByteArray bytes = reply->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(bytes);
  if (doc.isObject()) {
    currentUser_ = doc.object().toVariantMap();
    emit currentUserChanged();
  }
  reply->deleteLater();
}
void SpotifyApi::setClientId(const QString &id) {
  if (clientId_ != id) {
    clientId_ = id;
    emit clientIdChanged();
  }
}
void SpotifyApi::setScope(const QStringList &scope) {
  if (scope_ != scope) {
    scope_ = scope;
    emit scopeChanged();
  }
}

QList<Playlist *> SpotifyApi::playlists() { return playlists_; }

void SpotifyApi::updatePlaylists() {
  // Example Spotify Web API endpoint for current user's playlists
  QUrl url("https://api.spotify.com/v1/me/playlists");
  QNetworkRequest request(url);
  request.setRawHeader("Authorization", "Bearer " + accessToken_.toUtf8());

  QNetworkReply *reply = netw_.get(request);
  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    // Clear previous playlists and ensure proper deletion
    qDeleteAll(playlists_);
    playlists_.clear();

    if (reply->error()) {
      qWarning() << "Failed to fetch playlists:" << reply->errorString();
      emit playlistsChanged();
      reply->deleteLater();
      return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    if (doc.isObject()) {
      QJsonArray items = doc.object().value("items").toArray();
      for (const QJsonValue &value : items) {
        const QJsonObject obj = value.toObject();
        QString id = obj["id"].toString();
        QString name = obj["name"].toString();
        QString owner =
            obj["owner"].toObject().value("display_name").toString();
        QUrl image_url;
        QJsonArray images = obj["images"].toArray();
        if (!images.isEmpty()) {
          image_url = QUrl(images.at(0).toObject().value("url").toString());
        }
        int track_count = obj["tracks"].toObject().value("total").toInt();

        auto *playlist = new Playlist(id, name, owner, image_url, track_count,
                                      &netw_, &api_, this);

        playlists_.append(playlist);
      }
    }

    emit playlistsChanged();
    reply->deleteLater();
  });
}
