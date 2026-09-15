#include <Arduino.h>
#include "config.h"
#include "can_bus.h"

static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
    (gpio_num_t)PIN_CAN_TX,   // TX (not connected)
    (gpio_num_t)PIN_CAN_RX,   // RX
    TWAI_MODE_LISTEN_ONLY    
);
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

bool start_twai(){

    esp_err_t install_err = twai_driver_install(&g_config, &t_config, &f_config);
    if(install_err != ESP_OK){
        Serial.printf("[CAN] Failed to install Driver : %s\n", esp_err_to_name(install_err));
        return false;
    }

    esp_err_t start_err = twai_start();
    if(start_err != ESP_OK){
        Serial.printf("[CAN] Failed to start Driver : %s\n", esp_err_to_name(start_err));
        twai_driver_uninstall();
        return false;
    }
    return true;
}

bool canReceive(twai_message_t& msg){
    if(twai_receive(&msg, pdMS_TO_TICKS(10000)) == ESP_OK){
        Serial.printf("Message received\n");
    } else {
        Serial.printf("Failed to receive message\n");
        return;
    }
}
