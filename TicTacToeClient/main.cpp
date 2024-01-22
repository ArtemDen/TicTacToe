#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Worker.h"
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  app.setDesktopFileName("XO");
  app.setApplicationDisplayName("XO");

  Worker worker;

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("worker", &worker);
  engine.rootContext()->setContextProperty("battlefieldModel", worker.poGetModel());
  qmlRegisterUncreatableType<ClientState>("ClientState", 1, 0, "ClientState", "Enum type");
  qmlRegisterUncreatableType<Figure>("Figure", 1, 0, "Figure", "Enum type");

  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                   &app, [url](QObject *obj, const QUrl &objUrl) {
    if (!obj && url == objUrl)
      QCoreApplication::exit(-1);
  }, Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
//-------------------------------------------------------------------------------------------------
