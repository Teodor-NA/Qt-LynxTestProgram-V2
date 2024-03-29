#include "backend.h"

BackEnd::BackEnd(LynxManager * const lynx, QObject *parent) :
    QObject(parent),
    _lynx(lynx)
    // _uart(uart)
{
   //  _uart.open(4, 115200);

    _deviceInfoIndex = -1;
    _structInfoIndex = -1;

    _fullscreen = false;

}

//void BackEnd::scan()
//{
//    qDebug() << "\n------------ Sending Scan -------------\n";
//    _uart->scan();
//    // this->clearDevices();
//    // this->addDevice("desc");
//}

//void BackEnd::newDataReceived(QtLynxId * lynxId)
//{
//    QString value;
//    LynxLib::E_LynxSimplifiedType tempType;

//    // QtLynxId tmpQtId;

//    if (lynxId->lynxId().variableIndex < 0)
//    {
//        int varCount = _lynx->structVariableCount(lynxId->lynxId().structIndex);
//        LynxId tempId = lynxId->lynxId();
//        QtLynxId tempQtId = *lynxId;
//        // All variables
//        for (int i = 0; i < varCount; i++)
//        {
//            tempId.variableIndex = i;
//            tempType = _lynx->simplifiedType(tempId);

//            switch (tempType)
//            {
//            case LynxLib::eString:
//                value = QString(_lynx->getString(tempId));
//                break;
//            case LynxLib::eNumber:
//                value = QString::number(_lynx->getValue(tempId));
//                break;
//            default:
//                value = "Error";
//                break;
//            }

//            // tmpQtId = tempId;

//            this->changeVariableValue(&tempQtId, value); // (tempId.structIndex, tempId.variableIndex, value)
//        }
//    }
//    else
//    {
//        QtLynxId tempQtId = *lynxId;
//        // Single variable
//        tempType = _lynx->simplifiedType(lynxId->lynxId());
//        switch (tempType)
//        {
//        case LynxLib::eString:
//            value = QString(_lynx->getString(lynxId->lynxId()));
//            break;
//        case LynxLib::eNumber:
//            value = QString::number(_lynx->getValue(lynxId->lynxId()));
//            break;
//        default:
//            value = "Error";
//            break;
//        }

//        // tmpQtId = lynxId;

//        this->changeVariableValue(&tempQtId, value); // (lynxId.structIndex, lynxId.variableIndex, value)
//    }


//}

void BackEnd::newDeviceInfoReceived(const LynxDeviceInfo & deviceInfo, int portIndex)
{
    for (int i = 0; i < _deviceInfoList.count(); i++)
    {
        if (_deviceInfoList.at(i).deviceId == deviceInfo.deviceId)
        {
            qDebug() << "The deviceId" << deviceInfo.deviceId << "is already in the list";

            bool diff = false;

            if (_deviceInfoList.at(i).structs.count() != deviceInfo.structs.count())
                diff = true;
            else
            {
                for (int j = 0; j < _deviceInfoList.at(i).structs.count(); j++)
                {
                    if (_deviceInfoList.at(i).structs.at(j).variables.count() != deviceInfo.structs.at(j).variables.count())
                    {
                        diff = true;
                        break;
                    }

                    for (int k = 0; k < _deviceInfoList.at(i).structs.at(j).variables.count(); k++)
                    {
                        if (_deviceInfoList.at(i).structs.at(j).variables.at(k).dataType != deviceInfo.structs.at(j).variables.at(k).dataType)
                        {
                            diff = true;
                            break;
                        }
                    }

                    if (diff)
                        break;
                }
            }


            if (diff)
                qDebug() << "The signatures are different, so there is most likely a deviceId conflict";
            else
                qDebug() << "The signatures are the same, so it is most likely a duplicate scan";

            return;
        }
    }

    _deviceInfoList.append(deviceInfo);

    this->addDevice(
                QString(_deviceInfoList.last().description),
                QString::asprintf("0x%x", (int(_deviceInfoList.last().deviceId) & 0xff)),
                QString(_deviceInfoList.last().lynxVersion),
                QString::number(_deviceInfoList.last().structCount),
                portIndex
    );
}

//void BackEnd::refreshPortList()
//{
//    this->clearPortList();
//    this->addPort("Select port");

//    _portList = QSerialPortInfo::availablePorts();

//    QString tempName;
//    for (int i = 0; i < _portList.count(); i++)
//    {
//        tempName = _portList.at(i).portName();
//        tempName += " - ";
//        tempName += _portList.at(i).description();

