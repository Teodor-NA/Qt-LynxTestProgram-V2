#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QtDebug>
#include <QtQuick/QQuickView>
#include "lynxstructure.h"
#include "lynxuartqt.h"
// #include "lightcontrol.h"
// #include "teststruct.h"

struct AddedStruct
{
    int deviceIndex;
    int structIndex;
};

class BackEnd : public QObject
{
    Q_OBJECT

    LynxManager _lynx;
    LynxUartQt _uart;

    LynxInfo _receiveInfo;

    QList<QSerialPortInfo> _portList;
    QSerialPortInfo _selectedPort;

    unsigned long _baudrate = 115200;

    LynxList<LynxDeviceInfo> _deviceInfoList;

    int _selectedDevice;
    int _selectedStruct;

    LynxList<LynxDynamicId> _dynamicIds;
    LynxList<AddedStruct> _addedStructs;

    bool _fullscreen;

    LynxId findStructId(int variableIndex = -1);

public:
    explicit BackEnd(QObject *parent = nullptr);
    ~BackEnd() { _uart.close(); }

signals:
    void clearPortList();
    void addPort(const QString & portName);
    void clearDevices();
    void addDevice(const QString & device);
    void addDeviceInfo(const QString & description, const QString & id, const QString & version, const QString & count);
    void clearStructList();
    void addStruct(const QString & structInfo);
    void addStructInfo(const QString & description, const QString & id, const QString & count);
    void clearVariableList();
    void addVarable(const QString & description, const QString & index, const QString & type);
    void addVariableValue(int variableIndex, double value);

public slots:
    void scan();
    void readData();
    void refreshPortList();
    void portSelected(int index);
    void connectButtonClicked();
    void selectDevice(int index);
    void selectStruct(int index);
    int generateStruct();
    void pullStruct();
    void startPeriodic(unsigned int interval);
    void stopPeriodic();
    void sendVariable(int variableIndex, double value);
    void fullscreen();
    bool uartConnected() { return _uart.opened(); }
};

#endif // BACKEND_H
