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
#include "structure/tSpider.h"
#include "structure/tRainDrop.h"
#include "structure/tPowerUp.h"
#include "enum/ePowerUpType.h"
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
    char textBuffer[32]; // temporary buffer for text rendering
    int result = 0;

    // Draw score (top-left)
    result = snprintf(textBuffer, sizeof(textBuffer), "S:%d", pGame->mScore);
    if(result < 0) {
        textBuffer[0] = '\0';
    }
    else if((size_t)result >= sizeof(textBuffer)) {
        textBuffer[sizeof(textBuffer) - 1] = '\0';
    }
    Graphics_DrawText(pGraphics, 0, 0, textBuffer);

    // Draw remaining time (top-center)
    result = snprintf(textBuffer, sizeof(textBuffer), "T:%02d", Utils_CeilFloatToInt(pGame->mTimeLeftSec));
    if(result < 0) {
        textBuffer[0] = '\0';
    }
    else if((size_t)result >= sizeof(textBuffer)) {
        textBuffer[sizeof(textBuffer) - 1] = '\0';
    }
    Graphics_DrawText(pGraphics, 48, 0, textBuffer);

    // Draw pollen collection progress bar (top-right)
    Graphics_DrawText(pGraphics, 92, 0, "P:");

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
    // Draw flower outline circle
    Graphics_DrawCircle(pGraphics, centerX, centerY, FLOWER_RADIUS, false);

    // Draw flower petals
    Graphics_DrawEllipse(pGraphics, centerX - 5, centerY, 3, 1, true);
    Graphics_DrawEllipse(pGraphics, centerX + 5, centerY, 3, 1, true);
    Graphics_DrawEllipse(pGraphics, centerX, centerY - 5, 1, 3, true);
    Graphics_DrawEllipse(pGraphics, centerX, centerY + 5, 1, 3, true);

    // Draw center indicator (different for source and target)
    if(isTarget) {
        // Target flower: filled center dot + horizontal line
        Graphics_DrawCircle(pGraphics, centerX, centerY, 2, true);
        Graphics_DrawLine(pGraphics, centerX - 4, centerY, centerX + 4, centerY);
    }
    else {
        // Source flower: small center dot + vertical line
        Graphics_DrawCircle(pGraphics, centerX, centerY, 1, true);
        Graphics_DrawLine(pGraphics, centerX, centerY - 4, centerX, centerY + 4);
    }
} // Renderer_DrawFlower()

/**
 * @brief Draw the bee with animated wings.
 * @details Renders the bee body as a filled circle, animated wings based on frame,
 *          and a stinger. The wing position alternates between two states for
 *          a flapping animation effect.
 *
 * @param pGraphics Pointer to the graphics context used for drawing.
 * @param pBee Pointer to the bee state containing position.
 * @param frame Current animation frame counter.
 */
static void Renderer_DrawBee(const tGraphics *pGraphics, const tBee *pBee, const uint8_t frame) {
    // Validate input pointers
    if(pBee == NULL) {
        return;
    }

    // Round float position to integer pixel coordinates
    const int x = Utils_LRoundFloatToInt(pBee->mPosX);
    const int y = Utils_LRoundFloatToInt(pBee->mPosY);

    // Draw bee body (filled circle)
    Graphics_DrawCircle(pGraphics, x, y, BEE_RADIUS, true);

    // Animated wings (2-frame animation)
    if(frame % 2 == 0) {
        Graphics_DrawLine(pGraphics, x - 3, y - 2, x - 5, y - 4);
        Graphics_DrawLine(pGraphics, x + 3, y - 2, x + 5, y - 4);
    }
    else {
        Graphics_DrawLine(pGraphics, x - 3, y - 1, x - 5, y - 2);
        Graphics_DrawLine(pGraphics, x + 3, y - 1, x + 5, y - 2);
    }

    // Draw stinger
    Graphics_DrawLine(pGraphics, x, y + BEE_RADIUS, x, y + BEE_RADIUS + 2);
} // Renderer_DrawBee()

/**
 * @brief Draw a spider obstacle.
 * @details Renders a spider with a circular body and four legs extending
 *          diagonally from the body center.
 *
 * @param pGraphics Pointer to the graphics context used for drawing.
 * @param x X coordinate of spider center in pixels.
 * @param y Y coordinate of spider center in pixels.
 */
static void Renderer_DrawSpider(const tGraphics *pGraphics, const int x, const int y) {
    // Spider body
    Graphics_DrawCircle(pGraphics, x, y, 3, true);

    // Spider legs
    Graphics_DrawLine(pGraphics, x - 5, y - 3, x - 2, y - 1);
    Graphics_DrawLine(pGraphics, x - 5, y + 3, x - 2, y + 1);
    Graphics_DrawLine(pGraphics, x + 5, y - 3, x + 2, y - 1);
    Graphics_DrawLine(pGraphics, x + 5, y + 3, x + 2, y + 1);
} // Renderer_DrawSpider()

/**
 * @brief Draw a raindrop obstacle.
 * @details Renders a teardrop shape using a vertical line and two side pixels.
 *
 * @param pGraphics Pointer to the graphics context used for drawing.
 * @param x X coordinate of raindrop top in pixels.
 * @param y Y coordinate of raindrop top in pixels.
 */
static void Renderer_DrawRainDrop(const tGraphics *pGraphics, const int x, const int y) {
    // Raindrop shape
    Graphics_DrawEllipse(pGraphics, x, y + 2, 2, 3, true);
    Graphics_DrawLine(pGraphics, x, y, x, y + 2);
    Graphics_SetPixel(pGraphics, x - 1, y + 2, true);
    Graphics_SetPixel(pGraphics, x + 1, y + 2, true);
} // Renderer_DrawRainDrop()

