#ifndef SCOPESERVER_H
#define SCOPESERVER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
class ScopeServer : public QObject
{
    Q_OBJECT
    struct loggerInfo
    {
        int index;//index of logger list
        QString name;//name of signal
        QString unit;//optional: signal unit
    };

    QList<loggerInfo> signalInformation;
    QList<QVector<QPointF>> logger;

    bool _haltChartRefresh;
    double max;
    double min;
    bool _historicData;
    QTimer* newDataTimer;
public:
    explicit ScopeServer(QObject *parent = nullptr);

signals:
    void reScale();
    void refreshChart();

public slots:
    // Updates the logger list function
    void newDataRecived();//For demo
    int getNumberOfSignals() { return 0; }
    void writeToCSV(const QString & file);
    void readFromCSV(const QString & file);
};

#endif // SCOPESERVER_H
