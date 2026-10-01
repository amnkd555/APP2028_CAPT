# Index

| Mot | Sens |
|---|---|
| **`.elf`** | Le même programme + des infos pour chercher les bugs. On ne l'envoie pas sur le Pico |
| **`.uf2`** | Le langage machine mis dans un format qu'on peut glisser sur le Pico, comme sur une clé USB |
| **Bauds** | Vitesse du port série (115200). Doit être la même des deux côtés |
| **BOOTSEL** | Bouton qui fait apparaître le Pico comme une clé USB, pour y déposer un `.uf2` |
| **Breadboard** | Plaque d'essai : les trous d'une même ligne sont reliés entre eux |
| **Compiler** | Traduire ton code (texte que toi tu lis) en langage machine (nombres que le Pico lit) |
| **CSV** | Texte en colonnes séparées par des virgules, lisible par Excel ou Python |
| **Dupont** | Fil de prototypage avec embout mâle ou femelle |
| **Faux capteur (mock)** | Imitation du capteur, utilisée par les tests à la place du vrai |
| **Flash** | Mémoire du Pico qui garde le programme même débranché |
| **g** | Unité d'accélération : 1 g = la gravité terrestre |
| **Header** | Barrette de broches à souder sur un module |
| **I2C** | Façon de discuter à deux fils : bleu (SDA, les données) et jaune (SCL, le rythme) |
| **Langage machine** | Le seul langage que le processeur comprend : une suite de nombres = instructions très simples |
| **Module** | Petite carte qui porte une puce et tous ses composants, prête à brancher |
| **Registre** | Petite case mémoire du capteur. On y écrit des réglages, on y lit les mesures |
| **RTC** | Horloge temps réel : garde l'heure même éteinte, grâce à sa pile |
| **STEMMA QT** | Connecteur blanc à 4 fils d'Adafruit, pour relier des modules I2C sans souder |
| **Test** | Petit programme qui vérifie automatiquement un morceau du code |
| **WHO_AM_I** | Registre qui contient toujours `0x6C` : sert à vérifier que c'est bien notre capteur |
