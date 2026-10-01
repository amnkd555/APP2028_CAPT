// Implémentation simulée des fonctions du SDK Pico utilisées par les modules testés
#include "pico/stdlib.h"

bool pico_simule_usb_connecte = false;

void sleep_ms(uint32_t ms) {
    (void)ms;
}

void sleep_us(uint64_t us) {
    (void)us;
}

bool stdio_usb_connected(void) {
    return pico_simule_usb_connecte;
}
