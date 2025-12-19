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
#include "tJoystick.h"
#include "tButton.h"
#include "tFlower.h"
#include "tBee.h"

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
    eGameState mState;          /**< Current game state.                   */
    tBee mBee;                  /**< Player-controlled bee.                */
    tFlower mSourceFlower;      /**< Pollen source flower.                 */
    tFlower mTargetFlower;      /**< Pollen delivery target flower         */
    int mScore;                 /**< Player score (successful deliveries). */
    float mTimeLeftSec;         /**< Remaining game time (seconds).        */
    tJoystick mJoystick;        /**< Calibrated & filtered joystick.       */
    tButton mButton;            /**< Debounced button state.               */
} tGame;

#endif // T_GAME_H

/*** end of file tGame.h ***/
