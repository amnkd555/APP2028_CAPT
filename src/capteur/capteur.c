// Classe parent des capteurs : redirige chaque appel vers l'implémentation de l'enfant
#include "capteur/capteur.h"

bool capteur_initialiser(capteur_t *self) {
    return self->operations->initialiser(self);
}

capteur_lecture_t capteur_lire(capteur_t *self) {
    return self->operations->lire(self);
}
