/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tBee.h                                                        *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Represents the player-controlled bee used in the game. The    *
 *               structure stores 2D position in pixels, velocity in pixels    *
 *               per second, a flag indicating whether the bee currently holds *
 *               pollen, and a normalized collection progress value in the     *
 *               range [0.0 .. 1.0].                                           *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tBee.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Structure definition for the bee entity in the game.
 */

#ifndef T_BEE_H
#define T_BEE_H

#include <stdbool.h>  // bool

#define MAX_VELOCITY_X 85.0f   /**< Maximum horizontal velocity (pixels/second). */
#define MAX_VELOCITY_Y 70.0f   /**< Maximum vertical velocity (pixels/second).   */
#define BEE_RADIUS     3       /**< Bee sprite radius (pixels).                  */

/**
 * @struct tBee
 * @brief Structure representing the bee entity in the game.
 *
 * @details This structure holds the position, velocity, pollen carrying status,
 *          and pollen collection progress of the bee.
 */
typedef struct {
    float mPosX;            /**< Position X (pixels)                    */
    float mPosY;            /**< Position Y (pixels)                    */
    float mVelocityX;       /**< Velocity X (pixels/second)             */
    float mVelocityY;       /**< Velocity Y (pixels/second)             */
    float mPollenFill;      /**< Collection progress (0.0 to 1.0)       */
    bool mHasPollen;        /**< Carrying pollen flag (true = carrying) */
} tBee;

#endif // T_BEE_H

/*** end of file tBee.h ***/
