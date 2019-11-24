#include "qtlynxuartwrapper.h"

QtLynxUartWrapper::QtLynxUartWrapper(LynxManager * lynx, QObject *parent) :
    QObject(parent),
    _lynx(lynx)
{

}

QtLynxUartWrapper::~QtLynxUartWrapper()
{
    this->closeAllPorts();
}

void QtLynxUartWrapper::scanForUart()
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

void QtLynxUartWrapper::closeAllPorts()
{
    for (int i = 0; i < _uart.count(); i++)
    {
        _uart[i].close();
    }
}

int QtLynxUartWrapper::openUartPort()
{
    if (_portIndex < 0)
    {
        qDebug() << "Port is not in list, attempting to add";

        LynxList<bool> openList(_uart.count());

        for (int i  = 0; i < _uart.count(); i++)
        {
            openList.append(_uart.at(i).isOpen());
        }

        this->closeAllPorts();

        setPortIndex(_uart.append(LynxUartQt(_lynx)));

        for (int i = 0; i < openList.count(); i++)
        {
            // Set port index
            _uart[i].setPortIndex(i);
            // Re-connect lost connections
            QObject::connect(&(_uart[i].portContainer()), SIGNAL(readyRead(int)), this, SLOT(readData(int)));

            if (openList.at(i))
            {
                // Re-open previously opened port
                bool success = _uart[i].open();

                if (!success)
                {
                    qDebug() << "Re-opening of port" << _uart[i].portContainer().port().portName() << "was unsuccessful.";
                    qDebug() << "It may have to be re-opened manually";
                }
            }
        }

        if (_portIndex < 0)
        {
            qDebug() << "Adding port failed, aborting";
            return -1;
        }

        // Set index on the new port
        _uart.last().setPortIndex(_uart.count() - 1);
        // Connect the new port
        QObject::connect(&(_uart.last().portContainer()), SIGNAL(readyRead(int)), this, SLOT(readData(int)));
    }

    if (_uart.at(_portIndex).isOpen())
    {
        qDebug() << "Port is already open, attempting to reopen";
        _uart[_portIndex].close();
    }

    qDebug() << "Attempting to open port" << _uart.at(_portIndex).portContainer().port().portName();

    bool success = _uart[_portIndex].open(_selectedPort, _baudRate);

    if (success)
    {
        qDebug() << "Port successfully opened";

        // QObject::connect(&(_uart[_currentPortIndex].portContainer()), SIGNAL(readyRead(int)), this, SLOT(readData(int)));
        setUartConnected(true);
        return _portIndex;
    }

    qDebug() << "Port was not opened due to errors retuned from QT";
    setUartConnected(false);
    return -1;
}

void QtLynxUartWrapper::closeUartPort()
{
    if (_portIndex < 0)
    {
        qDebug() << "Port is not open";
        return;
    }

    _uart[_portIndex].close();
    setUartConnected(false);

}


void QtLynxUartWrapper::readData(int portIndex)
{
    if ((portIndex >= _uart.count()) || (portIndex < 0))
    {
        qDebug() << "An uart update was called, but the device index was out of bounds";
        return;
    }

    _receiveInfo = _uart[portIndex].update();

    if(_receiveInfo.state != LynxLib::eNoChange)
    {
        if(_receiveInfo.state == LynxLib::eNewDataReceived)
        {
            QtLynxId tempId(_receiveInfo.lynxId);
            emit newDataReceived(&tempId);
        }
        else if (_receiveInfo.state == LynxLib::eNewDeviceInfoReceived)
        {
            emit newDeviceInfoReceived(_uart[portIndex].lynxDeviceInfo(), portIndex);
        }
        else if(_receiveInfo.state > LynxLib::eErrors)
        {
            qDebug() << "New information was received with error:" << LynxTextList::lynxState(_receiveInfo.state);
        }
    }


//    for (int i = 0; i < _uart.count(); i++)
//     {
//        _receiveInfo = _uart[i].update();

//        if(_receiveInfo.state != LynxLib::eNoChange)
//        {
//            if(_receiveInfo.state == LynxLib::eNewDataReceived)
//            {
//                QtLynxId tempId(_receiveInfo.lynxId);
//                emit newDataReceived(&tempId);
//            }
//            else if (_receiveInfo.state == LynxLib::eNewDeviceInfoReceived)
//            {
//                emit newDeviceInfoReceived(_uart[i].lynxDeviceInfo());
//            }
//        }
//    }
}

