FabDevice

Présentation
------------

Le "FabDevice" est un appareil qui vous permet de déterminer votre position dans une pièce.

Contenu du "FabDevice"
--------------------------
4 Raspberry Pis

4 chargeurs and 4 batteries pour les RPi

1 Sensor Tag

4 carte SD

4 Bluetooth dongles

Installer toutes les Raspberrys
--------------------------

Mettez une raspberry à chaque coin de votre salle à manger.

Accédez à la RPi principale, spécifié avec le numéro ou client en utilisant un clavier et un écran. 

Si vous n'avez aucun de ces périphérique, vous pouvez accéder à la RPi par SSH. 

Accédez par SSH
---------------

Branchez un cable réseau entre la RPi and votre ordinateur (Linux).

Sur votre ordinateur, dans un terminal, tapez cette commande :

	sudo ifconfig eth0 192.168.1.22

Ensuite, accédez à la RPi par SSH

	ssh pi@192.168.1.44 (client)

Le mot de passe est rapsberry.

La calibration
------------------------

Vous devez imaginer une grille par dessus votre pièce. L'idéal est d'imaginer un découpage par des carreaux de 1m sur 1m.

Vous devez entrer les dimensions de la grille dans le fichier FINAL/include/simulation_data.h

Lancez le programme de calibration
-------------------------------
-------------------------------

Sur la RPi principale, allez dans le répertoire appelé FINAL.

Tapez :

	sudo ./calibration <name-file>

Le programme de calibration
------------------------
------------------------

Vous devez être deux pour réaliser la calibration.

L'un des deux se place à la position (0,0), à définir par vous-même.

L'autre personne est à l'ordinateur entre 0 lorsque que la personnne est positionnée.

Le programme va ensuite demander d'autres positions/

La prise de mesure
------------------

Quand la calibration est terminée, vous devez lancer le programme principal.

	sudo ./main <name-file> (the same name used for the calibration)

Récupérer les données
------------------

Vous pouvez récupérer les données quand vous voulez sur le client. 

En premier, mettez le programme en pause :

	CTRL-Z

Récupérer les données :

	scp pi@192.168.1.44:~/FINAL/<name-file> path

Relancer le programme :

	fg

Voir les données
----------------

Lancez le visualiseur.

Choisissez le fichier du données.

Observez toutes les positions. 


