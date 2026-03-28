/*
 * CapteurDistance.cpp
 *
 *   Implémentation de la classe CapteurDistance
 *   Conversion via table d'interpolation mesurée au labo
 *   sur ATmega324PA à 8 MHz, AREF = AVCC = 5.0153 V
 *
 *   Calibration effectuée avec un poteau sur surface plane,
 *   capteur fixe, lectures stables sur plus de 10 échantillons.
 *   Les valeurs typiques sont utilisées comme seuils.
 *   Une marge de ±15 cm est acceptable.
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "CapteurDistance.h"

namespace
{
    struct PointCalibration
    {
        uint16_t valeurBruteCan;
        uint8_t distanceCm;
    };

    // Valeurs brutes CAN mesurées au labo.
    // Le capteur GP2Y0A21YK0F est non linéaire :
    // la valeur brute diminue quand la distance augmente.
    constexpr PointCalibration TABLE_CALIBRATION[] =
    {
        {0x01F4, 10},
        {0x00FF, 20},
        {0x00A7, 30},
        {0x0072, 40},
        {0x0057, 50},
        {0x0044, 60},
        {0x003A, 80}
    };

    constexpr uint8_t TAILLE_TABLE =
        sizeof(TABLE_CALIBRATION) / sizeof(TABLE_CALIBRATION[0]);
}


CapteurDistance::CapteurDistance(uint8_t numeroBroche)
    : numeroBroche_(numeroBroche)
{
}


uint16_t CapteurDistance::lireBrut()
{
    return can_.lecture(numeroBroche_);
}


uint8_t CapteurDistance::lireDistance()
{
    _delay_ms(delaiMesureMs_);
    return calculerDistanceCm_(lireBrut());
}


bool CapteurDistance::objetDetecte(uint8_t distanceMax)
{
    const uint8_t distance = lireDistance();

    return (distance != distanceHorsPlage_) && (distance <= distanceMax);
}


uint8_t CapteurDistance::calculerDistanceCm_(uint16_t valeurCan)
{
    if (valeurCan >= TABLE_CALIBRATION[0].valeurBruteCan) {
        return distanceMinCm_;
    }

    if (valeurCan <= TABLE_CALIBRATION[TAILLE_TABLE - 1].valeurBruteCan) {
        return distanceHorsPlage_;
    }

    for (uint8_t i = 0; i < TAILLE_TABLE - 1; i++) {
        const uint16_t valeurBruteHaute =
            TABLE_CALIBRATION[i].valeurBruteCan;
        const uint16_t valeurBruteBasse =
            TABLE_CALIBRATION[i + 1].valeurBruteCan;

        if ((valeurCan <= valeurBruteHaute) &&
            (valeurCan >= valeurBruteBasse)) {
            const uint8_t distanceHaute =
                TABLE_CALIBRATION[i].distanceCm;
            const uint8_t distanceBasse =
                TABLE_CALIBRATION[i + 1].distanceCm;

            return distanceHaute +
                   (uint8_t)((uint32_t)(valeurBruteHaute - valeurCan) *
                             (distanceBasse - distanceHaute) /
                             (valeurBruteHaute - valeurBruteBasse));
        }
    }

    return distanceHorsPlage_;
}