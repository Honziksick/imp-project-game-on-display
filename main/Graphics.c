/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Graphics.c                                                    *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    20.12.2025                                                    *
 *                                                                             *
* Description:   Implementation of framebuffer-based graphics operations       *
 *               including pixel manipulation, line and shape drawing          *
 *               (rectangles, circles), and text rendering using a 5x7 font.   *
 *               Uses Bresenham algorithms for efficient integer-only          *
 *               rasterization.                                                *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Graphics.c
 * @author Jan Kalina \<xkalinj00>
 * @brief Implementation of graphics primitives and text rendering for
 *        framebuffer displays.
 */

#include "public/Graphics.h"
#include "public/Font5x7.h"
#include "structure/tGraphics.h"
#include <stdbool.h>  // bool
#include <stdint.h>   // uint8_t
#include <stddef.h>   // size_t
#include <string.h>   // memset

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
static void Graphics_DrawChar(const tGraphics *pGraphics, const int x, const int y, unsigned char ch) {
    // Map unsupported characters to '?'
    if(ch < FONT5X7_FIRST_ASCII || ch > FONT5X7_LAST_ASCII) {
        ch = '?';
    }

    // Retrieve character columns from Font5x7 and draw pixels
    const uint8_t *glyphCols = Font5x7[ch - FONT5X7_FIRST_ASCII].mCols;

    // For each of the 5 columns, set pixels for the 7 rows
    for(int iCol = 0; iCol < FONT5X7_WIDTH; iCol++) {
        const uint8_t colBits = glyphCols[iCol];

        for(int iRow = 0; iRow < FONT5X7_HEIGHT; iRow++) {
            const bool set = (colBits >> iRow) & 1;  // LSB = top pixel
            Graphics_SetPixel(pGraphics, x + iCol, y + iRow, set);
        }
    }
} // Graphics_DrawChar()

static size_t Graphics_GetPages(const tGraphics *pGraphics) {
    return (size_t)((pGraphics->mHeight + 7) / 8);
} // Graphics_GetPages()

void Graphics_Init(tGraphics *pGraphics, const int width, const int height, uint8_t *pFrameBuffer) {
    // Validate input parameters
    if(pGraphics == NULL || pFrameBuffer == NULL) {
        return;
    }
    if(width <= 0 || height <= 0) {
        return;
    }

    // Initialize graphics context fields
    pGraphics->mWidth = width;
    pGraphics->mHeight = height;
    pGraphics->mFrameBufferPtr = pFrameBuffer;

    // Ensure framebuffer starts cleared
    Graphics_Clear(pGraphics);
} // Graphics_Init()

void Graphics_Clear(const tGraphics *pGraphics) {
    // Validate input parameters
    if(pGraphics == NULL || pGraphics->mFrameBufferPtr == NULL) {
        return;
    }
    if(pGraphics->mWidth <= 0 || pGraphics->mHeight <= 0) {
        return;
    }

    // Fill the framebuffer with zeros based on width and height (pages = height/8)
    const size_t pages = Graphics_GetPages(pGraphics);
    const size_t bufferSize = (size_t)pGraphics->mWidth * pages;
    memset(pGraphics->mFrameBufferPtr, 0, bufferSize);
} // Graphics_Clear()

void Graphics_SetPixel(const tGraphics *pGraphics, const int x, const int y, const bool set) {
    // Validate input parameters
    if(pGraphics == NULL || pGraphics->mFrameBufferPtr == NULL) {
        return;
    }
    if(pGraphics->mWidth <= 0 || pGraphics->mHeight <= 0) {
        return;
    }

    // If the pixel is out of bounds, do nothing
    if(x < 0 || y < 0 || x >= pGraphics->mWidth || y >= pGraphics->mHeight) {
        return;
    }

    // Compute page (byte row) and bit within that byte for framebuffer
    const int page = y >> 3;  // page = y / 8
    const int bit = y & 7;    // bit = y % 8
    const int idx = page * pGraphics->mWidth + x;  // index in framebuffer

    // Set or clear the specific bit for the pixel
    if(set) {
        pGraphics->mFrameBufferPtr[idx] |= (1u << bit);   // set the bit (pixel)
    }
    else {
        pGraphics->mFrameBufferPtr[idx] &= ~(1u << bit);  // clear the bit (pixel)
    }
}

