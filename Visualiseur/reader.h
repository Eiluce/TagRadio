#ifndef READER_H
#define READER_H

#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QFile>
#include <QTime>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QMessageBox>
#include <QPushButton>
#include <QThread>

#include <iostream>
#include <windows.h>

class Reader : public QObject
{
    Q_OBJECT
public:
    explicit Reader(QObject *parent = 0);
    void requestWork(bool connected);
    void requestWorkConnect();
    void abort();
    void pauseWork(QPushButton *button);
    void setFilename(QString file);

public slots:
    void doWork();

signals:
    void finished();
    void workRequested();
    void sendPoint(int posX, int posY, double prob);
    void sendReset();

private:
    QString filename;
    bool _working;
    bool _abort;
    bool _pause;
    bool _connected;
    QMutex mutex;
    void doWorkOffline();
    void doWorkOnline();
};

#endif // Reader_H
