#include <cstdlib>
#include <iostream>
#include "uselessObject.h"

using namespace std;

UselessObject::UselessObject() { // Constructeur de base.
	this->nbPattes = 12;
	estOmnivore = false;
	this->monTableau = (uint32_t *)calloc(42, sizeof(uint32_t));
	for (uint8_t i = 0; i < 42; i++) {
		monTableau[i] = 12;
		this->monTableau[i] = monTableau[i]+13;
	}
}

UselessObject::~UselessObject() { // Destructeur.
	free(this->monTableau);
}

uint8_t UselessObject::getNbPattes() const {
	return this->nbPattes;
}

void UselessObject::incrPattes() {
	nbPattes++;
}

void UselessObject::display() const {
	cout << "Je suis un objet inutile avec " << this->getNbPattes() << "  pattes !\n";
	cout << "Voici mon tableau :\n [" << monTableau[0];
	for (uint8_t i = 1; i < 42; i++) {
		cout << "; " << this->monTableau[i];
	}
	cout << "]\n";
}
