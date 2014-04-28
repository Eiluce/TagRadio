#ifndef BLUETOOTHBOX_H
#define BLUETOOTHBOX_H

#include <QObject>

class BluetoothBox : public QWidget
{
    Q_OBJECT
public:
    explicit BluetoothBox(QWidget *parent = 0);

signals:

public slots:

private:
    QPushButton *p_ok;
    QPushButton *p_quit;
};

#endif // BLUETOOTHBOX_H
