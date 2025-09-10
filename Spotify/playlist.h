#pragma once

#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequestFactory>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QtQmlIntegration>

class Playlist : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString id READ id CONSTANT)
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString owner READ owner CONSTANT)
  Q_PROPERTY(QUrl imageUrl READ imageUrl CONSTANT)
  Q_PROPERTY(int trackCount READ trackCount CONSTANT)
  Q_PROPERTY(QStringList tracks READ tracks NOTIFY tracksChanged)
  Q_PROPERTY(bool tracksLoading READ tracksLoading NOTIFY tracksLoadingChanged)
  QML_ELEMENT

 public:
  Playlist(const QString& id, const QString& name, const QString& owner,
           const QUrl& imageUrl, int trackCount,
           QNetworkAccessManager* network_manager,
           QNetworkRequestFactory* request_factory, QObject* parent = nullptr);

  QString id() const { return m_id; }
  QString name() const { return m_name; }
  QString owner() const { return m_owner; }
  QUrl imageUrl() const { return m_imageUrl; }
  int trackCount() const { return m_trackCount; }

  QStringList tracks() const { return m_tracks; }
  bool tracksLoading() const { return m_tracksLoading; }

  Q_INVOKABLE void fetchTracks();
  Q_INVOKABLE void play();

 signals:
  void tracksChanged();
  void tracksLoadingChanged();

 private slots:
  void onTracksReply(QNetworkReply* reply);

 private:
  QString m_id;
  QString m_name;
  QString m_owner;
  QUrl m_imageUrl;
  int m_trackCount;
  QStringList m_tracks;
  bool m_tracksLoading = false;
  QNetworkAccessManager* m_netw;
  QNetworkRequestFactory* m_api;
};