// Source: https://moodle.vut.cz/pluginfile.php/1054968/mod_label/intro/izg_02_rasterizace_rev2022_169.pdf
void Graphics_DrawLine(const tGraphics *pGraphics, int x1, int y1, const int x2, const int y2) {
    // Validate input parameters
    if(pGraphics == NULL || pGraphics->mFrameBufferPtr == NULL) {
        return;
    }

    int dx, dy;  // absolute differences in X and Y (angle)
    int sx, sy;  // step direction in X and Y

    // Compute absolute difference in X and step direction in X
    if(x2 > x1) {
        dx = x2 - x1;  // from x1 to x2
        sx = 1;
    }
    else {
        dx = x1 - x2;  // from x2 to x1
        sx = -1;
    }

    // Compute absolute difference in Y and step direction in Y (inverted: dy = -abs(y2 - y1))
    if(y2 > y1) {
        dy = -(y2 - y1);  // from y1 to y2 (ensure negative abs)
        sy = 1;
    }
    else {
        dy = -(y1 - y2);  // from y2 to y1 (ensure negative abs)
        sy = -1;
    }

    // Initial error
    int error = dx + dy;

    // Main draw loop
    while(true) {
        // Draw the current pixel.
        Graphics_SetPixel(pGraphics, x1, y1, true);

        // If we've reached the end point, exit the loop
        if(x1 == x2 && y1 == y2) {
            break;
        }

        // Double the error to avoid floating-point arithmetic
        const int errorDoubled = 2 * error;

        // Step in x and adjust error
        if(errorDoubled >= dy) {
            error += dy;
            x1 += sx;
        }

        // Step in y and adjust error
        if(errorDoubled <= dx) {
            error += dx;
            y1 += sy;
        }
    }
} // Graphics_DrawLine()

void Graphics_DrawRectangle(const tGraphics *pGraphics, const int x, const int y,
                            const int width, const int height, const bool fill) {
    // Validate input parameters
    if(pGraphics == NULL || pGraphics->mFrameBufferPtr == NULL) {
        return;
    }
    if(width <= 0 || height <= 0) {
        return;
    }

    // Decide whether to fill or outline the rectangle
    if(fill) {
        // Simple scanline fill
        for(int iY = y; iY < y + height; iY++) {
            for(int iX = x; iX < x + width; iX++) {
                Graphics_SetPixel(pGraphics, iX, iY, true);
            }
        }
    }
    // Draw rectangle outline
    else {
        // Draw top and bottom borders
        for(int iX = x; iX < x + width; iX++) {
            Graphics_SetPixel(pGraphics, iX, y, true);
            Graphics_SetPixel(pGraphics, iX, y + height - 1, true);
        }
        // Draw left and right borders
        for(int iY = y; iY < y + height; iY++) {
            Graphics_SetPixel(pGraphics, x, iY, true);
            Graphics_SetPixel(pGraphics, x + width - 1, iY, true);
        }
    }
} // Graphics_DrawRectangle()

// Source: https://moodle.vut.cz/pluginfile.php/1054968/mod_label/intro/izg_02_rasterizace_rev2022_169.pdf
void Graphics_DrawCircle(const tGraphics *pGraphics, const int centerX, const int centerY,
                         const int radius, const bool fill) {
    // Validate input parameters
    if(pGraphics == NULL || pGraphics->mFrameBufferPtr == NULL) {
        return;
    }
    if(radius <= 0) {
        return;
    }

    // Midpoint circle algorithm parameters
    int x = radius;
    int y = 0;

    // Decision parameter and incremental deltas
    int P = 1 - radius;
    int X2 = 2 - 2 * radius;  // = -(2*r - 2)
    int Y2 = 3;

    // Main drawing loop
    while(x >= y) {
        // Fill
        if(fill) {
            Graphics_DrawLine(pGraphics, centerX - x, centerY + y, centerX + x, centerY + y);
            Graphics_DrawLine(pGraphics, centerX - x, centerY - y, centerX + x, centerY - y);
            Graphics_DrawLine(pGraphics, centerX - y, centerY + x, centerX + y, centerY + x);
            Graphics_DrawLine(pGraphics, centerX - y, centerY - x, centerX + y, centerY - x);
        }
        // Outline
        else {
            Graphics_SetPixel(pGraphics, centerX + x, centerY + y, true);
            Graphics_SetPixel(pGraphics, centerX + y, centerY + x, true);
            Graphics_SetPixel(pGraphics, centerX - y, centerY + x, true);
            Graphics_SetPixel(pGraphics, centerX - x, centerY + y, true);
            Graphics_SetPixel(pGraphics, centerX - x, centerY - y, true);
            Graphics_SetPixel(pGraphics, centerX - y, centerY - x, true);
            Graphics_SetPixel(pGraphics, centerX + y, centerY - x, true);
            Graphics_SetPixel(pGraphics, centerX + x, centerY - y, true);
        }

        // Update decision parameter and coordinates
        if(P >= 0) {
            P += X2;
            X2 += 2;
            --x;
        }

        P += Y2;
        Y2 += 2;
        ++y;
    } // while
} // Graphics_DrawCircle()

