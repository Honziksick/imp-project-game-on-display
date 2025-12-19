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
 * Description:                                                                *
 *                                                                             *
 ******************************************************************************/
/**
 * @file Utils.h
 * @author Jan Kalina \<xkalinj00>
 * @brief
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>  // uint32_t

float Utils_ClampFloat(float value, float low, float high);

float Utils_SignFloat1(float value);

uint32_t Utils_GenerateRandomU32();

int Utils_GetRandomInRange(int low, int high);

float Utils_DistanceSquareFloat(float ax, float ay, float bx, float by);

#endif // UTILS_H

/*** end of file Utils.h ***/
