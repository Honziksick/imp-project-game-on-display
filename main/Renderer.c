/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Renderer.c                                                    *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    20.12.2025                                                    *
 *                                                                             *
 * Description:  Implements rendering logic for the game. The module is        *
 *               responsible for composing each framebuffer frame: drawing     *
 *               HUD elements (score, time, pollen bar), rendering flowers     *
 *               (source and target markers), drawing the bee sprite based     *
 *               on its floating-point position, and presenting state-specific *
 *               screens (splash, calibration, pause, game over). The code     *
 *               uses the Graphics primitives and utility helpers to convert   *
 *               coordinates and to safely format text for display.            *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Renderer.c
 * @author Jan Kalina \<xkalinj00>
 * @brief Framebuffer rendering routines for HUD, game entities and state screens.
 */

#include "public/Renderer.h"
#include "public/Graphics.h"
#include "public/Utils.h"
#include "structure/tGame.h"
#include "structure/tGraphics.h"
#include "structure/tFlower.h"
#include "structure/tBee.h"
#include <stdbool.h>  // bool
#include <stdio.h>    // snprintf

/**
 * @brief Render the HUD (score, remaining time and pollen progress).
 * @details Renders score and remaining time into a temporary text buffer
 *          (safe snprintf handling), draws the pollen progress bar background
 *          and the filled portion based on bee pollen state. The function
 *          validates input pointers and clamps the fill width to the valid range.
 *
 * @param pGame Pointer to the current game state. If NULL the function returns
 *              without rendering.
 * @param pGraphics Pointer to the graphics context used for drawing. If NULL
 *                  the function returns without rendering.
 */
static void Renderer_DrawHUD(const tGame *pGame, const tGraphics *pGraphics) {
    // Validate input pointers
    if(pGame == NULL || pGraphics == NULL) {
        return;
    }

    char textBuffer[32]; // temporary buffer for text rendering
    int result = 0;

    // Draw score (top-left)
    result = snprintf(textBuffer, sizeof(textBuffer), "Score: %d", pGame->mScore);
    if(result < 0) {
        textBuffer[0] = '\0';
    }
    else if((size_t)result >= sizeof(textBuffer)) {
        textBuffer[sizeof(textBuffer) - 1] = '\0';
    }
    Graphics_DrawText(pGraphics, 0, 0, textBuffer);

    // Draw remaining time (top-center)
    result = snprintf(textBuffer, sizeof(textBuffer), "Time: %02d", Utils_CeilFloatToInt(pGame->mTimeLeftSec));
    if(result < 0) {
        textBuffer[0] = '\0';
    }
    else if((size_t)result >= sizeof(textBuffer)) {
        textBuffer[sizeof(textBuffer) - 1] = '\0';
    }
    Graphics_DrawText(pGraphics, 48, 0, textBuffer);

    // Draw pollen collection progress bar (top-right)
    Graphics_DrawText(pGraphics, 92, 0, "Pollen: ");

    // Bar background rectangle
    Graphics_DrawRectangle(pGraphics, RENDERRER_POLLEN_BAR_X_OFFSET, RENDERRER_POLLEN_BAR_Y_OFFSET,
                           RENDERRER_POLLEN_BAR_WIDTH, RENDERRER_POLLEN_BAR_HEIGHT, false);

    // Calculate filled portion of the pollen bar based on pollen progress
    const int maxFillWidth = RENDERRER_POLLEN_BAR_WIDTH - 2;
    int fillWidth = Utils_FloorFloatToInt(
            (float)maxFillWidth * Utils_ClampFloat(pGame->mBee.mPollenFill, 0.0f, 1.0f)
            );

    // If bee has collected pollen, fill the bar completely
    if(pGame->mBee.mHasPollen) {
        fillWidth = maxFillWidth;
    }

    // Clamp fill width to valid range if needed
    if(fillWidth < 0) {
        fillWidth = 0;
    }
    if(fillWidth > maxFillWidth) {
        fillWidth = maxFillWidth;
    }

    // Draw filled portion of the pollen bar
    if(fillWidth > 0) {
        Graphics_DrawRectangle(pGraphics, RENDERRER_POLLEN_BAR_X_OFFSET + 1,
                               RENDERRER_POLLEN_BAR_Y_OFFSET + 1, fillWidth,
                               RENDERRER_POLLEN_BAR_HEIGHT - 2, true);
    }
} // Renderer_DrawHUD()

/**
 * @brief Draw a flower marker at the given pixel coordinates.
 * @details Draws the flower outline and a center indicator. When isTarget is true
 *          the function draws a filled center dot and a horizontal marker line,
 *          otherwise it draws a smaller center dot and a vertical marker line.
 *          Coordinates are expected in integer pixels.
 *
 * @param pGraphics Pointer to the graphics context used for drawing. If NULL
 *                  nothing is drawn.
 * @param centerX X coordinate of the flower center in pixels.
 * @param centerY Y coordinate of the flower center in pixels.
 * @param isTarget If true draw the target variant (filled center + horizontal line).
 */
