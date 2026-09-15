#pragma once

// Broches des optocoupleurs (LED IR via 220 Ω)
#define PIN_VOL_PLUS 25
#define PIN_VOL_MINUS 26

// Broches du transceiver CAN.
// TX n'est pas relié à l'ESP32 : le D du module est câblé au 3V3 (écoute seule garantie).
#define PIN_CAN_TX 4
#define PIN_CAN_RX 5

#define MASK_STEERING_WHEEL 0x21F
