/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         I2C.c                                                         *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:                                                                *
 *                                                                             *
 ******************************************************************************/
/**
 * @file I2C.c
 * @author Jan Kalina \<xkalinj00>
 * @brief
 */

#include "public/I2C.h"
#include "driver/gpio.h"  // GPIO_PULLUP_ENABLE
#include "driver/i2c.h"   // I2C functions and types
#include "esp_err.h"      // ESP_ERROR_CHECK

void I2C_Init() {
    // Configure I2C master mode
    const i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_GPIO,
        .scl_io_num = I2C_SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,  // Enable internal pull-up
        .scl_pullup_en = GPIO_PULLUP_ENABLE,  // Enable internal pull-up
        .master.clk_speed = I2C_FREQUENCY_HZ       // 400kHz fast mode
    };

    // Apply configuration and install driver
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, config.mode, 0, 0, 0));
}

/*** end of file I2C.c ***/
