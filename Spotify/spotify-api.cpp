#include "./spotify-api.h"

#include <QtNetworkAuth/qabstractoauth.h>

#include <QJsonDocument>
#include <QJsonObject>

SpotifyApi::SpotifyApi(QObject *parent)
    : QObject(parent), authorization_(new Authorization(this)) {
  connect(authorization_, &Authorization::granted, this,
          &SpotifyApi::onAuthGranted);
  connect(authorization_, &Authorization::failed, this,
          &SpotifyApi::onAuthFailed);
  api_.setBaseUrl(QUrl("https://api.spotify.com/v1"));
}

void SpotifyApi::authorize(const QString &clientId, const QStringList &scopes) {
  QSet<QByteArray> scopeSet;
  for (const QString &s : scopes) {
    scopeSet.insert(s.toUtf8());
  }
  authorization_->authorize(clientId, scopeSet);
}

bool SpotifyApi::isAuthorized() const { return authorized_; }

void SpotifyApi::onAuthGranted(const QString &token) {
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
