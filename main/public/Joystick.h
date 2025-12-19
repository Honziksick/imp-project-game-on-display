/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Joystick.h                                                    *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Public API for analog two-axis joystick input and calibration.*
 *               Provides functions to initialize joystick hardware, perform   *
 *               a user-guided calibration to determine center and span, and   *
 *               read normalized X/Y values in the range -1.0...1.0. Includes  *
 *               support for an integrated push button. Designed for polled    *
 *               input in the game loop. It does not perform blocking waits or *
 *               manage ISR-based events.                                      *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Joystick.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Public API for analog two-axis joystick input and calibration.
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "structure/tGame.h"
#include "esp_adc/adc_oneshot.h"

// References: ESP32 Technical Reference Manual v5.6
#define JOYSTICK_X_CHANNEL   ADC_CHANNEL_6     /**< GPIO34: X-axis analog input.       */
#define JOYSTICK_Y_CHANNEL   ADC_CHANNEL_7     /**< GPIO35: Y-axis analog input.       */

#define JOYSTICK_DEADZONE  0.08f     /**< Joystick dead zone (8 % around center).      */
#define JOYSTICK_EMA_ALPHA 0.22f     /**< Exponential Moving Average smoothing factor. */

/**
 * @brief Initialize the joystick module.
 * @details Sets up necessary hardware configurations for joystick operation.
 *          This function must be called before any other joystick functions.
 */
void Joystick_Init();

/**
 * @brief Calibrate the joystick.
 * @details Guides the user through a calibration process to ensure accurate
 *          readings from the joystick. This function may involve user input
 *          to move the joystick to specific positions.
 *
 * @param pGame Pointer to the game structure for context during calibration.
 */
void Joystick_Calibrate(tGame * pGame);

/**
 * @brief Read the current position of the joystick.
 * @details Retrieves the current X and Y positions of the joystick,
 *          normalizing them to a range of -1.0 to 1.0.
 *
 * @param pGame Pointer to the game structure for context during reading.
 * @param pNormalizedX Pointer to store the normalized X position.
 * @param pNormalizedY Pointer to store the normalized Y position.
 */
void Joystick_Read(tGame *pGame, float *pNormalizedX, float *pNormalizedY);

#endif // JOYSTICK_H

/*** end of file Joystick.h ***/
