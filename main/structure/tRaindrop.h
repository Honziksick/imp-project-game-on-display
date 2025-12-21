/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tRaindrop.h                                                   *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      18.12.2025                                                    *
 * Last edit:    20.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tRainDrop structure representing a falling        *
 *               raindrop entity in the game. Stores floating-point position,  *
 *               vertical velocity, and activation state for each raindrop.    *
 *               Used for spawning, updating, and collision detection of       *
 *               raindrops that interact with the player.                      *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tRaindrop.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Defines the tRainDrop structure for raindrop entities in the game.
 */

#ifndef T_RAINDROP_H
#define T_RAINDROP_H

#include <stdbool.h>  // bool

#define MAX_RAINDROPS       50               /**< Maximum number of raindrops in the game.      */
#define RAINDROP_RADIUS     2                /**< Radius of a raindrop for collision detection. */
#define RAINDROP_FALL_SPEED 30.0f            /**< Fall speed of a raindrop (units per second).  */
#define RAINDROP_VELOCITY_MULTIPLIER 0.5f    /**< Slowdown factor when raindrop hits the bee.   */

/**
 * @struct tRainDrop
 * @brief Structure representing a raindrop entity in the game.
 *
 * @details The tRainDrop structure contains the position, velocity,
 *          and active state of a raindrop.
 */
typedef struct {
    float mPosX;            /**< The X position of the raindrop. */
    float mPosY;            /**< The Y position of the raindrop. */
    float mVelocityY;       /**< The vertical velocity of the raindrop. */
    bool mIsActive;         /**< Indicates whether the raindrop is active in the game. */
} tRainDrop;

#endif // T_RAINDROP_H

/*** end of file tRaindrop.h ***/
