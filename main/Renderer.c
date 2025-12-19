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
 * Description:                                                                *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Joystick.c
 * @author Jan Kalina \<xkalinj00>
 * @brief
 */

#include "public/Renderer.h"
#include "public/Graphics.h"
#include "public/Utils.h"
#include "structure/tGame.h"
#include "structure/tGraphics.h"
#include "structure/tBee.h"
#include <stdio.h>
#include <math.h>

static void Renderer_DrawHUD(tGame *pGame, tGraphics *pGraphics) {
    char textBuffer[32];

    // Draw score (top-left)
    snprintf(textBuffer, sizeof(textBuffer), "S:%d", pGame->mScore);
    Graphics_DrawText(pGraphics, 0, 0, textBuffer);

    // Draw remaining time (top-center)
    snprintf(textBuffer, sizeof(textBuffer), "T:%02d", (int)ceilf(pGame->mTimeLeftSec));
    Graphics_DrawText(pGraphics, 48, 0, textBuffer);

    // Draw pollen collection progress bar (top-right)
    Graphics_DrawText(pGraphics, 92, 0, "P");

    // Bar background rectangle
    int barX = 102, barY = 0, barWidth = 24, barHeight = 7;
    Graphics_DrawRectangle(pGraphics, barX, barY, barWidth, barHeight, 0);

    // Calculate filled portion based on pollen progress
    int fillWidth = (int)floorf((barWidth - 2) * Utils_ClampFloat(pGame->mBee.mPollenFill, 0.0f, 1.0f));
    if(pGame->mBee.mHasPollen) {
        fillWidth = (barWidth - 2);
    }

    // Draw filled portion
    if(fillWidth > 0) {
        Graphics_DrawRectangle(pGraphics, barX + 1, barY + 1, fillWidth, barHeight - 2, 1);
    }
}

static void Renderer_DrawFlower(tGraphics *pGraphics, int x, int y, int isTarget) {
    // Draw flower outline circle
    Graphics_DrawCircle(pGraphics, x, y, FLOWER_RADIUS, 0);

    // Draw center indicator (different for source vs target)
    if(isTarget) {
        // Target: filled center dot + horizontal line
        Graphics_DrawCircle(pGraphics, x, y, 2, 1);
        Graphics_DrawLine(pGraphics, x - 4, y, x + 4, y);
    }
    else {
        // Source: small center dot + vertical line
        Graphics_DrawCircle(pGraphics, x, y, 1, 1);
        Graphics_DrawLine(pGraphics, x, y - 4, x, y + 4);
    }
}

static void Renderer_DrawBee(tGraphics *pGraphics, const tBee *pBee) {
    // Round float position to integer pixel coordinates
    int pixelX = (int)lroundf(pBee->mPosX);
    int pixelY = (int)lroundf(pBee->mPosY);

    // Draw bee body (filled circle)
    Graphics_DrawCircle(pGraphics, pixelX, pixelY, BEE_RADIUS, 1);

    // Draw wing pixels
    Graphics_SetPixel(pGraphics, pixelX - 3, pixelY - 2, 1);
    Graphics_SetPixel(pGraphics, pixelX + 3, pixelY - 2, 1);
    Graphics_SetPixel(pGraphics, pixelX - 2, pixelY - 3, 1);
    Graphics_SetPixel(pGraphics, pixelX + 2, pixelY - 3, 1);
}

void Renderer_Draw(tGame *pGame, tGraphics *pGraphics, float normalizedX, float normalizedY) {
    (void)normalizedX;
    (void)normalizedY;

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
        snprintf(scoreBuffer, sizeof(scoreBuffer), "Score: %d", pGame->mScore);
        Graphics_DrawText(pGraphics, 26, 34, scoreBuffer);
        Graphics_DrawText(pGraphics, 18, 52, "Press SW to retry");
    }
}

/*** end of file Renderer.c ***/
