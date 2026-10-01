// Bus I2C simulé : remplace src/bus/bus_i2c.c dans les tests.
// Un seul composant, vu comme 256 registres, répond à bus_i2c_simule_adresse.
#ifndef BUS_I2C_SIMULE_H
#define BUS_I2C_SIMULE_H

#include <stdbool.h>
#include <stdint.h>

#include "bus/bus_i2c.h"

extern uint8_t bus_i2c_simule_registres[256];   // contenu des registres du composant
extern uint8_t bus_i2c_simule_adresse;          // adresse à laquelle il répond
extern bool bus_i2c_simule_muet;                // true = plus aucun acquittement (fil coupé)

// Remet le composant simulé à zéro : registres nuls, adresse 0x6A, bus fonctionnel
void bus_i2c_simule_reinitialiser(void);

#endif
