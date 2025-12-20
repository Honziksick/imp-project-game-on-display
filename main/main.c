/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         main.c                                                        *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Implements the application's entry point and main loop.       *
 *               Responsible for hardware and peripheral initialization        *
 *               (I2C, display, joystick, buttons), game state management,     *
 *               frame timing, input processing (joystick and debounced        *
 *               button), rendering pipeline invocation and framebuffer        *
 *               delivery to the SSD1306 display. Maintains the game           *
 *               finite-state machine (FSM) and ensures stable frame timing    *
 *               and safe handling of peripheral return values.                *
 *                                                                             *
 ******************************************************************************/
/**
 * @file main.c
 * @author Jan Kalina \<xkalinj00>
 * @brief Application entry point and main loop.
 */

#include "public/Button.h"
#include "public/GameLogic.h"
#include "public/Graphics.h"
#include "public/I2C.h"
#include "public/Joystick.h"
#include "public/Renderer.h"
#include "public/SSD1306.h"
#include "structure/tGame.h"
#include "structure/tGraphics.h"
#include "structure/tSSD1306.h"
#include "freertos/FreeRTOS.h"  // pdMS_TO_TICKS
#include "freertos/task.h"      // vTaskDelay
#include "esp_timer.h"          // esp_timer_get_time
#include "esp_log.h"            // ESP_LOGI
#include "esp_err.h"            // ESP_ERROR_CHECK
#include <stdbool.h>            // bool
#include <stdint.h>             // uint8_t, int64_t

/**
 * @brief Framebuffer for the SSD1306 OLED display.
 * @details Holds raw pixel data that is rendered and sent to the display driver.
 *          The total buffer size is defined by FRAMEBUFFER_SIZE.
 */
static uint8_t gFrameBuffer[FRAMEBUFFER_SIZE];

/**
 * @brief SSD1306 driver instance.
 * @details Stores driver state, configuration and I2C-related information
 *          required to communicate with the OLED display.
 */
static tSSD1306 gDisplay;

/**
 * @brief Graphics rendering context.
 * @details Contains rendering state, dimensions and a reference to the
 *          framebuffer used by the renderer to compose frames before sending
 *          them to the display.
 */
static tGraphics gGraphicsContext;

void app_main() {
    // Initialize hardware peripherals
    I2C_Init();
    Joystick_Init();
    Button_Init();

    // Initialize OLED display driver
    ESP_ERROR_CHECK(SSD1306_Init(&gDisplay, I2C_PORT, SSD1306_ADDRESS, SSD1306_WIDTH, SSD1306_HEIGHT));

    // Initialize graphics context
    Graphics_Init(&gGraphicsContext, SSD1306_WIDTH, SSD1306_HEIGHT, gFrameBuffer);

    // Initialize game state
    tGame game = {0};
    game.mState = STATE_SPLASH;
    game.mButton.mIsRawState = false;
    game.mButton.mIsDebouncedState = false;
    game.mButton.mStateChangedTimestampMs = esp_timer_get_time();

    // Button edge detection state
    bool buttonEdgePreviousState = false;

    // Frame timing
    int64_t lastTimeMicros = esp_timer_get_time();

    // Main game loop
    while(true) {
        // Calculate frame delta time
        const int64_t currentTimeMicros = esp_timer_get_time();
        float deltaTime = (float)(currentTimeMicros - lastTimeMicros) / 1000000.0f;

        // Clamp delta time to avoid huge jumps
        if(deltaTime < 0.0f) {
            deltaTime = 0.0f;
        }
        if(deltaTime > 0.2f) {
            deltaTime = 0.2f;
        }

        lastTimeMicros = currentTimeMicros;

        // Update button state and detect press edge
        Button_IsDebouncedState(&game);
        const bool buttonPressed = Button_IsRisingEdge(&game, &buttonEdgePreviousState);

        // Joystick input variables
        float normalizedX = 0.0f;
        float normalizedY = 0.0f;

        // FSM
        switch(game.mState) {
            case STATE_SPLASH: {
                if(buttonPressed) {
                    game.mState = STATE_CALIB;
                    Renderer_Draw(&game, &gGraphicsContext);
                    SSD1306_SendFrameBuffer(&gDisplay, gFrameBuffer, FRAMEBUFFER_SIZE);
                    Joystick_Calibrate(&game);
                    GameLogic_StartGame(&game);
                }
                break;
            }
            case STATE_PLAY: {
                Joystick_Read(&game, &normalizedX, &normalizedY);
                if(buttonPressed) {
                    game.mState = STATE_PAUSE;
                }
                GameLogic_UpdatePlay(&game, deltaTime, normalizedX, normalizedY);
            }
            case STATE_GAMEOVER: {
                if(buttonPressed) {
                    game.mState = STATE_CALIB;
                    Renderer_Draw(&game, &gGraphicsContext);
                    SSD1306_SendFrameBuffer(&gDisplay, gFrameBuffer, FRAMEBUFFER_SIZE);
                    Joystick_Calibrate(&game);
                    GameLogic_StartGame(&game);
                }
                break;
            }
            case STATE_CALIB: {
                Joystick_Calibrate(&game);
                GameLogic_StartGame(&game);
                break;
            }
            case STATE_PAUSE:
            default: {
                Joystick_Read(&game, &normalizedX, &normalizedY);
                if(buttonPressed) {
                    game.mState = STATE_PLAY;
                }
                break;
            }
        } // switch(game.mState)

        // Render current frame
        Renderer_Draw(&game, &gGraphicsContext);
        SSD1306_SendFrameBuffer(&gDisplay, gFrameBuffer, FRAMEBUFFER_SIZE);

        // Frame rate limiting (~30 FPS)
        vTaskDelay(pdMS_TO_TICKS(33));
    }
} // app_main()

/*** end of file main.c ***/
