#include "scopeserver.h"

ScopeServer::ScopeServer(QObject *parent) : QObject(parent)
{
    _haltChartRefresh = false;

    // SETUP LOGGING
    signalInformation.append(loggerInfo{0,"Voltage",""});
    signalInformation.append(loggerInfo{1,"Current",""});
    signalInformation.append(loggerInfo{2,"Power",""});
    signalInformation.append(loggerInfo{3,"Flux",""});
    signalInformation.append(loggerInfo{4,"Pressure",""});

    QVector<QPointF> points;
    points.reserve(20);

    logger.append(points);
    logger.append(points);
    logger.append(points);
    logger.append(points);
    logger.append(points);

    newDataTimer = new QTimer(this);
    connect(newDataTimer, SIGNAL(timeout()), this, SLOT(newDataRecived()),Qt::UniqueConnection);
    newDataTimer->start(10);
}

void ScopeServer::writeToCSV(const QString & file)
{
    // Write file
    qDebug() << "Write to file is not implemented yet: File: "<<file;
}

void ScopeServer::readFromCSV(const QString & file)
{
    // Read file
    qDebug() << "Read from file is not implemented yet. File: "<<file;
}
