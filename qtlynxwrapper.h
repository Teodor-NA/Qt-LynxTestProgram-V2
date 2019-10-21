#ifndef QTLYNXWRAPPER_H
#define QTLYNXWRAPPER_H

#include <QObject>
#include "LynxStructure.h"
#include "lynxuartqt.h"

class QtLynxWrapper : public QObject
{
    Q_OBJECT

    LynxManager _lynx;
    LynxUartQt _uart;

    LynxInfo _receiveInfo;

public:
    explicit QtLynxWrapper(QObject *parent = nullptr);
    ~QtLynxWrapper() { _uart.close(); }

    LynxManager * lynxPointer() { return &_lynx; }
    LynxUartQt * lynxUartPointer() { return &_uart; }

signals:
    void newDataReceived(const LynxId & lynxId);
    void newDeviceInfoReceived(const LynxDeviceInfo & deviceInfo);

public slots:
    void readData();
};

#endif // QTLYNXWRAPPER_H
