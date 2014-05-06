.PHONY: all, hci, main, server

all:
	@echo "coucou"
hci:
	gcc -std=gnu99 -I include/ cfu.c bt_device.c cfuhash.c cfustring.c hci_controler.c hci_socket.c hci_utils.c l2cap_client.c l2cap_server.c l2cap_socket.c list.c hci_test.c -o hci_test -lbluetooth -lpthread
main:
	gcc -std=gnu99 -I include/ cfu.c bt_device.c cfuhash.c cfustring.c hci_controler.c hci_socket.c hci_utils.c l2cap_client.c l2cap_server.c l2cap_socket.c list.c main.c -o main -lbluetooth -lpthread
server:
	gcc -std=gnu99 -I include/ cfu.c bt_device.c cfuhash.c cfustring.c hci_controler.c hci_socket.c hci_utils.c l2cap_client.c l2cap_server.c l2cap_socket.c list.c server.c -o server -lbluetooth -lpthread