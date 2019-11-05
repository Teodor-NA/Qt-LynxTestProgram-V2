#include "scopeserver.h"
#include <QtCore/QtMath>
#include <QtCore/QRandomGenerator>

#include <QtCharts/QXYSeries>
#include <QtCharts/QAreaSeries>
ScopeServer::ScopeServer(LynxManager * lynx, QObject *parent) : QObject(parent), _lynx(lynx)
{
    _haltChartRefresh = true;
    //_seriesCreated = false;
    _historicData = false;

    // createDemo();
    _haltLogging = true;

    newDataTimer = new QTimer(this);
    connect(newDataTimer, SIGNAL(timeout()), this, SLOT(updateChart()),Qt::UniqueConnection);
    newDataTimer->start(10);

}
void ScopeServer::updateChart()
{
    if(!_haltChartRefresh)
    {
        emit refreshChart();
    }
}
void ScopeServer::newDataRecived(const QtLynxId *id)
{
    if (signalInformation.count() < 1)
        return;

    QDateTime momentInTime = QDateTime::currentDateTime();
    LynxId tempId;
    for (int j = 0; j < signalInformation.count(); j++)
    {
        tempId = signalInformation.at(j).id;

        if(id->lynxId().structIndex == tempId.structIndex)
        {
            if(id->lynxId().variableIndex < 0 || id->lynxId().variableIndex == tempId.variableIndex)
            {
                logger[j].append(QPointF(momentInTime.toMSecsSinceEpoch(), _lynx->getValue(tempId)));
            }
        }
    }
}

void ScopeServer::calcAxisIndex(qint64 msMin, qint64 msMax)
{
    qDebug() << "msMin:" << msMin;
    qDebug() << "msMax:" << msMax;

    if (logger.count() < 1)
    {
        qDebug() << "Nothing to scale.";
        return;
    }


    double maxValue; //= logger.at(0).at(0).y();
    double minValue; //= logger.at(0).at(0).y();
    bool signalExists=false;
    for (int i=0;i<logger.count();i++)
    {
        if(signalInformation.at(i).visibility)
        {
            maxValue = logger.at(i).at(0).y();
            minValue = logger.at(i).at(0).y();
            signalExists=true;
        }

    }
    if (!signalExists)
    {
        qDebug() << "A signal exists, but there is nothing in it.";
        return;
    }
    int startIndex, endIndex;

    // qint64 closestStart, closestEnd;

    for (int i = 0; i < logger.count(); i++)
    {
        if(!signalInformation.at(i).visibility)
            continue;
        startIndex = 0;
        endIndex = logger.at(i).count();

        for (int j = (logger.at(i).count() - 1); j >= 0; j--)
        {
            if (qint64(logger.at(i).at(j).x()) <= msMax)
            {
                endIndex = j;
                break;
            }
        }

        for (int j = endIndex; j >= 0; j--)
        {
            if (qint64(logger.at(i).at(j).x()) <= msMin)
            {
                startIndex = j;
                break;
            }
        }

        qDebug() << "Start index: " << startIndex;
        qDebug() << "End index: " << endIndex;
        qDebug() << "Value at start index: " << logger.at(i).at(startIndex).x();
        qDebug() << "Value at end index: " << logger.at(i).at(endIndex).x();

        for (int j = startIndex; j <= endIndex; j++)
        {
            if (logger.at(i).at(j).y() < minValue)
                minValue = logger.at(i).at(j).y();

            if (logger.at(i).at(j).y() > maxValue)
                maxValue = logger.at(i).at(j).y();
        }
    }

    qDebug() << "Max value: " << maxValue;
    qDebug() << "Min value: " << minValue;

    if (minValue == maxValue)
    {
        qDebug() << "Min and max is the same, adding 0.5 as margin.";
        minValue -= 0.5;
        maxValue += 0.5;
    }

    frameMin = minValue;
    frameMax = maxValue;

}

