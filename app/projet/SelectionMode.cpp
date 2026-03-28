/*
 * SelectionMode.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "SelectionMode.h"

#include "Bouton.h"
#include "Del.h"
#include "OutilsProjet.h"

namespace
{
    constexpr uint16_t DELAI_DEMARRAGE_MS = 2000;
    constexpr uint16_t DELAI_ATTENTE_BOUTON_MS = 2000;
    constexpr uint16_t DELAI_ENTRE_MODES_MS = 1000;
    constexpr uint16_t PAS_VERIFICATION_BOUTON_MS = 20;

    bool boutonAppuyePendantDuree(Bouton& bouton, uint16_t dureeMs)
    {
        for (uint16_t tempsEcoule = 0;
             tempsEcoule < dureeMs;
             tempsEcoule += PAS_VERIFICATION_BOUTON_MS) {
            if (bouton.estAppuye()) {
                return true;
            }

            attendreMs(PAS_VERIFICATION_BOUTON_MS);
        }

        return false;
    }
}

ModeProjet selectionnerMode(Bouton& bouton, Del& del)
{
    attendreMs(DELAI_DEMARRAGE_MS);

    del.vert();

    if (boutonAppuyePendantDuree(bouton, DELAI_ATTENTE_BOUTON_MS)) {
        del.eteindre();
        return ModeProjet::INSTRUCTION;
    }

    del.eteindre();

    attendreMs(DELAI_ENTRE_MODES_MS);

    del.rouge();

    if (boutonAppuyePendantDuree(bouton, DELAI_ATTENTE_BOUTON_MS)) {
        del.eteindre();
        return ModeProjet::RAPPORT;
    }

    del.eteindre();

    return ModeProjet::EXECUTION;
}