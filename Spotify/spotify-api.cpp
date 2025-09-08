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
    // Optionally: trigger auth refresh if both clientId_ and scope_ are set
  }
}
void SpotifyApi::setScope(const QStringList &scope) {
  if (scope_ != scope) {
    scope_ = scope;
    emit scopeChanged();
    // Optionally: trigger auth refresh if both are set
  }
}
