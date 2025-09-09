
#include "playlist.h"

#include <qnetworkaccessmanager.h>
#include <qnetworkrequestfactory.h>

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Playlist::Playlist(const QString& id, const QString& name, const QString& owner,
                   const QUrl& imageUrl, int trackCount,
                   QNetworkAccessManager* netw, QNetworkRequestFactory* api,
                   QObject* parent)
    : QObject(parent),
      m_id(id),
      m_name(name),
      m_owner(owner),
      m_imageUrl(imageUrl),
      m_trackCount(trackCount),
      m_netw(netw),
      m_api(api) {}

void Playlist::fetchTracks() {
  if (m_tracksLoading) return;
  m_tracksLoading = true;
  emit tracksLoadingChanged();

  auto url = QString("playlists/%1/tracks").arg(m_id);

  QNetworkReply* reply = m_netw->get(m_api->createRequest(url));
  connect(reply, &QNetworkReply::finished, this,
          [this, reply]() { onTracksReply(reply); });
}

void Playlist::onTracksReply(QNetworkReply* reply) {
  m_tracksLoading = false;
  emit tracksLoadingChanged();

  if (reply->error()) {
    qWarning() << "Error fetching tracks for playlist:" << reply->errorString();
    reply->deleteLater();
    return;
  }

  QStringList newTracks;
  QByteArray resp = reply->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(resp);
  if (doc.isObject()) {
    QJsonObject obj = doc.object();
    QJsonArray items = obj["items"].toArray();
    for (const QJsonValue& val : items) {
      QJsonObject trackObj = val.toObject().value("track").toObject();
      QString trackName = trackObj.value("name").toString();
      newTracks.push_back(trackName);
    }
  }

  m_tracks = newTracks;
  emit tracksChanged();

  reply->deleteLater();
}