static void Renderer_DrawFlower(const tGraphics *pGraphics, const int centerX,
                                const int centerY, const bool isTarget) {
    // Validate input pointer
    if(pGraphics == NULL) {
        return;
    }

    // Draw flower outline circle
    Graphics_DrawCircle(pGraphics, centerX, centerY, FLOWER_RADIUS, false);

    // Draw center indicator (different for source and target)
    if(isTarget) {
        // Target: filled center dot + horizontal line
        Graphics_DrawCircle(pGraphics, centerX, centerY, 2, true);
        Graphics_DrawLine(pGraphics, centerX - 4, centerY, centerX + 4, centerY);
    }
    else {
        // Source: small center dot + vertical line
        Graphics_DrawCircle(pGraphics, centerX, centerY, 1, true);
        Graphics_DrawLine(pGraphics, centerX, centerY - 4, centerX, centerY + 4);
    }
} // Renderer_DrawFlower()

/**
 * @brief Draw the bee at its current position.
 * @details Rounds the bee's floating-point position to integer pixel coordinates
 *          (via utility functions), draws the bee body as a filled circle and
 *          additional wing pixels. The function validates its input pointers
 *          and performs no drawing if either pointer is NULL.
 *
 * @param pGraphics Pointer to the graphics context used for drawing. If NULL
 *                  nothing is drawn.
 * @param pBee Pointer to the bee state containing the floating-point position.
 *             If NULL nothing is drawn.
 */
static void Renderer_DrawBee(const tGraphics *pGraphics, const tBee *pBee) {
    // Validate input pointers
    if(pGraphics == NULL || pBee == NULL) {
        return;
    }

    // Round float position to integer pixel coordinates
    const int pixelX = Utils_LRoundFloatToInt(pBee->mPosX);
    const int pixelY = Utils_LRoundFloatToInt(pBee->mPosY);

    // Draw bee body (filled circle)
    Graphics_DrawCircle(pGraphics, pixelX, pixelY, BEE_RADIUS, true);

    // Draw wing pixels
    Graphics_SetPixel(pGraphics, pixelX - 3, pixelY - 2, true);
    Graphics_SetPixel(pGraphics, pixelX + 3, pixelY - 2, true);
    Graphics_SetPixel(pGraphics, pixelX - 2, pixelY - 3, true);
    Graphics_SetPixel(pGraphics, pixelX + 2, pixelY - 3, true);
} // Renderer_DrawBee()

void Renderer_Draw(const tGame *pGame, const tGraphics *pGraphics) {
    // Validate input pointers
    if(pGame == NULL || pGraphics == NULL) {
        return;
    }

    // Clear framebuffer for new frame
    Graphics_Clear(pGraphics);

    // Render splash screen
    if(pGame->mState == STATE_SPLASH) {
        Graphics_DrawText(pGraphics, 12, 14, "BEE POLLINATION");
        Graphics_DrawText(pGraphics, 12, 28, "Joystick: move");
        Graphics_DrawText(pGraphics, 12, 38, "SW: start/pause");
        Graphics_DrawText(pGraphics, 12, 52, "Press SW...");
        return;
    }

    // Render calibration screen
    if(pGame->mState == STATE_CALIB) {
        Graphics_DrawText(pGraphics, 18, 18, "CALIBRATING...");
        Graphics_DrawText(pGraphics, 12, 34, "Keep joystick idle");
        return;
    }

    // Render game field
    Renderer_DrawHUD(pGame, pGraphics);
    Renderer_DrawFlower(pGraphics, pGame->mSourceFlower.mPosX, pGame->mSourceFlower.mPosY, 0);
    Renderer_DrawFlower(pGraphics, pGame->mTargetFlower.mPosX, pGame->mTargetFlower.mPosY, 1);
    Renderer_DrawBee(pGraphics, &pGame->mBee);

    // Render pause overlay
    if(pGame->mState == STATE_PAUSE) {
        Graphics_DrawRectangle(pGraphics, 20, 22, 88, 20, 0);
        Graphics_DrawText(pGraphics, 42, 28, "PAUSED");
        Graphics_DrawText(pGraphics, 28, 40, "Press SW...");
    }
    // Render game over overlay
    else if(pGame->mState == STATE_GAMEOVER) {
        Graphics_DrawRectangle(pGraphics, 14, 18, 100, 30, 0);
        Graphics_DrawText(pGraphics, 26, 24, "GAME OVER");

        char scoreBuffer[24];
        const int result = snprintf(scoreBuffer, sizeof(scoreBuffer), "Score: %d", pGame->mScore);
        if(result < 0) {
            scoreBuffer[0] = '\0';
        }
        else if((size_t)result >= sizeof(scoreBuffer)) {
            scoreBuffer[sizeof(scoreBuffer) - 1] = '\0';
        }

        Graphics_DrawText(pGraphics, 26, 34, scoreBuffer);
        Graphics_DrawText(pGraphics, 18, 52, "Press SW to retry");
    }
} // Renderer_Draw()

/*** end of file Renderer.c ***/
