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
 * Description:                                                                *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Button.c
 * @author Jan Kalina \<xkalinj00>
 * @brief
 */

#include "public/Button.h"
#include "structure/tGame.h"
#include "driver/gpio.h"  // GPIO functions
#include "esp_timer.h"    // esp_timer_get_time
#include "esp_err.h"      // ESP_ERROR_CHECK
#include <stdbool.h>      // bool
#include <stdint.h>       // int64_t

void Button_Init() {
    // Configure button GPIO as input with pull-up
    const gpio_config_t config = {
        .pin_bit_mask = (1ull << JOYSTICK_BUTTON_GPIO),  // select button GPIO
        .mode = GPIO_MODE_INPUT,                         // set as input
        .pull_up_en = GPIO_PULLUP_ENABLE,                // enable pull-up resistor
        .pull_down_en = GPIO_PULLDOWN_DISABLE,           // disable pull-down resistor
        .intr_type = GPIO_INTR_DISABLE                   // no interrupt
    };

    ESP_ERROR_CHECK(gpio_config(&config));
} // Button_Init()

static int Button_ReadRawState() {
    // Read GPIO level and invert (active-low)
    return gpio_get_level(JOYSTICK_BUTTON_GPIO) ? 0 : 1;
} // Button_ReadRaw()

bool Button_IsDebouncedState(tGame *pGame) {
    // Get current time in ms
    const int64_t currentTimeMs = esp_timer_get_time();

    // Read raw button state
    const int rawState = Button_ReadRawState();

    // Detect state change
    if(rawState != pGame->mButton.mRawState) {
        pGame->mButton.mRawState = rawState;
        pGame->mButton.mStateChangedTimestampMs = currentTimeMs;
    }

    // If state has been stable for 20ms, accept it as debounced
    if((currentTimeMs - pGame->mButton.mStateChangedTimestampMs) > 20000) {
        pGame->mButton.mDebouncedState = rawState;
    }

    return pGame->mButton.mDebouncedState;
} // Button_UpdateDebounced()

bool Button_IsRisingEdge(const tGame *pGame, int *pPreviousState) {
    // Get current debounced state
    const int currentState = pGame->mButton.mDebouncedState;

    // Detect rising edge (0 -> 1 transition)
    const int pressedEdge = (currentState == 1 && *pPreviousState == 0);

    // Update previous state for next call
    *pPreviousState = currentState;

    return pressedEdge;
} // Button_PressedEdge()

/*** end of file Button.c ***/
