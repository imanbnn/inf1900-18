#ifndef DEPLACEMENTS_H
#define DEPLACEMENTS_H

/*
 * Deplacements.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Regroupe des manœuvres simples basées sur le contrôle des moteurs
 *   afin de réutiliser les mêmes séquences partout
 *   sans dupliquer le code
 */

#include <stdint.h>

#include "ControleMoteurs.h"

class Deplacements
{
public:
    explicit Deplacements(ControleMoteurs& moteurs);

    void avancerPendant(uint8_t vitessePourcent,
                        uint16_t dureeMillisecondes);

    void reculerPendant(uint8_t vitessePourcent,
                        uint16_t dureeMillisecondes);

    void tournerGauche90();
    void tournerDroite90();
    void demiTour();

    void tournerAngleBalayage(bool estEntreeAGauche,
                              int8_t angleDegres);

private:
    void tournerGauchePendant_(uint16_t dureeMillisecondes);
    void tournerDroitePendant_(uint16_t dureeMillisecondes);

    ControleMoteurs& moteurs_;
};

#endif