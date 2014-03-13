#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <iostream>
#include "bluetooth.h"

using namespace std;

Bluetooth::Bluetooth()
{
    // Create a discovery agent and connect to its signals
    this->discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    // Start a discovery
    QObject::connect(this->discoveryAgent,SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo &)),this,SLOT(deviceDiscovered(const QBluetoothDeviceInfo &)));
    this->discoveryAgent->start();
}


// In your local slot, read information about the found devices
void Bluetooth::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    cout << "Found new device:" << device.name().toStdString() << '(' << device.address().toString().toStdString() << ')' << endl;
}

Bluetooth::~Bluetooth()
{

}
