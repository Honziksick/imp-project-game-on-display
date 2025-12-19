/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tFlower.h                                                     *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tFlower structure holding the flower's center     *
 *               coordinates in pixel space. Stores integer X and Y positions  *
 *               (pixels) used by rendering, collision checks and game logic.  *
 *               This is a plain POD value type intended for storage in arrays *
 *               or inside the owning `tGame` structure. It manages no         *
 *               resources.                                                    *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tFlower.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Flower center coordinates in pixel space (integer pixels).
 */

#ifndef T_FLOWER_H
#define T_FLOWER_H

#define FLOWER_RADIUS 7   /**< Flower sprite radius (pixels). */

/**
 * @struct tFlower
 * @brief Structure representing a flower's position on the screen.
 *
 * @details The tFlower structure holds the center coordinates of a flower
 *          in pixel space. It uses integer values for the X and Y positions,
 *          which are suitable for rendering, collision detection, and game
 *          logic.
 */
typedef struct {
    int mPosX;      /**< Center X (pixels). */
    int mPosY;      /**< Center Y (pixels). */
} tFlower;

#endif // T_FLOWER_H

/*** end of file tFlower.h ***/
