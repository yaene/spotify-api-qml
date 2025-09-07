#pragma once
#include <qtmetamacros.h>

#include <QtNetwork/QRestAccessManager>
#include <QtNetworkAuth/QOAuth2AuthorizationCodeFlow>
#include <QtNetworkAuth/QOAuthHttpServerReplyHandler>

class Authorization : public QObject {
  Q_OBJECT
 public:
  explicit Authorization(QObject* parent = nullptr, quint16 port = 8888);
  Q_INVOKABLE void authorize(const QString& clientId,
                             const QSet<QByteArray>& requestedScopeTokens);
 signals:
  void granted(const QString& accessToken);
  void error(const QString& msg);

 private:
  QOAuth2AuthorizationCodeFlow oauth_;
  QOAuthHttpServerReplyHandler* handler_ = nullptr;
};
