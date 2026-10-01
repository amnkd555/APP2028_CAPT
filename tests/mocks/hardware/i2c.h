// Simulation minimale de « hardware/i2c.h » : seul le type du contrôleur est nécessaire,
// les accès au bus sont remplacés par bus_i2c_simule.c
#ifndef MOCK_HARDWARE_I2C_H
#define MOCK_HARDWARE_I2C_H

#include "pico/stdlib.h"

typedef struct i2c_inst i2c_inst_t;

#endif
