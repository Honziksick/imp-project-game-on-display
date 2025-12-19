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
 * Description:                                                                *
 *                                                                             *
 ******************************************************************************/
/**
 * @file main.c
 * @author Jan Kalina \<xkalinj00>
 * @brief
 */


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_err.h"

#include "public/SSD1306.h"
#include "public/Graphics.h"
#include "public/I2C.h"
#include "public/Joystick.h"
#include "public/Button.h"
#include "public/GameLogic.h"
#include "public/Renderer.h"
#include "structure/tGame.h"
#include "structure/tSSD1306.h"
#include "structure/tGraphics.h"

#include <stdbool.h>

static const char *TAG = "bee";

// Global framebuffer and device handles
static uint8_t framebuffer[FRAMEBUFFER_SIZE];
static tSSD1306 oledDisplay;
static tGraphics graphicsContext;

void app_main() {
    // Initialize hardware peripherals
    I2C_Init();
    Joystick_Init();
    Button_Init();

    // Initialize OLED display driver
    ESP_ERROR_CHECK(SSD1306_Init(&oledDisplay, I2C_PORT, SSD1306_ADDRESS, SSD1306_WIDTH, SSD1306_HEIGHT));

    // Initialize graphics context
    Graphics_Init(&graphicsContext, SSD1306_WIDTH, SSD1306_HEIGHT, framebuffer);

    // Initialize game state
    tGame game = {0};
    game.mState = STATE_SPLASH;
    game.mButton.mRawState = 0;
    game.mButton.mDebouncedState = 0;
    game.mButton.mStateChangedTimestampMs = esp_timer_get_time();

    // Button edge detection state
    int buttonEdgePrevState = 0;

    // Frame timing
    int64_t lastTimeMicros = esp_timer_get_time();

    // Main game loop
    while(1) {
        // Calculate frame delta time
        int64_t currentTimeMicros = esp_timer_get_time();
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
        bool buttonPressed = Button_IsRisingEdge(&game, &buttonEdgePrevState);

        // Joystick input variables
        float normalizedX = 0.0f, normalizedY = 0.0f;

        // State machine logic
        if(game.mState == STATE_SPLASH) {
            if(buttonPressed) {
                game.mState = STATE_CALIB;
                Renderer_Draw(&game, &graphicsContext, 0, 0);
                SSD1306_SendFrameBuffer(&oledDisplay, framebuffer, FRAMEBUFFER_SIZE);
                Joystick_Calibrate(&game);
                GameLogic_StartGame(&game);
            }
        }
        else if(game.mState == STATE_PLAY) {
            Joystick_Read(&game, &normalizedX, &normalizedY);
            if(buttonPressed) {
                game.mState = STATE_PAUSE;
            }
            GameLogic_UpdatePlay(&game, deltaTime, normalizedX, normalizedY);
        }
        else if(game.mState == STATE_PAUSE) {
            Joystick_Read(&game, &normalizedX, &normalizedY);
            if(buttonPressed) {
                game.mState = STATE_PLAY;
            }
        }
        else if(game.mState == STATE_GAMEOVER) {
            if(buttonPressed) {
                game.mState = STATE_CALIB;
                Renderer_Draw(&game, &graphicsContext, 0, 0);
                SSD1306_SendFrameBuffer(&oledDisplay, framebuffer, FRAMEBUFFER_SIZE);
                Joystick_Calibrate(&game);
                GameLogic_StartGame(&game);
            }
        }
        else if(game.mState == STATE_CALIB) {
            Joystick_Calibrate(&game);
            GameLogic_StartGame(&game);
        }

        // Render current frame
        Renderer_Draw(&game, &graphicsContext, normalizedX, normalizedY);
        SSD1306_SendFrameBuffer(&oledDisplay, framebuffer, FRAMEBUFFER_SIZE);

        // Frame rate limiting (~30 FPS)
        vTaskDelay(pdMS_TO_TICKS(33));
    }
}

/*** end of file main.c ***/
