/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         I2C.h                                                         *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Public API for I2C peripheral initialization on ESP32.        *
 *               Provides a single function to configure the I2C master bus    *
 *               with GPIO pin assignments, clock speed, and internal pull-up  *
 *               resistors. The initialized I2C port is used for communication *
 *               with external devices such as the SSD1306 display.            *
 *                                                                             *
 ******************************************************************************/
/**
 * @file I2C.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Public interface for I2C peripheral initialization.
 */

#ifndef I2C_H
#define I2C_H

#include "driver/gpio.h"  // GPIO_NUM_21, GPIO_NUM_22
#include <driver/i2c.h>   // I2C_NUM_0

// References: ESP32 Pinout Reference and I2C specification
#define I2C_PORT         I2C_NUM_0         /**< I2C port number 0. */
#define I2C_SDA_GPIO     GPIO_NUM_21       /**< GPIO21 as SDA.     */
#define I2C_SCL_GPIO     GPIO_NUM_22       /**< GPIO22 as SCL.     */
#define I2C_FREQUENCY_HZ 400000u           /**< 400kHz fast mode.  */

/**
 * @brief Initializes the I2C peripheral for communication.
 * @details This function configures the necessary registers and settings
 *          to enable I2C communication on the microcontroller.
 */
void I2C_Init();

#endif // I2C_H

/*** end of file I2C.h ***/
