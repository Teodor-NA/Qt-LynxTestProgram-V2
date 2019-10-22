#include "qtlynxwrapper.h"

QtLynxWrapper::QtLynxWrapper(QObject *parent) :
    QObject(parent),
    _lynx(0x25, "Device 1"),
    _uart(_lynx)
{
    connect(_uart.portPointer(), SIGNAL(readyRead()), this, SLOT(readData()));
}

void QtLynxWrapper::readData()
{
    _receiveInfo = _uart.update();

    if(_receiveInfo.state != LynxLib::eNoChange)
    {
        if(_receiveInfo.state == LynxLib::eNewDataReceived)
        {
            emit newDataReceived(_receiveInfo.lynxId);
        }
        else if (_receiveInfo.state == LynxLib::eNewDeviceInfoReceived)
        {
            emit newDeviceInfoReceived(_uart.lynxDeviceInfo());
        }
    }
}

QString QtLynxWrapper::getStructName(QtLynxId * lynxId)
{
    return QString(_lynx.getStructName(lynxId->lynxId()));
}

QString QtLynxWrapper::getVariableName(QtLynxId * lynxId)
{
    return QString(_lynx.getVariableName(lynxId->lynxId()));
}
