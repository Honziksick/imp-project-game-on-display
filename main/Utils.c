/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Utils.c                                                       *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Utility functions for common mathematical and randomization   *
 *               operations. Provides floating-point clamping, sign extraction,*
 *               hardware random number generation via ESP32 RNG, range-bound  *
 *               integer random values, and squared Euclidean distance         *
 *               computation.                                                  *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Utils.c
 * @author Jan Kalina \<xkalinj00>
 * @brief Mathematical and random number utility functions.
 */

#include "public/Utils.h"
#include "esp_random.h"
#include <math.h>  // isnan, isinf

float Utils_ClampFloat(const float value, const float low, const float high) {
    // Handle invalid inputs
    if(isnan(value) || isnan(low) || isnan(high)) {
        return low;
    }
    if(isinf(value)) {
        return (value > 0.0f) ? high : low;
    }
    if(low > high) {
        return low;
    }

    // Clamp value to [low, high]
    if(value < low) {
        return low;
    }
    if(value > high) {
        return high;
    }
    return value;
}

float Utils_SignFloat1(const float value) {
    // Handle NaN
    if(isnan(value)) {
        return 1.0f;
    }

    // Return -1.0 for negative (including -0.0), +1.0 otherwise
    return (value < 0.0f) ? -1.0f : 1.0f;
}

uint32_t Utils_GenerateRandomU32() {
    return esp_random();
}

int Utils_GetRandomInRange(const int low, const int high) {
    // Validate range
    if(low > high) {
        return low;
    }

    // Handle edge case where low == high
    if(low == high) {
        return low;
    }

    // Compute span safely (high - low + 1) without overflow
    const uint32_t span = (uint32_t)(high - low + 1);
    const uint32_t randomValue = Utils_GenerateRandomU32();

    // Modulo to fit range and add offset
    return low + (int)(randomValue % span);
}

float Utils_DistanceSquareFloat(const float ax, const float ay, const float bx, const float by) {
    // Handle NaN/Inf inputs
    if(isnan(ax) || isnan(ay) || isnan(bx) || isnan(by) ||
        isinf(ax) || isinf(ay) || isinf(bx) || isinf(by)) {
        return INFINITY;
    }

    // Compute squared distance
    const float deltaX = ax - bx;
    const float deltaY = ay - by;

    return deltaX * deltaX + deltaY * deltaY;
}

/*** end of file Utils.c ***/
