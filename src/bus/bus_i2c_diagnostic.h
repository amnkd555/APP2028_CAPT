// Diagnostic du câblage d'un bus I2C (extension de bus_i2c, sans type propre)
#ifndef BUS_I2C_DIAGNOSTIC_H
#define BUS_I2C_DIAGNOSTIC_H

#include <stdint.h>

#include "bus/bus_i2c.h"

// Affiche sur le port série : réponse du composant attendu (fils normaux puis SDA/SCL
// inversés, en I2C logiciel) et liste des adresses qui répondent sur le bus
void bus_i2c_diagnostic_afficher(const bus_i2c_t *bus, uint8_t adresse_attendue);

#endif
