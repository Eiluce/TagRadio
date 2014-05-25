#ifndef __HCI_UTILS__ 
#define __HCI_UTILS__

/* Package "hci_utils".
 * Description : this package gives some useful functions to deal with
 an hci_controller's intern information.
 * Author : FabLab ENSIMAG team : BERTAULD Thomas, BIANCHERI Cedric, BRELOT Lucie, FONT Ludovic.
 * Last modification : 23/05/2014.
 */

#include <bluetooth/bluetooth.h>
#include <stdint.h>

/* Function "hci_display_LE_supported_states" :
   Displays on the standard output the supported states of an
   hci_controller. The parameter "states" is a boolean filter
   giving the supported states of a controller.
   This filter has to have previously been retrieved from a real
   hci_controller by using the function "hci_LE_read_supported_states" in the
   "hci_controller" package.
*/
extern void hci_display_LE_supported_states(uint64_t states);

#endif 
