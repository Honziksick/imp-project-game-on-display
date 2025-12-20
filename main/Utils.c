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
 * Last edit:    20.12.2025                                                    *
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
#include "esp_random.h"  // esp_random()
#include <limits.h>      // INT_MAX, INT_MIN
#include <stdint.h>      // uint32_t, int64_t, uint64_t
#include <math.h>        // isnan, isinf

/**
 * @brief Safely converts a long integer to int, clamping to INT_MAX/INT_MIN
 *        on overflow.
 *
 * @param longValue The long integer value to convert.
 * @return The converted int value, clamped if out of range.
 */
static int Utils_SafeLongToInt(const long longValue) {
    if(longValue > (long)INT_MAX) {
        return INT_MAX;
    }
    if(longValue < (long)INT_MIN) {
        return INT_MIN;
    }

    return (int)longValue;
} // Utils_SafeLongToInt()

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
} // Utils_ClampFloat()

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
} // Utils_GenerateRandomU32()

int Utils_GetRandomInRange(const int low, const int high) {
    // Validate range
    if(low > high) {
        return low;
    }

    if(low == high) {
        return low;
    }

    // Compute span in 64-bit to avoid overflow before cast
    const int64_t diff = (int64_t)high - (int64_t)low;
    const uint64_t span = (uint64_t)diff + 1ull;

    // Build a 64-bit random value from two 32-bit RNG calls
    const uint64_t randomHighU32 = Utils_GenerateRandomU32();
    const uint64_t randomLowU32 = Utils_GenerateRandomU32();
    const uint64_t randomU64 = (randomHighU32 << 32) | randomLowU32;

    // Reduce to range and add offset (modulo on 64-bit span)
    const uint64_t mod = randomU64 % span;
    return (int)((int64_t)low + (int64_t)mod);
} // Utils_GetRandomInRange()

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
} // Utils_DistanceSquareFloat()

float Utils_AbsoluteFloat(const float value) {
    return (value < 0.0f) ? -value : value;
} // Utils_AbsoluteFloat()

int Utils_CeilFloatToInt(const float floatValue) {
    // Handle NaN inputs
    if(isnan(floatValue)) {
        return 0;
    }

    // Clamp extreme values to INT_MAX/INT_MIN to avoid undefined casts
    if(floatValue >= (float)INT_MAX) {
        return INT_MAX;
    }
    if(floatValue <= (float)INT_MIN) {
        return INT_MIN;
    }

    // Truncate towards zero to get integer part
    const int integerPart = (int)floatValue;

    // For positive values with fractional part, increment because truncation goes towards zero
    if(floatValue > 0.0f && (float)integerPart < floatValue) {
        return integerPart + 1;
    }

    return integerPart;
} // Utils_CeilFloatToInt()

int Utils_FloorFloatToInt(const float floatValue) {
    // Handle NaN inputs
    if(isnan(floatValue)) {
        return 0;
    }

    // Clamp extreme values to INT_MAX/INT_MIN to avoid undefined casts
    if(floatValue >= (float)INT_MAX) {
        return INT_MAX;
    }
    if(floatValue <= (float)INT_MIN) {
        return INT_MIN;
    }

    // Truncate towards zero to get integer part
    const int integerPart = (int)floatValue;

    // For positive values with fractional part, decrement because truncation goes towards zero
    if(floatValue < 0.0f && (float)integerPart > floatValue) {
        return integerPart - 1;
    }
    return integerPart;
} // Utils_FloorFloatToInt()

int Utils_LRoundFloatToInt(const float floatValue) {
    // Handle NaN inputs
    if(isnan(floatValue)) {
        return 0l;
    }

    // If value is infinite, return value clamped to the appropriate limit
    if(isinf(floatValue)) {
        long limit = (floatValue > 0.0f) ? LONG_MAX : LONG_MIN;
        return Utils_SafeLongToInt(limit);
    }

    // Use double for safer boundary comparisons
    const double doubleValue = (double)floatValue;

    // If adding/subtracting 0.5 would overflow, return the limit
    if(doubleValue >= (double)LONG_MAX - 0.5) {
        return Utils_SafeLongToInt(LONG_MAX);
    }
    if(doubleValue <= (double)LONG_MIN + 0.5) {
        return Utils_SafeLongToInt(LONG_MIN);
    }

    // Round half values away from zero
    long rounded;
    if(doubleValue >= 0.0) {
        rounded = (long)(doubleValue + 0.5);
    }
    else {
        rounded = (long)(doubleValue - 0.5);
    }

    return Utils_SafeLongToInt(rounded);
} // Utils_LRoundFloatToLong()

/*** end of file Utils.c ***/
