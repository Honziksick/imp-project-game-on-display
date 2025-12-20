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
 * Description:  Implements core gameplay logic and state management.          *
 *               Responsible for round initialization, mapping joystick input  *
 *               to bee velocity, integrating movement, enforcing screen       *
 *               bounds, handling pollen collection/delivery, updating score   *
 *               and the game timer, and transitioning between game states.    *
 *                                                                             *
 ******************************************************************************/
/**
 * @file GameLogic.c
 * @author Jan Kalina \<xkalinj00>
 * @brief Implements core gameplay logic and state management.
 */

#include "public/GameLogic.h"
#include "public/Utils.h"
#include "public/SSD1306.h"
#include "structure/tGame.h"
#include "structure/tFlower.h"
#include <stddef.h>  // NULL

/**
 * @brief Place a flower at a random position avoiding overlap with another flower.
 * @details Attempts multiple random placements to find a valid position that
 *          does not overlap with the specified other flower. If no valid position
 *          is found after the maximum attempts, places the flower at the center
 *          as a fallback.
 *
 * @param pFlower Pointer to the flower to be placed.
 * @param pOtherFlower Pointer to another flower to avoid overlapping with (can be NULL).
 */
static void GameLogic_PlaceFlower(tFlower *pFlower, const tFlower *pOtherFlower) {
    // Validate input pointer
    if(pFlower == NULL) {
        return;
    }

    // Try multiple random placements
    for(int iAttempt = 0; iAttempt < GAME_LOGIC_PLACEMENT_ATTEMPTS; iAttempt++) {
        // Generate random position with margin from edges
        const int randomX = Utils_GetRandomInRange(FLOWER_RADIUS + 2, SSD1306_WIDTH - FLOWER_RADIUS - 3);
        const int randomY = Utils_GetRandomInRange(FLOWER_RADIUS + 10, SSD1306_HEIGHT - FLOWER_RADIUS - 3);

        // Check for overlap with other flower
        if(pOtherFlower) {
            const int deltaX = randomX - pOtherFlower->mPosX;
            const int deltaY = randomY - pOtherFlower->mPosY;

            // If too close, try again (minimum distance is 6 flower radii)
            const int minDistance = GAME_LOGIC_MIN_FLOWER_DISTANCE_MULTIPLIER * FLOWER_RADIUS;
            const int minDistanceSquare = minDistance * minDistance;
            if(deltaX * deltaX + deltaY * deltaY < minDistanceSquare) {
                continue;
            }
        }

        // Position is valid
        pFlower->mPosX = randomX;
        pFlower->mPosY = randomY;
        return;
    }

    // Fallback if no valid position found (just in case)
    pFlower->mPosX = SSD1306_WIDTH / 2;
    pFlower->mPosY = SSD1306_HEIGHT / 2;
} // GameLogic_PlaceFlower()

void GameLogic_NewRound(tGame *pGame) {
    // Validate input pointer
    if(pGame == NULL) {
        return;
    }

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
} // GameLogic_NewRound()

void GameLogic_StartGame(tGame *pGame) {
    // Validate input pointer
    if(pGame == NULL) {
        return;
    }

    // Reset score and timer
    pGame->mScore = 0;
    pGame->mTimeLeftSec = GAME_TIME_SEC;

    // Setup first round
    GameLogic_NewRound(pGame);

    // Transition to play state
    pGame->mState = STATE_PLAY;
} // GameLogic_StartGame()

void GameLogic_UpdatePlay(tGame *pGame, const float deltaTime, const float normalizedX, const float normalizedY) {
    if(pGame == NULL) {
        return;
    }

    // ignore non-positive timestep
    if(deltaTime <= 0.0f) {
        return;
    }

    // normalizované vstupy omezit na rozmezí [-1,1]
    const float clampedX = Utils_ClampFloat(normalizedX, -1.0f, 1.0f);
    const float clampedY = Utils_ClampFloat(normalizedY, -1.0f, 1.0f);

    // Map joystick input to bee velocity
    pGame->mBee.mVelocityX = clampedX * MAX_VELOCITY_X;
    pGame->mBee.mVelocityY = clampedY * MAX_VELOCITY_Y;

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

    // Calculate collision radii
    const float radius = (float)(FLOWER_RADIUS + 1);
    const float radiusSquared = radius * radius;
    const float sourceRadiusSquared = radiusSquared;
    const float targetRadiusSquared = radiusSquared;

    const float beeX = pGame->mBee.mPosX;
    const float beeY = pGame->mBee.mPosY;

    // Pollen collection logic
    if(!pGame->mBee.mHasPollen) {
        // Collecting pollen at source flower
        const float distanceSquared = Utils_DistanceSquareFloat(beeX, beeY,
                                                                (float)pGame->mSourceFlower.mPosX,
                                                                (float)pGame->mSourceFlower.mPosY);

        if(distanceSquared <= sourceRadiusSquared) {
            // Bee is inside source flower -> accumulate pollen
            pGame->mBee.mPollenFill += deltaTime / COLLECT_TIME_SEC;

            // Check if collection is complete
            if(pGame->mBee.mPollenFill >= 1.0f) {
                pGame->mBee.mPollenFill = 1.0f;
                pGame->mBee.mHasPollen = true;
            }
        }
        else {
            // Bee left flower --> slowly decay progress
            pGame->mBee.mPollenFill -= deltaTime * 0.35f;
            if(pGame->mBee.mPollenFill < 0.0f) {
                pGame->mBee.mPollenFill = 0.0f;
            }
        }
    }
    // Pollen delivery logic
    else {
        // Delivering pollen to target flower
        const float distSquared = Utils_DistanceSquareFloat(beeX, beeY,
                                                            (float)pGame->mSourceFlower.mPosX,
                                                            (float)pGame->mTargetFlower.mPosY);

        // Successful delivery
        if(distSquared <= targetRadiusSquared) {
            pGame->mScore += 1;
            GameLogic_NewRound(pGame);
        }
    }
} // GameLogic_UpdatePlay()

/*** end of file GameLogic.c ***/