void ScopeServer::writeToCSV(const QString &filepath)
{
    // Open csv-file
    pauseLogging();

    std::string x = filepath.toStdString().substr(filepath.toStdString().find("///") + 3);

    QFile file(QString::fromUtf8(x.c_str()));


    file.remove();
    if ( !file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        qDebug() << file.errorString();
        return ;
    }
    qDebug()<<"File created or it exits";
    // Write data to file
    QTextStream stream(&file);
    QString separator(",");

    for (int n = 0; n < logger.count(); n++)
    {
        stream << QString("Time") + separator;
        stream<< getStructName(n) + ":" + getSignalText(n)  + separator;
        qDebug()<<"signal name: "<<getSignalText(n);
    }
    stream << endl;
    int maxCount=0;
    for (int j=0;j<logger.count();j++)
    {
        qDebug()<<"signal name: "<<getSignalText(j) <<" has count "<<logger.at(j).count();
        if (logger.at(j).count() > maxCount)
        {

            maxCount = logger.at(j).count();
        }

    }

    for (int i = 0; i < maxCount; i++)//Line number
    {

        for (int n = 0; n < logger.count(); n++)//Colum numer
        {
            if(i<logger.at(n).count() )
            {
                stream << QDateTime::fromMSecsSinceEpoch(qint64(logger.at(n).at(i).x())).toString(Qt::ISODateWithMs) + separator;
                stream <<  QString::number(logger.at(n).at(i).y()) + separator;
            }
            else {
                stream << separator << separator;
            }
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
    //pauseChartviewRefresh();
    std::string x =filepath.toStdString().substr(filepath.toStdString().find("///") + 3);

    QFile file(QString::fromUtf8(x.c_str()));
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << file.errorString();
        return ;
    }
    else {
        qDebug()<<"File read sucessfully";
    }
    QByteArray firstline = file.readLine();
    int colums = firstline.split(',').count()-1;

    qDebug()<<"First line: "<<firstline;
    qDebug()<<"Colums: "<<colums;
    int nSignals = colums /2;
    qDebug()<<"Signals: "<<nSignals;
    logger.clear();


    QStringList wordList;
    QVector<QPointF> points;
    signalInformation.clear();
    LynxId tmpId;
    for (int c=0;c<nSignals;c++)
    {

        logger.append(points);
        QString structAndSignalName =firstline.split(',').at(2*c+1);
//        qDebug()<<structAndSignalName.split(":").first();
//        qDebug()<<structAndSignalName.split(":").last();
        signalInformation.append(LoggerInfo{signalInformation.count(), tmpId, structAndSignalName.split(":").last(), "", "",true,structAndSignalName.split(":").first()});
    }

    while (!file.atEnd())
    {
        QByteArray line = file.readLine();



//        qDebug()<<line;
        for (int n=0;n<colums/2;n++)
        {

            QByteArray xpoint = line.split(',').at(2*n);
            if(xpoint != "")
            {
                QDateTime Date = QDateTime::fromString(xpoint,Qt::ISODateWithMs);

                logger[n].append(QPointF(qint64(Date.toMSecsSinceEpoch()),line.split(',').at(2*n+1).toDouble()));

            }

        }

    }
    for (int i=0;i<logger.count();i++)
    {
        qDebug()<<"logger at "<<i<<" is " <<logger.at(i).count();

    }
    _historicData=true;
//    frameMin = 0;
//    frameMax = 0;
    pauseChartviewRefresh();
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

    if (_lynx->outOfBounds(tmpId))
    {
        qDebug() << "Struct id is out of bounds. Most likely because the struct has not been added.";
        return 0;
    }

    QString name = QString(_lynx->getVariableName(tmpId));
    QString stuctName = QString(_lynx->getStructName(tmpId));

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

    signalInformation.append(LoggerInfo{signalInformation.count(), tmpId, name, "", "",true,stuctName});

    QVector<QPointF> points;
    points.reserve(100000);

    logger.append(points);

    // signalInformation.last().id = tmpId;
    // signalInformation.last().name = name;

//    qDebug() << "Item" << signalInformation.last().name << "with structIndex:" << signalInformation.last().id.structIndex << "and variableIndex:" << signalInformation.last().id.variableIndex << "was added";
//    qDebug() << "Count is now:" << signalInformation.count();

    emit createSeries();

    return 1;
}


LynxList<LynxId> ScopeServer::getIdList()
{
    LynxList<LynxId> temp(signalInformation.count());

    for (int i = 0; i < signalInformation.count(); i++)
    {
        temp.append(signalInformation.at(i).id);
    }

    return temp;
}

int ScopeServer::getParentIndex(const QString &structname, int signalIndex)
{
    int i;
    //Returns index of parent for checkboxes. If negative value it means that there is a new parent.
    for (i=0;i<_parentList.count();i++)
    {
        if (_parentList.at(i).structName==structname)
        {
            //qDebug()<<"returning index: "<<i<< " which corr. to the structname: "<<structname;
            return _parentList.at(i).index;
        }

    }
    //qDebug()<<"appending new index: "<<signalIndex<< " which corr. to the structname: "<<structname;

    _parentList.append(parentList{signalIndex,structname});
    return (_parentList.last().index);
}
void ScopeServer::checkIt(int index,bool parent,bool checked)
{
    _checkBoxList[index].checked = checked;
    //qDebug()<<"function checkIt()";
    int nTrue = 0;
    int nFalse = 0;
//    qDebug()<<"index is: "<<index<<" and is parentIndex is: "<<_checkBoxList.at(index).parentIndex << " while checked is: "<<checked;
//     qDebug()<<"";
    //qDebug()<<"count is: "<<_checkBoxList.count();
    if(parent)
    {
        //qDebug()<<"click is a parent and checked is:"<<checked;
        for (int n=0;n<_checkBoxList.count();n++)
        {
            if(_checkBoxList.at(n).parentIndex==_checkBoxList.at(index).parentIndex)
            {
                //qDebug()<<"setting child index: "<<n <<"with checked"<<checked;
                emit updateChild(n,checked);
                emit updateChild(n,!checked);
                emit updateChild(n,checked);
            }
        }
    }
    else //child
    {
        for (int i=0;i<_checkBoxList.count();i++)
        {

            if(_checkBoxList.at(i).parentIndex == _checkBoxList.at(index).parentIndex && i!=_checkBoxList.at(index).parentIndex)
            {
                _checkBoxList.at(i).checked ? nTrue++ : nFalse++;
            }
        }

        if(nTrue==0)
             emit setParent(_checkBoxList.at(index).parentIndex,false);
        else
            emit setParent(_checkBoxList.at(index).parentIndex,true);

    }
}
