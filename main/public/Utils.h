/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         Utils.h                                                       *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Provides lightweight numeric and RNG utilities used across    *
 *               the project. Contains floating-point helpers (clamp, sign,    *
 *               absolute, square, distance squared) optimized for hot loops,  *
 *               and RNG wrapper declarations that use hardware entropy.       *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Utils.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Utility function declarations for math and RNG operations.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>  // uint32_t

/**
 * @brief Clamps a floating-point value within the specified range [low, high].
 * @details If the value is NaN or infinite, it returns low or high accordingly.
 *          If low is greater than high, it returns low.
 *
 * @param value The floating-point value to clamp.
 * @param low The lower bound of the range.
 * @param high The upper bound of the range.
 * @return The clamped floating-point value.
 */
float Utils_ClampFloat(float value, float low, float high);

/**
 * @brief Determines the sign of a floating-point value.
 * @details Returns -1.0 if the value is negative (including -0.0), +1.0 otherwise.
 *          If the value is NaN, it returns +1.0.
 *
 * @param value The floating-point value to evaluate.
 * @return -1.0 for negative values, +1.0 for non-negative values.
 */
float Utils_SignFloat1(float value);

/**
 * @brief Generates a random 32-bit unsigned integer using hardware RNG.
 *
 * @return A random uint32_t value.
 */
uint32_t Utils_GenerateRandomU32();

/**
 * @brief Generates a random integer within the specified range [low, high).
 * @details Uses hardware RNG to produce a uniformly distributed random integer.
 *
 * @param low The inclusive lower bound of the range.
 * @param high The exclusive upper bound of the range.
 * @return A random integer in the range [low, high).
 */
int Utils_GetRandomInRange(int low, int high);

/**
 * @brief Computes the squared Euclidean distance between two points in 2D space.
 * @details Euclidean distance squared is calculated as: (ax - bx)^2 + (ay - by)^2.
 *
 * @param ax The x-coordinate of the first point.
 * @param ay The y-coordinate of the first point.
 * @param bx The x-coordinate of the second point.
 * @param by The y-coordinate of the second point.
 * @return The squared distance between the two points.
 */
float Utils_DistanceSquareFloat(float ax, float ay, float bx, float by);

/**
 * @brief Computes the absolute value of a floating-point number.
 *
 * @param value The floating-point value.
 * @return The absolute value of the input.
 */
float Utils_AbsoluteFloat(float value);

/**
 * @brief Ceilings a floating-point number to the nearest integer.
 * @details If the input is NaN, returns 0. Clamps extreme values to INT_MAX/INT_MIN.
 *
 * @param floatValue The floating-point value to ceiling.
 * @return The smallest integer greater than or equal to floatValue.
 */
int Utils_CeilFloatToInt(float floatValue);

/**
 * @brief Floors a floating-point number to the nearest integer.
 * @details If the input is NaN, returns 0. Clamps extreme values to INT_MAX/INT_MIN.
 *
 * @param floatValue The floating-point value to floor.
 * @return The largest integer less than or equal to floatValue.
 */
int Utils_FloorFloatToInt(float floatValue);

/**
 * @brief Rounds a floating-point number to the nearest long integer and clamps
 *        it to integeer.
 * @details If the input is NaN, returns 0. Clamps extreme values to INT_MAX/INT_MIN.
 *
 * @param floatValue The floating-point value to round.
 * @return The nearest long integer to floatValue.
 */
int Utils_LRoundFloatToInt(float floatValue);

#endif // UTILS_H

/*** end of file Utils.h ***/
