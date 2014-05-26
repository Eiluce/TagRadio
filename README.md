TagRadio
========

Localisation of tag by bluetooth.
Created by ENSIMAG FabLab Team : BERTAULD Thomas, BIANCHERI Cédric, BRELOT Lucie, FONT Ludovic.

Requirements 
------------

The library bluez : 5.2

GCC compiler



Presentation of the repository
------------------------------

* The huge majority of the documentation we used throughout the developement of our project can
  be found in the "Doc" directory.
  
* All of the sources of our software are located in the "Application" directory.

* The visualisation programm's sources can be found in the "Visualiseur" directory while the
  executable is located in the "Visualiseur_release" directory.

How to run the software
-----------------------

At first, you need to compile the files on the four Raspberries : one of them should run either
"calibration.c" or "main.c" and the other should run one of the servers. 

Compilation commands :

* make calibration compile the calibration application.

* make main compile the main application (the one which deals with the RSSI measures for locating 
  the sensor).
  
* make server compile the three needed servers. Each server should be launched before running either the
  "main" or "calibration" application.


