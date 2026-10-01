// Bus I2C matériel du RP2350 : démarrage, accès aux registres d'un composant, déblocage
#ifndef BUS_I2C_H
#define BUS_I2C_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "hardware/i2c.h"

// Description d'un bus I2C : contrôleur utilisé, broches et réglages
typedef struct {
    i2c_inst_t *port;          // contrôleur matériel (i2c0 ou i2c1)
    uint broche_sda;           // GPIO de données (SDA)
    uint broche_scl;           // GPIO d'horloge (SCL)
    uint32_t frequence_hz;     // vitesse d'horloge (400 kHz = mode « Fast »)
    uint32_t delai_max_us;     // durée max d'une transaction avant abandon
} bus_i2c_t;

// Connecte les broches au contrôleur I2C et démarre l'horloge du bus
void bus_i2c_demarrer(const bus_i2c_t *bus);

// Écrit un octet dans un registre du composant ; true si le composant a acquitté
bool bus_i2c_ecrire_registre(const bus_i2c_t *bus, uint8_t adresse, uint8_t registre, uint8_t valeur);

// Lit n registres consécutifs à partir de « registre » ; true si la lecture a réussi
bool bus_i2c_lire_registres(const bus_i2c_t *bus, uint8_t adresse, uint8_t registre,
                            uint8_t *destination, size_t n);

// Teste si un composant acquitte l'adresse donnée
bool bus_i2c_sonder(const bus_i2c_t *bus, uint8_t adresse);

// Libère un bus bloqué (composant qui maintient SDA à 0) puis redémarre le contrôleur
void bus_i2c_debloquer(const bus_i2c_t *bus);

#endif
