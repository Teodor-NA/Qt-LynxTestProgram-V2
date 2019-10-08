#include "scopeserver.h"
#include <QtCore/QtMath>
#include <QtCore/QRandomGenerator>
ScopeServer::ScopeServer(QObject *parent) : QObject(parent)
{
    _haltChartRefresh = true;

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
void ScopeServer::newDataRecived()
{
    QDateTime momentInTime = QDateTime::currentDateTime();
    if(!_haltChartRefresh)
    {
        logger[0].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10* qSin(momentInTime.toMSecsSinceEpoch()/1000.0*6.0/10.0)+double(1*QRandomGenerator::global()->generateDouble())));
        logger[1].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10*qCos(momentInTime.toMSecsSinceEpoch()/1000.0*6/10)+1*QRandomGenerator::global()->generateDouble()));
        logger[2].append(QPointF(momentInTime.toMSecsSinceEpoch(), 7*qSin(momentInTime.toMSecsSinceEpoch()/1000.0*6/9)+0.2*QRandomGenerator::global()->generateDouble()));
        logger[3].append(QPointF(momentInTime.toMSecsSinceEpoch(), 5*qCos(momentInTime.toMSecsSinceEpoch()/1000.0*6/7)+0.1*QRandomGenerator::global()->generateDouble()));
        logger[4].append(QPointF(momentInTime.toMSecsSinceEpoch(), 4*qSin(qCos(momentInTime.toMSecsSinceEpoch()/1000.0*6/3)+0.4*QRandomGenerator::global()->generateDouble())));
    }

    if(!_haltChartRefresh)
    {
        emit refreshChart();
   }

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
