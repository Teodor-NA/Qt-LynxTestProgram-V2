#ifndef SCOPESERVER_H
#define SCOPESERVER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QAbstractSeries>
#include "LynxStructure.h"

struct LoggerInfo
{
    int index;      // Index of logger list
    LynxId id;      // Id of logging signal
    QString name;   // Name of signal
    QString unit;   // Optional: signal unit
    QString color;  // Chart color if needed
};

QT_CHARTS_USE_NAMESPACE
class ScopeServer : public QObject
{
    Q_OBJECT

    QList<LoggerInfo> signalInformation;
    QList<QVector<QPointF>> logger;
    bool _seriesCreated;
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
    int _firstIndex;
    int _secondIndex;
    // void createDemo();
    LynxManager * _lynx;

public:
    explicit ScopeServer(LynxManager * lynx, QObject *parent = nullptr);

signals:
    void reScale();
    void refreshChart(); 
    void createSeries();

public slots:
    void resumeRealtime()
    {
        // logger.clear();
        // signalInformation.clear();
        // createDemo();
        emit createSeries();
        // _haltLogging = false;
        _haltChartRefresh = false;
    }
    // Return the number of signals that should be logged
    int getNumberOfSignals() { return signalInformation.count(); }

    // Updates the logger list function
    void newDataRecived();//For demo
    void writeToCSV(const QString  &filepath);
    void readFromCSV(const QString & filepath);
    // Starts the chartview refresh emit
    void resumeChartviewRefresh()
    {
        // if(!_seriesCreated)
        // {
        //     return;
        //     // _seriesCreated = true;
        //     // emit createSeries();
        // }
        _haltChartRefresh = false;
    }

    // Stops the chartview refresh emit and store the current x axis values
    void pauseChartviewRefresh()
    {
        xLastPause=getLastX();
        xFirstPause=getFirstX();
        _haltChartRefresh = true;
    }
    //
    void calcAxisIndex(qint64 msMin,qint64 msMax)
    {
        //int n=0;
        int firstIndex=0,secondIndex=0;
        for (int i=logger[0].count();i>0;i--)
        {
            if(abs(qint64(logger[0].at(i).x())-msMin)<10 && !firstIndex)
            {
                firstIndex=i;
            }
            else if (abs(qint64(logger[0].at(i).x())-msMax)<10 && !secondIndex)
            {
                secondIndex=i;
            }
            if(firstIndex && secondIndex)
                break;
        }
        qDebug()<<"first index found: "<<firstIndex;
        qDebug()<<"second index found: "<<secondIndex;
        _firstIndex = firstIndex;
        _secondIndex = secondIndex;


    }
    // Get frame min max
    void calcMinMaxY();

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
    void update(QAbstractSeries *series,int index);
    // Get signalText
    const QString getSignalText(int index) { return signalInformation.at(index).name; }
    const QString getSignalColor(int index) { return signalInformation.at(index).color; }

    int changePlotItem(int structIndex, int variableIndex, bool checked);
};

#endif // SCOPESERVER_H
