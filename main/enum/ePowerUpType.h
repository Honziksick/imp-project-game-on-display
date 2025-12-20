/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         ePowerUpType.h                                                *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      18.12.2025                                                    *
 * Last edit:    20.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the ePowerUpType enumeration identifying available    *
 *               power-up types in the game (shield, honey). Used to classify  *
 *               collectible items that provide temporary gameplay benefits.   *
 *                                                                             *
 ******************************************************************************/
/**
 * @file ePowerUpType.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Enumeration of power-up types available in the game.
 */

#ifndef E_POWER_UP_TYPE_H
#define E_POWER_UP_TYPE_H

/**
 * @enum ePowerUpType
 * @brief Enumeration of power-up types available in the game.
 */
typedef enum {
	NONE = 0,       /**< No power-up.     */
	SHIELD,         /**< Shield power-up. */
	HONEY           /**< Honey power-up.  */
} ePowerUpType;

#endif // E_POWER_UP_TYPE_H

/*** end of file ePowerUpType.h ***/
