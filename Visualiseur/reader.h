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
    void stepDate(QPushButton *button);

public slots:
    void doWork();

signals:
    void finished();
    void workRequested();
    void sendPoint(int posX, int posY, double prob);
    void sendReset();
    void sendDate(QString date);

private:
    QString filename;
    bool _working;
    bool _abort;
    bool _pause;
    bool _step;
    bool _connected;
    QPushButton *pauseButton;
    QMutex mutex;
    void doWorkOffline();
    void doWorkOnline();
};

#endif // Reader_H
