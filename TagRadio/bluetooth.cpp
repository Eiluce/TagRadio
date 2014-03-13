#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <iostream>
#include "bluetooth.h"

using namespace std;

Bluetooth::Bluetooth()
{
    // Create a discovery agent and connect to its signals
    cout << "constructeur" << endl;
    this->discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    // Start a discovery
    QObject::connect(this->discoveryAgent,SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo &)),this,SLOT(deviceDiscovered(const QBluetoothDeviceInfo &)));
    this->discoveryAgent->start();
    QList<QBluetoothDeviceInfo> list = discoveryAgent->discoveredDevices();
    if ( !list.isEmpty()) {
        QBluetoothDeviceInfo infos = list.first();
        cout << infos.name().toStdString() << endl;
    }

}

void Bluetooth::startScan()
{
    // Start a discovery
    QObject::connect(this->discoveryAgent,SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo &)),this,SLOT(deviceDiscovered(const QBluetoothDeviceInfo &)));
    this->discoveryAgent->start();


}

// In your local slot, read information about the found devices
void Bluetooth::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    QList<QBluetoothDeviceInfo> list = discoveryAgent->discoveredDevices();
    QBluetoothDeviceInfo infos = list.first();
    cout << infos.name().toStdString() << endl;
    cout << "Found new device:" << device.name().toStdString() << '(' << device.address().toString().toStdString() << ')';
}

Bluetooth::~Bluetooth()
{

}
