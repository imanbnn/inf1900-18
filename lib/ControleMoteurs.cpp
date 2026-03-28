/*
 * ControleMoteurs.cpp
 *
 *   Implémentation de la classe ControleMoteurs
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Registres affectés
 *   Via Moteur et MinuteriePwm0
 *   TCCR0A, TCCR0B, OCR0A, OCR0B, DDRx, PORTx
 */

#include "ControleMoteurs.h"

namespace
{
    constexpr uint8_t POURCENTAGE_MAX = 100;

    uint8_t limiterPourcentage(uint8_t vitessePourcentage)
    {
        if (vitessePourcentage > POURCENTAGE_MAX) {
            return POURCENTAGE_MAX;
        }

        return vitessePourcentage;
    }
}


ControleMoteurs::ControleMoteurs(Moteur& moteurGauche,
                                 Moteur& moteurDroit)
    : moteurGauche_(moteurGauche),
      moteurDroit_(moteurDroit),
      estInitialisee_(false)
{
}


void ControleMoteurs::initialiser(PrescalerTimer0 prescaler)
{
    if (estInitialisee_) {
        return;
    }

    moteurGauche_.initialiser();
    moteurDroit_.initialiser();

    MinuteriePwm0::demarrer(prescaler);

    estInitialisee_ = true;
}


void ControleMoteurs::arreter()
{
    moteurGauche_.arreter();
    moteurDroit_.arreter();
}


void ControleMoteurs::reculer(uint8_t vitessePourcent)
{
    const int8_t vitesseSignee = (int8_t)limiterPourcentage(vitessePourcent);

    ajusterVitesses(vitesseSignee, vitesseSignee);
}


void ControleMoteurs::avancer(uint8_t vitessePourcent)
{
    const int8_t vitesseSignee = (int8_t)limiterPourcentage(vitessePourcent);

    ajusterVitesses(-vitesseSignee, -vitesseSignee);
}


void ControleMoteurs::tournerGauche(uint8_t vitessePourcent)
{
    const int8_t vitesseSignee = (int8_t)limiterPourcentage(vitessePourcent);

    ajusterVitesses(-vitesseSignee, vitesseSignee);
}


void ControleMoteurs::tournerDroite(uint8_t vitessePourcent)
{
    const int8_t vitesseSignee = (int8_t)limiterPourcentage(vitessePourcent);

    ajusterVitesses(vitesseSignee, -vitesseSignee);
}


void ControleMoteurs::ajusterVitesses(int8_t vitesseGauche,
                                      int8_t vitesseDroite)
{
    moteurGauche_.ajusterVitessePourcent(vitesseGauche);
    moteurDroit_.ajusterVitessePourcent(vitesseDroite);
}