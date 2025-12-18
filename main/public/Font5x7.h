/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Font5x7.h                                                     *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    18.12.2025                                                    *
 *                                                                             *
 * Description:  Public header declaring the 5x7 bitmap font used by the       *
 *               graphics subsystem. Provides glyph definitions and font       *
 *               metrics constants for characters in the ASCII range 32-127.   *
 *               The font is organized as 5 columns x 7 rows per glyph.        *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Font5x7.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Declaration of the 5x7 monospaced bitmap font definitions and constants.
 */

#ifndef FONT5X7_PUBLIC_H
#define FONT5X7_PUBLIC_H

#include "structure/tFontGlyph.h"

#define FONT5X7_WIDTH       5                   /**< Width of each character in pixels.        */
#define FONT5X7_HEIGHT      7                   /**< Height of each character in pixels.       */

#define FONT5X7_COUNT       96                  /**< Total number of characters in the font.   */
#define FONT5X7_FIRST_IDX   0                   /**< Index of the first character in the font. */
#define FONT5X7_LAST_IDX    (FONT5X7_COUNT - 1) /**< Index of the last character in the font.  */
#define FONT5X7_FIRST_ASCII 32                  /**< ASCII code of the first character.        */
#define FONT5X7_LAST_ASCII  127                 /**< ASCII code of the last character.         */

/**
 * @brief The 5x7 font glyphs for ASCII characters 32 to 127.
 */
extern const tFontGlyph Font5x7[FONT5X7_COUNT];

#endif // FONT5X7_PUBLIC_H

/*** end of file Font5x7.h ***/
