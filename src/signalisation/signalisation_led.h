// LED intégrée de la carte (GP25 sur Pico / Pico 2, puce CYW43 sur Pico W / Pico 2 W)
#ifndef SIGNALISATION_LED_H
#define SIGNALISATION_LED_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool allumee;   // état actuel de la LED
} signalisation_led_t;

// Prépare la LED (démarre la puce CYW43 si besoin) ; false si la puce ne répond pas
bool signalisation_led_initialiser(signalisation_led_t *self);

// Allume ou éteint la LED
void signalisation_led_ecrire(signalisation_led_t *self, bool allumee);

// Inverse l'état de la LED
void signalisation_led_basculer(signalisation_led_t *self);

// Clignote « nombre » fois (allumée puis éteinte, demi_periode_ms chacune) ; finit éteinte
void signalisation_led_clignoter(signalisation_led_t *self, int nombre, uint32_t demi_periode_ms);

#endif
