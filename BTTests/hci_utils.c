#include "hci_utils.h"

#include <string.h>
 
char hci_compare_addresses(const bdaddr_t *a1, const bdaddr_t *a2) {
	return (memcmp((const void *)a1, (const void *)a2, sizeof(bdaddr_t)) == 0);
}
