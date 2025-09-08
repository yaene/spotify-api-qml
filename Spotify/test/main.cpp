// main.cpp
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QGuiApplication>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <QtQml/QQmlApplicationEngine>

#include "../authorization.h"

using namespace Qt::StringLiterals;

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  const QUrl url(u"qrc:/qt/qml/test/main.qml"_s);
  engine.load(url);
  return app.exec();
}
