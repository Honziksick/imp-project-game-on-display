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
 * Description:  Public API for simple framebuffer graphics operations.        *
 *               Provides initialization, framebuffer management, pixel        *
 *               manipulation, basic primitives (lines, rectangles, circles)   *
 *               and text rendering using the 5x7 font.                        *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Graphics.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Public interface for framebuffer-based graphics operations.
 */

#ifndef GRAPHICS_PUBLIC_H
#define GRAPHICS_PUBLIC_H

#include "structure/tGraphics.h"
#include <stdbool.h>  // bool
#include <stdint.h>   // uint8_t

/**
 * @brief Initializes a graphics context with the specified dimensions and framebuffer.
 * @details This function sets up the graphics context by configuring its width,
 *          height, and framebuffer pointer. It also clears the framebuffer to
 *          ensure a clean initial state.
 *
 * @param pGraphics Pointer to the graphics context to initialize.
 * @param width The width of the display in pixels.
 * @param height The height of the display in pixels.
 * @param pFrameBuffer Pointer to the framebuffer array where pixel data will
 *                     be stored.
 */
void Graphics_Init(tGraphics *pGraphics, int width, int height, uint8_t *pFrameBuffer);

/**
 * @brief Clears the entire framebuffer by setting all pixels to off (0).
 * @details This function fills the framebuffer with zeros, effectively clearing
 *          the display. The size of the cleared area is calculated based on the
 *          graphics context dimensions.
 *
 * @param pGraphics Pointer to the graphics context whose framebuffer will be cleared.
 */
void Graphics_Clear(const tGraphics *pGraphics);

/**
 * @brief Sets or clears a single pixel at the specified coordinates.
 * @details This function modifies a single pixel in the framebuffer. It performs
 *          bounds checking to ensure the coordinates are within the display area.
 *          The framebuffer is organized in pages (8 pixels vertically per byte).
 *
 * @param pGraphics Pointer to the graphics context.
 * @param x The x-coordinate of the pixel (0 to width-1).
 * @param y The y-coordinate of the pixel (0 to height-1).
 * @param set True to turn the pixel on (set), false to turn it off (clear).
 */
void Graphics_SetPixel(const tGraphics *pGraphics, int x, int y, bool set);

/**
 * @brief Draws a line between two points using Bresenham's line algorithm.
 * @details This function draws a straight line from (x0, y0) to (x1, y1) by
 *          setting pixels along the path. The implementation uses Bresenham's
 *          algorithm for optimal performance without floating-point operations.
 *
 * @note Implementation inspired by IZG course materials on rasterization:
 *       `// Source: https://moodle.vut.cz/pluginfile.php/1054968/mod_label/intro/izg_02_rasterizace_rev2022_169.pdf`
 *
 * @param pGraphics Pointer to the graphics context.
 * @param x1 The x-coordinate of the starting point.
 * @param y1 The y-coordinate of the starting point.
 * @param x2 The x-coordinate of the ending point.
 * @param y2 The y-coordinate of the ending point.
 */
void Graphics_DrawLine(const tGraphics *pGraphics, int x1, int y1, int x2, int y2);

/**
 * @brief Draws a rectangle on the display.
 * @details This function draws either a filled or outlined rectangle. For filled
 *          rectangles, all interior pixels are set. For outlined rectangles, only
 *          the border pixels are set.
 *
 * @param pGraphics Pointer to the graphics context.
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param width The width of the rectangle in pixels.
 * @param height The height of the rectangle in pixels.
 * @param fill True for a filled rectangle, false for an outline only.
 */
void Graphics_DrawRectangle(const tGraphics *pGraphics, int x, int y, int width, int height, bool fill);

/**
 * @brief Draws a circle on the display using the midpoint circle algorithm.
 * @details This function draws either a filled or outlined circle centered at the
 *          specified coordinates. The implementation uses Bresenham's midpoint
 *          circle algorithm for efficiency.
 *
 * @note Implementation inspired by IZG course materials on rasterization:
 *       `// Source: https://moodle.vut.cz/pluginfile.php/1054968/mod_label/intro/izg_02_rasterizace_rev2022_169.pdf`
 *
 * @param pGraphics Pointer to the graphics context.
 * @param centerX The x-coordinate of the circle center.
 * @param centerY The y-coordinate of the circle center.
 * @param radius The radius of the circle in pixels.
 * @param fill True for a filled circle, false for an outline only.
 */
void Graphics_DrawCircle(const tGraphics *pGraphics, int centerX, int centerY, int radius, bool fill);

/**
 * @brief Draws a text string at the specified position.
 * @details This function renders a null-terminated string using the Font5x7 font.
 *          Characters are drawn sequentially with 6-pixel spacing (5 pixels for
 *          the character plus 1 pixel spacing). Newline characters (`\n`) move
 *          to the next line.
 *
 * @param pGraphics Pointer to the graphics context.
 * @param x The x-coordinate of the starting position.
 * @param y The y-coordinate of the starting position (top of characters).
 * @param text Pointer to the null-terminated string to draw.
 */
void Graphics_DrawText(const tGraphics *pGraphics, int x, int y, const char text[]);

#endif // GRAPHICS_PUBLIC_H

/*** end of file Graphics.h ***/
