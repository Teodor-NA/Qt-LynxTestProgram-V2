#include "qtlynxwrapper.h"

QtLynxWrapper::QtLynxWrapper(QObject *parent, char deviceId, const LynxString & deviceName) :
    QObject(parent),
    _lynx(deviceId, deviceName)
{
    // connect(_uart.portPointer(), SIGNAL(readyRead()), this, SLOT(readData()));
}

QtLynxWrapper::~QtLynxWrapper()
{
}



QString QtLynxWrapper::getStructName(const QtLynxId * lynxId)
{
    return QString(_lynx.getStructName(lynxId->lynxId()));
}

QString QtLynxWrapper::getVariableName(const QtLynxId * lynxId)
{
    return QString(_lynx.getVariableName(lynxId->lynxId()));
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




