#ifndef __HCI_UTILS__ 
#define __HCI_UTILS__

#include <bluetooth/bluetooth.h>
#include <stdint.h>

extern char hci_compare_addresses(const bdaddr_t *a1, const bdaddr_t *a2); 

extern void hci_display_LE_supported_states(uint64_t states);

#endif 
