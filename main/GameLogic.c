/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         GameLogic.c                                                   *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:                                                                *
 *                                                                             *
 ******************************************************************************/
/**
 * @file GameLogic.c
 * @author Jan Kalina \<xkalinj00>
 * @brief
 */

#include "public/Utils.h"
#include "public/SSD1306.h"
#include "structure/tGame.h"
#include "structure/tFlower.h"
#include <stddef.h>  // NULL

static void GameLogic_PlaceFlower(tFlower *pFlower, const tFlower *pOtherFlower) {
    // Try multiple random placements
    for(int iAttempt = 0; iAttempt < 200; iAttempt++) {
        // Generate random position with margin from edges
        const int randomX = Utils_GetRandomInRange(FLOWER_RADIUS + 2, SSD1306_WIDTH - FLOWER_RADIUS - 3);
        const int randomY = Utils_GetRandomInRange(FLOWER_RADIUS + 10, SSD1306_HEIGHT - FLOWER_RADIUS - 3);

        // Check for overlap with other flower
        if(pOtherFlower) {
            const int deltaX = randomX - pOtherFlower->mPosX;
            const int deltaY = randomY - pOtherFlower->mPosY;

            // If too close, try again (minimum distance: 6 flower radii)
            if(deltaX * deltaX + deltaY * deltaY < (FLOWER_RADIUS * FLOWER_RADIUS * 6)) {
                continue;
            }
        }

        // Position is valid
        pFlower->mPosX = randomX;
        pFlower->mPosY = randomY;
        return;
    }

    // Fallback if no valid position found (rare)
    pFlower->mPosX = SSD1306_WIDTH / 2;
    pFlower->mPosY = SSD1306_HEIGHT / 2;
}

void GameLogic_NewRound(tGame *pGame) {
    // Place flowers in non-overlapping positions
    GameLogic_PlaceFlower(&pGame->mSourceFlower, NULL);
    GameLogic_PlaceFlower(&pGame->mTargetFlower, &pGame->mSourceFlower);

    // Reset bee pollen state
    pGame->mBee.mHasPollen = false;
    pGame->mBee.mPollenFill = 0.0f;

    // Respawn bee near bottom-center of screen
    pGame->mBee.mPosX = SSD1306_WIDTH * 0.5f;
    pGame->mBee.mPosY = SSD1306_HEIGHT * 0.65f;
    pGame->mBee.mVelocityX = 0.0f;
    pGame->mBee.mVelocityY = 0.0f;
}

void GameLogic_StartGame(tGame *pGame) {
    // Reset score and timer
    pGame->mScore = 0;
    pGame->mTimeLeftSec = GAME_TIME_SEC;

    // Setup first round
    GameLogic_NewRound(pGame);

    // Transition to play state
    pGame->mState = STATE_PLAY;
}

void GameLogic_UpdatePlay(tGame *pGame, const float deltaTime, const float normalizedX, const float normalizedY) {
    // Map joystick input to bee velocity
    pGame->mBee.mVelocityX = normalizedX * MAX_VELOCITY_X;
    pGame->mBee.mVelocityY = normalizedY * MAX_VELOCITY_Y;

    // Integrate velocity to update position (Euler integration)
    pGame->mBee.mPosX += pGame->mBee.mVelocityX * deltaTime;
    pGame->mBee.mPosY += pGame->mBee.mVelocityY * deltaTime;

    // Clamp bee position to screen bounds (leave top HUD area free)
    pGame->mBee.mPosX = Utils_ClampFloat(pGame->mBee.mPosX, (float)BEE_RADIUS, (float)(SSD1306_WIDTH - 1 - BEE_RADIUS));
    pGame->mBee.mPosY = Utils_ClampFloat(pGame->mBee.mPosY, 10.0f + (float)BEE_RADIUS, (float)(SSD1306_HEIGHT - 1 - BEE_RADIUS));

    // Update game timer
    pGame->mTimeLeftSec -= deltaTime;
    if(pGame->mTimeLeftSec <= 0.0f) {
        pGame->mTimeLeftSec = 0.0f;
        pGame->mState = STATE_GAMEOVER;
        return;
    }

    // Calculate collision radii (squared distances for efficiency)
    const float beeX = pGame->mBee.mPosX;
    const float beeY = pGame->mBee.mPosY;
    const float sourceRadiusSquared = (float)((FLOWER_RADIUS + 1) * (FLOWER_RADIUS + 1));
    const float targetRadiusSquared = (float)((FLOWER_RADIUS + 1) * (FLOWER_RADIUS + 1));

    // Pollen collection/delivery logic
    if(!pGame->mBee.mHasPollen) {
        // Collecting pollen at source flower
        const float distanceSquared = Utils_DistanceSquareFloat(beeX, beeY,
                                                                (float)pGame->mSourceFlower.mPosX,
                                                                (float)pGame->mSourceFlower.mPosY);

        if(distanceSquared <= sourceRadiusSquared) {
            // Bee is inside source flower - accumulate pollen
            pGame->mBee.mPollenFill += deltaTime / COLLECT_TIME_SEC;

            // Check if collection complete
            if(pGame->mBee.mPollenFill >= 1.0f) {
                pGame->mBee.mPollenFill = 1.0f;
                pGame->mBee.mHasPollen = true;
            }
        }
        else {
            // Bee left flower - slowly decay progress
            pGame->mBee.mPollenFill -= deltaTime * 0.35f;
            if(pGame->mBee.mPollenFill < 0.0f) {
                pGame->mBee.mPollenFill = 0.0f;
            }
        }
    }
    else {
        // Delivering pollen to target flower
        const float distSquared = Utils_DistanceSquareFloat(beeX, beeY, (float)pGame->mSourceFlower.mPosX, (float)pGame->mTargetFlower.mPosY);

        if(distSquared <= targetRadiusSquared) {
            // Successful delivery!
            pGame->mScore += 1;
            GameLogic_NewRound(pGame);
        }
    }
}

/*** end of file GameLogic.c ***/
