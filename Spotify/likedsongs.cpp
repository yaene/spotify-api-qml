
#include "likedsongs.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

using namespace Qt::StringLiterals;

LikedSongsPlaylist::LikedSongsPlaylist(const QString& userId,
                                       QNetworkAccessManager* netw,
                                       QNetworkRequestFactory* api,
                                       QObject* parent)
    : Playlist("liked", QObject::tr("Liked Songs"), "me",
               QUrl("https://misc.scdn.co/liked-songs/liked-songs-300.jpg"), 0,
               netw, api, parent),
      m_userId(userId) {}

void LikedSongsPlaylist::fetchTracks() {
  if (m_tracksLoading) return;
  m_tracksLoading = true;
  emit tracksLoadingChanged();

  auto url = QString("me/tracks");

  QNetworkReply* reply = m_netw->get(m_api->createRequest(url));
  connect(reply, &QNetworkReply::finished, this,
          [this, reply]() { onTracksReply(reply); });
}

void LikedSongsPlaylist::play() {
  auto url = QString("me/player/play");
  QJsonObject json;
  json["context_uri"] = u"spotify:user:%1:collection"_s.arg(m_userId);
  QJsonDocument doc(json);

  m_netw->put(m_api->createRequest(url), doc.toJson());
}
