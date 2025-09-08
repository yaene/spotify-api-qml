#include "authorization.h"

#include <QtNetworkAuth/qoauthhttpserverreplyhandler.h>
#include <qabstractoauth2.h>
#include <qlogging.h>
#include <qt6keychain/keychain.h>

#include <QFile>
#include <QMetaEnum>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslKey>
#include <QtGui/QDesktopServices>
#include <QtNetworkAuth/QOAuth2AuthorizationCodeFlow>
#include <QtNetworkAuth/QOAuthHttpServerReplyHandler>

static constexpr auto authorizationUrl =
    "https://accounts.spotify.com/authorize";
static constexpr auto accessTokenUrl = "https://accounts.spotify.com/api/token";

Authorization::Authorization(QObject* parent, quint16 port)
    : QObject(parent), handler_(new QOAuthHttpServerReplyHandler(port, this)) {
  handler_->setCallbackPath("/callback");
  oauth_.setReplyHandler(handler_);
  oauth_.setAuthorizationUrl(QUrl(authorizationUrl));
  oauth_.setTokenUrl(QUrl(accessTokenUrl));
  oauth_.setAutoRefresh(true);

  QObject::connect(&oauth_, &QAbstractOAuth2::tokenChanged, this,
                   &Authorization::onAccessTokenChanged);

  QObject::connect(&oauth_, &QAbstractOAuth2::refreshTokenChanged, this,
                   &Authorization::onRefreshTokenChanged);

  QObject::connect(&oauth_, &QAbstractOAuth::requestFailed, this,
                   &Authorization::onRequestFailed);

  connect(&oauth_, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, this,
          &QDesktopServices::openUrl);
}

void Authorization::refreshTokens(
    const QString& clientId, const QSet<QByteArray>& requestedScopeTokens) {
  oauth_.setRequestedScopeTokens(requestedScopeTokens);
  oauth_.setClientIdentifier(clientId);
  auto* job = new QKeychain::ReadPasswordJob("spotifyqml");
  job->setKey("refresh_token");
  connect(job, &QKeychain::ReadPasswordJob::finished, this, [this, job] {
    if (job->error()) {
      qInfo() << "Failed to read refresh token:" << job->errorString();
    } else {
      QString refreshToken = job->textData();
      oauth_.setRefreshToken(refreshToken);
      oauth_.refreshTokens();
    }
    job->deleteLater();
  });
  job->start();
}
void Authorization::persistRefreshToken(const QString& refreshToken) {
  qInfo() << "persisting token: " << refreshToken;
  auto* job = new QKeychain::WritePasswordJob("spotifyqml");
  job->setKey("refresh_token");
  job->setTextData(refreshToken);
  connect(job, &QKeychain::WritePasswordJob::finished, this, [job] {
    if (job->error()) {
      qInfo() << "Failed to write refresh token:" << job->errorString();
    } else {
      qInfo() << "Persisted refresh token successfully";
    }
    job->deleteLater();
  });
  job->start();
}

void Authorization::authorize(const QString& clientId,
                              const QSet<QByteArray>& requestedScopeTokens) {
  oauth_.setRequestedScopeTokens(requestedScopeTokens);
  oauth_.setClientIdentifier(clientId);

  if (handler_->isListening()) {
    oauth_.grant();
  }
}

void Authorization::onAccessTokenChanged(const QString& token) {
  qInfo() << "access token changed";
  emit granted(token);
}
void Authorization::onRefreshTokenChanged(const QString& token) {
  qInfo() << "refresh token changed";
  persistRefreshToken(token);
}
void Authorization::onRequestFailed(const QAbstractOAuth::Error error) {
  emit failed(error);
}
