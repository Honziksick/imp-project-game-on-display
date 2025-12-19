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
 * Last edit:    19.12.2025                                                    *
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

/**
 * @brief Draw the game state to the graphics buffer.
 * @details Renders the current game state onto the provided graphics buffer
 *          based on normalized coordinates.
 *
 * @param pGame Pointer to the game structure containing the current state.
 * @param pGraphics Pointer to the graphics structure for rendering.
 * @param normalizedX Normalized X coordinate for rendering context.
 * @param normalizedY Normalized Y coordinate for rendering context.
 */
void Renderer_Draw(tGame *pGame, tGraphics *pGraphics, float normalizedX, float normalizedY);

#endif // RENDERER_H

/*** end of file Renderer.h ***/