//        if (!_portList.at(i).isNull())
//            this->addPort(tempName);
//    }
//}

//void BackEnd::portSelected(int portIndex)
//{
//    qDebug() << "Port selsected";
//    qDebug() << "Index: " << portIndex;
//    if ((portIndex < 0) || (portIndex >= _portList.count()))
//        return;

//    _selectedPort = _portList.at(portIndex);
//}

//void BackEnd::connectButtonClicked()
//{
//    // qDebug() << "Button clicked";
//    if (_uart->opened())
//    {
//        qDebug() << "Closing port";
//        _uart->close();
//    }
//    else
//    {
//        qDebug() << "Attempting to open";
//        qDebug() << _selectedPort.portName();
//        qDebug() << _selectedPort.description();
//        if (_uart->open(_selectedPort, _baudrate))
//            qDebug() << "Opened successfully";
//        else
//            qDebug() << "Open failed";
//    }
//}

void BackEnd::selectDevice(int portIndex) //int infoIndex)
{
    qDebug() << "deviceIndex:" << _deviceInfoIndex;

    if ((_deviceInfoIndex >= _deviceInfoList.count()) || (_deviceInfoIndex < 0))
    {
        qDebug() << "Cannot find device";
    }
    else
    {
        this->clearStructList();
//        this->addStruct("No selection");
        for (int i = 0; i < _deviceInfoList.at(_deviceInfoIndex).structs.count(); i++)
        {
            this->addStruct(
                QString(_deviceInfoList.at(_deviceInfoIndex).structs.at(i).description),
                QString::asprintf("0x%x", _deviceInfoList.at(_deviceInfoIndex).structs.at(i).structId),
                QString::number(_deviceInfoList.at(_deviceInfoIndex).structs.at(i).variableCount),
                portIndex
            );
        }
    }
}

void BackEnd::selectStruct(int portIndex) // int infoIndex)
{
    qDebug() << "structIndex:" << _structInfoIndex;

    if ((_deviceInfoIndex >= _deviceInfoList.count()) || (_deviceInfoIndex < 0))
    {
        qDebug() << "Cannot find device";
    }
    else if ((_structInfoIndex >= _deviceInfoList.at(_deviceInfoIndex).structs.count()) || (_structInfoIndex < 0))
    {
        qDebug() << "Cannot find struct";
    }
    else
    {
        qDebug() << "var count:" << _deviceInfoList.at(_deviceInfoIndex).structs.at(_structInfoIndex).variableCount;

        // Add struct index if it exists
        LynxId tempId;
        for (int i = 0; i < _dynamicIds.count(); i++)
        {
            if (_dynamicIds.at(i).structId == _deviceInfoList.at(_deviceInfoIndex).structs.at(_structInfoIndex).structId)
            {
                qDebug() << "Struct index:" << _dynamicIds.at(i).structLynxId.structIndex << "updated";
                tempId.structIndex = i;
                break;
            }
        }

        this->addStructIndex(tempId.structIndex);

        if (tempId.structIndex < 0)
        {
            qDebug() << "Struct has not been added yet, index was set to -1";
        }

        QString data;

        // Update variable info in gui
        this->clearVariableList();

        for (int i = 0; i < _deviceInfoList.at(_deviceInfoIndex).structs.at(_structInfoIndex).variables.count(); i++)
        {
            if (tempId.structIndex >= 0)
            {
                // Add the value if the struct is added
                tempId.variableIndex = i;
                LynxLib::E_LynxSimplifiedType dataType = _lynx->simplifiedType(tempId);

                switch (dataType)
                {
                case LynxLib::eString:
                    data = QString(_lynx->getString(tempId));
                    break;
                case LynxLib::eNumber:
                    data = QString::number(_lynx->getValue(tempId));
                    break;
                case LynxLib::eBool:
                    data = (_lynx->getBool(tempId) ? "True" : "False");
                    break;
                default:
                    data = "Error";
                    break;
                }
            }
            else
            {
                data = "Not set";
            }

            bool checked = false;

            LynxList<LynxId> idList = this->getIdList();

            for (int j = 0; j < idList.count(); j++)
            {
                if (tempId == idList.at(j))
                {
                    checked = true;
                    break;
                }
            }

            this->addVariable(
                QString(_deviceInfoList.at(_deviceInfoIndex).structs.at(_structInfoIndex).variables.at(i).description),
                i,
                QString(LynxTextList::lynxDataType(_deviceInfoList.at(_deviceInfoIndex).structs.at(_structInfoIndex).variables.at(i).dataType)),
                data,
                (LynxLib::accessMode(_deviceInfoList.at(_deviceInfoIndex).structs.at(_structInfoIndex).variables.at(i).dataType) == LynxLib::eReadWrite),
                checked,
                portIndex
            );
        }
    }
}

