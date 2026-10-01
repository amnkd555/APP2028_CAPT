# Spécifie la version minimale de CMake requise pour compiler
cmake_minimum_required(VERSION 3.13)

# Carte par défaut : Pico 2 W (RP2350 + puce radio CYW43) ; à fixer AVANT d'inclure le SDK
set(PICO_BOARD pico2_w CACHE STRING "Type de carte Pico")

# Inclut le fichier d'initialisation officiel du SDK Raspberry Pi Pico
include(pico_sdk_import.cmake)

# Définit le nom du projet et les langages utilisés (C, C++, Assembleur)
project(anti_sedentarite C CXX ASM)

# Fixe le standard du compilateur C à la norme C11
set(CMAKE_C_STANDARD 11)

# Fixe le standard du compilateur C++ à la norme C++17
set(CMAKE_CXX_STANDARD 17)

# Initialise l'ensemble des cibles matérielles et bibliothèques du SDK Pico
pico_sdk_init()

# Étape 2 : prise en main (LED + printf)
add_executable(prise_en_main
    src/programmes/prise_en_main.c
    src/signalisation/signalisation_led.c
    src/communication/communication_moniteur.c
)
target_link_libraries(prise_en_main
    pico_stdlib
)

# Phase 3 : acquisition LSM6DSOX en I2C, sortie CSV
add_executable(mesure_capteur
    src/programmes/mesure_capteur.c
    src/acquisition/acquisition.c
    src/bus/bus_i2c.c
    src/bus/bus_i2c_diagnostic.c
    src/capteur/capteur.c
    src/capteur/capteur_mouvement.c
    src/capteur/capteur_mouvement_lsm6dsox.c
    src/communication/communication_moniteur.c
    src/communication/communication_moniteur_csv.c
    src/signalisation/signalisation_led.c
)
# hardware_i2c : pilote du contrôleur I2C du RP2350
target_link_libraries(mesure_capteur
    pico_stdlib
    hardware_i2c
)

# Réglages communs aux deux programmes
foreach(cible prise_en_main mesure_capteur)
    # Les modules s'incluent par « dossier/module.h » depuis src/
    target_include_directories(${cible} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

    # Sur Pico W, la LED est pilotée par la puce CYW43 : lie sa bibliothèque (sans pile réseau)
    if (PICO_CYW43_SUPPORTED)
        target_link_libraries(${cible} pico_cyw43_arch_none)
    endif()

    # Redirige les flux printf directement vers le port micro-USB de la carte
    pico_enable_stdio_usb(${cible} 1)

    # Désactive la sortie série sur les broches physiques UART (GP0 et GP1)
    pico_enable_stdio_uart(${cible} 0)

    # Génère automatiquement le fichier binaire au format .uf2 prêt à être flashé
    pico_add_extra_outputs(${cible})
endforeach()
