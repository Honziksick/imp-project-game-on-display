/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         SSD1306.h                                                     *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Public header for the SSD1306 display. Declares the public    *
 *               API for initializing the SSD1306 display, configuring I2C     *
 *               parameters, and sending a framebuffer to the device. The      *
 *               driver supports standard I2C-addressed SSD1306 displays.      *
 *                                                                             *
 ******************************************************************************/
/**
 * @file SSD1306.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Public API for SSD1306 OLED display driver over I2C.
 */

#ifndef SSD1306_PUBLIC_H
#define SSD1306_PUBLIC_H

#include "structure/tSSD1306.h"
#include "driver/i2c.h"  // i2c_port_t
#include "esp_err.h"     // esp_err_t
#include <stdint.h>      // uint8_t

#define SSD1306_CONTROL_COMMAND 0x00  /**< Control byte for command transmission (Co=0, D/C#=0). */
#define SSD1306_CONTROL_DATA    0x40  /**< Control byte for data transmission (Co=0, D/C#=1).    */

#define SSD1306_MAX_COMMANDS_PER_TRANSFER 32   /**< Maximum number of commands per I2C transfer.   */
#define SSD1306_MAX_DATA_PER_TRANSFER     128  /**< Maximum number of data bytes per I2C transfer. */
#define SSD1306_CONTROL_BYTES             1    /**< Number of control bytes per I2C transmission.  */

/**
 * @brief Initialize an SSD1306 display context and hardware.
 * @details Configures the provided tSSD1306 context with I2C port, device
 *          address and display geometry. Performs any required hardware
 *          initialization sequence. Returns ESP_OK on success or an esp_err_t
 *          error code on failure.
 *
 * @param pDisplay Pointer to the display context to initialize.
 * @param i2cPort I2C port to use (ESP-IDF i2c_port_t).
 * @param address 7-bit I2C device address of the SSD1306.
 * @param width Display width in pixels.
 * @param height Display height in pixels.
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t SSD1306_Init(tSSD1306 *pDisplay, i2c_port_t i2cPort, uint8_t address, uint8_t width, uint8_t height);

/**
 * @brief Transmit a prepared framebuffer to the SSD1306 device.
 * @details Sends raw framebuffer bytes to the display over I2C. The framebuffer
 *          is expected to be in the page-oriented format (`width * height/8 bytes`).
 *          The function does not modify the framebuffer buffer provided by the
 *          caller.
 *
 * @param pDisplay Pointer to the initialized display context.
 * @param pFrameBuffer Pointer to the framebuffer data to send.
 * @param frameBufferLength Length of the framebuffer in bytes.
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t SSD1306_SendFrameBuffer(const tSSD1306 *pDisplay, const uint8_t *pFrameBuffer, int frameBufferLength);

#endif // SSD1306_PUBLIC_H

/*** end of file SSD1306.h ***/
