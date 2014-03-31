#ifndef __RSSI__
#define __RSSI__

#include <cstdint>

extern "C" {

struct rssi_t {
	bdaddr_t mac;
	int8_t rssi;
};

}

#endif
