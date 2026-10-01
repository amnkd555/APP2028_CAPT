// Enfant de capteur_mouvement : accéléromètre ST LSM6DSOX sur bus I2C (±2 g, 104 Hz)
#ifndef CAPTEUR_MOUVEMENT_LSM6DSOX_H
#define CAPTEUR_MOUVEMENT_LSM6DSOX_H

#include <stdint.h>

#include "bus/bus_i2c.h"
#include "capteur/capteur_mouvement.h"

// Adresse I2C 7 bits : 0x6A si SDO/SA0 à la masse (défaut breakout Adafruit), 0x6B sinon
#define CAPTEUR_MOUVEMENT_LSM6DSOX_ADRESSE_DEFAUT  0x6A

// Registres utilisés (datasheet ST, section 9)
#define CAPTEUR_MOUVEMENT_LSM6DSOX_REG_WHO_AM_I    0x0F   // identifiant, vaut toujours 0x6C
#define CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL1_XL    0x10   // fréquence + pleine échelle accéléro
#define CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL3_C     0x12   // reset, BDU, auto-incrément
#define CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL9_XL    0x18   // désactivation de l'I3C
#define CAPTEUR_MOUVEMENT_LSM6DSOX_REG_STATUS      0x1E   // bit 0 : nouvelle mesure prête
#define CAPTEUR_MOUVEMENT_LSM6DSOX_REG_OUTX_L_A    0x28   // début des 6 octets X/Y/Z

// Valeurs écrites ou attendues
#define CAPTEUR_MOUVEMENT_LSM6DSOX_WHO_AM_I        0x6C
#define CAPTEUR_MOUVEMENT_LSM6DSOX_CTRL3_RESET     0x01   // SW_RESET
#define CAPTEUR_MOUVEMENT_LSM6DSOX_CTRL3_BDU_INC   0x44   // BDU (bit 6) + IF_INC (bit 2)
#define CAPTEUR_MOUVEMENT_LSM6DSOX_CTRL9_SANS_I3C  0xE2   // défaut 0xE0 + I3C_DISABLE (bit 1)
#define CAPTEUR_MOUVEMENT_LSM6DSOX_CTRL1_104HZ_2G  0x40   // ODR 0100 = 104 Hz, FS 00 = ±2 g
#define CAPTEUR_MOUVEMENT_LSM6DSOX_STATUS_XLDA     0x01

// Sensibilité à ±2 g : 0,061 mg par LSB (datasheet, tableau 2), en g
#define CAPTEUR_MOUVEMENT_LSM6DSOX_G_PAR_LSB       0.000061f

typedef struct {
    capteur_mouvement_t parent;     // toujours en premier
    const bus_i2c_t *bus;     // bus sur lequel le capteur est câblé
    uint8_t adresse;          // adresse I2C du capteur
} capteur_mouvement_lsm6dsox_t;

// Prépare l'objet (aucun accès au bus) ; capteur_initialiser() fait ensuite la configuration
void capteur_mouvement_lsm6dsox_creer(capteur_mouvement_lsm6dsox_t *self, const bus_i2c_t *bus, uint8_t adresse);

// Convertit les deux octets bruts d'un axe (complément à 2) en g
float capteur_mouvement_lsm6dsox_convertir_g(uint8_t octet_bas, uint8_t octet_haut);

#endif
