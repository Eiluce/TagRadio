#ifndef __BT_DEVICE__
#define __BT_DEVICE__

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

class BT_Device { 
	
 private:
	std::string realName;
	std::string customName;
	bdaddr_t mac;
	
	static uint8_t numDevices;

	static void incrNumDevices();
 public:
	BT_Device(); 
 
	BT_Device(std::string realName, bdaddr_t mac);
	BT_Device(std::string realName, std::string customName, bdaddr_t mac);

	~BT_Device();
	
	std::string getRealName() const;
	std::string getCustomName() const;
	bdaddr_t getMac() const;

	static uint8_t getNumDevices();

	virtual void displayInfo() const =0; 
};

#endif
