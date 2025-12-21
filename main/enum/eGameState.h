/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         eGameState.h                                                  *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    20.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the eGameState enumeration representing the major     *
 *               runtime states of the game. Intended for use by the main      *
 *               loop, input handling and rendering subsystems to control      *
 *               application flow and UI transitions.                          *
 *                                                                             *
 ******************************************************************************/
/**
 * @file eGameState.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Game state enumeration for controlling application flow and UI.
 */

#ifndef E_GAME_STATE_H
#define E_GAME_STATE_H

/**
 * @enum eGameState
 * @brief Enumeration of the main runtime states of the game.
 *
 * @details This enumeration defines the various states the game can be in,
 *          allowing different subsystems to adjust their behavior accordingly.
 */
typedef enum {
    STATE_HOME = 0,         /**< Title screen / instructions */
    STATE_CALIB,            /**< Joystick calibration        */
    STATE_PLAY,             /**< Active gameplay             */
    STATE_PAUSE,            /**< Paused state                */
    STATE_GAMEOVER          /**< Game over screen            */
} eGameState;

#endif // E_GAME_STATE_H

/*** end of file eGameState.h ***/
