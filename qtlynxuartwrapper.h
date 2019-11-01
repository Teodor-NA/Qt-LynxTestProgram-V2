#ifndef QTLYNXUARTWRAPPER_H
#define QTLYNXUARTWRAPPER_H

#include <QObject>
#include "LynxStructure.h"
#include "lynxuartqt.h"
#include "qtlynxwrapper.h"

class QtLynxUartWrapper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool uartConnected MEMBER _uartConnected NOTIFY uartConnectedChanged)
    Q_PROPERTY(int portIndex MEMBER _portIndex NOTIFY portIndexChanged)

    LynxManager * _lynx;

    LynxList<LynxUartQt> _uart;

    LynxInfo _receiveInfo;

    QList<QSerialPortInfo> _portList;
    QSerialPortInfo _selectedPort;

    unsigned long _baudRate = 115200;

    bool _uartConnected = false;

    int _portIndex = -1;

    void setUartConnected(bool state) { if (state == _uartConnected) return; _uartConnected = state; emit uartConnectedChanged(); }

    // Open uart port
    int openUartPort();
    // Close uart port
    void closeUartPort();

public:
    explicit QtLynxUartWrapper(LynxManager * lynx, QObject *parent = nullptr);
    ~QtLynxUartWrapper();

    void closeAllPorts();

    void setPortIndex(int index) { if (index == _portIndex) return; _portIndex = index; emit portIndexChanged(); }

signals:
    void newDataReceived(const QtLynxId * lynxId);
    void newDeviceInfoReceived(const LynxDeviceInfo & deviceInfo, int portIndex);

    void addPort(const QString & port);
    void clearPortList();

    void uartConnectedChanged();
    void portIndexChanged();

    void changeLocalDeviceId(char oldId, char newId);

public slots:
    // Checks all open ports for new data
    void readData(int portIndex);
    // Scan for uart ports
    void scanForUart();
    // Select uart port
    void selectPort(int portIndex);
    // Connect button
    void connectButtonClicked();
    // Attempts to parse value to an appropriate type and sends it via uart
    void sendVariable(int portIndex, const QtLynxId * lynxId, const QString & value);
    // Scans all open ports for lynx
    void scan();
    // Pull struct from device
    void pullStruct(int portIndex, const QtLynxId * lynxId);
    // Start perodic transmit of the selected struct
    void startPeriodic(int portIndex, unsigned int interval, const QtLynxId * lynxId); // int structIndex = -1);
    // Stop perodic transmit of the selected struct
    void stopPeriodic(int portIndex, const QtLynxId * lynxId);  // int structIndex = -1);
    // Change deviceId on the remote device
    void changeRemoteDeviceId(int portIndex, const QString & oldDeviceId, const QString & deviceId);

};

#endif // QTLYNXUARTWRAPPER_H
