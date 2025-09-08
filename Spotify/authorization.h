#pragma once

#include <QAbstractOAuth>
#include <QtNetwork/QRestAccessManager>
#include <QtNetworkAuth/QOAuth2AuthorizationCodeFlow>
#include <QtNetworkAuth/QOAuthHttpServerReplyHandler>

class Authorization : public QObject {
  Q_OBJECT
 public:
  explicit Authorization(QObject* parent = nullptr, quint16 port = 8888);
  Q_INVOKABLE void authorize(const QString& clientId,
                             const QSet<QByteArray>& requestedScopeTokens);
  Q_INVOKABLE void refreshTokens(const QString& clientId,
                                 const QSet<QByteArray>& requestedScopeTokens);
 signals:
  void granted(const QString& accessToken);
  void failed(const QAbstractOAuth::Error& error);

 private slots:
  void onAccessTokenChanged(const QString& token);
  void onRefreshTokenChanged(const QString& token);
  void onRequestFailed(const QAbstractOAuth::Error error);

 private:
  QOAuth2AuthorizationCodeFlow oauth_;
  QOAuthHttpServerReplyHandler* handler_ = nullptr;

  void persistRefreshToken(const QString& refreshToken);
};
