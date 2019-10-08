#include "scopeserver.h"

ScopeServer::ScopeServer(QObject *parent) : QObject(parent)
{

}

void ScopeServer::writeToCSV(const QString & file)
{
    // Write file
    qDebug() << "Write to file is not implemented yet";
}

void ScopeServer::readFromCSV(const QString & file)
{
    // Read file
    qDebug() << "Read from file is not implemented yet";
}
