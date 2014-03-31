#ifndef __BT_CONTROLER__
#define __BT_CONTROLER__

#ifdef __cplusplus
extern "C" {
#endif
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#ifdef __cplusplus
}
#endif

#include <string>
#include <cstdint>
#include <list>

#include "rssi.h"
#include "BT_Device.h"

class BT_Controler : public BT_Device { 

 private:
	int8_t devId;
	std::list<int8_t> socketList;

	bool leAble; 

	
 public:
	BT_Controler();
	BT_Controler(int8_t devId);

	~BT_Controler(); //Destructeur.

	int8_t getDevId() const;
	std::list<int8_t> getSocketList() const;

	bool isLEAble() const;

	int8_t openSocket();
	int8_t closeSocket(int8_t socket);

	std::list<BT_Device> runScan();
	std::list<BT_Device> runLEScan();

	std::list<struct rssi_t> runRSSIScan();
	std::list<struct rssi_t> runLERSSIScan();
	
	int8_t getRSSI(bdaddr_t destMac);
	int8_t getLERSSI(bdaddr_t destMac);

	virtual void displayInfo() const; 
};

#endif
