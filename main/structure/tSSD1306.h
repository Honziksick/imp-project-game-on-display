/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tSSD1306.h                                                    *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    18.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tSSD1306 structure holding configuration          *
 *               parameters for an SSD1306 OLED display controller. Include    *
 *               I2C communication settings (port and address) and display     *
 *               dimensions required for initialization and operation.         *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tSSD1306.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Configuration structure for SSD1306 OLED display over I2C.
 */

#ifndef T_SSD1306_H
#define T_SSD1306_H

#include <driver/i2c.h>  // i2c_port_t
#include <stdint.h>      // uint8_t

/**
 * @struct tSSD1306
 * @brief Structure representing an SSD1306 OLED display configuration.
 *
 * @details This structure holds the necessary parameters to interface with
 *          an SSD1306 OLED display over I2C. It includes the I2C port number,
 *          the I2C address of the display, and the dimensions of the display
 *          in pixels.
 */
typedef struct {
    i2c_port_t mI2CPort;    /**< I2C port number.                                     */
    uint8_t mAddress;       /**< I2C address of the SSD1306 display (typically 0x3C). */
    uint8_t mWidth;         /**< Width of the display in pixels (128).                */
    uint8_t mHeight;        /**< Height of the display in pixels (32 or 64).          */
} tSSD1306;

#endif // T_SSD1306_H

/*** end of file tSSD1306.h ***/
