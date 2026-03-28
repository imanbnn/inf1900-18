#ifndef MOTEUR_H
#define MOTEUR_H

/*
 * Moteur.h
 *
 *   Contrôle d'un moteur avec PWM et direction
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Identifications matérielles
 *   Canal A  OC0A PB3  partagé ISP, débrancher avant make install
 *   Canal B  OC0B PB4  partagé ISP, débrancher avant make install
 *   DIR      broche configurable via BrocheIo
 *
 * Constructeur
 *   configuration.canalPwm               CanalPwm0::CANAL_A ou CANAL_B
 *   configuration.brocheDirection        broche de direction  PB2, PB5
 *   configuration.estNiveauHautPourAvant true si niveau haut = avant
 *                                        false si niveau bas = avant
 *
 * Convention pour ajusterVitessePourcent(int8_t)
 *   Valeur positive  rotation vers l'avant
 *   Valeur négative  rotation vers l'arrière
 *   0                arrêt
 *
 * Dépendances
 *   BrocheIo
 *   MinuteriePwm0
 */

#include <stdint.h>

#include "BrocheIo.h"
#include "MinuteriePwm0.h"

class Moteur
{
public:
    struct Configuration
    {
        CanalPwm0 canalPwm;
        BrocheIo brocheDirection;
        bool estNiveauHautPourAvant;
    };

    explicit Moteur(const Configuration& configuration);

    void initialiser();
    void ajusterVitessePourcent(int8_t pourcentSigne);
    void arreter();

private:
    void ajusterDirection_(bool estAvant);
    static uint8_t bornerValeurAbsoluePourcent_(int16_t valeur);

    CanalPwm0 canalPwm_;
    BrocheIo brocheDirection_;
    bool estNiveauHautPourAvant_;
    bool estInitialisee_;
};

#endif