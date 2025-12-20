/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tPowerUp.h                                                    *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      18.12.2025                                                    *
 * Last edit:    20.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tPowerUp structure representing a collectible     *
 *               power-up entity in the game. Stores floating-point position,  *
 *               activation state, and type (shield or honey boost). Used for  *
 *               spawning, tracking, and rendering power-ups that grant        *
 *               temporary effects to the player.                              *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tPowerUp.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Defines the tPowerUp structure representing a collectible power-up
 *        entity in the game.
 */

#ifndef T_POWERUP_H
#define T_POWERUP_H

#include "enum/ePowerUpType.h"
#include <stdbool.h>  // bool

#define MAX_POWERUPS         2          /**< Maximum number of power-ups in the game.        */
#define POWERUP_RADIUS       3          /**< Radius of a power-up entity.                    */
#define POWERUP_SPAWN_CHANCE 0.02f      /**< Chance of spawning a power-up each frame is 2%. */

/**
 * @struct tPowerUp
 * @brief Structure representing a power-up entity in the game.
 *
 * @details The tPowerUp structure contains the position, active state,
 *          and type of the power-up.
 */
typedef struct {
    float mPosX;            /**< The X position of the power-up. */
    float mPosY;            /**< The Y position of the power-up. */
    bool mIsActive;         /**< Indicates whether the power-up is active in the game. */
    ePowerUpType mType;     /**< The type of the power-up (0=NONE, 1=SHIELD, 2=HONEY). */
} tPowerUp;

#endif // T_POWERUP_H

/*** end of file tPowerUp.h ***/
