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
 * Last edit:    20.12.2025                                                    *
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
#include "enum/ePowerUpType.h"
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

/**
 * @brief Randomly spawn a new collectible power-up.
 *
 * @param pGame Pointer to the game state structure.
 */
static void GameLogic_SpawnPowerUp(tGame *pGame) {
    // Low probability spawn check (0.2% per frame)
    if(Utils_GenerateRandomU32() % 1000 >= 2) {
        return;
    }

    // Find first inactive power-up slot
    for(int iPowerUp = 0; iPowerUp < MAX_POWERUPS; iPowerUp++) {
        if(!pGame->mPowerUps[iPowerUp].mIsActive) {
            // Generate random position
            pGame->mPowerUps[iPowerUp].mPosX = (float)Utils_GetRandomInRange(POWERUP_RADIUS + 2, SSD1306_WIDTH - POWERUP_RADIUS - 2);
            pGame->mPowerUps[iPowerUp].mPosY = (float)Utils_GetRandomInRange(15 + POWERUP_RADIUS, SSD1306_HEIGHT - POWERUP_RADIUS - 2);

            // Randomly choose power-up type
            pGame->mPowerUps[iPowerUp].mType = (Utils_GenerateRandomU32() % 2 == 0) ? SHIELD : HONEY;
            pGame->mPowerUps[iPowerUp].mIsActive = true;
            break;
        }
    }
} // GameLogic_SpawnPowerUp()

/**
 * @brief Reset all game obstacles to inactive state.
 *
 * @param pGame Pointer to the game state structure.
 */
static void GameLogic_ResetObstacles(tGame *pGame) {
    // Deactivate all spiders
    for(int iSpider = 0; iSpider < MAX_SPIDERS; iSpider++) {
        pGame->mSpiders[iSpider].mIsActive = false;
    }

    // Deactivate all raindrops
    for(int iRaindrop = 0; iRaindrop < MAX_RAINDROPS; iRaindrop++) {
        pGame->mRainDrops[iRaindrop].mIsActive = false;
    }
} // GameLogic_ResetObstacles()

/**
 * @brief Reset all power-ups to inactive state.
 *
 * @param pGame Pointer to the game state structure.
 */
static void GameLogic_ResetPowerUps(tGame *pGame) {
    // Deactivate all collectible power-ups
    for(int iPowerUp = 0; iPowerUp < MAX_POWERUPS; iPowerUp++) {
        pGame->mPowerUps[iPowerUp].mIsActive = false;
    }

    // Reset active power-up effects
    pGame->mShield.mIsActive = false;
    pGame->mShield.mTimeLeft = 0.0f;
    pGame->mHoney.mIsActive = false;
    pGame->mHoney.mTimeLeft = 0.0f;
} // GameLogic_ResetPowerUps()

/**
 * @brief Calculate velocity multiplier based on active power-ups.
 *
 * @param pGame Pointer to the game state structure.
 * @return Velocity multiplier (1.0 for normal speed, higher for honey boost).
 */
static float GameLogic_GetVelocityMultiplier(const tGame *pGame) {
    return pGame->mHoney.mIsActive ? HONEY_MULTIPLIER : 1.0f;
} // GameLogic_GetVelocityMultiplier()

/**
 * @brief Update active power-up timers and deactivate expired ones.
 *
 * @param pGame Pointer to the game state structure.
 * @param deltaTime Time elapsed since last update in seconds.
 */
static void GameLogic_UpdatePowerUpTimers(tGame *pGame, const float deltaTime) {
    // Update shield boost timer
    if(pGame->mShield.mIsActive) {
        pGame->mShield.mTimeLeft -= deltaTime;

        if(pGame->mShield.mTimeLeft <= 0.0f) {
            pGame->mShield.mIsActive = false;
            pGame->mShield.mTimeLeft = 0.0f;
        }
    }

    // Update honey boost timer
    if(pGame->mHoney.mIsActive) {
        pGame->mHoney.mTimeLeft -= deltaTime;

        if(pGame->mHoney.mTimeLeft <= 0.0f) {
            pGame->mHoney.mIsActive = false;
            pGame->mHoney.mTimeLeft = 0.0f;
        }
    }
} // GameLogic_UpdatePowerUpTimers()

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

    // Reset animation frame
    pGame->mBeeFrame = 0;

    // Reset all obstacles and power-ups
    GameLogic_ResetObstacles(pGame);
    GameLogic_ResetPowerUps(pGame);

    // Spawn initial obstacles
    GameLogic_SpawnSpiders(pGame);
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

    // Reset animation frame
    pGame->mBeeFrame = 0;

    // Reset all game elements
    GameLogic_ResetObstacles(pGame);
    GameLogic_ResetPowerUps(pGame);

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

    // Update rain system
    GameLogic_UpdateRain(pGame, deltaTime);

    // Update power-up timers
    GameLogic_UpdatePowerUpTimers(pGame, deltaTime);

    // Calculate velocity multiplier from active power-ups
    const float velocityMultiplier = GameLogic_GetVelocityMultiplier(pGame);

    // Clamp normalized inputs to [-1,1] range
    const float clampedX = Utils_ClampFloat(normalizedX, -1.0f, 1.0f);
    const float clampedY = Utils_ClampFloat(normalizedY, -1.0f, 1.0f);

    // Map joystick input to bee velocity with multiplier
    pGame->mBee.mVelocityX = clampedX * MAX_VELOCITY_X * velocityMultiplier;
    pGame->mBee.mVelocityY = clampedY * MAX_VELOCITY_Y * velocityMultiplier;

    // Integrate velocity to update position (Euler integration)
    pGame->mBee.mPosX += pGame->mBee.mVelocityX * deltaTime;
    pGame->mBee.mPosY += pGame->mBee.mVelocityY * deltaTime;

    // Clamp bee position to screen bounds (leave top HUD area free)
    pGame->mBee.mPosX = Utils_ClampFloat(pGame->mBee.mPosX, (float)BEE_RADIUS, (float)(SSD1306_WIDTH - 1 - BEE_RADIUS));
    pGame->mBee.mPosY = Utils_ClampFloat(pGame->mBee.mPosY, 10.0f + (float)BEE_RADIUS, (float)(SSD1306_HEIGHT - 1 - BEE_RADIUS));

    // Check collisions with obstacles and power-ups
    GameLogic_CheckCollisions(pGame);

    // If game over from collision, return early
    if(pGame->mState == STATE_GAMEOVER) {
        return;
    }

    // Update animation frame
    pGame->mBeeFrame = (pGame->mBeeFrame + 1) % 60;

    // Randomly spawn power-ups
    GameLogic_SpawnPowerUp(pGame);

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

