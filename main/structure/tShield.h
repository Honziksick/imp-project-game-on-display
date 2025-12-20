/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tShield.h                                                     *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      18.12.2025                                                    *
 * Last edit:    20.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tShield structure representing a temporary        *
 *               shield power-up state in the game. Contains activation flag   *
 *               and remaining duration timer to track the shield effect       *
 *               that grants the player temporary invulnerability.             *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tShield.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Defines the tShield structure for shield power-up state in the game.
 */

#ifndef T_SHIELD_H
#define T_SHIELD_H

#include <stdbool.h>  // bool

#define SHIELD_DURATION 5.0f    /**< Duration of the shield power-up effect in seconds. */

/**
 * @struct tShield
 * @brief Structure representing a shield entity in the game.
 *
 * @details The tShield structure contains the active state and remaining time
 *          of the shield.
 */
typedef struct {
    bool mIsActive;         /**< Indicates whether the shield is active in the game. */
    float mTimeLeft;        /**< The remaining time the shield is active.            */
} tShield;

#endif // T_SHIELD_H

/*** end of file tShield.h ***/
