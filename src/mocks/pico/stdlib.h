// Simulation minimale de « pico/stdlib.h » pour compiler les modules sur le Mac
#ifndef MOCK_PICO_STDLIB_H
#define MOCK_PICO_STDLIB_H

#include <stdbool.h>
#include <stdint.h>

typedef unsigned int uint;

// Temporisations : sans effet sur le Mac (voir pico_simule.c)
void sleep_ms(uint32_t ms);
void sleep_us(uint64_t us);

// État du port USB, piloté par les tests via pico_simule_usb_connecte
bool stdio_usb_connected(void);
extern bool pico_simule_usb_connecte;

#endif
