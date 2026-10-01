# Compte rendu n°3 – Premier test du capteur LSM6DSOX

## Introduction

Ce compte rendu présente le premier test du capteur LSM6DSOX branché au Raspberry Pi Pico 2 W. L'objectif était de vérifier que le capteur fournit des mesures d'accélération cohérentes, avant de l'utiliser pour les essais sur le bras robotisé UR3.

## Déroulé

N'ayant pas trouvé de programme en langage C pour ce capteur, nous avons écrit le nôtre. Au démarrage, il vérifie d'abord que le capteur répond et qu'il s'agit bien d'un LSM6DSOX : le capteur doit renvoyer son code d'identification, `0x6C`.

Le programme règle ensuite le capteur. La plage de mesure est fixée à ±2 g, ce qui suffit largement pour les mouvements humains du quotidien et donne la meilleure précision. La fréquence est fixée à 104 mesures par seconde.

**Pourquoi 104 mesures par seconde ?**

- Les mouvements humains sont lents. Marcher, s'asseoir ou bouger le bras produisent des variations de moins de 20 par seconde (20 Hz).
- Il faut mesurer au moins 2 fois plus vite que le mouvement le plus rapide, sinon on le rate ou on le déforme (c'est le théorème de Shannon). Il faut donc au moins 40 mesures par seconde.
- Le capteur ne propose que certaines vitesses : 12,5 / 26 / 52 / 104 / 208… mesures par seconde. 104 est la première au-dessus de 40 qui laisse une bonne marge. Elle permet aussi de voir les petites vibrations des moteurs pendant le test des perturbations du robot.
- Plus vite serait inutile, et coûterait plus de batterie et de place sur la carte SD.

Chaque mesure est ensuite envoyée à l'ordinateur sous la forme d'une ligne de texte contenant le temps, l'accélération sur les trois axes X, Y et Z, et l'accélération totale.

Pour vérifier les mesures, nous avons utilisé une référence connue : la gravité terrestre, qui vaut 1 g. Un capteur immobile doit mesurer exactement cette valeur, sur l'axe orienté vers le sol. Nous avons donc relevé les mesures avec le capteur posé à plat, puis tourné à 90°.

[📷 Capture d'écran : les mesures avec le capteur à plat]

[📷 Capture d'écran : les mesures avec le capteur tourné à 90°]

## Résultats

| Position du capteur | X (g) | Y (g) | Z (g) | Total (g) |
|---|---|---|---|---|
| À plat (légèrement incliné) | 0,07 | 0,21 | 0,95 | 0,98 |
| Tourné à 90° | −0,01 | −1,05 | −0,09 | 1,05 |

Lorsque le capteur est à plat, la gravité est mesurée presque entièrement sur l'axe Z. Après une rotation de 90°, elle passe sur l'axe Y. Le capteur détecte donc correctement son orientation.

Dans les deux positions, l'accélération totale reste proche de 1 g. Le léger écart observé (0,98 et 1,05) provient d'un petit décalage de fabrication propre à chaque capteur. Il ne gêne pas les essais sur le robot, mais devra être corrigé par un étalonnage avant le calcul du score d'activité.

## Conclusion

Le capteur LSM6DSOX fonctionne et fournit des mesures fiables et cohérentes, à raison d'environ 100 mesures par seconde. La prochaine étape consiste à souder ses headers, puis à réaliser le test sur le bras robotisé UR3, afin de déterminer la distance minimale à respecter entre le capteur et les moteurs du robot.
