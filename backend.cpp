#include "backend.h"

BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    _lynx(0x25, "Device 1"),
    _uart(_lynx)
{
   //  _uart.open(4, 115200);

    _selectedDevice = -1;
    _selectedStruct = -1;

    _fullscreen = false;

    connect(_uart.portPointer(), SIGNAL(readyRead()), this, SLOT(readData()));
}

void BackEnd::scan()
{
    qDebug() << "\n------------ Sending Scan -------------\n";
    _uart.scan();
    // this->clearDevices();
    // this->addDevice("desc");
}

void BackEnd::readData()
{
    _receiveInfo = _uart.update();

    if(_receiveInfo.state != LynxLib::eNoChange)
    {
        qDebug() << "";
        qDebug() << "------------- Received ----------------";
        qDebug() << QString::asprintf("Device ID: 0x%x", _receiveInfo.deviceId);
        qDebug() << QString::asprintf("Struct ID: 0x%x", _lynx.structId(_receiveInfo.lynxId));
        qDebug() << "Struct Index: " << _receiveInfo.lynxId.structIndex;
        qDebug() << "Variable Index: " << _receiveInfo.lynxId.variableIndex;
        qDebug() << "Length: " << _receiveInfo.dataLength;
        qDebug() << "State: " << LynxTextList::lynxState(_receiveInfo.state);
        qDebug() << "---------------------------------------";

        if(_receiveInfo.state == LynxLib::eNewDataReceived)
        {
            int deviceIndex = -1;

            for (int i = 0; i < _deviceInfoList.count(); i++)
            {
                if (_deviceInfoList.at(i).deviceId == _receiveInfo.deviceId)
                {
                    deviceIndex = i;
                    break;
                }
            }

            if (deviceIndex < 0)
            {
                qDebug() << "Could not find device";
                return;
            }

            int structIndex = -1;

            for (int i = 0; i < _deviceInfoList.at(deviceIndex).structs.count(); i++)
            {
                if (_deviceInfoList.at(deviceIndex).structs.at(i).structId == _receiveInfo.structId)
                {
                    structIndex = i;
                    break;
                }
            }

            if (structIndex < 0)
            {
                qDebug() << "Could not find struct";
                return;
            }

            int dynStructIndex = -1;

            for (int i = 0; i < _addedStructs.count(); i++)
            {
                if ((deviceIndex == _addedStructs.at(i).deviceIndex) && (structIndex == _addedStructs.at(i).structIndex))
                {
                    dynStructIndex = i;
                    break;
                }
            }

            if (dynStructIndex < 0)
            {
                qDebug() << "Dynamic struct not found";
                return;
            }

            qDebug() << "-------------- Variables --------------";
            for (int i = 0; i < _deviceInfoList.at(deviceIndex).structs.at(structIndex).variables.count(); i++)
            {
                qDebug() << (QString(_deviceInfoList.at(deviceIndex).structs.at(structIndex).variables.at(i).description) + ":") <<
                           _lynx.getValue(_dynamicIds.at(dynStructIndex).variableIds.at(i));

            }
            qDebug()<< "---------------------------------------";


            if ((deviceIndex == _selectedDevice) && (structIndex == _selectedStruct))
            {
                for (int i = 0; i < _dynamicIds.at(dynStructIndex).variableIds.count(); i++)
                {
                    this->addVariableValue(i, _lynx.getValue(_dynamicIds.at(dynStructIndex).variableIds.at(i)));
                }
            }

        }
        else if (_receiveInfo.state == LynxLib::eNewDeviceInfoReceived)
        {
            _deviceInfoList.append(_uart.lynxDeviceInfo());
            this->addDevice(QString(_deviceInfoList.last().description) + QString::asprintf(" - Id: 0x%x", _deviceInfoList.last().deviceId));
        }
    }
}