int BackEnd::generateStruct()
{
    qDebug() << "Attempting to add struct with device index:" << _deviceInfoIndex << "and struct index:" <<_structInfoIndex;

    if ((_deviceInfoIndex < 0) || (_deviceInfoIndex >= _deviceInfoList.count()))
    {
        qDebug() << "Could not add struct because device index is out of range";
        return -1;
    }
    else if ((_structInfoIndex < 0) || (_structInfoIndex >= _deviceInfoList.at(_deviceInfoIndex).structs.count()))
    {
        qDebug() << "Could not add struct because struct index is out of range";
        return -1;
    }

    LynxDynamicId temp = _lynx->addStructure(_deviceInfoList.at(_deviceInfoIndex).structs.at(_structInfoIndex));

    if (temp.structLynxId == LynxId())
    {
        qDebug() << "Struct could not be added (most likely because a struct wth that name or id already exists)";
        return -1;
    }

    _dynamicIds.append(temp);
//    _addedStructs.append();
//    _addedStructs.last().deviceIndex = _selectedDevice;
//    _addedStructs.last().structIndex = _selectedStruct;

    this->addStructIndex(_dynamicIds.last().structLynxId.structIndex);

    qDebug() << "Struct was succesfully added, new index:" << (_dynamicIds.count() - 1);

    // Pull the datagram to update the values
    // _uart->pullDatagram(_dynamicIds.last().structLynxId);

    return (_dynamicIds.count() - 1);
}



//LynxId BackEnd::findStructId(int variableIndex)
//{
//    for (int i = 0; i < _addedStructs.count(); i++)
//    {
//        if ((_addedStructs.at(i).deviceIndex == _selectedDevice) && (_addedStructs.at(i).structIndex == _selectedStruct))
//        {
//            if (variableIndex < 0)
//                return _dynamicIds.at(i).structId;
//            else if(variableIndex < _dynamicIds.at(i).variableIds.count())
//                return _dynamicIds.at(i).variableIds.at(variableIndex);
//            else
//                break;
//        }
//    }

//    return LynxId();
//}

void BackEnd::fullscreenButtonClicked()
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

    emit fullscreenChanged();
}

//int BackEnd::changePlotItem(int structIndex, int variableIndex, const QString & name, bool checked)
//{

//    LynxId tmpId(structIndex, variableIndex);

//    for (int i = 0; i < _plotItems.count(); i++)
//    {
//        if (tmpId == _plotItems.at(i).id)
//        {
//            if (checked)
//            {
//                qDebug() << "Item:" << name << "was not added since it is already in the list.";
//                return 0;
//            }
//            else
//            {
//                _plotItems.remove(i);
//                qDebug() << "Item:" << name << "was removed from the list.";
//                qDebug() << "Count is now:" << _plotItems.count();
//                return -1;
//            }
//        }
//    }

//    if (!checked)
//    {
//        qDebug() << "Item:" << name << "was not removed, since it was not in the list.";
//        return 0;
//    }

//    _plotItems.append();
//    _plotItems.last().id = tmpId;
//    _plotItems.last().name = name;

//    qDebug() << "Item" << _plotItems.last().name << "with structIndex:" << _plotItems.last().id.structIndex << "and variableIndex:" << _plotItems.last().id.variableIndex << "was added";
//    qDebug() << "Count is now:" << _plotItems.count();

//    return 1;
//}

//void BackEnd::sendData(int structIndex, int variableIndex)
//{
//    LynxId tempId(structIndex, variableIndex);

//    qDebug() << "structIndex:" << tempId.structIndex;
//    qDebug() << "variableIndex:" << tempId.variableIndex;
//}

void BackEnd::changeLocalDeviceId(char oldId, char newId)
{
    for (int i = 0; i < _deviceInfoList.count(); i++)
    {
        if (_deviceInfoList.at(i).deviceId == oldId)
        {
            _deviceInfoList[i].deviceId = newId;
            emit changeDeviceId("0x" + QString::number((int(newId) & 0xff), 16));
            return;
        }
    }

    qDebug() << "A call for a devic id change was made, but the old id could not be found";
    qDebug() << "Old id: 0x" + QString::number((int(oldId) & 0xff), 16) << "New id: 0x" + QString::number((int(newId) & 0xff), 16);
}
