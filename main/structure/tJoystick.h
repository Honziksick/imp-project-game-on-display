/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tJoystick.h                                                   *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Defines the tJoystick structure storing calibrated analog     *
 *               center positions and the exponential moving average (EMA)     *
 *               filtered normalized axes. Contains integer ADC center values  *
 *               for X and Y and floating-point normalized values in the       *
 *               range [-1.0 ... 1.0].                                         *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tJoystick.h
 * @author Jan Kalina \<xkalinj00>
 * @brief
 */

#ifndef T_JOYSTICK_H
#define T_JOYSTICK_H

/**
 * @struct tJoystick
 * @brief Structure representing a joystick with calibrated center and
 *        EMA-filtered normalized axes.
 *
 * @details This structure holds the calibrated center positions in ADC units
 *          and the exponentially moving average (EMA) filtered normalized
 *          values for both X and Y axes in the range [-1.0 ... 1.0].
 */
typedef struct {
    int mCenterX;           /**< Calibrated center X (ADC units).          */
    int mCenterY;           /**< Calibrated center Y (ADC units).          */
    float mNormalizedX;     /**< EMA-filtered normalized X [-1.0 ... 1.0]. */
    float mNormalizedY;     /**< EMA-filtered normalized Y [-1.0 ... 1.0]. */
} tJoystick;

#endif // T_JOYSTICK_H

/*** end of file tJoystick.h ***/
