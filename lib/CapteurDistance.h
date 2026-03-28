#ifndef CAPTEUR_DISTANCE_H
#define CAPTEUR_DISTANCE_H

/*
 * CapteurDistance.h
 *
 *   Encapsulation du capteur infrarouge Sharp GP2Y0A21YK0F
 *   Mesure de distance par sortie analogique via le CAN
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouacer
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Constructeur
 *   numeroBroche : numéro de broche du port A, de 0 à 7,
 *                  branché sur Vo du capteur
 *
 * Opérations disponibles
 *   lireDistance()              : retourne la distance en cm,
 *                                 de 10 à 80, 0 si hors plage
 *   lireBrut()                  : retourne la valeur brute du
 *                                 CAN, de 0 à 1023
 *   objetDetecte(distanceMax)   : retourne true si un objet est
 *                                 à moins de distanceMax cm
 *
 * Branchement
 *   Vo  fil blanc  -> PA0
 *   GND fil noir   -> GND
 *   Vcc fil rouge  -> 5 V
 *
 * Référence AREF
 *   AVCC = 5 V, mesuré au labo à 5.0153 V
 *
 * Plage valide
 *   10 à 80 cm
 *
 * Calibration
 *   Mesurée au labo sur ATmega324PA à 8 MHz
 *   Valeurs typiques relevées à 10, 20, 30, 40, 50, 60 et 80 cm
 *   Voir TABLE_CALIBRATION dans CapteurDistance.cpp
 *
 * Délai de mesure
 *   Environ 40 ms par lecture, selon le datasheet Sharp
 *
 * Dépendances
 *   Can.h
 *
 * Interruptions requises
 *   Aucune
 */

#include <stdint.h>

#include "Can.h"

class CapteurDistance
{
public:
    explicit CapteurDistance(uint8_t numeroBroche);

    uint8_t lireDistance();
    uint16_t lireBrut();
    bool objetDetecte(uint8_t distanceMax);

private:
    static const uint8_t delaiMesureMs_ = 40;
    static const uint8_t distanceMinCm_ = 10;
    static const uint8_t distanceMaxCm_ = 80;
    static const uint8_t distanceHorsPlage_ = 0;

    Can can_;
    uint8_t numeroBroche_;

    uint8_t calculerDistanceCm_(uint16_t valeurCan);
};

#endif