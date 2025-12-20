/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tHoney.h                                                      *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      18.12.2025                                                    *
 * Last edit:    20.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tHoney structure representing a temporary speed   *
 *               boost power-up state in the game. Contains activation flag    *
 *               and remaining duration timer to track the honey boost effect  *
 *               that increases bee movement speed by a defined multiplier.    *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tHoney.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Defines the tHoney structure representing a temporary speed boost
 *        power-up state in the game.
 */

#ifndef T_HONEY_H
#define T_HONEY_H

#include <stdbool.h>  // bool

#define HONEY_DURATION   3.0f       /**< Duration of the honey power-up effect in seconds. */
#define HONEY_MULTIPLIER 1.5f       /**< Speed multiplier when honey power-up is active.   */

/**
 * @struct tHoney
 * @brief Structure representing a speed boost entity in the game.
 *
 * @details The tHoney structure contains the active state and remaining time
 *          of the speed boost.
 */
typedef struct {
    bool mIsActive;         /**< Indicates whether the speed boost is active in the game. */
    float mTimeLeft;        /**< The remaining time the speed boost is active.            */
} tHoney;

#endif // T_HONEY_H

/*** end of file tHoney.h ***/
