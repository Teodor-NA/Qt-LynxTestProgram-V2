#include "scopeserver.h"
#include <QtCore/QtMath>
#include <QtCore/QRandomGenerator>

#include <QtCharts/QXYSeries>
#include <QtCharts/QAreaSeries>
ScopeServer::ScopeServer(LynxManager * lynx, QObject *parent) : QObject(parent), _lynx(lynx)
{
    _haltChartRefresh = true;
    _seriesCreated = false;
    _historicData = false;

    // createDemo();
    _haltLogging = true;

    newDataTimer = new QTimer(this);
    connect(newDataTimer, SIGNAL(timeout()), this, SLOT(newDataRecived()),Qt::UniqueConnection);
    newDataTimer->start(10);

}
//void ScopeServer::createDemo()
//{
//    // SETUP LOGGING
//    signalInformation.append(loggerInfo{0,"Voltage","","red"});
//    signalInformation.append(loggerInfo{1,"Current","","blue"});
//    signalInformation.append(loggerInfo{2,"Power","","red"});
//    signalInformation.append(loggerInfo{3,"Flux","","orange"});
//    signalInformation.append(loggerInfo{4,"Pressure","","black"});

//    QVector<QPointF> points;
//    points.reserve(100000);

//    logger.append(points);
//    logger.append(points);
//    logger.append(points);
//    logger.append(points);
//    logger.append(points);
//}
void ScopeServer::newDataRecived()
{
    if (signalInformation.count() < 1)
        return;

    QDateTime momentInTime = QDateTime::currentDateTime();

    for (int i = 0; i < signalInformation.count(); i++)
    {
        logger[i].append(QPointF(momentInTime.toMSecsSinceEpoch(), _lynx->getValue(signalInformation.at(i).id)));
    }

//    if(!_haltLogging)
//    {
//        logger[0].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10* qSin(momentInTime.toMSecsSinceEpoch()/1000.0*6.0/10.0)+double(1*QRandomGenerator::global()->generateDouble())));
//        logger[1].append(QPointF(momentInTime.toMSecsSinceEpoch(), 10*qCos(momentInTime.toMSecsSinceEpoch()/1000.0*6/10)+1*QRandomGenerator::global()->generateDouble()));
//        logger[2].append(QPointF(momentInTime.toMSecsSinceEpoch(), 7*qSin(momentInTime.toMSecsSinceEpoch()/1000.0*6/9)+0.2*QRandomGenerator::global()->generateDouble()));
//        logger[3].append(QPointF(momentInTime.toMSecsSinceEpoch(), 5*qCos(momentInTime.toMSecsSinceEpoch()/1000.0*6/7)+0.1*QRandomGenerator::global()->generateDouble()));
//        logger[4].append(QPointF(momentInTime.toMSecsSinceEpoch(), 4*qSin(qCos(momentInTime.toMSecsSinceEpoch()/1000.0*6/3)+0.4*QRandomGenerator::global()->generateDouble())));
//    }

    if(!_haltChartRefresh)
    {
        emit refreshChart();
    }

}
void ScopeServer::calcMinMaxY()
{
    double min_=0;
    double max_=0;

    for (int i=0;i<logger.count();i++)
    {
        //double delta = logger[0].at(logger[i].count()-1).x()-logger[i].at(logger[i].count()-2).x();
        QList<double> liste;
        liste.reserve(abs(_secondIndex-_firstIndex));
        for (int n=_firstIndex;n<_secondIndex;n++)
        {
            liste.append(logger[i].at(n).y());

        }
        auto mm = std::minmax_element(liste.begin(), liste.end());
        qDebug()<<"min: "<< *mm.first<<"Max: "<<*mm.second;
        if(*mm.first < min_)
            min_ = *mm.first;
        if(*mm.second > max_)
            max_ = *mm.second;
    }

    //qDebug() << "min: "<<*mm.first <<" max " << *mm.second;
    frameMin = min_;
    frameMax = max_;
}

