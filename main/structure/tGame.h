/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tGame.h                                                       *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tGame structure which aggregates the complete     *
 *               runtime state of the application. The structure contains      *
 *               the current game state enumerator, the player-controlled bee, *
 *               the source and target flower descriptors, the player's score, *
 *               the remaining game time (seconds), and input state objects    *
 *               (calibrated joystick and debounced button).                   *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tGame.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Structure definition for the overall game state.
 */

#ifndef T_GAME_H
#define T_GAME_H

#include "enum/eGameState.h"
#include "structure/tJoystick.h"
#include "structure/tButton.h"
#include "structure/tBee.h"
#include "structure/tFlower.h"
#include "structure/tSpider.h"
#include "structure/tRainDrop.h"
#include "structure/tPowerUp.h"
#include "structure/tShield.h"
#include "structure/tHoney.h"

#define COLLECT_TIME_SEC 1.2f      /**< Time to collect pollen (seconds) */
#define GAME_TIME_SEC    60.0f     /**< Total game duration (seconds) */

/**
 * @struct tGame
 * @brief Structure representing the overall game state.
 *
 * @details This structure aggregates all relevant components of the game's
 *          runtime state, including the current game state, player-controlled
 *          bee, source and target flowers, player score, remaining time, and
 *          input states.
 */
typedef struct {
    eGameState mState;                      /**< Current game state.                   */
    tBee mBee;                              /**< Player-controlled bee.                */
    uint8_t mBeeFrame;                      /**< Current animation frame of the bee.   */
    tFlower mSourceFlower;                  /**< Pollen source flower.                 */
    tFlower mTargetFlower;                  /**< Pollen delivery target flower         */
    tSpider mSpiders[MAX_SPIDERS];          /**< Active spiders in the game.           */
    tRainDrop mRainDrops[MAX_RAINDROPS];    /**< Active raindrops in the game.         */
    tPowerUp mPowerUps[MAX_POWERUPS];       /**< Active power-ups in the game.         */
    tShield mShield;                        /**< Shield power-up.                      */
    tHoney mHoney;                          /**< Honey (speed boost) power-up.         */
    int mScore;                             /**< Player score (successful deliveries). */
    float mTimeLeftSec;                     /**< Remaining game time (seconds).        */
    tJoystick mJoystick;                    /**< Calibrated & filtered joystick.       */
    tButton mButton;                        /**< Debounced button state.               */
} tGame;

#endif // T_GAME_H

/*** end of file tGame.h ***/
