#pragma once
#include "driver/twai.h" //https://docs.espressif.com/projects/esp-idf/en/v4.4.7/esp32/api-reference/peripherals/twai.html#twai-protocol-summary

/**
 * @brief   Install and start TWAI driver on listen Only mode.
 *  If fails, driver still uninstalled
 *
 * @return
 *      - true  : Driver Installed
 *      - false : Installation Failed
 */
bool start_twai();


bool canReceive(twai_message_t& msg);