void BackEnd::refreshPortList()
{
    this->clearPortList();
    this->addPort("Select port");

    _portList = QSerialPortInfo::availablePorts();

    QString tempName;
    for (int i = 0; i < _portList.count(); i++)
    {
        tempName = _portList.at(i).portName();
        tempName += " - ";
        tempName += _portList.at(i).description();

        if (!_portList.at(i).isNull())
            this->addPort(tempName);
    }
}

void BackEnd::portSelected(int index)
{
    qDebug() << "Port selsected";
    qDebug() << "Index: " << index;
    if ((index < 0) || (index >= _portList.count()))
        return;

    _selectedPort = _portList.at(index);
}

void BackEnd::connectButtonClicked()
{
    // qDebug() << "Button clicked";
    if (_uart.opened())
    {
        qDebug() << "Closing port";
        _uart.close();
    }
    else
    {
        qDebug() << "Attempting to open";
        qDebug() << _selectedPort.portName();
        qDebug() << _selectedPort.description();
        if (_uart.open(_selectedPort, _baudrate))
            qDebug() << "Opened successfully";
        else
            qDebug() << "Open failed";
    }
}

void BackEnd::selectDevice(int index)
{
    qDebug() << "deviceIndex:" << index;

    _selectedDevice = index - 1;

    if ((_selectedDevice >= _deviceInfoList.count()) || (_selectedDevice < 0))
    {
        this->addDeviceInfo("No selection", "No selection", "No selection", "No selection");
        this->clearStructList();
        // this->addStruct("No selection");
        // this->addStructInfo("No selection", "No selection", "No selection");
        // this->clearVariableList();
        // this->addVarable("No selection", "No selection", "No selection");
    }
    else
    {
        this->addDeviceInfo(
                    QString(_deviceInfoList.at(index - 1).description),
                    "0x" + QString::number(_deviceInfoList.at(index - 1).deviceId, 16),
                    QString(_deviceInfoList.at(index - 1).lynxVersion),
                    QString::number(_deviceInfoList.at(index - 1).structCount)
                    );

        this->clearStructList();
        // this->addStruct("No selection");
        for (int i = 0; i < _deviceInfoList.at(index - 1).structs.count(); i++)
        {
            this->addStruct(
                QString(_deviceInfoList.at(index - 1).structs.at(i).description) +
                QString::asprintf(" - 0x%x", _deviceInfoList.at(index - 1).structs.at(i).structId)
            );
        }
    }
}

void BackEnd::selectStruct(int index)
{
    qDebug() << "structIndex:" << index;

    _selectedStruct = index - 1;

    if ((_selectedDevice >= _deviceInfoList.count()) || (_selectedDevice < 0))
    {
        this->addStructInfo("No selection", "No selection", "No selection");
        this->clearVariableList();
        this->addVarable("No selection", "No selection", "No selection");
    }
    else if ((_selectedStruct >= _deviceInfoList.at(_selectedDevice).structs.count()) || (_selectedStruct < 0))
    {
        this->addStructInfo("No selection", "No selection", "No selection");
        this->clearVariableList();
        this->addVarable("No selection", "No selection", "No selection");
    }
    else
    {
        qDebug() << "var count:" << _deviceInfoList.at(_selectedDevice).structs.at(_selectedStruct).variableCount;
        this->addStructInfo(
                    QString(_deviceInfoList.at(_selectedDevice).structs.at(_selectedStruct).description),
                    QString::asprintf("0x%x", _deviceInfoList.at(_selectedDevice).structs.at(_selectedStruct).structId),
                    QString::number(_deviceInfoList.at(_selectedDevice).structs.at(_selectedStruct).variableCount)
        );

        this->clearVariableList();

        for (int i = 0; i < _deviceInfoList.at(_selectedDevice).structs.at(_selectedStruct).variables.count(); i++)
        {
            this->addVarable(
                        QString(_deviceInfoList.at(_selectedDevice).structs.at(_selectedStruct).variables.at(i).description),
                        QString::number(_deviceInfoList.at(_selectedDevice).structs.at(_selectedStruct).variables.at(i).index),
                        QString(LynxTextList::lynxDataType(_deviceInfoList.at(_selectedDevice).structs.at(_selectedStruct).variables.at(i).dataType))
            );
        }
    }

}

