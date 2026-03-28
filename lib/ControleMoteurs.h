#ifndef CONTROLE_MOTEURS_H
#define CONTROLE_MOTEURS_H

/*
 * ControleMoteurs.h
 *
 *   Commandes haut niveau pour deux moteurs
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Identifications materielles
 *   Moteur A  DIR PB2, PWM PB3 OC0A
 *   Moteur B  PWM PB4 OC0B, DIR PB5
 *   Toujours debrancher avant make install
 *
 * Constructeur
 *   moteurGauche  instance de Moteur passee par reference
 *   moteurDroit   instance de Moteur passee par reference
 *
 * initialiser(PrescalerTimer0) doit etre appelee
 * avant toute commande.
 *
 * Dependances
 *   Moteur
 *   MinuteriePwm0
 */

#include <stdint.h>

#include "Moteur.h"
#include "MinuteriePwm0.h"

class ControleMoteurs
{
public:
    ControleMoteurs(Moteur& moteurGauche,
                    Moteur& moteurDroit);

    void initialiser(PrescalerTimer0 prescaler);

    void arreter();

    void avancer(uint8_t vitessePourcent);
    void reculer(uint8_t vitessePourcent);

    void tournerGauche(uint8_t vitessePourcent);
    void tournerDroite(uint8_t vitessePourcent);

    void ajusterVitesses(int8_t vitesseGauche,
                         int8_t vitesseDroite);

private:
    static const uint8_t pourcentageMax = 100;

    Moteur& moteurGauche_;
    Moteur& moteurDroit_;
    bool estInitialisee_;
};

#endif