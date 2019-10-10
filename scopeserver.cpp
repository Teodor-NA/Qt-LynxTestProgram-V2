#include "scopeserver.h"
#include <QtCore/QtMath>
#include <QtCore/QRandomGenerator>

#include <QtCharts/QXYSeries>
#include <QtCharts/QAreaSeries>
ScopeServer::ScopeServer(QObject *parent) : QObject(parent)
{
    _haltChartRefresh = true;
    _seriesCreated = false;
    _historicData = false;
    // SETUP LOGGING
    signalInformation.append(loggerInfo{0,"Voltage","",""});
    signalInformation.append(loggerInfo{1,"Current","",""});
    signalInformation.append(loggerInfo{2,"Power","",""});
    signalInformation.append(loggerInfo{3,"Flux","",""});
    signalInformation.append(loggerInfo{4,"Pressure","",""});

    QVector<QPointF> points;
    points.reserve(2000);

    logger.append(points);
    logger.append(points);
    logger.append(points);
    logger.append(points);
    logger.append(points);

    newDataTimer = new QTimer(this);
    connect(newDataTimer, SIGNAL(timeout()), this, SLOT(newDataRecived()),Qt::UniqueConnection);
    newDataTimer->start(10);
    QList<int> l {2,34,5,2,1,-1,4,3,5,12,22,123,-244,41,2,3,1,152,515};
    auto mm = std::minmax_element(l.begin(), l.end());
    qDebug() << "min: "<<*mm.first <<" max " << *mm.second;
}
void ScopeServer::newDataRecived()
{
    QDateTime momentInTime = QDateTime::currentDateTime();
    if(!_haltLogging)
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
void ScopeServer::calcMinMaxY(int time)
{
    double min_=0;
    double max_=0;

    for (int i=0;i<logger.count();i++)
    {
        double delta = logger[0].at(logger[0].count()-1).x()-logger[0].at(logger[0].count()-2).x();
        QList<double> liste;
        liste.reserve(int(time/delta));
        for (int n=1;n<int(time/delta);n++)
        {
            liste.append(logger[0].at(logger[0].count()-n).y());

        }
        auto mm = std::minmax_element(liste.begin(), liste.end());
        if(*mm.first < min_)
            min_ = *mm.first;
        if(*mm.second > max_)
            max_ = *mm.second;
    }

    //qDebug() << "min: "<<*mm.first <<" max " << *mm.second;
    frameMin = min_;
    frameMax = max_;
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
void ScopeServer::update(QAbstractSeries *series,int index)
{

    if (series)
    {
        QXYSeries *xySeries = static_cast<QXYSeries *>(series);
        QVector<QPointF> points = logger.at(index);
        if(points.last().y()>max)
        {
            max=points.last().y();
        }
        if(points.last().y()<min)
        {
            min=points.last().y();

        }
        // Use replace instead of clear + append, it's optimized for performance
        xySeries->replace(points);


    }
}
