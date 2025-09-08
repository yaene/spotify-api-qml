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
  QML_ELEMENT
  QML_SINGLETON
 public:
  explicit SpotifyApi(QObject *parent = nullptr);

  Q_INVOKABLE void authorize(const QString &clientId,
                             const QStringList &scopes);
  Q_INVOKABLE void updateCurrentUser();

  bool isAuthorized() const;
  QVariantMap currentUser() const;

 signals:
  void authorizedChanged();
  void authorizationFailed(const QString &msg);
  void currentUserChanged();

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
};