/**
 * @brief Draw a power-up collectible.
 * @details Renders different visual representations based on power-up type.
 *          Shield power-up: circle outline
 *          Honey power-up: filled hexagon
 *
 * @param pGraphics Pointer to the graphics context used for drawing.
 * @param x X coordinate of power-up center in pixels.
 * @param y Y coordinate of power-up center in pixels.
 * @param type Type of power-up (shield or honey).
 */
static void Renderer_DrawPowerUp(const tGraphics *pGraphics, const int x,
                                 const int y, const ePowerUpType type) {
    // Draw shield
    if(type == SHIELD) {
        Graphics_DrawCircle(pGraphics, x, y, POWERUP_RADIUS, false);
        Graphics_DrawLine(pGraphics, x - 2, y, x + 2, y);
    }
    //Draw honey
    else if(type == HONEY) {
        Graphics_DrawCircle(pGraphics, x, y, POWERUP_RADIUS - 1, true);
        Graphics_SetPixel(pGraphics, x - 3, y, true);
        Graphics_SetPixel(pGraphics, x + 3, y, true);
    }
} // Renderer_DrawPowerUp()

/**
 * @brief Draw active power-up indicators.
 * @details Displays small icons in the top-left corner for currently active
 *          power-ups (shield and/or honey boost).
 *
 * @param pGame Pointer to the current game state.
 * @param pGraphics Pointer to the graphics context used for drawing.
 */
static void Renderer_DrawActivePowerUps(const tGame *pGame, const tGraphics *pGraphics) {
    int offsetX = 0;

    // Draw shield indicator if active
    if(pGame->mShield.mIsActive) {
        Graphics_DrawCircle(pGraphics, 6 + offsetX, 52, 3, false);
        offsetX += 10;
    }

    // Draw honey boost indicator if active
    if(pGame->mHoney.mIsActive) {
        Graphics_DrawCircle(pGraphics, 6 + offsetX, 52, 2, true);
    }
} // Renderer_DrawActivePowerUps()

/**
 * @brief Render all active spiders.
 *
 * @param pGame Pointer to the current game state.
 * @param pGraphics Pointer to the graphics context used for drawing.
 */
static void Renderer_DrawSpiders(const tGame *pGame, const tGraphics *pGraphics) {
    for(int iSpider = 0; iSpider < MAX_SPIDERS; iSpider++) {
        if(pGame->mSpiders[iSpider].mIsActive) {
            Renderer_DrawSpider(pGraphics,
                                Utils_LRoundFloatToInt(pGame->mSpiders[iSpider].mPosX),
                                Utils_LRoundFloatToInt(pGame->mSpiders[iSpider].mPosY));
        }
    }
} // Renderer_DrawSpiders()

/**
 * @brief Render all active raindrops.
 *
 * @param pGame Pointer to the current game state.
 * @param pGraphics Pointer to the graphics context used for drawing.
 */
static void Renderer_DrawRainDrops(const tGame *pGame, const tGraphics *pGraphics) {
    for(int iRaindrop = 0; iRaindrop < MAX_RAINDROPS; iRaindrop++) {
        if(pGame->mRainDrops[iRaindrop].mIsActive) {
            Renderer_DrawRainDrop(pGraphics,
                                  Utils_LRoundFloatToInt(pGame->mRainDrops[iRaindrop].mPosX),
                                  Utils_LRoundFloatToInt(pGame->mRainDrops[iRaindrop].mPosY));
        }
    }
} // Renderer_DrawRainDrops()

/**
 * @brief Render all active collectible power-ups.
 *
 * @param pGame Pointer to the current game state.
 * @param pGraphics Pointer to the graphics context used for drawing.
 */
static void Renderer_DrawPowerUps(const tGame *pGame, const tGraphics *pGraphics) {
    for(int iPowerUp = 0; iPowerUp < MAX_POWERUPS; iPowerUp++) {
        if(pGame->mPowerUps[iPowerUp].mIsActive) {
            Renderer_DrawPowerUp(pGraphics,
                                 Utils_LRoundFloatToInt(pGame->mPowerUps[iPowerUp].mPosX),
                                 Utils_LRoundFloatToInt(pGame->mPowerUps[iPowerUp].mPosY),
                                 pGame->mPowerUps[iPowerUp].mType);
        }
    }
} // Renderer_DrawPowerUps()

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
        Graphics_DrawText(pGraphics, 12, 38, "Joystick button: start/pause");
        Graphics_DrawText(pGraphics, 12, 52, "Press joystick button to start...");
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
    Renderer_DrawFlower(pGraphics, pGame->mSourceFlower.mPosX, pGame->mSourceFlower.mPosY, false);
    Renderer_DrawFlower(pGraphics, pGame->mTargetFlower.mPosX, pGame->mTargetFlower.mPosY, true);

    // Render all game entities
    Renderer_DrawSpiders(pGame, pGraphics);
    Renderer_DrawRainDrops(pGame, pGraphics);
    Renderer_DrawPowerUps(pGame, pGraphics);
    Renderer_DrawBee(pGraphics, &pGame->mBee, pGame->mBeeFrame);

    // Draw active power-up indicators
    Renderer_DrawActivePowerUps(pGame, pGraphics);

    // Render pause overlay
    if(pGame->mState == STATE_PAUSE) {
        Graphics_DrawRectangle(pGraphics, 20, 22, 88, 20, 0);
        Graphics_DrawText(pGraphics, 42, 28, "PAUSED");
        Graphics_DrawText(pGraphics, 28, 40, "Press joystick...");
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
        Graphics_DrawText(pGraphics, 18, 52, "Press joystick to retry");
    }
} // Renderer_Draw()

/*** end of file Renderer.c ***/
