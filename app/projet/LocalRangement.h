#ifndef LOCAL_RANGEMENT_H
#define LOCAL_RANGEMENT_H

/*
 * LocalRangement.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Gestion d'un local de rangement
 *   Comptage des objets au sol avec le suiveur de ligne
 */

#include <stdint.h>

class BrocheIo;
class ControleMoteurs;
class Deplacements;
class SuiveurLigne;

class LocalRangement
{
public:
    LocalRangement(SuiveurLigne& suiveurLigne,
                   ControleMoteurs& moteurs,
                   BrocheIo& delLibre,
                   Deplacements& deplacements);

    uint8_t gerer(bool estEntreeAGauche);

private:
    uint8_t compterObjetsAuSol_();

    SuiveurLigne& suiveurLigne_;
    ControleMoteurs& moteurs_;
    BrocheIo& delLibre_;
    Deplacements& deplacements_;
};

#endif