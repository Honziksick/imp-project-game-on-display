/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Button.h                                                      *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Public API for GPIO-based button input with software          *
 *               debouncing and edge detection. Provides initialization,       *
 *               debounced state reading, and rising-edge detection for        *
 *               reliable button press events. Integrates with the game        *
 *               structure to update button state during the input polling     *
 *               cycle.                                                        *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Button.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Public API for GPIO-based button input with software debouncing and
 *        edge detection.
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "structure/tGame.h"
#include "driver/gpio.h"  // GPIO_NUM_27
#include <stdbool.h>      // bool

// References: ESP32 Technical Reference Manual v5.6
#define BUTTON_GPIO GPIO_NUM_27     /**< GPIO27: Button switch, active-low.   */
#define BUTTON_DEBOUNCE_US 20000    /**< 20 ms debounce time in microseconds. */

/**
 * @brief Initialize button module.
 * @details Sets up necessary hardware configurations for button input.
 *          This function should be called once during the system
 *          initialization.
 */
void Button_Init();

/**
 * @brief Update the debounced state of the button.
 * @details Reads the raw button input and applies debouncing logic to filter
 *          out any noise or false triggers. Updates the game structure with
 *          the current debounced state.
 *
 * @param pGame Pointer to the game structure.
 */
void Button_IsDebouncedState(tGame *pGame);

/**
 * @brief Detects a rising edge on the button press.
 * @details Compares the current debounced state of the button with the
 *          previous state to determine if a rising edge (transition from not
 *          pressed to pressed) has occurred.
 *
 * @param pGame Pointer to the game structure.
 * @param pPreviousState Pointer to an boolean representing the previous state of
 *                       the button.
 * @return True if a rising edge is detected, false otherwise.
 */
bool Button_IsRisingEdge(const tGame *pGame, bool *pPreviousState);

#endif // BUTTON_H

/*** end of file Button.h ***/