void Graphics_DrawEllipse(const tGraphics *pGraphics, const int centerX, const int centerY,
                          const int radiusX, const int radiusY, const bool fill) {
    // Validate input parameters
    if(pGraphics == NULL || pGraphics->mFrameBufferPtr == NULL) {
        return;
    }
    if(radiusX <= 0 || radiusY <= 0) {
        return;
    }

    // Midpoint ellipse algorithm parameters
    int x = 0;
    int y = radiusY;
    const int radiusX2 = radiusX * radiusX;
    const int radiusY2 = radiusY * radiusY;
    int error = radiusY2 - (2 * radiusY - 1) * radiusX2;
    int stopX = 0;
    int stopY = 2 * radiusY2 * radiusX;

    // Region 1
    while(stopX <= stopY) {
        // Fill
        if(fill) {
            Graphics_DrawLine(pGraphics, centerX - x, centerY + y, centerX + x, centerY + y);
            Graphics_DrawLine(pGraphics, centerX - x, centerY - y, centerX + x, centerY - y);
        }
        // Outline
        else {
            Graphics_SetPixel(pGraphics, centerX + x, centerY + y, true);
            Graphics_SetPixel(pGraphics, centerX - x, centerY + y, true);
            Graphics_SetPixel(pGraphics, centerX - x, centerY - y, true);
            Graphics_SetPixel(pGraphics, centerX + x, centerY - y, true);
        }

        // Update decision parameter and coordinates
        x++;
        stopX += 2 * radiusY2;
        error += 2 * (x * radiusY2 + radiusY2);

        // Region 2
        if(2 * error + (2 * radiusY - 1) * radiusX2 > 0) {
            y--;
            stopY -= 2 * radiusX2;
            error += radiusX2 - 2 * y * radiusX2;
        }
    } // while
} // Graphics_DrawEllipse()


void Graphics_DrawText(const tGraphics *pGraphics, const int x, int y, const char text[]) {
    // Validate input parameters
    if(pGraphics == NULL || pGraphics->mFrameBufferPtr == NULL || text == NULL) {
        return;
    }

    // Current x position
    int cursorX = x;

    // Draw a null-terminated string using 5x7 font ('\n' moves to next line)
    for(const char *iChar = text; *iChar; iChar++) {
        // Move cursor to beginning of next line (8 pixels tall font + 1 spacing)
        if(*iChar == '\n') {
            cursorX = x;
            y += 8;
            continue;
        }

        // Draw a single character at current cursor position
        Graphics_DrawChar(pGraphics, cursorX, y, *iChar);

        // Advance cursor by 6 (5 pixels glyph + 1 spacing).
        cursorX += 6;
    }
} // Graphics_DrawText()

void Graphics_DrawPausedScreen(const tGraphics *pGraphics) {
    // Validate input parameters
    if(pGraphics == NULL) {
        return;
    }

    // Draw pause screen
    Graphics_DrawRectangle(pGraphics, 32, 24, 64, 14, false);
    Graphics_DrawText(pGraphics, 42, 28, "PAUSED");
    Graphics_DrawText(pGraphics, 17, 44, "Press joystick...");
} // Graphics_DrawPausedScreen()

void Graphics_DrawGameOverScreen(const tGraphics *pGraphics, const int score) {
    // Validate input parameters
    if(pGraphics == NULL) {
        return;
    }

    // Draw game over box
    Graphics_DrawRectangle(pGraphics, 14, 18, 100, 30, false);
    Graphics_DrawText(pGraphics, 26, 24, "GAME OVER");

    // Prepare score text
    char scoreBuffer[24];
    const int result = snprintf(scoreBuffer, sizeof(scoreBuffer), "Score: %d", score);
    if(result < 0) {
        scoreBuffer[0] = '\0';
    }
    else if((size_t)result >= sizeof(scoreBuffer)) {
        scoreBuffer[sizeof(scoreBuffer) - 1] = '\0';
    }

    // Draw score and exit prompt
    Graphics_DrawText(pGraphics, 26, 34, scoreBuffer);
    Graphics_DrawText(pGraphics, 9, 52, "Press SW to exit...");
} // Graphics_DrawGameOverScreen()

/*** end of file Graphics.c ***/
