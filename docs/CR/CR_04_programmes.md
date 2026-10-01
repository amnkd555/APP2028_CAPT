# Compte rendu n°4 – Les programmes du Pico et leurs évolutions

## Introduction

Ce compte rendu détaille le code écrit pour le Raspberry Pi Pico 2 W : les programmes successifs, les modifications apportées à chaque problème rencontré, et la réorganisation finale du code. Tous les programmes sont écrits en langage C avec le kit de développement officiel du Pico (SDK).

## Déroulé

### Version 1 – Programme de prise en main

Le premier programme fait clignoter la LED de la carte et envoie un message à l'ordinateur chaque seconde :

```c
while (true) {
    etat_led = !etat_led;                       // inverse l'état de la LED
    gpio_put(PICO_DEFAULT_LED_PIN, etat_led);   // allume ou éteint la broche GP25
    printf("[%lu s] Microcontroleur actif - Etat LED : %s\n", compteur, ...);
    compteur++;
    sleep_ms(1000);                             // pause d'une seconde
}
```

**Modification 1 – LED du Pico 2 W.** La LED ne clignotait pas : sur le Pico 2 W, elle n'est pas reliée à la broche GP25 mais à la puce Wi-Fi. Nous avons ajouté l'initialisation de cette puce et remplacé la commande de la LED :

```c
cyw43_arch_init();                                       // démarre la puce Wi-Fi
cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, etat_led);    // la LED passe par elle
```

Il a aussi fallu ajouter la bibliothèque correspondante (`pico_cyw43_arch_none`) dans le fichier de compilation `CMakeLists.txt`, et compiler pour la carte `pico2_w`.

**Modification 2 – Message de démarrage perdu.** Le programme attendait 3 secondes puis affichait un message de démarrage, qui partait avant que le moniteur série de l'ordinateur soit ouvert. Nous avons remplacé cette attente fixe par une détection de l'ouverture du moniteur : le message est renvoyé à chaque nouvelle connexion.

```c
if (stdio_usb_connected() && !moniteur_connecte) {   // le moniteur vient de s'ouvrir
    printf("Projet Anti-Sedentarite : Pico demarre !\n");
}
moniteur_connecte = stdio_usb_connected();            // mémorise l'état pour le tour suivant
```

### Version 2 – Lecture du capteur LSM6DSOX

Ce second programme, `mesure_capteur`, lit le capteur par le bus I2C. Il démarre d'abord le contrôleur I2C du Pico sur les broches GP4 (données) et GP5 (horloge) :

```c
i2c_init(i2c0, 400000);                          // bus I2C à 400 kHz
gpio_set_function(4, GPIO_FUNC_I2C);             // GP4 = SDA
gpio_set_function(5, GPIO_FUNC_I2C);             // GP5 = SCL
```

Il vérifie ensuite l'identité du capteur en lisant son registre `WHO_AM_I`, qui doit contenir `0x6C`, puis le configure en écrivant dans trois registres :

| Registre | Valeur écrite | Effet |
|---|---|---|
| CTRL3_C | 0x44 | Les octets d'une même mesure restent cohérents ; lecture des 6 octets d'un coup |
| CTRL9_XL | 0xE2 | Désactive le mode I3C, pour rester en I2C |
| CTRL1_XL | 0x40 | 104 mesures par seconde, plage ±2 g |

Dans sa boucle, le programme attend qu'une nouvelle mesure soit prête, lit les 6 octets (2 par axe) et les convertit en g :

```c
int16_t x_brut = (int16_t)((octet_haut << 8) | octet_bas);   // assemble les 2 octets
float ax = x_brut * 0.000061f;                                // 0,061 mg par unité (±2 g)
printf("%lu,%.5f,%.5f,%.5f,%.5f\n", temps_ms, ax, ay, az, total);
```

Chaque ligne envoyée à l'ordinateur suit le format `temps, ax, ay, az, total`, lisible directement par un tableur ou un script Python.

### Version 3 – Fiabilité de la communication

**Modification 3 – Erreurs en boucle.** Lorsque le capteur cessait de répondre, le programme affichait des centaines de milliers de messages d'erreur sans jamais repartir. Nous avons ajouté un compteur d'erreurs consécutives : au bout de 10 erreurs, le programme débloque le bus I2C (il envoie 9 impulsions d'horloge pour libérer le capteur) puis réinitialise le capteur. Une pause de 1 ms après chaque erreur limite le nombre de messages.

```c
if (erreurs_consecutives >= 10) {
    printf("# bus I2C bloque : deblocage et reinitialisation\n");
    i2c_debloquer();            // libère le bus
    capteur_pret = false;       // le capteur sera reconfiguré
}
```

**Modification 4 – Diagnostic du câblage.** Pour trouver l'origine des pannes, le programme affiche désormais, à chaque échec, un diagnostic : il teste si le capteur répond avec les fils SDA et SCL dans le bon ordre, puis inversés, et liste toutes les adresses qui répondent sur le bus.

**Modification 5 – Test retiré.** Une première version du diagnostic indiquait si chaque fil était relié, mais elle donnait de faux résultats : un défaut connu de la puce RP2350 (référencé « erratum E9 ») fausse ce type de mesure. Ce test a été supprimé.

### Version 4 – Réorganisation du code

Le programme `mesure_capteur` était devenu un seul long fichier, avec une fonction principale de 119 lignes. Nous l'avons réorganisé selon des règles fixées pour le projet : un dossier par fonctionnalité, un fichier par élément, et 30 lignes maximum par fonction.

| Dossier | Rôle |
|---|---|
| `src/bus/` | Communication I2C, déblocage et diagnostic |
| `src/capteur/` | Interface générale d'un capteur, puis le pilote du LSM6DSOX |
| `src/communication/` | Envoi des mesures à l'ordinateur |
| `src/signalisation/` | Commande de la LED |
| `src/acquisition/` | Boucle de mesure |
| `src/programmes/` | Les deux programmes (prise en main et mesure) |

Le comportement du Pico est resté identique, ce que nous avons vérifié sur la carte. Des tests automatiques ont également été ajoutés : ils vérifient sur l'ordinateur, sans le Pico, la configuration du capteur, la conversion des mesures en g et le format des lignes envoyées. Ils se lancent à chaque modification du code.

## Résultats

| Version | Ce que fait le programme | Vérifié sur la carte |
|---|---|---|
| 1 | LED qui clignote, messages chaque seconde | ✅ |
| 2 | Lecture du capteur, 104 mesures par seconde | ✅ |
| 3 | Reprise automatique et diagnostic en cas de panne | ✅ |
| 4 | Code réorganisé et testé automatiquement | ✅ |

## Conclusion

Le code a évolué d'un simple programme de test vers un programme de mesure fiable, capable de signaler et de corriger lui-même les problèmes de connexion. Sa réorganisation en modules facilitera l'ajout des prochains éléments : horloge, carte SD, calcul du score MET et moteur vibrant.
