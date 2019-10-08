#ifndef SCOPESERVER_H
#define SCOPESERVER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QList>
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

    bool _haltChartRefresh;
    double max;
    double min;
    bool _historicData;
    bool _haltLogging;
    QTimer* newDataTimer;
    qint64 xFirstPause;
    qint64 xLastPause;
public:
    explicit ScopeServer(QObject *parent = nullptr);

signals:
    void reScale();
    void refreshChart(); 
    void createSeries();

public slots:
    // Updates the logger list function
    void newDataRecived();//For demo
    int getNumberOfSignals() { return 0; }
    void writeToCSV(const QString & file);
    void readFromCSV(const QString & file);
    // Starts the chartview refresh emit
    void resumeChartviewRefresh(){ _haltChartRefresh = false; }

    // Stops the chartview refresh emit and store the current x axis values
    void pauseChartviewRefresh()
    {
        xLastPause=getLastX();
        xFirstPause=getFirstX();
        _haltChartRefresh = true;
    }
    // Enable the logger append
    void resumeLogging(){_haltLogging = false;_historicData=false;}

    // Disable the logger append
    void pauseLogging(){_haltLogging = true;}

    // Returns the max Y axis
    double getMaxY() { return max; }

    // Returns the min Y axis
    double getMinY() { return min; }

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
};

#endif // SCOPESERVER_H
