/*
 * Moteur.cpp
 *
 *   Implémentation de la classe Moteur
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Registres affectés
 *   Via MinuteriePwm0  TCCR0A, TCCR0B, OCR0A, OCR0B
 *   Via BrocheIo       DDRx, PORTx
 */

#include "Moteur.h"

namespace
{
    constexpr uint8_t POURCENTAGE_MAX = 100;
}


Moteur::Moteur(const Configuration& configuration)
    : canalPwm_(configuration.canalPwm),
      brocheDirection_(configuration.brocheDirection),
      estNiveauHautPourAvant_(configuration.estNiveauHautPourAvant),
      estInitialisee_(false)
{
}


void Moteur::initialiser()
{
    if (estInitialisee_) {
        return;
    }

    MinuteriePwm0::initialiser();
    brocheDirection_.configurerEnSortie();
    arreter();

    estInitialisee_ = true;
}


void Moteur::ajusterVitessePourcent(int8_t pourcentSigne)
{
    if (!estInitialisee_) {
        initialiser();
    }

    const bool estAvant = (pourcentSigne >= 0);

    int16_t amplitude = 0;

    if (estAvant) {
        amplitude = pourcentSigne;
    }
    else {
        amplitude = (int16_t)(-pourcentSigne);
    }

    const uint8_t rapportCyclique =
        bornerValeurAbsoluePourcent_(amplitude);

    if (rapportCyclique == 0) {
        arreter();
        return;
    }

    ajusterDirection_(estAvant);
    MinuteriePwm0::ajusterRapportCycliquePourcent(
        canalPwm_,
        rapportCyclique);
}


void Moteur::arreter()
{
    MinuteriePwm0::ajusterRapportCycliquePourcent(canalPwm_, 0);
}


void Moteur::ajusterDirection_(bool estAvant)
{
    const bool estNiveauHaut =
        estAvant ? estNiveauHautPourAvant_ : !estNiveauHautPourAvant_;

    brocheDirection_.ecrire(estNiveauHaut);
}


uint8_t Moteur::bornerValeurAbsoluePourcent_(int16_t valeur)
{
    if (valeur < 0) {
        valeur = 0;
    }

    if (valeur > POURCENTAGE_MAX) {
        valeur = POURCENTAGE_MAX;
    }

    return (uint8_t)valeur;
}