void GameLogic_SpawnSpiders(tGame *pGame) {
    for(int iSpider = 0; iSpider < MAX_SPIDERS; iSpider++) {
        if(!pGame->mSpiders[iSpider].mIsActive) {
            pGame->mSpiders[iSpider].mPosX = (float)Utils_GetRandomInRange(10, SSD1306_WIDTH - 10);
            pGame->mSpiders[iSpider].mPosY = (float)Utils_GetRandomInRange(15, SSD1306_HEIGHT - 10);
            pGame->mSpiders[iSpider].mIsActive = true;
            break;
        }
    }
} // GameLogic_SpawnSpiders()

void GameLogic_UpdateRain(tGame *pGame, const float deltaTime) {
    // Spawn rate is 5% chance per frame
    if(Utils_GenerateRandomU32() % 100 < 5) {
        for(int iRainDrop = 0; iRainDrop < MAX_RAINDROPS; iRainDrop++) {
            if(!pGame->mRainDrops[iRainDrop].mIsActive) {
                pGame->mRainDrops[iRainDrop].mPosX = (float)Utils_GetRandomInRange(0, SSD1306_WIDTH);
                pGame->mRainDrops[iRainDrop].mPosY = 10.0f;
                pGame->mRainDrops[iRainDrop].mVelocityY = RAINDROP_FALL_SPEED;
                pGame->mRainDrops[iRainDrop].mIsActive = true;
                break;
            }
        }
    }

    // Update existing rain drops
    for(int iRaindrop = 0; iRaindrop < MAX_RAINDROPS; iRaindrop++) {
        if(pGame->mRainDrops[iRaindrop].mIsActive) {
            pGame->mRainDrops[iRaindrop].mPosY += pGame->mRainDrops[iRaindrop].mVelocityY * deltaTime;

            // Remove if off-screen
            if(pGame->mRainDrops[iRaindrop].mPosY > SSD1306_HEIGHT) {
                pGame->mRainDrops[iRaindrop].mIsActive = false;
            }
        }
    }
}

void GameLogic_CheckCollisions(tGame *pGame) {
    const float beeX = pGame->mBee.mPosX;
    const float beeY = pGame->mBee.mPosY;

    // Check spider collision
    if(!pGame->mShield.mIsActive) {
        for(int iSpider = 0; iSpider < MAX_SPIDERS; iSpider++) {
            if(pGame->mSpiders[iSpider].mIsActive) {
                const float distanceSquare = Utils_DistanceSquareFloat(beeX, beeY,
                                                                       pGame->mSpiders[iSpider].mPosX,
                                                                       pGame->mSpiders[iSpider].mPosY);

                if(distanceSquare < (BEE_RADIUS + SPIDER_RADIUS) * (BEE_RADIUS + SPIDER_RADIUS)) {
                    pGame->mState = STATE_GAMEOVER;
                    return;
                }
            }
        }
    }

    // Check raindrop collision (slowdown)
    for(int iRaindrop = 0; iRaindrop < MAX_RAINDROPS; iRaindrop++) {
        if(pGame->mRainDrops[iRaindrop].mIsActive) {
            const float distanceSquare = Utils_DistanceSquareFloat(beeX, beeY,
                                                                   pGame->mRainDrops[iRaindrop].mPosX,
                                                                   pGame->mRainDrops[iRaindrop].mPosY);

            if(distanceSquare < 16.0f) {
                pGame->mBee.mVelocityX *= RAINDROP_SLOWDOWN;
                pGame->mBee.mVelocityY *= RAINDROP_SLOWDOWN;
            }
        }
    }

    // Check powerup collection
    for(int iPowerUp = 0; iPowerUp < MAX_POWERUPS; iPowerUp++) {
        if(pGame->mPowerUps[iPowerUp].mIsActive) {
            const float dist = Utils_DistanceSquareFloat(beeX, beeY,
                                                         pGame->mPowerUps[iPowerUp].mPosX,
                                                         pGame->mPowerUps[iPowerUp].mPosY);

            if(dist < (BEE_RADIUS + POWERUP_RADIUS) * (BEE_RADIUS + POWERUP_RADIUS)) {
                if(pGame->mPowerUps[iPowerUp].mType == 0) {
                    pGame->mShield.mIsActive = true;
                    pGame->mShield.mTimeLeft = SHIELD_DURATION;
                }
                else {
                    pGame->mHoney.mIsActive = true;
                    pGame->mHoney.mTimeLeft = HONEY_DURATION;
                }

                pGame->mPowerUps[iPowerUp].mIsActive = false;
            }
        }
    }
} // GameLogic_CheckCollisions()

/*** end of file GameLogic.c ***/
