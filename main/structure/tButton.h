/*******************************************************************************
 *                                                                             *
 * Project:      Game on Display                                               *
 * Assignor:     Ing. Vojtěch Mrázek, Ph.D.                                    *
 * University:   Faculty of Information Technology, BUT                        *
 * Subject:      IMP: Microprocessors and Embedded Systems                     *
 *                                                                             *
 * File:         tButton.h                                                     *
 * Author:       Jan Kalina <xkalinj00>                                        *
 *                                                                             *
 * Created:      15.12.2025                                                    *
 * Last edit:    19.12.2025                                                    *
 *                                                                             *
 * Description:  Holds debounced button input state for a single physical      *
 *               button. Stores the most recent raw sample, the current        *
 *               debounced logical state and the timestamp (milliseconds)      *
 *               of the last state transition. Intended for simple polling     *
 *               debounce logic executed from the main loop. This structure    *
 *               does not manage hardware resources or interrupts.             *
 *                                                                             *
 ******************************************************************************/
/**
 * @file tButton.h
 * @author Jan Kalina \<xkalinj00>
 * @brief Structure for button debouncing.
 */

#ifndef T_BUTTON_H
#define T_BUTTON_H

#include <stdint.h>  // int64_t

/**
 * @struct tButton
 * @brief Structure for button debouncing.
 *
 * @details This structure holds the necessary information for debouncing
 *          a button, including the previous raw state, the debounced state,
 *          and the timestamp of the last state change in milliseconds.
 */
typedef struct {
    int mRawState;                     /**< Previous raw button state.           */
    int mDebouncedState;               /**< Debounced button state.              */
    int64_t mStateChangedTimestampMs;  /**< Timestamp of last state change (ms). */
} tButton;

#endif // T_BUTTON_H

/*** end of file tButton.h ***/
