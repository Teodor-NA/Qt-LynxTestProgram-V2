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
            QtLynxId tempId(_receiveInfo.lynxId);
            emit newDataReceived(&tempId);
        }
        else if (_receiveInfo.state == LynxLib::eNewDeviceInfoReceived)
        {
            emit newDeviceInfoReceived(_uart.lynxDeviceInfo());
        }
    }
}

QString QtLynxWrapper::getStructName(const QtLynxId * lynxId)
{
    return QString(_lynx.getStructName(lynxId->lynxId()));
}

QString QtLynxWrapper::getVariableName(const QtLynxId * lynxId)
{
    return QString(_lynx.getVariableName(lynxId->lynxId()));
}

void QtLynxWrapper::sendVariable(const QtLynxId * lynxId, const QString & value)
{
    if (lynxId->lynxId().structIndex < 0)
    {
        qDebug() << "Struct is not added";
        return;
    }

    if (lynxId->lynxId().variableIndex < 0)
    {
        qDebug() << "Sending whole struct, value ignored";
        _uart.send(lynxId->lynxId());
        return;
    }

    switch (_lynx.simplifiedType(lynxId->lynxId()))
    {
    case LynxLib::eString:
        qDebug() << "Sending value:" << value;
        _lynx.setString(value.toLatin1().data(), lynxId->lynxId());
        _uart.send(lynxId->lynxId());
        break;
    case LynxLib::eNumber:
    {
        bool parseOk;
        double tempVal = value.toDouble(&parseOk);

        if (!parseOk)
        {
            qDebug() << value << "Could not be converted to a number, nothing will be sent";
            return;
        }

        qDebug() << "Sending value:" << tempVal;

        _lynx.setValue(tempVal, lynxId->lynxId());
        _uart.send(lynxId->lynxId());

    }
        break;
    case LynxLib::eBool:
    {
        if (value.toLower() == "false")
        {
            qDebug() << "Sending value: false";
            _lynx.setBool(false, lynxId->lynxId());
            _uart.send(lynxId->lynxId());
            return;
        }
        else if (value.toLower() == "true")
        {
            qDebug() << "Sending value: true";
            _lynx.setBool(true, lynxId->lynxId());
            _uart.send(lynxId->lynxId());
            return;
        }

        bool parseOk;
        double tempVal = value.toDouble(&parseOk);

        if (!parseOk)
        {
            qDebug() << value << "Could not be converted to a number or boolean value, nothing will be sent";
            return;
        }

        if (tempVal == 0.0)
        {
            qDebug() << "Sending value: false";
            _lynx.setBool(false, lynxId->lynxId());
        }
        else
        {
            qDebug() << "Sending value: true";
            _lynx.setBool(true, lynxId->lynxId());
        }

        _uart.send(lynxId->lynxId());

    }
        break;
    default:
        qDebug() << "Error: type not found";
        return;
    }
}

double QtLynxWrapper::getValueAsNumber(const QtLynxId * lynxId)
{
        LynxLib::E_LynxSimplifiedType dataType = _lynx.simplifiedType(lynxId->lynxId());

        if ((dataType == LynxLib::eNumber) || (dataType == LynxLib::eBool))
            return _lynx.getValue(lynxId->lynxId());

        return 0;
}

QString QtLynxWrapper::getValueAsString(const QtLynxId * lynxId)
{
    LynxLib::E_LynxSimplifiedType dataType = _lynx.simplifiedType(lynxId->lynxId());

    switch (dataType)
    {
    case LynxLib::eString:
        return QString(_lynx.getString(lynxId->lynxId()));
    case LynxLib::eNumber:
        return QString::number(_lynx.getValue(lynxId->lynxId()));
    case LynxLib::eBool:
        return (_lynx.getBool(lynxId->lynxId()) ? "True" : "False");
    default:
        break;
    }

    return "Not initialized";
}

bool QtLynxWrapper::getValueAsBool(const QtLynxId * lynxId)
{
        LynxLib::E_LynxSimplifiedType dataType = _lynx.simplifiedType(lynxId->lynxId());

        if (dataType == LynxLib::eBool)
            return _lynx.getBool(lynxId->lynxId());

        return false;
}

QtLynxWrapper::QtLynxType QtLynxWrapper::getDataType(const QtLynxId * lynxId)
{
    return QtLynxWrapper::QtLynxType(_lynx.simplifiedType(lynxId->lynxId()));
}

void QtLynxWrapper::changeRemoteDeviceId(const QString & deviceId)
{
    bool parseOk;
    char tmpId = static_cast<char>(deviceId.toShort(&parseOk, 16));

    if (!parseOk)
    {
        qDebug() << "Value" << deviceId << "could not be converted to a number";
        return;
    }

    if (tmpId == 0)
    {
        qDebug() << "Device id 0 is invalid";
        return;
    }
    else if (tmpId == char(255))
    {
        qDebug() << "Warning: device id 0xff is considered un-initialized, but not invalid";
    }

    qDebug() << "Setting remote device id to 0x" + QString::number(tmpId, 16);
    _uart.changeRemoteDeviceId(tmpId);
}
