/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tSpider.h                                                     *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      18.12.2025                                                    *
 * Last edit:    20.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tSpider structure representing a spider entity    *
 *               in the game. Stores floating-point position and activation    *
 *               state for each spider. Used for spawning, updating, and       *
 *               collision detection of spiders as moving obstacles.           *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tSpider.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Defines the tSpider structure for spider entities in the game.
 */

#ifndef T_SPIDER_H
#define T_SPIDER_H

#include <stdbool.h>  // bool

#define MAX_SPIDERS   3     /**< Maximum number of spiders in the game. */
#define SPIDER_RADIUS 4     /**< Radius of a spider entity. */

/**
 * @struct tSpider
 * @brief Structure representing a spider entity in the game.
 *
 * @details The tSpider structure contains the position and active state
 *          of a spider.
 */
typedef struct {
    float mPosX;        /**< The X position of the spider. */
    float mPosY;        /**< The Y position of the spider. */
    bool mIsActive;     /**< Indicates whether the spider is active in the game. */
} tSpider;

#endif // T_SPIDER_H

/*** end of file tSpider.h ***/
