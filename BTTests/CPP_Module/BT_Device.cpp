#include <iostream>
#include "BT_Device.h"

using namespace std;

uint8_t BT_Device::numDevices = 0;

BT_Device::BT_Device() {
	BT_Device::incrNumDevices();
	realName = "UNKNOWN";
	customName = "UNKNOWN";
	bdaddr_t = 0x000000000000;
}

BT_Device::BT_Device(string realName, bdaddr_t mac) {
	BT_Device::incrNumDevices();
	realName = realName;
	customName = realName;
	bdaddr_t = mac;
}

BT_Device::BT_Device(string realName, string customName, bdaddr_t mac) {
	BT_Device::incrNumDevices();
	realName = realName;
	customName = customName;
	bdaddr_t = mac;
}

string BT_Device::getRealName() const {
	return this->realName;
}

string BT_Device::getCustomName() const {
	return this->customName;
}

string BT_Device::getMac() const {
	return this->mac;
}

string BT_Device::getNumDevices() const {
	return BT_Device::numDevices;
}

void BT_Device::incrNumDevices() {
	BT_Device::numDevices++;
}
	