void ScopeServer::writeToCSV(const QString &filepath)
{
    // Open csv-file
    pauseLogging();

    std::string x = filepath.toStdString().substr(filepath.toStdString().find("///") + 3);

    QFile file(QString::fromUtf8(x.c_str()));

    //QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    //QDir dir;
    //QFile file(path + "\\file.csv");
    file.remove();
    if ( !file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        qDebug() << file.errorString();
        return ;
    }

    // Write data to file
    QTextStream stream(&file);
    QString separator(",");
    stream << QString("Time");
    for (int n = 0; n < logger.size(); ++n)
    {
        stream<<separator + getSignalText(n);
    }
    stream << separator<<endl;

    for (int i = 0; i < logger.at(0).size()-1; ++i)
    {
        stream << QDateTime::fromMSecsSinceEpoch(qint64(logger.at(0).at(i).x())).toString(Qt::ISODateWithMs);
        for (int n = 0; n < logger.size(); ++n)
        {
            stream << separator << QString::number(logger.at(n).at(i).y());
        }
        stream << endl;
    }
    stream.flush();
    file.close();
    resumeLogging();
}

void ScopeServer::readFromCSV(const QString & filepath)
{
    // Open csv-file
    pauseLogging();
    pauseChartviewRefresh();
    //QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    //QDir dir;
    //QFile file(path + "\\file.csv");
    std::string x =filepath.toStdString().substr(filepath.toStdString().find("///") + 3);

    QFile file(QString::fromUtf8(x.c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
                    qDebug() << file.errorString();
                    return ;
                }
    else {
        qDebug()<<"File read";
    }
    QStringList wordList;
    logger.clear();
    int colums = file.readLine().split(',').count()-1;
    qDebug()<<"Colums: "<<colums;
    QVector<QPointF> points;

    for (int c=0;c<colums-1;c++)
    {
        logger.append(points);
    }
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        QByteArray xpoint = line.split(',') .first();
        QDateTime Date = QDateTime::fromString(xpoint,Qt::ISODateWithMs);
        //qDebug()<<line;
        for (int n=0;n<colums-1;n++)
        {
            logger[n].append(QPointF(qint64(Date.toMSecsSinceEpoch()),line.split(',').at(n+1).toDouble()));
        }
    }

    _historicData=true;
    frameMin = 0;
    frameMax = 0;
    _firstIndex=0;
    _secondIndex=0;
    emit createSeries();
    emit refreshChart();
    emit reScale();
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

int ScopeServer::changePlotItem(int structIndex, int variableIndex, bool checked)
{

    LynxId tmpId(structIndex, variableIndex);

    QString name = QString(_lynx->getVariableName(tmpId));

    for (int i = 0; i < signalInformation.count(); i++)
    {
        if (tmpId == signalInformation.at(i).id)
        {
            if (checked)
            {
                qDebug() << "Item:" << name << "was not added since it is already in the list.";
                return 0;
            }
            else
            {
                signalInformation.removeAt(i);
                logger.removeAt(i);
                qDebug() << "Item:" << name << "was removed from the list.";
                qDebug() << "Count is now:" << signalInformation.count();
                emit createSeries();
                return -1;
            }
        }
    }

    if (!checked)
    {
        qDebug() << "Item:" << name << "was not removed, since it was not in the list.";
        return 0;
    }

    signalInformation.append(LoggerInfo{signalInformation.count(), tmpId, name, "", ""});

    QVector<QPointF> points;
    points.reserve(100000);

    logger.append(points);

    // signalInformation.last().id = tmpId;
    // signalInformation.last().name = name;

    qDebug() << "Item" << signalInformation.last().name << "with structIndex:" << signalInformation.last().id.structIndex << "and variableIndex:" << signalInformation.last().id.variableIndex << "was added";
    qDebug() << "Count is now:" << signalInformation.count();

    emit createSeries();

    return 1;
}
