/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tGraphics.h                                                   *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    18.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tGraphics structure holding the graphics context  *
 *               state for framebuffer operations. Contains display dimensions *
 *               and a pointer to the framebuffer memory where pixel data is   *
 *               stored in page-oriented format (8 vertical pixels per byte).  *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tGraphics.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Graphics context structure definition for framebuffer-based displays.
 */

#ifndef T_GRAPHICS_H
#define T_GRAPHICS_H

#include <stdint.h>  // uint8_t

/**
 * @struct tGraphics
 * @brief Structure representing the graphics context for framebuffer operations.
 *
 * @details This structure holds the width and height of the display in pixels,
 *          as well as a pointer to the framebuffer memory where pixel data is
 *          stored. The framebuffer uses a page-oriented format, with each byte
 *          representing 8 vertical pixels.
 */

typedef struct {
    int mWidth;                /**< Width of the display in pixels.    */
    int mHeight;               /**< Height of the display in pixels.   */
    uint8_t *mFrameBufferPtr;  /**< Pointer to the framebuffer memory. */
} tGraphics;

#endif // T_GRAPHICS_H

/*** end of file tGraphics.h ***/
