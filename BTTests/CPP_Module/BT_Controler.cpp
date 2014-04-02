#include <iostream>
#include "BT_Controler.h"

using namespace std;

BT_Controler::BT_Controler() : 
	BT_Device() 
{
	this->devId = -1;
	leAble = false;
}

BT_Controler::BT_Controler(int8_t devId) : 
	BT_Device() 
{
	this->devId = devId;
	leAble = false;
}

int8_t BT_Controler::getDevId() const {
	return this->devId;
}


list<int8_t> BT_Controler::getSocketList() const {
	return this->socketList;
}

bool BT_Controler::isLEAble() const {
	return this->leAble;
}

int8_t BT_Controler::openSocket() {
	this->sockeList.push_front(-1);
	return -1;
}

int8_t BT_Controler::closeSocket(int8_t socket) {
	this->sockeList.remove(-1);
	return -1;
}

list<BT_Device> BT_Controler::runScan() {
	list<BT_Device> res;
	return res;
}

list<BT_Device> BT_Controler::runLEScan() {
	list<BT_Device> res;
	return res;
}

list<struct rssi_t> BT_Controler::runRSSIScan() {
	list<struct rssi_t> res;
	return res;
}

list<struct rssi_t> BT_Controler::runLERSSIScan() {
	list<struct rssi_t> res;
	return res;
}
	
int8_t BT_Controler::getRSSI(bdaddr_t destMac) {
	return -1;
}

int8_t BT_Controler::getLERSSI(bdaddr_t destMac) {
	return -1;
}

virtual void BT_Controler::displayInfo() const {
	return;
}
