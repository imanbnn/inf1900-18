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
 *   Déclaration de la classe responsable du traitement du local de rangement
 */

#include <stdint.h>

class ControleMoteurs;
class Deplacements;
class Del;
class SuiveurLigne;

class LocalRangement
{
public:
    LocalRangement(SuiveurLigne& suiveurLigne,
                   ControleMoteurs& moteurs,
                   Del& del,
                   Deplacements& deplacements);

    uint8_t gerer(bool estEntreeAGauche);

private:
    uint8_t compterObjets_();

    SuiveurLigne& suiveurLigne_;
    ControleMoteurs& moteurs_;
    Del& del_;
    Deplacements& deplacements_;
};

#endif