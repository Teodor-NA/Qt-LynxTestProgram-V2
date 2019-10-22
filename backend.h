#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QtDebug>
#include <QtQuick/QQuickView>
#include "LynxStructure.h"
#include "lynxuartqt.h"
#include "qtlynxwrapper.h"
// #include "lightcontrol.h"
// #include "teststruct.h"

//class LynxQtId : public QObject
//{
//    Q_OBJECT

//    // Q_PROPERTY(int structIndex MEMBER _structIndex NOTIFY structIndexChanged)
//    // Q_PROPERTY(int variableIndex MEMBER _variableIndex NOTIFY variableIndexChanged)

//    int _structIndex;
//    int _variableIndex;

//public:
//    explicit LynxQtId(QObject * parent = nullptr) :
//        QObject(parent), _structIndex(-1), _variableIndex(-1) {}

//    LynxQtId(LynxId lynxId, QObject * parent = nullptr) :
//        QObject(parent), _structIndex(lynxId.structIndex), _variableIndex(lynxId.variableIndex) {}

//    operator const LynxId() const { return LynxId(_structIndex, _variableIndex); }

//    const LynxQtId & operator = (const LynxId & other)
//    {
//        _structIndex = other.structIndex;
//        _variableIndex = other.variableIndex;

//        return *this;
//    }

//signals:

//public slots:
//};

//struct PlotItem
//{
//    QString name;
//    LynxId id;
//};

class BackEnd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool fullscreen MEMBER _fullscreen NOTIFY fullscreenChanged)

    LynxManager * const _lynx;
    LynxUartQt * const _uart;


    // LynxInfo _receiveInfo;

    QList<QSerialPortInfo> _portList;
    QSerialPortInfo _selectedPort;

    unsigned long _baudrate = 115200;

    LynxList<LynxDeviceInfo> _deviceInfoList;

    int _selectedDevice;
    int _selectedStruct;

    LynxList<LynxDynamicId> _dynamicIds;
    // LynxList<AddedStruct> _addedStructs;

    // LynxList<PlotItem> _plotItems;

    bool _fullscreen;

    // LynxId findStructId(int variableIndex = -1);

public:
    explicit BackEnd(LynxManager * const lynx, LynxUartQt * const uart, QObject *parent = nullptr);
    // ~BackEnd() {}

signals:
    void clearPortList();
    void addPort(const QString & portName);
    void clearDevices();
    void addDevice(const QString & device);
    void addDeviceInfo(const QString & description, const QString & id, const QString & version, const QString & count);
    void clearStructList();
    void addStruct(const QString & structInfo);
    void addStructInfo(const QString & description, const QString & id, const QString & count);
    void addStructIndex(int structIndex);
    void clearVariableList();
    void addVariable(const QString & variableName, int variableIndex, const QString & variableType, const QString & variableValue, bool enableInput, bool checked);
    void changeVariableValue(QtLynxId * lynxId, const QString & value); // (int structIndex, int variableIndex, const QString & value);
    void fullscreenChanged();
    LynxList<LynxId> getIdList();

public slots:
    void scan();
    void refreshPortList();
    void portSelected(int portIndex);
    void connectButtonClicked();
    void selectDevice(int infoIndex);
    void selectStruct(int infoIndex);
    int generateStruct();
    void pullStruct(int structIndex);
    void startPeriodic(unsigned int interval, QtLynxId * lynxId); // int structIndex = -1);
    void stopPeriodic(QtLynxId * lynxId);  // int structIndex = -1);
    void sendVariable(QtLynxId * lynxId, const QString & value);// int structIndex, int variableIndex, const QString & value);
    bool uartConnected() { return _uart->opened(); }
    void fullscreenButtonClicked();
    void newDataReceived(const LynxId & lynxId);
    void newDeviceInfoReceived(const LynxDeviceInfo & deviceInfo);
    // int changePlotItem(int structIndex, int variableIndex, const QString & name, bool checked);
    // void sendData(int structIndex, int variableIndex);

};

#endif // BACKEND_H
