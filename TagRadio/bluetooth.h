#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QObject>

class Bluetooth : public QObject
{
    Q_OBJECT
public:
    Bluetooth();
    void startScan();
    ~Bluetooth();
private :
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
public slots :
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
};



#endif // BLUETOOTH_H
