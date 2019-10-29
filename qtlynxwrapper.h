#ifndef QTLYNXWRAPPER_H
#define QTLYNXWRAPPER_H

#include <QObject>
#include "LynxStructure.h"
#include "lynxuartqt.h"

class QtLynxId : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int structIndex READ structIndex WRITE setStructIndex NOTIFY structIndexChanged)
    Q_PROPERTY(int variableIndex READ variableIndex WRITE setVariableIndex NOTIFY variableIndexChanged)

    LynxId _lynxId;

public:
    explicit QtLynxId(QObject *parent = nullptr) : QObject(parent) {}
    QtLynxId(const QtLynxId & other) : QObject(nullptr), _lynxId(other._lynxId) {}
    QtLynxId(const LynxId & other) : QObject(nullptr), _lynxId(other) {}

    const QtLynxId & operator =(const QtLynxId other) { _lynxId = other._lynxId; return *this; }
    const QtLynxId & operator =(const LynxId other) { _lynxId = other; return *this; }

    bool operator ==(const QtLynxId & other) { return (_lynxId == other._lynxId); }
    bool operator !=(const QtLynxId & other) { return (_lynxId != other._lynxId); }

    int structIndex() { return _lynxId.structIndex; }
    int variableIndex() { return _lynxId.variableIndex; }

    void setStructIndex(int index) { if (index == _lynxId.structIndex) return; _lynxId.structIndex = index; emit structIndexChanged(); }
    void setVariableIndex(int index) { if (index == _lynxId.variableIndex) return; _lynxId.variableIndex = index; emit variableIndexChanged(); }

    LynxId & lynxId() { return _lynxId; }
    const LynxId & lynxId() const { return _lynxId; }

signals:
    void structIndexChanged();
    void variableIndexChanged();

public slots:

};


class QtLynxWrapper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool uartConnected MEMBER _uartConnected NOTIFY uartConnectedChanged)

    LynxManager _lynx;
    LynxList<LynxUartQt> _uart;

    LynxInfo _receiveInfo;

    QList<QSerialPortInfo> _portList;
    QSerialPortInfo _selectedPort;

    unsigned long _baudRate = 115200;

    bool _uartConnected = false;

    int _currentPortIndex = -1;

    void setUartConnected(bool state) { if (state == _uartConnected) return; _uartConnected = state; emit uartConnectedChanged(); }

    // Open uart port
    int openUartPort();
    // Close uart port
    void closeUartPort();

public:
    explicit QtLynxWrapper(QObject *parent = nullptr);
    ~QtLynxWrapper();

    LynxManager * lynxPointer() { return &_lynx; }
    LynxUartQt * lynxUartPointer(int index);

    void closeAllPorts();

    enum QtLynxType
    {
        E_NotInit = 0,
        E_Number,
        E_String,
        E_Bool
    };

    Q_ENUM(QtLynxType)

signals:
    void newDataReceived(const QtLynxId * lynxId);
    void newDeviceInfoReceived(const LynxDeviceInfo & deviceInfo);

    void addPort(const QString & port);
    void clearPortList();

    void uartConnectedChanged();

public slots:
    void readData();

    // Returns the description of the struct pointed to by lynxId. If lynxId points to a variable it returns the name of the containing struct
    QString getStructName(const QtLynxId * lynxId);
    // Returns the description of the variable pointed to by lynxId. If lynxId points to a struct an empty string is returned
    QString getVariableName(const QtLynxId * lynxId);

    // Attempts to parse value to an appropriate type and sends it via uart
    void sendVariable(const QtLynxId * lynxId, int deviceIndex, const QString & value);
    // Returns 0 if lynxId points to a string
    double getValueAsNumber(const QtLynxId * lynxId);
    // Returns any value type as a string
    QString getValueAsString(const QtLynxId * lynxId);
    // Returns false if lynxId does not point to a bool
    bool getValueAsBool(const QtLynxId * lynxId);
    // Returns the simplified data type (not init, number, string or bool)
    QtLynxWrapper::QtLynxType getDataType(const QtLynxId * lynxId);

    // Change deviceId on the remote device
    void changeRemoteDeviceId(const QString & deviceId, int deviceIndex);

    // Scan for uart ports
    void scanForUart();
    // Select uart port
    void selectPort(int portIndex);
    // Connect button
    void connectButtonClicked();


};

#endif // QTLYNXWRAPPER_H
