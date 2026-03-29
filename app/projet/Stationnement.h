#ifndef STATIONNEMENT_H
#define STATIONNEMENT_H

/*
 * Stationnement.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Logique de stationnement finale
 */

#include <stdint.h>

class ControleMoteurs;
class Deplacements;
class SuiveurLigne;
class SuiviLigneParcours;

class Stationnement
{
public:
    Stationnement(SuiveurLigne& suiveurLigne,
                  ControleMoteurs& moteurs,
                  Deplacements& deplacements,
                  SuiviLigneParcours& suiviLigne);

    void executer(bool estCoinEst,
                  uint8_t numeroStationnement);

private:
    SuiveurLigne& suiveurLigne_;
    ControleMoteurs& moteurs_;
    Deplacements& deplacements_;
    SuiviLigneParcours& suiviLigne_;
};

#endif