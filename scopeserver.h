#ifndef SCOPESERVER_H
#define SCOPESERVER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QAbstractSeries>
#include "LynxStructure.h"
#include "qtlynxwrapper.h"
struct LoggerInfo
{
    int index;      // Index of logger list
    LynxId id;      // Id of logging signal
    QString name;   // Name of signal
    QString unit;   // Optional: signal unit
    QString color;  // Chart color if needed
    bool visibility;// Signal is visible in the chart
    QString structName;
};
struct checkboxList
{
    int index;
    int parentIndex;
    bool parent;
    bool checked;
    int varIndex;
};
struct parentList
{
    int index;
    QString structName;
};

QT_CHARTS_USE_NAMESPACE
class ScopeServer : public QObject
{
    Q_OBJECT
    QList<checkboxList> _checkBoxList;
    QList<parentList> _parentList;
    QList<LoggerInfo> signalInformation;
    QList<QVector<QPointF>> logger;
   // bool _seriesCreated;
    bool _haltChartRefresh;
    double max;
    double min;
    bool _historicData;
    bool _haltLogging;
    QTimer* newDataTimer;
    qint64 xFirstPause;
    qint64 xLastPause;
    double frameMin;
    double frameMax;
    LynxManager * _lynx;

public:
    explicit ScopeServer(LynxManager * lynx, QObject *parent = nullptr);

signals:
    void updateVisibility(int signalIndex);
    void reScale();
    void refreshChart(); 
    void createSeries();

    void setParent(int parentIndex,bool check);
    void updateChild(int childIndex,bool check);

public slots:
    void updateChart();
    bool signalIsVisible(int signalIndex) { return signalInformation.at(signalIndex).visibility;}
    void clearParents()
    {
        _checkBoxList.clear();
        _parentList.clear();
    }
    void updateList(int index,bool parent,bool checked)
    {
        if(parent)
            return;
        _checkBoxList[index].checked = checked;
        signalInformation[_checkBoxList.at(index).varIndex].visibility = checked;
        emit updateVisibility(_checkBoxList.at(index).varIndex);
        qDebug()<<" -List updated, index: "<<index<<" has checkstate: "<<checked;
    }
    void checkIt(int index,bool parent,bool checked);

    void appendList(int index,int parentIndex,bool parent,bool checked,int varIndex)
    {
        _checkBoxList.append(checkboxList{index,parentIndex,parent,checked,varIndex});
        qDebug()<<"appended parent:"<<parent<<" with " <<index<<" with parent index: "<<parentIndex << " while checked is: "<<checked;

    }
    LynxList<LynxId> getIdList();
    int getParentIndex(const QString &structname,int signalIndex);
    QString getStructName(int index)
    {
        if(_historicData)
            return signalInformation.at(index).structName;
        else
            return QString(_lynx->getStructName(signalInformation.at(index).id));

    }
    void resumeRealtime()
    {
        emit createSeries();
        _haltChartRefresh = false;
    }

    // Return the number of signals that should be logged
    int getNumberOfSignals() { return signalInformation.count(); }

    // Updates the logger list function
    void newDataRecived(const QtLynxId *id);

    void writeToCSV(const QString  &filepath);
    void readFromCSV(const QString & filepath);
    // Starts the chartview refresh emit
    void resumeChartviewRefresh() { _haltChartRefresh = false; }

    // Stops the chartview refresh emit and store the current x axis values
    void pauseChartviewRefresh()
    {
        xLastPause=getLastX();
        xFirstPause=getFirstX();
        _haltChartRefresh = true;
    }

    //
    void calcAxisIndex(qint64 msMin, qint64 msMax);

    // Enable the logger append
    void resumeLogging(){_haltLogging = false;_historicData=false;}

    // Disable the logger append
    void pauseLogging(){_haltLogging = true;}

    // Returns the max Y axis
    double getMaxY() { return max; }

    // Returns the min Y axis
    double getMinY() { return min; }

    // Returns the max Y axis
    double getFrameMaxY() { return frameMax; }

    // Returns the min Y axis
    double getFrameMinY() { return frameMin; }

    // Resets the min and max Y axis
    void resetY() { max = 0; min = 0; }

    // Get the first Xaxis data. Dependant on realtime or history logging

    qint64 getFirstX()
    {
        if(_haltChartRefresh && !_historicData)
            return xFirstPause;
        else
            return qint64(logger.at(0).first().x());
    }

    // Get the last Xaxis data. Dependant on realtime or history logging
    qint64 getLastX()
    {
        if(_haltChartRefresh && !_historicData)
            return xLastPause;
        else
            return qint64(logger.at(0).last().x());
    }

    // Update the chart series with new data
    void update(QAbstractSeries *series,int index);

    // Returns the signal name
    const QString getSignalText(int index) { return signalInformation.at(index).name; }

    // Returns the signal color
    const QString getSignalColor(int index) { return signalInformation.at(index).color; }

    int changePlotItem(int structIndex, int variableIndex, bool checked);
};

#endif // SCOPESERVER_H
