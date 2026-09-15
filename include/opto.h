#pragma once

/**
 * @brief Configure les broches des optocoupleurs en sortie et les éteint toutes.
 *        À appeler une fois dans setup().
 */
void optoInit();

/**
 * @brief Éteint tous les optocoupleurs (aucun bouton appuyé).
 */
void optoReleaseAll();

/**
 * @brief Allume un seul optocoupleur, les autres sont éteints.
 *        Un seul à la fois : sinon les résistances se retrouvent en parallèle
 *        et l'autoradio lit une valeur qui ne correspond à aucun bouton.
 *
 * @param state true = appuyé, false = relâché
 * @param pin   broche de l'opto (PIN_VOL_PLUS, PIN_VOL_MINUS…)
 */
void optoPress(bool state, int pin);
