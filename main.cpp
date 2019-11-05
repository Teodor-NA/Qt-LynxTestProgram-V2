#include <QApplication>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlContext>
#include <QQmlApplicationEngine>

#include "qtlynxwrapper.h"
#include "qtlynxuartwrapper.h"
#include "backend.h"
#include "scopeserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("LynxDynamics");

    QApplication app(argc, argv);

    QQuickView viewer;

    QtLynxWrapper lynx(&viewer, 0x25, "Device 1");
    QtLynxUartWrapper lynxUart(lynx.lynxPointer(), &viewer);
    BackEnd backEnd(lynx.lynxPointer(), &viewer);
    ScopeServer scopeServer(lynx.lynxPointer(), &viewer);

    viewer.rootContext()->setContextProperty("lynx", &lynx);
    viewer.rootContext()->setContextProperty("lynxUart", &lynxUart);
    viewer.rootContext()->setContextProperty("backEnd", &backEnd);
    viewer.rootContext()->setContextProperty("scopeServer", &scopeServer);

    QObject::connect(viewer.engine(), &QQmlEngine::quit, &viewer, &QWindow::close);
    QObject::connect(&lynx,SIGNAL(newDataReceived(const QtLynxId *)),&scopeServer,SLOT(newDataRecived(const QtLynxId *)));
    // QObject::connect(&lynx, SIGNAL(newDataReceived(const QtLynxId *)), &backEnd, SLOT(newDataReceived(const QtLynxId *)));
    QObject::connect(&lynxUart, SIGNAL(newDeviceInfoReceived(const LynxDeviceInfo &, int)), &backEnd, SLOT(newDeviceInfoReceived(const LynxDeviceInfo &, int)));
    QObject::connect(&backEnd, SIGNAL(getIdList()), &scopeServer, SLOT(getIdList()));
    QObject::connect(&lynxUart, SIGNAL(changeLocalDeviceId(char, char)), &backEnd, SLOT(changeLocalDeviceId(char, char)));

    qmlRegisterType<QtLynxId>("lynxlib", 1, 0, "LynxId");
    qRegisterMetaType<QtLynxId*>("const QtLynxId*");

    qmlRegisterType<QtLynxWrapper>("lynxlib", 1, 0, "Lynx");

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
