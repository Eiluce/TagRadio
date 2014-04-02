#ifndef __USELESSOBJECT__
#define __USELESSOBJECT__

#include <cstdint>

class UselessObject { 

 private:

	uint8_t nbPattes;
	bool estOmnivore;
	uint32_t *monTableau;

	uint8_t getNbPattes() const;

 public:
	UselessObject(); // Constructeur de base.

	~UselessObject(); //Destructeur.

	void incrPattes();

	void display() const; //const indique que cette méthode ne modifie aucun attribut.
};

#endif
