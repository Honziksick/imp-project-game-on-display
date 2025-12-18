/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Graphics.h                                                    *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    18.12.2025                                                    *
 *                                                                             *
 * Description:  Private header for internal graphics helper functions.        *
 *               Declares internal routines used by the public Graphics API,   *
 *               such as character rendering helpers. This file is intended    *
 *               for use only within the graphics module and is not part of    *
 *               the public API exposed to application code.                   *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Graphics.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Private declarations for internal framebuffer graphics helpers.
 */

#ifndef GRAPHICS_PRIVATE_H
#define GRAPHICS_PRIVATE_H

#include "structure/tGraphics.h"

/**
 * @brief Draws a single character on the graphics context at the specified position.
 * @details This function uses the font defined in the graphics context to render
 *          the character. It handles character mapping and pixel setting based
 *          on the font glyph data.
 *
 * @param pGraphics Pointer to the graphics context.
 * @param x The x-coordinate where the character will be drawn.
 * @param y The y-coordinate where the character will be drawn.
 * @param ch The character to be drawn.
 */
static void Graphics_DrawChar(const tGraphics *pGraphics, int x, int y, char ch);

#endif // GRAPHICS_PRIVATE_H

/*** end of file Graphics.h ***/