void QtLynxUartWrapper::sendVariable(int portIndex, const QtLynxId * lynxId, const QString & value)
{
    if ((portIndex >= _uart.count()) || (portIndex < 0))
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
        _uart[portIndex].send(lynxId->lynxId());
        return;
    }

    switch (_lynx->simplifiedType(lynxId->lynxId()))
    {
    case LynxLib::eString:
        qDebug() << "Sending value:" << value;
        _lynx->setString(value.toLatin1().data(), lynxId->lynxId());
        _uart[portIndex].send(lynxId->lynxId());
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

        _lynx->setValue(tempVal, lynxId->lynxId());
        _uart[portIndex].send(lynxId->lynxId());

    }
        break;
    case LynxLib::eBool:
    {
        if (value.toLower() == "false")
        {
            qDebug() << "Sending value: false";
            _lynx->setBool(false, lynxId->lynxId());
            _uart[portIndex].send(lynxId->lynxId());
            return;
        }
        else if (value.toLower() == "true")
        {
            qDebug() << "Sending value: true";
            _lynx->setBool(true, lynxId->lynxId());
            _uart[portIndex].send(lynxId->lynxId());
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
            _lynx->setBool(false, lynxId->lynxId());
        }
        else
        {
            qDebug() << "Sending value: true";
            _lynx->setBool(true, lynxId->lynxId());
        }

        _uart[portIndex].send(lynxId->lynxId());

    }
        break;
    default:
        qDebug() << "Error: type not found";
        return;
    }
}

void QtLynxUartWrapper::selectPort(int portIndex)
{
    if ((portIndex < 0) || (portIndex >= _portList.count()))
    {
        qDebug() << "Port index is out of bounds";
        return;
    }

    _selectedPort = _portList.at(portIndex);

    // qDebug() << "Selected port:" << _selectedPort.portName();

    setPortIndex(-1);
    for (int i = 0; i < _uart.count(); i++)
    {
        // qDebug() << "Port candidate:" << _uart.at(i).portContainer().port().portName();

        if (_uart.at(i).portContainer().port().portName() == _selectedPort.portName())
        {
            setPortIndex(i);
            break;
        }
    }

    if (_portIndex < 0)
    {
        setUartConnected(false);
    }
    else
    {
        setUartConnected(_uart.at(_portIndex).isOpen());
    }
}

void QtLynxUartWrapper::connectButtonClicked()
{
    if (_uartConnected)
        this->closeUartPort();
    else
        this->openUartPort();
}

void QtLynxUartWrapper::scan()
{
    for (int i = 0; i < _uart.count(); i++)
    {
        _uart[i].scan();
    }
}

void QtLynxUartWrapper::pullStruct(int portIndex, const QtLynxId * lynxId)
{
    if (lynxId->lynxId().structIndex < 0)
    {
        qDebug() << "Could not find datagram. Did you remember to add the struct?";
        return;
    }

    if ((portIndex >= _uart.count()) || (portIndex < 0))
    {
        qDebug() << "Port index" << portIndex << "is out of bounds";
        return;
    }

    qDebug() << "Pulling datagram with structIndex:" << lynxId->lynxId().structIndex << "and variableIndex:" << lynxId->lynxId().variableIndex;
    _uart[portIndex].pullDatagram(lynxId->lynxId());
}

void QtLynxUartWrapper::startPeriodic(int portIndex, unsigned int interval, const QtLynxId * lynxId)
{
    // LynxId tmpId(structIndex);

    if (lynxId->lynxId().structIndex < 0)
    {
        qDebug() << "Could not find datagram. Did you remember to add the struct?";
        return;
    }

    if ((portIndex >= _uart.count()) || (portIndex < 0))
    {
        qDebug() << "Port index" << portIndex << "is out of bounds";
        return;
    }

    qDebug() << "Starting periodic transmit at:" << interval << "ms interval";
    _uart[portIndex].remotePeriodicStart(lynxId->lynxId(), interval);
}

void QtLynxUartWrapper::stopPeriodic(int portIndex, const QtLynxId * lynxId)
{
    // LynxId tmpId(structIndex);

    if (lynxId->lynxId().structIndex < 0)
    {
        qDebug() << "Could not find datagram. Did you remember to add the struct?";
        return;
    }

    if ((portIndex >= _uart.count()) || (portIndex < 0))
    {
        qDebug() << "Port index" << portIndex << "is out of bounds";
        return;
    }

    qDebug() << "Stopping periodic transmit";
    _uart[portIndex].remotePeriodicStop(lynxId->lynxId());
}

void QtLynxUartWrapper::changeRemoteDeviceId(int portIndex, const QString & oldDeviceId, const QString & newDeviceId)
{
    if ((portIndex >= _uart.count()) || (portIndex < 0))
    {
        qDebug() << "Port index is out of bounds";
        return;
    }

    bool parseOk;

    char tmpNewId = static_cast<char>(newDeviceId.toShort(&parseOk, 16));
    if (!parseOk)
    {
        qDebug() << "Value" << newDeviceId << "could not be converted to a number";
        return;
    }

    if (tmpNewId == 0)
    {
        qDebug() << "Device id 0 is invalid";
        return;
    }
    else if (tmpNewId == char(255))
    {
        qDebug() << "Warning: device id 0xff is considered un-initialized, but not invalid";
    }

    char tmpOldId = static_cast<char>(oldDeviceId.toShort(&parseOk, 16));

    if (!parseOk)
    {
        qDebug() << "Value" << oldDeviceId << "could not be converted to a number";
        return;
    }

    qDebug() << "Changing remote device id" << oldDeviceId << "to 0x" + QString::number((int(tmpNewId) & 0xff), 16);
    _uart[portIndex].changeRemoteDeviceId(tmpNewId);

    emit changeLocalDeviceId(tmpOldId, tmpNewId);
}
