/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Renderer.h                                                    *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    20.12.2025                                                    *
 *                                                                             *
 * Description:  Public renderer interface responsible for translating game    *
 *               state into framebuffer drawing commands. Accepts normalized   *
 *               input coordinates and updates the supplied graphics context   *
 *               with sprites, UI elements and debug overlays. The renderer    *
 *               focuses solely on composing pixels in the provided            *
 *               framebuffer and does not perform display I/O, timing, or      *
 *               resource loading.                                             *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Renderer.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Public renderer interface for drawing game state.
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "structure/tGame.h"
#include "structure/tGraphics.h"

#define RENDERRER_POLLEN_BAR_X_OFFSET 102   /**< X offset for pollen bar from the right edge. */
#define RENDERRER_POLLEN_BAR_Y_OFFSET 0     /**< Y offset for pollen bar from the top edge.   */

#define RENDERRER_POLLEN_BAR_WIDTH  24  /**< Width of the pollen progress bar.  */
#define RENDERRER_POLLEN_BAR_HEIGHT 7   /**< Height of the pollen progress bar. */

/**
 * @brief Draw the game state to the graphics buffer.
 * @details Renders the current game state onto the provided graphics buffer.
 *
 * @param pGame Pointer to the game structure containing the current state.
 * @param pGraphics Pointer to the graphics structure for rendering.
 */
void Renderer_Draw(const tGame *pGame, const tGraphics *pGraphics);

#endif // RENDERER_H

/*** end of file Renderer.h ***/
