#include "qtlynxwrapper.h"

QtLynxWrapper::QtLynxWrapper(QObject *parent) :
    QObject(parent),
    _lynx(0x25, "Device 1")
{
    // connect(_uart.portPointer(), SIGNAL(readyRead()), this, SLOT(readData()));
}

QtLynxWrapper::~QtLynxWrapper()
{
    for (int i = 0; i < _uart.count(); i++)
        _uart[i].close();
}

void QtLynxWrapper::scanForUart()
{
    qDebug() << "Attempting to scan for ports";
    _portList = QSerialPortInfo::availablePorts();

    qDebug() << _portList.count() << "ports found";

    emit clearPortList();

    for (int i = 0; i < _portList.count(); i++)
    {
        emit addPort(_portList.at(i).portName() + " - " + _portList.at(i).description());
    }
}

void QtLynxWrapper::closeAllPorts()
{
    for (int i = 0; i < _uart.count(); i++)
    {
        _uart[i].close();
    }
}

int QtLynxWrapper::openUartPort()
{
    if (_currentPortIndex < 0)
    {
        qDebug() << "Port is not in list, attempting to add";

        LynxList<bool> openList(_uart.count());

        for (int i  = 0; i < _uart.count(); i++)
        {
            openList.append(_uart.at(i).isOpen());
        }

        this->closeAllPorts();

        _currentPortIndex = _uart.append(LynxUartQt(&_lynx));

        for (int i = 0; i < openList.count(); i++)
        {
            if (openList.at(i))
                _uart[i].open();
        }

        if (_currentPortIndex < 0)
        {
            qDebug() << "Adding port failed, aborting";
            return -1;
        }
    }

    if (_uart.at(_currentPortIndex).isOpen())
    {
        qDebug() << "Port is already open, attempting to reopen";
        _uart[_currentPortIndex].close();
    }

    qDebug() << "Attempting to open port" << _uart.at(_currentPortIndex).port().portName();

    bool success = _uart[_currentPortIndex].open(_selectedPort, _baudRate);

    if (success)
    {
        qDebug() << "Port successfully opened";
        QObject::connect(&(_uart[_currentPortIndex].port()), SIGNAL(readyRead()), this, SLOT(readData()));
        setUartConnected(true);
        return _currentPortIndex;
    }

    qDebug() << "Port was not opened due to errors retuned from QT";
    setUartConnected(false);
    return -1;
}

void QtLynxWrapper::closeUartPort()
{
    if (_currentPortIndex < 0)
    {
        qDebug() << "Port is not open";
        return;
    }

    _uart[_currentPortIndex].close();
    setUartConnected(false);

}

LynxUartQt * QtLynxWrapper::lynxUartPointer(int index)
{
    if ((index >= _uart.count()) || (index < 0))
        return nullptr;

    return &(_uart[index]);
}

void QtLynxWrapper::readData()
{
//    if ((deviceIndex >= _uart.count()) || (deviceIndex < 0))
//    {
//        qDebug() << "An uart update was called, but the device index was out of bounds";
//        return;
//    }
    for (int i = 0; i < _uart.count(); i++)
     {
        _receiveInfo = _uart[i].update();

        if(_receiveInfo.state != LynxLib::eNoChange)
        {
            if(_receiveInfo.state == LynxLib::eNewDataReceived)
            {
                QtLynxId tempId(_receiveInfo.lynxId);
                emit newDataReceived(&tempId);
            }
            else if (_receiveInfo.state == LynxLib::eNewDeviceInfoReceived)
            {
                emit newDeviceInfoReceived(_uart[i].lynxDeviceInfo());
            }
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

void QtLynxWrapper::sendVariable(const QtLynxId * lynxId, int deviceIndex, const QString & value)
{
    if ((deviceIndex >= _uart.count()) || (deviceIndex < 0))
    {
        qDebug() << "Device index is out of bounds";
        return;
    }


    if (lynxId->lynxId().structIndex < 0)
    {
        qDebug() << "Struct is not added";
        return;
    }

    if (lynxId->lynxId().variableIndex < 0)
    {
        qDebug() << "Sending whole struct, value ignored";
        _uart[deviceIndex].send(lynxId->lynxId());
        return;
    }

    switch (_lynx.simplifiedType(lynxId->lynxId()))
    {
    case LynxLib::eString:
        qDebug() << "Sending value:" << value;
        _lynx.setString(value.toLatin1().data(), lynxId->lynxId());
        _uart[deviceIndex].send(lynxId->lynxId());
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
        _uart[deviceIndex].send(lynxId->lynxId());

    }
        break;
    case LynxLib::eBool:
    {
        if (value.toLower() == "false")
        {
            qDebug() << "Sending value: false";
            _lynx.setBool(false, lynxId->lynxId());
            _uart[deviceIndex].send(lynxId->lynxId());
            return;
        }
        else if (value.toLower() == "true")
        {
            qDebug() << "Sending value: true";
            _lynx.setBool(true, lynxId->lynxId());
            _uart[deviceIndex].send(lynxId->lynxId());
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

        _uart[deviceIndex].send(lynxId->lynxId());

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

void QtLynxWrapper::changeRemoteDeviceId(const QString & deviceId, int deviceIndex)
{
    if ((deviceIndex >= _uart.count()) || (deviceIndex < 0))
    {
        qDebug() << "Device index is out of bounds";
        return;
    }

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
    _uart[deviceIndex].changeRemoteDeviceId(tmpId);
}

void QtLynxWrapper::selectPort(int portIndex)
{
    if ((portIndex < 0) || (portIndex >= _portList.count()))
    {
        qDebug() << "Port index is out of bounds";
        return;
    }

    _selectedPort = _portList.at(portIndex);

    qDebug() << "Selected port:" << _selectedPort.portName();

    _currentPortIndex = -1;
    for (int i = 0; i < _uart.count(); i++)
    {
        qDebug() << "Port candidate:" << _uart.at(i).port().portName();

        if (_uart.at(i).port().portName() == _selectedPort.portName())
        {
            _currentPortIndex = i;
            break;
        }
    }

    if (_currentPortIndex < 0)
    {
        setUartConnected(false);
    }
    else
    {
        setUartConnected(_uart.at(_currentPortIndex).isOpen());
    }
}

void QtLynxWrapper::connectButtonClicked()
{
    if (_uartConnected)
        this->closeUartPort();
    else
        this->openUartPort();
}
