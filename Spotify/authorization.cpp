#include "authorization.h"

#include <QtNetworkAuth/qoauthhttpserverreplyhandler.h>

#include <QFile>
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
}

void Authorization::authorize(const QString& clientId,
                              const QSet<QByteArray>& requestedScopeTokens) {
  // oauth_.setRequestedScopeTokens(
  //     {"user-read-playback-state", "user-modify-playback-state",
  //      "playlist-read-private", "user-library-read"});
  oauth_.setRequestedScopeTokens(requestedScopeTokens);
  oauth_.setClientIdentifier(clientId);

  QObject::connect(&oauth_, &QAbstractOAuth::granted, this,
                   [this] { emit granted(oauth_.token()); });
  connect(&oauth_, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, this,
          &QDesktopServices::openUrl);
  if (handler_->isListening()) {
    oauth_.grant();
  }
}
