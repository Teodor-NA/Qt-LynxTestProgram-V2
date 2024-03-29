#ifndef QTLYNXWRAPPER_H
#define QTLYNXWRAPPER_H

#include <QObject>
#include "LynxStructure.h"
// #include "lynxuartqt.h"

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

    LynxManager _lynx;

public:
    explicit QtLynxWrapper(QObject *parent = nullptr, char deviceId = char(0xff), const LynxString & deviceName = "");
    ~QtLynxWrapper();

    LynxManager * lynxPointer() { return &_lynx; }

    enum QtLynxType
    {
        E_NotInit = 0,
        E_Number,
        E_String,
        E_Bool
    };

    Q_ENUM(QtLynxType)

signals:


public slots:
    // Returns the description of the struct pointed to by lynxId. If lynxId points to a variable it returns the name of the containing struct
    QString getStructName(const QtLynxId * lynxId);
    // Returns the description of the variable pointed to by lynxId. If lynxId points to a struct an empty string is returned
    QString getVariableName(const QtLynxId * lynxId);

    // Returns 0 if lynxId points to a string
    double getValueAsNumber(const QtLynxId * lynxId);
    // Returns any value type as a string
    QString getValueAsString(const QtLynxId * lynxId);
    // Returns false if lynxId does not point to a bool
    bool getValueAsBool(const QtLynxId * lynxId);
    // Returns the simplified data type (not init, number, string or bool)
    QtLynxWrapper::QtLynxType getDataType(const QtLynxId * lynxId);


};

#endif // QTLYNXWRAPPER_H
