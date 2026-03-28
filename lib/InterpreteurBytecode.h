#ifndef INTERPRETEUR_BYTECODE_H
#define INTERPRETEUR_BYTECODE_H

/*
 * InterpreteurBytecode.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Lecture et interprétation du bytecode du TP9
 */

#include <stdint.h>

#include "ControleMoteurs.h"
#include "Del.h"
#include "ProgrammeBytecode.h"
#include "Sonorite.h"

class InterpreteurBytecode
{
public:
    InterpreteurBytecode(ProgrammeBytecode& programme,
                         Del& del,
                         ControleMoteurs& moteurs,
                         Sonorite& sonorite);

    void executer();

private:
    static void attendreMultiple25ms_(uint8_t facteur);
    static uint8_t convertirVitesseOperandeEnPourcent_(uint8_t operande);

    void executerDel_(uint8_t instruction, uint8_t operande);
    void executerMoteurs_(uint8_t instruction, uint8_t operande);
    void executerSonorite_(uint8_t instruction, uint8_t operande);
    bool executerBoucle_(uint8_t instruction,
                         uint8_t operande,
                         uint16_t& prochaineAdresse);

    void executerVirageDroite_();
    void executerVirageGauche_();

    ProgrammeBytecode& programme_;
    Del& del_;
    ControleMoteurs& moteurs_;
    Sonorite& sonorite_;

    bool estDebutTrouve_;
    bool estBoucleActive_;

    uint16_t adresseDebutBoucle_;
    uint8_t compteurBoucle_;
};

#endif