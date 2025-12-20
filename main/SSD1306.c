/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         SSD1306.c                                                     *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Driver implementation for SSD1306 OLED display controller     *
 *               over I2C. Provides initialization, command transmission,      *
 *               and framebuffer update functions for 128x64 monochrome        *
 *               displays using page-oriented memory addressing.               *
 *                                                                             *
 ******************************************************************************/
/**
 * @file SSD1306.c
 * @author Jan Kalina \<xkalinj00>
 * @brief SSD1306 OLED driver implementation for I2C communication.
 * @note References: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
 */

#include "public/SSD1306.h"
#include "structure/tSSD1306.h"
#include "freertos/FreeRTOS.h"  // pdMS_TO_TICKS
#include "freertos/task.h"      // vTaskDelay
#include "driver/i2c.h"         // I2C master functions

/**
 * @brief Writes a sequence of bytes to an I2C device.
 * @details Creates an I2C command link, builds a write transaction
 *          (START + address\|write + data + STOP), executes it with a 100 ms
 *          timeout and deletes the command link. Caller must ensure that pData
 *          points to valid memory and that dataLength iswithin allowed limits
 *          for the underlying driver/adapter.
 *
 * @param port I2C port to use for the transaction.
 * @param address 7-bit I2C address of the target device.
 * @param pData Pointer to the data buffer to transmit (caller supplies control
 *              byte if needed).
 * @param dataLength Number of bytes to transmit from pData.
 * @return esp_err_t Returns ESP_OK on success or an ESP error code on failure.
 */
static esp_err_t SSD1306_I2CWriteBytes(const i2c_port_t port, const uint8_t address,
                                       const uint8_t *pData, const size_t dataLength) {
    // Create I2C command link for transaction
    i2c_cmd_handle_t commandLink = i2c_cmd_link_create();

    // Build I2C write transaction: START + ADDRESS + DATA + STOP (p20, section 8.1.5)
    esp_err_t result = i2c_master_start(commandLink);                                       // send START condition
    result |= i2c_master_write_byte(commandLink, (address << 1) | I2C_MASTER_WRITE, true);  // write address with R/W# bit=0
    result |= i2c_master_write(commandLink, (uint8_t*)pData, dataLength, true);             // write data bytes with ACK check
    result |= i2c_master_stop(commandLink);                                                 // send STOP condition

    // Execute I2C transaction with 100ms timeout
    result |= i2c_master_cmd_begin(port, commandLink, pdMS_TO_TICKS(100));

    // Clean up command link resources
    i2c_cmd_link_delete(commandLink);

    return result;
} // SSD1306_I2CWriteBytes()

/**
 * @brief Sends a single command byte to the SSD1306 display.
 * @details Prepares a 2-byte buffer containing the SSD1306 command-mode control
 *          byte followed by the command byte, then forwards the buffer to
 *          SSD1306_I2CWriteBytes() for transmission.
 *
 * @param pDisplay Pointer to the tSSD1306 display configuration (provides I2C
 *                 port and address).
 * @param command Single command byte to send to the display.
 * @return esp_err_t Returns ESP_OK on success or an ESP error code returned
 *         by the I2C write.
 */
static esp_err_t SSD1306_SendCommand(const tSSD1306 *pDisplay, const uint8_t command) {
    // Prepare 2-byte buffer for Control byte + Command byte (p20, section 8.1.5.2)
    const uint8_t commandBuffer[2] = {SSD1306_CONTROL_COMMAND, command};  // Co=0, D/C#=0 for command

    // Send via I2C
    return SSD1306_I2CWriteBytes(pDisplay->mI2CPort, pDisplay->mAddress, commandBuffer, sizeof(commandBuffer));
} // SSD1306_SendCommand()

/**
 * @brief Sends a list of command bytes to the SSD1306 in chunks.
 * @details Splits the provided commands array into chunks of up to
 *          SSD1306_MAX_COMMANDS_PER_TRANSFER, prefixes each chunk with the
 *          SSD1306 command-mode control byte and transmits each chunk via
 *          SSD1306_I2CWriteBytes(). The function aborts and returns the first
 *          error encountered.
 *
 * @param pDisplay Pointer to the tSSD1306 display configuration.
 * @param commands Pointer to the array of command bytes to send.
 * @param commandsCount Number of bytes in the commands array.
 * @return esp_err_t Returns ESP_OK if all chunks were transmitted successfully,
 *         otherwise returns the first non-ESP_OK error code.
 */
static esp_err_t SSD1306_SendCommandList(const tSSD1306 *pDisplay, const uint8_t commands[], size_t commandsCount) {
    uint8_t transmitBuffer[SSD1306_CONTROL_BYTES + SSD1306_MAX_COMMANDS_PER_TRANSFER];  // control byte + max 32 command bytes per transfer
    esp_err_t result = ESP_OK;

    // Send commands in chunks of up to 32 bytes
    while(commandsCount > 0) {
        // Determine chunk size (max 32 commands per transmission)
        const size_t transferChunkSize = (commandsCount > SSD1306_MAX_COMMANDS_PER_TRANSFER)
                                             ? SSD1306_MAX_COMMANDS_PER_TRANSFER
                                             : commandsCount;

        // Set control byte to indicate following bytes are commands (p20, section 8.1.5.2)
        transmitBuffer[0] = SSD1306_CONTROL_COMMAND;  // Co=0, D/C#=0 for command mode

        // Copy commands to transmit buffer
        for(size_t iCommand = 0; iCommand < transferChunkSize; iCommand++) {
            transmitBuffer[SSD1306_CONTROL_BYTES + iCommand] = commands[iCommand];
        }

        // Send chunk via I2C
        result = SSD1306_I2CWriteBytes(pDisplay->mI2CPort, pDisplay->mAddress,
                                       transmitBuffer, SSD1306_CONTROL_BYTES + transferChunkSize);
        if(result != ESP_OK) {
            return result;  // Abort on transmission error
        }

        // Advance to next command chunk
        commands += transferChunkSize;
        commandsCount -= transferChunkSize;
    }

    return ESP_OK;
} // SSD1306_SendCommandList()

