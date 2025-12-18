/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tFontGlyph.h                                                  *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    18.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tFontGlyph structure representing a single        *
 *               character glyph in the 5x7 monospace font. Each glyph         *
 *               consists of a character and its 5-column bitmap where each    *
 *               column byte encodes 7 vertical pixels (LSB = top pixel).      *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tFontGlyph.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Structure definition for 5x7 font glyph bitmap representation.
 */

#ifndef T_FONT_GLYPH_H
#define T_FONT_GLYPH_H

#include <stdint.h>  // uint8_t

/**
 * @struct tFontGlyph
 * @brief Structure representing a single glyph in the 5x7 font.
 *
 * @details Each glyph consists of a character and its corresponding
 *          5-column bitmap representation. Each column is represented
 *          by a byte where the least significant 7 bits correspond to
 *          the vertical pixels of that column (LSB = top pixel).
 */
typedef struct {
    char mCh;           /**< The character represented by the glyph.          */
    uint8_t mCols[5];   /**< The 5-column bitmap representation of the glyph. */
} tFontGlyph;

#endif // T_FONT_GLYPH_H

/*** end of file tFontGlyph.h ***/