int BackEnd::generateStruct()
{
    qDebug() << "Attempting to add struct with device index:" << _selectedDevice << "and struct index:" <<_selectedStruct;

    if ((_selectedDevice < 0) || (_selectedDevice >= _deviceInfoList.count()))
    {
        qDebug() << "Could not add struct because device index is out of range";
        return -1;
    }
    else if ((_selectedStruct < 0) || (_selectedStruct >= _deviceInfoList.at(_selectedDevice).structs.count()))
    {
        qDebug() << "Could not add struct because struct index is out of range";
        return -1;
    }

    LynxDynamicId temp = _lynx.addStructure(_deviceInfoList.at(_selectedDevice).structs.at(_selectedStruct));

    if (temp.structId == LynxId())
    {
        qDebug() << "Struct could not be added (most likely because a struct wth that name or id already exists)";
        return -1;
    }

    _dynamicIds.append(temp);
    _addedStructs.append();
    _addedStructs.last().deviceIndex = _selectedDevice;
    _addedStructs.last().structIndex = _selectedStruct;

    qDebug() << "Struct was succesfully added, new index:" << (_dynamicIds.count() - 1);

    return (_dynamicIds.count() - 1);
}

void BackEnd::pullStruct()
{
    LynxId tmpId = this->findStructId();

    if (tmpId.structIndex < 0)
    {
        qDebug() << "Could not find datagram. Did you remember to add the struct?";
        return;
    }

    qDebug() << "Pulling datagram";
    _uart.pullDatagram(tmpId);
}

void BackEnd::startPeriodic(unsigned int interval)
{
    LynxId tmpId = this->findStructId();

    if (tmpId.structIndex < 0)
    {
        qDebug() << "Could not find datagram. Did you remember to add the struct?";
        return;
    }

    qDebug() << "Starting periodic transmit at:" << interval << "ms interval";
    _uart.remotePeriodicStart(tmpId, interval);
}

void BackEnd::stopPeriodic()
{
    LynxId tmpId = this->findStructId();

    if (tmpId.structIndex < 0)
    {
        qDebug() << "Could not find datagram. Did you remember to add the struct?";
        return;
    }

    qDebug() << "Stopping periodic transmit";
    _uart.remotePeriodicStop(tmpId);
}

void BackEnd::sendVariable(int variableIndex, double value)
{
    LynxId tmpId = this->findStructId(variableIndex);

    if (tmpId.structIndex < 0)
    {
        qDebug() << "Could not find datagram. Did you remember to add the struct?";
        return;
    }

    qDebug() << QString::asprintf(
                    "Sending value %f to target '%s'",
                    value,
                    _deviceInfoList.at(_selectedDevice).structs.at(_selectedStruct).variables.at(variableIndex).description.toCharArray()
                    );
    _lynx.setValue(value, tmpId);
    _uart.send(tmpId);
}


LynxId BackEnd::findStructId(int variableIndex)
{
    for (int i = 0; i < _addedStructs.count(); i++)
    {
        if ((_addedStructs.at(i).deviceIndex == _selectedDevice) && (_addedStructs.at(i).structIndex == _selectedStruct))
        {
            if (variableIndex < 0)
                return _dynamicIds.at(i).structId;
            else if(variableIndex < _dynamicIds.at(i).variableIds.count())
                return _dynamicIds.at(i).variableIds.at(variableIndex);
            else
                break;
        }
    }

    return LynxId();
}

void BackEnd::fullscreen()
{
    if (_fullscreen)
    {
        _fullscreen = false;
        static_cast<QQuickView *>(parent())->show();
    }
    else
    {
        _fullscreen = true;
        static_cast<QQuickView *>(parent())->showFullScreen();
    }
}
