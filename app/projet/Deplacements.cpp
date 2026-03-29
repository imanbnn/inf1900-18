/*
 * Deplacements.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "Deplacements.h"

#include "ConfigurationProjet.h"
#include "OutilsProjet.h"

namespace
{
    constexpr int8_t ANGLE_NUL = 0;
    constexpr int8_t ANGLE_BALAYAGE_15_DEGRES = 15;
    constexpr int8_t ANGLE_BALAYAGE_30_DEGRES = 30;
}

Deplacements::Deplacements(ControleMoteurs& moteurs)
    : moteurs_(moteurs)
{
}

void Deplacements::avancerPendant(uint8_t vitessePourcent,
                                  uint16_t dureeMillisecondes)
{
    moteurs_.avancer(vitessePourcent);
    attendreMillisecondes(dureeMillisecondes);
    moteurs_.arreter();
}

void Deplacements::reculerPendant(uint8_t vitessePourcent,
                                  uint16_t dureeMillisecondes)
{
    moteurs_.reculer(vitessePourcent);
    attendreMillisecondes(dureeMillisecondes);
    moteurs_.arreter();
}

void Deplacements::tournerGauchePendant_(uint16_t dureeMillisecondes)
{
    moteurs_.tournerGauche(VITESSE_TOURNER);
    attendreMillisecondes(dureeMillisecondes);
    moteurs_.arreter();
}

void Deplacements::tournerDroitePendant_(uint16_t dureeMillisecondes)
{
    moteurs_.tournerDroite(VITESSE_TOURNER);
    attendreMillisecondes(dureeMillisecondes);
    moteurs_.arreter();
}

void Deplacements::tournerGauche90()
{
    tournerGauchePendant_(DUREE_TOURNER_90_MS);
}

void Deplacements::tournerDroite90()
{
    tournerDroitePendant_(DUREE_TOURNER_90_MS);
}

void Deplacements::demiTour()
{
    tournerDroitePendant_(DUREE_DEMI_TOUR_MS);
}

void Deplacements::tournerAngleBalayage(bool estEntreeAGauche,
                                        int8_t angleDegres)
{
    if (angleDegres == ANGLE_NUL) {
        return;
    }

    const bool estAngleNegatif = (angleDegres < ANGLE_NUL);
    const bool estRotationVersGauche =
        estEntreeAGauche ? !estAngleNegatif : estAngleNegatif;

    uint8_t amplitudeDegres = (uint8_t)angleDegres;

    if (estAngleNegatif) {
        amplitudeDegres = (uint8_t)(-angleDegres);
    }

    uint16_t dureeMillisecondes = DUREE_TOURNER_30_MS;

    if (amplitudeDegres == ANGLE_BALAYAGE_15_DEGRES) {
        dureeMillisecondes = DUREE_TOURNER_15_MS;
    }
    else if (amplitudeDegres == ANGLE_BALAYAGE_30_DEGRES) {
        dureeMillisecondes = DUREE_TOURNER_30_MS;
    }

    if (estRotationVersGauche) {
        tournerGauchePendant_(dureeMillisecondes);
    }
    else {
        tournerDroitePendant_(dureeMillisecondes);
    }
}