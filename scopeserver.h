#ifndef SCOPESERVER_H
#define SCOPESERVER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QAbstractSeries>

QT_CHARTS_USE_NAMESPACE
class ScopeServer : public QObject
{
    Q_OBJECT
    struct loggerInfo
    {
        int index;//index of logger list
        QString name;//name of signal
        QString unit;//optional: signal unit
        QString color;//chart color if needed
    };

    QList<loggerInfo> signalInformation;
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
public:
    explicit ScopeServer(QObject *parent = nullptr);

signals:
    void reScale();
    void refreshChart(); 
    void createSeries();

public slots:
    // Return the number of signals that should be logged
    int getNumberOfSignals() { return signalInformation.count(); }

    // Updates the logger list function
    void newDataRecived();//For demo
    void writeToCSV(const QString & file);
    void readFromCSV(const QString & file);
    // Starts the chartview refresh emit
    void resumeChartviewRefresh()
    {
        if(!_seriesCreated)
        {
            _seriesCreated=true;
            emit createSeries();
        }
        _haltChartRefresh = false;
    }

    // Stops the chartview refresh emit and store the current x axis values
    void pauseChartviewRefresh()
    {
        xLastPause=getLastX();
        xFirstPause=getFirstX();
        _haltChartRefresh = true;
    }

    // Get frame min max
    void calcMinMaxY(int time);

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
};

#endif // SCOPESERVER_H
