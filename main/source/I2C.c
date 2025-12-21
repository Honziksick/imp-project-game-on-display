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
 * Description:  I2C master initialization and driver setup for ESP32.         *
 *               Configures SDA/SCL pins, internal pull-ups, clock speed and   *
 *               installs the I2C driver used by peripheral modules (e.g.      *
 *               SSD1306). Designed to be simple and robust for embedded       *
 *               framebuffer applications.                                     *
 *                                                                             *
 ******************************************************************************/
/**
 * @file I2C.c
 * @author Jan Kalina \<xkalinj00>
 * @brief I2C master initialization and driver setup for ESP32.
 */

#include "public/I2C.h"
#include "driver/gpio.h"  // GPIO_PULLUP_ENABLE
#include "driver/i2c.h"   // I2C functions and types
#include "esp_err.h"      // ESP_ERROR_CHECK

void I2C_Init() {
    // Configure I2C
    const i2c_config_t config = {
        .mode = I2C_MODE_MASTER,                // set master mode
        .sda_io_num = I2C_SDA_GPIO,             // set SDA GPIO
        .scl_io_num = I2C_SCL_GPIO,             // set SCL GPIO
        .sda_pullup_en = GPIO_PULLUP_ENABLE,    // enable internal pull-up for SDA
        .scl_pullup_en = GPIO_PULLUP_ENABLE,    // enable internal pull-up for SCL
        .master.clk_speed = I2C_FREQUENCY_HZ    // set 400kHz fast mode
    };

    // Apply configuration and install driver
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, config.mode, 0, 0, 0));
} // I2C_Init

/*** end of file I2C.c ***/
