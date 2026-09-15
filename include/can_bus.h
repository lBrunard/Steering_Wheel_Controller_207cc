#pragma once
#include "driver/twai.h" //https://docs.espressif.com/projects/esp-idf/en/v4.4.7/esp32/api-reference/peripherals/twai.html#twai-protocol-summary

/**
 * @brief   Installe et démarre le driver TWAI en écoute seule.
 *
 * La configuration (broches, débit, filtre) est privée au module can_bus.
 * En cas d'échec, le driver est laissé désinstallé.
 *
 * @return
 *      - true  : driver installé et démarré, les trames peuvent être lues
 *      - false : échec, le détail est affiché sur le port série
 */
bool start_twai();