esp_err_t SSD1306_Init(tSSD1306 *pDisplay, const i2c_port_t i2cPort, const uint8_t address,
                       const uint8_t width, const uint8_t height) {
    // Store display configuration parameters
    pDisplay->mI2CPort = i2cPort;
    pDisplay->mAddress = address;
    pDisplay->mWidth = width;
    pDisplay->mHeight = height;

    // Initialization sequence for 128x64 display (references sections 9 Command Table and section 10 Command Descriptions)
    const uint8_t initSequence[] = {
        0xAE,        // display OFF
        0xD5,
        0x80,  // set display clock divide ratio/oscillator frequency (0x80 = recommended default)
        0xA8,
        0x3F,  // set multiplex ratio (0x3F -> 64 rows; value = N-1)
        0xD3,
        0x00,  // set display offset (0x00 = no offset)
        0x40,        // set start line address (0x40 = start line = 0)
        0x8D,
        0x14,  // charge pump setting (0x14 = enable internal charge pump)
        0x20,
        0x00,  // set memory addressing mode (0x00 = horizontal addressing)
        0xA1,        // set segment re-map (0xA1 = column address 127 mapped to SEG0)
        0xC8,        // set COM output scan direction (0xC8 = remapped)
        0xDA,
        0x12,  // set COM pins hardware configuration (0x12 = alt. COM pins config for 128x64)
        0x81,
        0x7F,  // set contrast control (0x7F = mid-level)
        0xD9,
        0xF1,  // set pre-charge period (0xF1 = phase2/phase1 settings)
        0xDB,
        0x40,  // set VCOMH deselect level (0x40 ~ 0.77*VCC)
        0xA4,        // entire display ON (resume from RAM)
        0xA6,        // normal display (not inverted)
        0x2E,        // deactivate scroll
        0xAF         // display ON
    };

    // Send initialization commands to display
    const esp_err_t initResult = SSD1306_SendCommandList(pDisplay, initSequence, sizeof(initSequence));

    // Wait for display to stabilize after initialization
    vTaskDelay(pdMS_TO_TICKS(50));

    return initResult;
} // SSD1306_Init()

esp_err_t SSD1306_SendFrameBuffer(const tSSD1306 *pDisplay, const uint8_t *pFrameBuffer,
                                  const int frameBufferLength) {
    esp_err_t result = ESP_OK;

    // Set column address range (0 to width-1) (p35, section 10.1.4)
    result |= SSD1306_SendCommand(pDisplay, 0x21);                  // column address command
    result |= SSD1306_SendCommand(pDisplay, 0x00);                  // start column
    result |= SSD1306_SendCommand(pDisplay, pDisplay->mWidth - 1);  // end column

    // Set page address range (0 to pages-1, where pages = height/8) (p36, section 10.1.5)
    result |= SSD1306_SendCommand(pDisplay, 0x22);                                  // page address command
    result |= SSD1306_SendCommand(pDisplay, 0x00);                                  // start page
    result |= SSD1306_SendCommand(pDisplay, (uint8_t)(pDisplay->mHeight / 8) - 1);  // end page

    // Return if setup commands failed
    if(result != ESP_OK) {
        return result;
    }

    // Prepare buffer for data transmission (p20, Section 8.1.5.2)
    uint8_t transmitBuffer[SSD1306_CONTROL_BYTES + SSD1306_MAX_DATA_PER_TRANSFER];  // control byte + max 128 data bytes
    transmitBuffer[0] = SSD1306_CONTROL_DATA;                                       // Co=0, D/C#=1 for data

    // Send framebuffer in chunks (max 128 bytes per I2C transaction)
    int bytesSent = 0;
    while(bytesSent < frameBufferLength) {
        // Remaining bytes to send
        int chunkSize = frameBufferLength - bytesSent;
        if(chunkSize > 128) {
            chunkSize = 128;  // clamp to maximum chunk size
        }

        // Copy chunk from framebuffer to transmit buffer
        for(int iChunk = 0; iChunk < chunkSize; iChunk++) {
            transmitBuffer[1 + iChunk] = pFrameBuffer[bytesSent + iChunk];
        }

        // Send chunk via I2C
        result = SSD1306_I2CWriteBytes(pDisplay->mI2CPort, pDisplay->mAddress,
                                       transmitBuffer, 1 + chunkSize);

        if(result != ESP_OK) {
            return result;
        }

        bytesSent += chunkSize;  // advance to next chunk
    }

    return ESP_OK;
} // SSD1306_SendFrameBuffer()

/*** end of file SSD1306.c ***/
