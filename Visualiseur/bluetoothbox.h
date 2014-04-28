#ifndef BLUETOOTHBOX_H
#define BLUETOOTHBOX_H

#include <QWidget>
#include <QPushButton>

#include <reader.h>

class BluetoothBox : public QWidget
{
    Q_OBJECT
public:
    explicit BluetoothBox(Reader *read,QWidget *parent = 0);

signals:

public slots:

private:
    QPushButton *p_ok;
    QPushButton *p_quit;
    QPushButton *p_startProgram;

    Reader *reader;

};

#endif // BLUETOOTHBOX_H
