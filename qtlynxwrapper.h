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
    QtLynxId(const LynxId & other) : QObject(nullptr), _lynxId(other) {}

    const LynxId & operator =(const LynxId other) { return(_lynxId = other); }

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

    LynxManager _lynx;
    LynxUartQt _uart;

    LynxInfo _receiveInfo;

public:
    explicit QtLynxWrapper(QObject *parent = nullptr);
    ~QtLynxWrapper() { _uart.close(); }

    LynxManager * lynxPointer() { return &_lynx; }
    LynxUartQt * lynxUartPointer() { return &_uart; }

signals:
    void newDataReceived(const LynxId & lynxId);
    void newDeviceInfoReceived(const LynxDeviceInfo & deviceInfo);

public slots:
    void readData();
    QString getStructName(QtLynxId * lynxId);
    QString getVariableName(QtLynxId * lynxId);
};

#endif // QTLYNXWRAPPER_H
