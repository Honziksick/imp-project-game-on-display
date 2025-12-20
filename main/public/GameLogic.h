/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         GameLogic.h                                                   *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Public API for core game logic controlling gameplay flow,     *
 *               round initialization, and state updates. Provides functions   *
 *               to start a new game session, initialize individual rounds,    *
 *               and update game state each frame based on elapsed time and    *
 *               normalized joystick input.                                    *
 *                                                                             *
 ******************************************************************************/
/**
 * @file GameLogic.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Public API for core game logic controlling gameplay flow.
 */

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "structure/tGame.h"

#define GAME_LOGIC_PLACEMENT_ATTEMPTS 200            /**< Maximal attempts to place flowers without overlap.       */
#define GAME_LOGIC_MIN_FLOWER_DISTANCE_MULTIPLIER 6  /**< Minimal distance between flowers in multiples of radius. */

/**
 * @brief Initializes a new round in the game.
 * @details Resets necessary game parameters to start a new round.
 *
 * @param pGame Pointer to the game structure.
 */
void GameLogic_NewRound(tGame *pGame);

/**
 * @brief Starts the game.
 * @details Initializes game parameters to begin gameplay.
 *
 * @param pGame Pointer to the game structure.
 */
void GameLogic_StartGame(tGame *pGame);

/**
 * @brief Updates the game state during play.
 * @details Updates game parameters based on elapsed time and player input.
 *
 * @param pGame Pointer to the game structure.
 * @param deltaTime Time elapsed since the last update (in seconds).
 * @param normalizedX Normalized X coordinate of player input (range: -1.0 to 1.0).
 * @param normalizedY Normalized Y coordinate of player input (range: -1.0 to 1.0).
 */
void GameLogic_UpdatePlay(tGame *pGame, float deltaTime, float normalizedX, float normalizedY);

#endif // GAME_LOGIC_H

/*** end of file GameLogic.h ***/
