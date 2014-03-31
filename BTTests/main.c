 #include "hci_controler.h"

int main(int argc, char **argv)
{

	hci_LE_get_RSSI(NULL, NULL, 100, 0x00, 0x10, 0x10, 0x00, 0x00);
	return 0;
}
