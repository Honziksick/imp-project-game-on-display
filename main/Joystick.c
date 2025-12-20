/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Joystick.c                                                    *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Implements joystick input handling and calibration logic.     *
 *               Provides sampling of analog/digital joystick inputs,          *
 *               dead‑zone filtering, debouncing and calibration routines.     *
 *               Maps raw input to normalized game control values and          *
 *               exposes functions used by the game loop to poll and update    *
 *               the current joystick state. Designed to be lightweight and    *
 *               suitable for framebuffer-based embedded systems with a GUI.   *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Joystick.c
 * @author Jan Kalina \<xkalinj00>
 * @brief Implements joystick input handling and calibration logic.
 */

#include "public/Joystick.h"
#include "public/Utils.h"
#include "freertos/FreeRTOS.h"    // pdMS_TO_TICKS
#include "freertos/task.h"        // vTaskDelay
#include "esp_adc/adc_oneshot.h"  // oneshot API
#include "esp_err.h"              // ESP_ERROR_CHECK
#include <stdint.h>               // int64_t
#include <math.h>                 // powf

/**
 * @brief Handle for ADC1 oneshot unit.
 * @details Used for performing single-shot ADC reads on joystick channels.
 */
static adc_oneshot_unit_handle_t ADC1_HANDLE = NULL;

void Joystick_Init() {
    // Create oneshot unit
    const adc_oneshot_unit_init_cfg_t initConfig = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&initConfig, &ADC1_HANDLE));

    // Configure both X and Y channels (with 12-bit resolution, 11 dB attenuation)
    const adc_oneshot_chan_cfg_t channelConfig = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(ADC1_HANDLE, JOYSTICK_X_CHANNEL, &channelConfig));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(ADC1_HANDLE, JOYSTICK_Y_CHANNEL, &channelConfig));
} // Joystick_Init()

void Joystick_Calibrate(tGame *pGame) {
    int64_t sumX = 0;
    int64_t sumY = 0;
    const int sampleCount = 200;

    // Collect multiple samples to determine center position
    for(int iSample = 0; iSample < sampleCount; iSample++) {
        int rawX = 0;
        int rawY = 0;

        // Blocking read of raw ADC values
        ESP_ERROR_CHECK(adc_oneshot_read(ADC1_HANDLE, JOYSTICK_X_CHANNEL, &rawX));
        ESP_ERROR_CHECK(adc_oneshot_read(ADC1_HANDLE, JOYSTICK_Y_CHANNEL, &rawY));

        sumX += rawX;
        sumY += rawY;

        vTaskDelay(pdMS_TO_TICKS(5));
    }

    // Compute average center position
    pGame->mJoystick.mCenterX = (int)(sumX / sampleCount);
    pGame->mJoystick.mCenterY = (int)(sumY / sampleCount);

    // Reset filter state after calibration
    pGame->mJoystick.mNormalizedX = 0.0f;
    pGame->mJoystick.mNormalizedY = 0.0f;
} // Joystick_Calibrate()

void Joystick_Read(tGame *pGame, float *pNormalizedX, float *pNormalizedY) {
    int rawX = 0;
    int rawY = 0;

    // Blocking read of raw ADC values
    ESP_ERROR_CHECK(adc_oneshot_read(ADC1_HANDLE, JOYSTICK_X_CHANNEL, &rawX));
    ESP_ERROR_CHECK(adc_oneshot_read(ADC1_HANDLE, JOYSTICK_Y_CHANNEL, &rawY));

    // Normalize to -1.0 ... 1.0 range
    float normalizedX = (float)(rawX - pGame->mJoystick.mCenterX) / 2048.0f; // 2048 = half of 12-bit range (4096)
    float normalizedY = (float)(rawY - pGame->mJoystick.mCenterY) / 2048.0f;

    // Clamp to valid range
    normalizedX = Utils_ClampFloat(normalizedX, -1.0f, 1.0f);
    normalizedY = Utils_ClampFloat(normalizedY, -1.0f, 1.0f);

    // Apply deadzone
    if(Utils_AbsoluteFloat(normalizedX) < JOYSTICK_DEADZONE) {
        normalizedX = 0.0f;
    }
    if(Utils_AbsoluteFloat(normalizedY) < JOYSTICK_DEADZONE) {
        normalizedY = 0.0f;
    }

    // Apply non-linear scaling for finer control near center
    if(normalizedX != 0.0f) {
        normalizedX = Utils_SignFloat1(normalizedX) * powf(Utils_AbsoluteFloat(normalizedX), 1.7f);
    }
    if(normalizedY != 0.0f) {
        normalizedY = Utils_SignFloat1(normalizedY) * powf(Utils_AbsoluteFloat(normalizedY), 1.7f);
    }

    // Apply Exponential Moving Average (EMA) filtering
    pGame->mJoystick.mNormalizedX =
            (1.0f - JOYSTICK_EMA_ALPHA) * pGame->mJoystick.mNormalizedX + JOYSTICK_EMA_ALPHA * normalizedX;
    pGame->mJoystick.mNormalizedY =
            (1.0f - JOYSTICK_EMA_ALPHA) * pGame->mJoystick.mNormalizedY + JOYSTICK_EMA_ALPHA * normalizedY;

    // Update filtered values
    *pNormalizedX = pGame->mJoystick.mNormalizedX;
    *pNormalizedY = pGame->mJoystick.mNormalizedY;
} // Joystick_Read()

/*** end of file Joystick.c ***/
