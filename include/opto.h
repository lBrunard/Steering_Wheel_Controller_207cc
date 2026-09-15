#pragma once

/**
 * @brief Set the optocoupler pins as outputs and turn them all off.
 *        Call once from setup().
 */
void optoInit();

/**
 * @brief Turn every optocoupler off (no button pressed).
 */
void optoReleaseAll();

/**
 * @brief Turn on a single optocoupler, all the others are turned off.
 *        Only one at a time: otherwise the resistors end up in parallel
 *        and the head unit reads a value matching no button.
 *
 * @param state true = pressed, false = released
 * @param pin   optocoupler pin (PIN_VOL_PLUS, PIN_VOL_MINUS...)
 */
void optoPress(bool state, int pin);
