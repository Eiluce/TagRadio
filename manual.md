FabDevice

Presentation
------------

The FabDevice is a device which allows you to determine your location in a room. 

Content of the "FabDevice"
--------------------------
4 Raspberry Pis

4 chargers and 4 batteries for the RPi

1 Sensor Tag

4 SD cards

4 Bluetooth dongles

Install all the raspberrys
--------------------------

Put the raspberrys on each corner of your dining room.

Access to the main RPi, specified with the number 4 or client using a keyboard and a screen. If you have neither of these, access it with SSH.

Access with ssh
---------------

Plug a network cable in the RPi and in your computer (Linux).

On your computer, on a terminal, type this command :

	sudo ifconfig eth0 192.168.1.22

Then, acces with ssh :

	ssh pi@192.168.1.44 (client)

The password is raspberry.

The calibration
------------------------

You have to imagine a grid on your room. 

You have to enter the dimensions of the grid in the file FINAL/include/simulation_data.h 

Launch the calibration programm
-------------------------------
-------------------------------

On the RPi, go to the directory named FINAL.

Type :

	sudo ./calibration <name-file>

The calibration programm
------------------------
------------------------

You have to be two to do the calibration.

Someone must go on the position (0,0) defined by yourself.

The other personn then enters 0 when it's ok. 

The programm then will ask for others position.

The measures
------------

When the calibration is done, you have to launch the main programm.

	sudo ./main <name-file> (the same name used for the calibration)

Retrieve the data
------------------

You can retrieve the data anytime you want on the client. 

First, pause the programm :

	CTRL-Z

Get the data :

	scp pi@192.168.1.44:~/FINAL/<name-file> path

See the data
------------

Launch the viewer.

Select the programm.

Observe all your positions. 


