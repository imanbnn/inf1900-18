/*
 * SuiveurLigne.cpp
 *
 *   Implémentation de la classe SuiveurLigne
 *   Lecture numérique directe sur PINC
 *   Switch Maker Line sur DARK : 1 = ligne détectée
 *
 *   Utilisations dans le projet
 *       Navigation : lirePosition() pour suivre les murs
 *                    et tourner aux coins
 *       Détection  : estSurObjet() pour compter les zones
 *                    endommagées dans les couloirs et les
 *                    objets au sol dans les locaux
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "SuiveurLigne.h"

namespace
{
    bool correspondAuMasqueExact(uint8_t capteursLus,
                                 uint8_t masqueAttendu,
                                 uint8_t masqueTousCapteurs)
    {
        const uint8_t masqueNonAttendu =
            masqueTousCapteurs & (uint8_t)~masqueAttendu;

        return (capteursLus & masqueAttendu) &&
               !(capteursLus & masqueNonAttendu);
    }
}


SuiveurLigne::SuiveurLigne(uint8_t numeroBrocheDepart)
    : numeroBrocheDepart_(numeroBrocheDepart)
{
    const uint8_t masqueCapteurs =
        (uint8_t)(masqueTousCapteurs_ << numeroBrocheDepart_);

    DDRC &= (uint8_t)~masqueCapteurs;
}


uint8_t SuiveurLigne::lireCapteurs()
{
    return (PINC >> numeroBrocheDepart_) & masqueTousCapteurs_;
}


bool SuiveurLigne::estSurObjet()
{
    return lireCapteurs() != 0;
}


SuiveurLigne::Position SuiveurLigne::lirePosition()
{
    const uint8_t capteursLus = lireCapteurs();

    if (capteursLus == 0) {
        return LIGNE_NON_DETECTEE;
    }

    if (capteursLus == masqueTousCapteurs_) {
        return CROISEMENT;
    }

    if (correspondAuMasqueExact(capteursLus,
                                masqueCapteur1_,
                                masqueTousCapteurs_)) {
        return EXTREME_GAUCHE;
    }

    if (correspondAuMasqueExact(capteursLus,
                                masqueCapteur5_,
                                masqueTousCapteurs_)) {
        return EXTREME_DROITE;
    }

    if (correspondAuMasqueExact(capteursLus,
                                masqueCapteursGauche_,
                                masqueTousCapteurs_)) {
        return GAUCHE;
    }

    if (correspondAuMasqueExact(capteursLus,
                                masqueCapteursDroite_,
                                masqueTousCapteurs_)) {
        return DROITE;
    }

    return CENTRE;
}