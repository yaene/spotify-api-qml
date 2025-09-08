#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QAbstractOAuth>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequestFactory>
#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVariantMap>

#include "./authorization.h"

class SpotifyApi : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool authorized READ isAuthorized NOTIFY authorizedChanged)
  Q_PROPERTY(QVariantMap currentUser READ currentUser NOTIFY currentUserChanged)
  Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY
                 clientIdChanged REQUIRED)
  Q_PROPERTY(QStringList scope READ scope WRITE setScope NOTIFY scopeChanged)
  QML_ELEMENT
 public:
  explicit SpotifyApi(QObject *parent = nullptr);

  Q_INVOKABLE void init();
  Q_INVOKABLE void authorize();
  Q_INVOKABLE void updateCurrentUser();

  bool isAuthorized() const;
  QVariantMap currentUser() const;
  QString clientId() const { return clientId_; }
  void setClientId(const QString &id);
  QStringList scope() const { return scope_; }
  void setScope(const QStringList &scope);

 signals:
  void authorizedChanged();
  void authorizationFailed(const QString &msg);
  void currentUserChanged();
  void clientIdChanged();
  void scopeChanged();

 private slots:
  void onCurrentUserReply(QNetworkReply *reply);
  void onAuthGranted(const QString &token);
  void onAuthFailed(const QAbstractOAuth::Error &error);

 private:
  Authorization *authorization_ = nullptr;
  QNetworkAccessManager netw_;
  QNetworkRequestFactory api_;
  QString accessToken_;
  QVariantMap currentUser_;
  bool authorized_ = false;
  QString clientId_;
  QStringList scope_;
};
