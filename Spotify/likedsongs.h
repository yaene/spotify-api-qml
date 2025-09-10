#pragma once
#include "playlist.h"

class LikedSongsPlaylist : public Playlist {
  Q_OBJECT
 public:
  explicit LikedSongsPlaylist(const QString& userId,
                              QNetworkAccessManager* netw,
                              QNetworkRequestFactory* api,
                              QObject* parent = nullptr);

  void fetchTracks() override;
  void play() override;

 private:
  QString m_userId;
};
