#include <QApplication>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlContext>
#include <QQmlApplicationEngine>

#include "backend.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQuickView viewer;

    BackEnd backEnd(&viewer);

    viewer.rootContext()->setContextProperty("backEnd", &backEnd);

    QObject::connect(viewer.engine(), &QQmlEngine::quit, &viewer, &QWindow::close);

    viewer.setTitle("Qt-Lynx Test Application");
    viewer.setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    viewer.setWidth(1280);
    viewer.setHeight(720);
    viewer.setMinimumSize(QSize(720,480));
    // viewer.show();
    // viewer.showFullScreen();
    viewer.show();

    return app.exec();
}
