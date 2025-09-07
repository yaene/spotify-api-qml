// main.cpp
#include <QtGui/qguiapplication.h>

#include <QDebug>
#include <QTimer>
#include <QtGui/QGuiApplication>

#include "authorization.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  // Use your real Spotify client ID!
  const QString clientId = "d66dea877b004271813e5579438321df";

  Authorization auth;

  QObject::connect(&auth, &Authorization::granted, [&](const QString &token) {
    qDebug() << "Access token granted:" << token;
    app.quit();
  });
  QObject::connect(&auth, &Authorization::error, [&](const QString &err) {
    qDebug() << "Error:" << err;
    app.quit();
  });

  // QOAuth may require event loop already running, so delay slightly
  QTimer::singleShot(0, [&]() {
    auth.authorize(clientId,
                   {"user-read-playback-state", "user-modify-playback-state",
                    "playlist-read-private", "user-library-read"});
  });

  return app.exec();
}
