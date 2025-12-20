/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Button.c                                                      *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Handles joystick button input. Provides initialization of the *
 *               GPIO pin, raw active-low sampling, timestamp-based debouncing *
 *               and utility functions to query debounced state and rising     *
 *               edges. Suitable for the game's input handling on target HW.   *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Button.c
 * @author Jan Kalina \<xkalinj00>
 * @brief Joystick button input handling with debouncing.
 */

#include "public/Button.h"
#include "structure/tGame.h"
#include "driver/gpio.h"  // GPIO functions
#include "esp_timer.h"    // esp_timer_get_time
#include "esp_err.h"      // ESP_ERROR_CHECK
#include <stdbool.h>      // bool
#include <stdint.h>       // int64_t

/**
 * @brief GPIO number where the button is connected.
 * @details Adjust this value based on your hardware configuration. Read GPIO
 *          level and invert (active-low).
 */
static bool Button_ReadRawState() {
    return gpio_get_level(BUTTON_GPIO) ? false : true;
} // Button_ReadRaw()

void Button_Init() {
    // Configure button GPIO as input with pull-up
    const gpio_config_t config = {
        .pin_bit_mask = (1ull << BUTTON_GPIO),  // select button GPIO
        .mode = GPIO_MODE_INPUT,                         // set as input
        .pull_up_en = GPIO_PULLUP_ENABLE,                // enable pull-up resistor
        .pull_down_en = GPIO_PULLDOWN_DISABLE,           // disable pull-down resistor
        .intr_type = GPIO_INTR_DISABLE                   // no interrupt
    };

    ESP_ERROR_CHECK(gpio_config(&config));
} // Button_Init()

void Button_IsDebouncedState(tGame *pGame) {
    // Validate input pointer
    if(pGame == NULL) {
        return;
    }

    // Get current time in MICROseconds since boot
    const int64_t currentTimeUs = esp_timer_get_time();

    // Read raw button state
    const bool rawState = Button_ReadRawState();

    // Detect state change
    if(rawState != pGame->mButton.mIsRawState) {
        pGame->mButton.mIsRawState = rawState;
        pGame->mButton.mStateChangedTimestampMs = currentTimeUs;
    }

    // If state has been stable for 20ms, accept it as debounced
    const int64_t deltaUs = currentTimeUs - pGame->mButton.mStateChangedTimestampMs;
    if(deltaUs >= BUTTON_DEBOUNCE_US) {
        pGame->mButton.mIsDebouncedState = rawState;
    }
} // Button_UpdateDebounced()

bool Button_IsRisingEdge(const tGame *pGame, bool *pPreviousState) {
    // Validate input pointers
    if(pGame == NULL || pPreviousState == NULL) {
        return false;
    }

    // Get current debounced state
    const bool currentState = pGame->mButton.mIsDebouncedState;

    // Detect rising edge (0 -> 1 transition)
    const bool pressedEdge = (currentState && !(*pPreviousState));

    // Update previous state for next call
    *pPreviousState = currentState;

    return pressedEdge;
} // Button_PressedEdge()

/*** end of file Button.c ***/
