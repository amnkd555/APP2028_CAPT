cmake_minimum_required(VERSION 3.13)

# Carte par défaut : Pico 2 W (à fixer avant d'inclure le SDK)
set(PICO_BOARD pico2_w CACHE STRING "Type de carte Pico")
include(cmake/pico_sdk_import.cmake)

project(anti_sedentarite C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

# Modules (une bibliothèque par dossier de src/)
add_subdirectory(src)

# Programmes : un .uf2 chacun dans build/, printf envoyé sur l'USB (pas sur l'UART GP0/GP1)
add_executable(prise_en_main src/programmes/prise_en_main.c)
target_link_libraries(prise_en_main signalisation communication)

add_executable(mesure_capteur src/programmes/mesure_capteur.c)
target_link_libraries(mesure_capteur acquisition)

foreach(programme prise_en_main mesure_capteur)
    pico_enable_stdio_usb(${programme} 1)
    pico_enable_stdio_uart(${programme} 0)
    pico_add_extra_outputs(${programme})
endforeach()
