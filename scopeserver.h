#ifndef SCOPESERVER_H
#define SCOPESERVER_H

#include <QObject>
#include <QDebug>

class ScopeServer : public QObject
{
    Q_OBJECT
public:
    explicit ScopeServer(QObject *parent = nullptr);

signals:
    void reScale();
    void refreshChart();

public slots:
    int getNumberOfSignals() { return 0; }
    void writeToCSV(const QString & file);
    void readFromCSV(const QString & file);
};

#endif // SCOPESERVER_H
