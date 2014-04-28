This document explains how to set up Bluetooth LE on a Raspberry Pi with Raspbian

First, Install Raspbian
-------------------------
Get the rasbian image, then use image writer to pre-installed it on the sd card.

As the ssh daemon is always running on raspbian, you can install it without keyboard or screen.
You can find the IP of Raspberry with nmap and then connect on ssh :

	nmap -sP 192.168.1.0/24

	ssh pi@192.168.1.(the number you get with nmap)

and then configure your raspberry :

	sudo raspi-config

Then, reboot it :

	sudo reboot

Get bluetooth
---------------
At first, get all the packages

	sudo apt-get install bluetooth bluez bluez-utils blueman

	sudo apt-get install libusb-dev libdbus-1-dev libglib2.0-dev

Plug the dongle, write the file :
Not sure it's really usefull but, in case :

	sudo nano /etc/udev/rules.d/10-bluetooth.rules :

		SUBSYSTEM=="usb", ATTRS{idVendor}=="050d", ATTRS{idProduct}=="065a", RUN+="/bin/sh -c 'echo 050d 065a > /sys/bus/usb/drivers/btusb/new_id'"

and then run the command :

	sudo modprobe btusb

Write directely in the file, /sys/bus/usb/drivers/btusb/new_id : 050d 065a

Write the script
-----------------
Write a bash script containing the following command :

	modprobe btusb

	echo "050d" "065a" >> /sys/bus/usb/drivers/btusb/new_id

Place it in /etc/init.d/ in order to launch when the Pi boots
Allow the scripts to be start with root rights :

	sudo update-rc.d btdev.sh defaults

Check if all has been installed
-------------------------------
Check if the dongle is recognized :

	hcitool dev

If you have a BT LE device, try to discover it :

	hcitool lescan

Access to the Raspberry without local network
---------------------------------------------
On the Raspberry, change the /etc/network/interfaces file adding :

	iface eth0 inet static
	address 192.168.1.21
	netmask 255.255.255.0
	network 192.168.1.0
	gateway 192.168.1.22 

Then on your computer, use the following command :

	sudo ifconfig eth0 192.168.1.22

Then, try to ping the Pi :

	ping -c 3 192.168.1.21

If you received packets, it's Ok, you can acces with SSH :

	ssh pi@192.168.1.21