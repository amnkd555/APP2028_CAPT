# Un module = un dossier de src/ = une bibliothèque INTERFACE (convention du SDK Pico) :
# ses sources sont compilées dans chaque programme qui la lie.
function(module nom)
    cmake_parse_arguments(MODULE "" "" "SOURCES;DEPENDANCES" ${ARGN})
    list(TRANSFORM MODULE_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
    add_library(${nom} INTERFACE)
    target_sources(${nom} INTERFACE ${MODULE_SOURCES})
    # Les en-têtes s'incluent par « dossier/module.h » depuis src/
    target_include_directories(${nom} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${nom} INTERFACE ${MODULE_DEPENDANCES})
endfunction()

module(signalisation
    SOURCES      signalisation/signalisation_led.c
    DEPENDANCES  pico_stdlib)
if (PICO_CYW43_SUPPORTED)
    # Pico W : la LED est commandée par la puce radio CYW43 (sans pile réseau)
    target_link_libraries(signalisation INTERFACE pico_cyw43_arch_none)
endif()

module(bus
    SOURCES      bus/bus_i2c.c bus/bus_i2c_diagnostic.c
    DEPENDANCES  pico_stdlib hardware_i2c)

module(capteur
    SOURCES      capteur/capteur.c capteur/capteur_mouvement.c capteur/capteur_mouvement_lsm6dsox.c
    DEPENDANCES  bus)

module(communication
    SOURCES      communication/communication_moniteur.c communication/communication_moniteur_csv.c
    DEPENDANCES  capteur)

module(acquisition
    SOURCES      acquisition/acquisition.c
    DEPENDANCES  bus capteur communication signalisation)
