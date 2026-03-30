/*
 * SelectionMode.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Implémentation de la sélection du mode de fonctionnement
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

    bool boutonAppuyePendantAttente(Bouton& bouton, uint16_t dureeMs)
    {
        const uint32_t debutAttenteMs = obtenirTempsProjetMs();

        while (!tempsEcouleDepuis(debutAttenteMs, dureeMs)) {
            if (bouton.estAppuye()) {
                return true;
            }

            attendreMillisecondes(PAS_VERIFICATION_BOUTON_MS);
        }

        return false;
    }
}

ModeProjet selectionnerMode(Bouton& bouton, Del& del)
{
    attendreMillisecondes(DELAI_DEMARRAGE_MS);

    del.vert();

    if (boutonAppuyePendantAttente(bouton, DELAI_ATTENTE_BOUTON_MS)) {
        del.eteindre();
        return ModeProjet::INSTRUCTION;
    }

    del.eteindre();
    attendreMillisecondes(DELAI_ENTRE_MODES_MS);

    del.rouge();

    if (boutonAppuyePendantAttente(bouton, DELAI_ATTENTE_BOUTON_MS)) {
        del.eteindre();
        return ModeProjet::RAPPORT;
    }

    del.eteindre();

    return ModeProjet::EXECUTION;
}