// LED intégrée : implémentation selon le modèle de carte
#include "signalisation/signalisation_led.h"

#include "pico/stdlib.h"

// Sur Pico W / Pico 2 W, la LED est pilotée par la puce radio CYW43
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

// Sur Pico / Pico 2 sans radio, la LED est sur la broche GP25
#ifndef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN 25
#endif

bool signalisation_led_initialiser(signalisation_led_t *self) {
    self->allumee = false;
#ifdef CYW43_WL_GPIO_LED_PIN
    // Démarre la puce CYW43 (0 = succès)
    return cyw43_arch_init() == 0;
#else
    // Configure GP25 en sortie numérique
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return true;
#endif
}

void signalisation_led_ecrire(signalisation_led_t *self, bool allumee) {
    self->allumee = allumee;
#ifdef CYW43_WL_GPIO_LED_PIN
    // Ordre envoyé à la puce CYW43, qui commande la LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, allumee);
#else
    // Niveau logique écrit directement sur GP25
    gpio_put(PICO_DEFAULT_LED_PIN, allumee);
#endif
}

void signalisation_led_basculer(signalisation_led_t *self) {
    signalisation_led_ecrire(self, !self->allumee);
}

void signalisation_led_clignoter(signalisation_led_t *self, int nombre, uint32_t demi_periode_ms) {
    for (int i = 0; i < nombre; i++) {
        signalisation_led_ecrire(self, true);
        sleep_ms(demi_periode_ms);
        signalisation_led_ecrire(self, false);
        sleep_ms(demi_periode_ms);
    }
}
