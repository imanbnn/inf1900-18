/*
 * ModeInstruction.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Implémentation du mode instruction
 */

#include "ModeInstruction.h"

#include <stdint.h>

#include "ConfigurationProjet.h"
#include "Debug.h"
#include "Del.h"
#include "JeuInstructions.h"
#include "Memoire_24.h"
#include "OutilsProjet.h"
#include "ProgrammeBytecode.h"
#include "Sonorite.h"
#include "StockageProjet.h"
#include "UART0.h"

namespace
{
    constexpr uint16_t TAILLE_ENTETE_PROGRAMME = 2;
    constexpr uint16_t TAILLE_INSTRUCTION_OCTETS = 2;
    constexpr uint16_t TAILLE_PROGRAMME_ATTENDUE = 16;

    constexpr uint16_t INDEX_INSTRUCTION_DEBUT = 2;
    constexpr uint16_t INDEX_INSTRUCTION_SENS = 4;
    constexpr uint16_t INDEX_PREMIERE_NOTE = 6;
    constexpr uint16_t INDEX_INSTRUCTION_STATIONNEMENT = 12;
    constexpr uint16_t INDEX_INSTRUCTION_FIN = 14;

    constexpr uint16_t DUREE_CONFIRMATION_STATIONNEMENT_MS = 500;
    constexpr uint16_t DUREE_CLIGNOTEMENT_MODE_MS = 2000;

    void mettreRobotEnEtatSecuritaire(Del& del, Sonorite& sonorite)
    {
        sonorite.arreter();
        del.eteindre();
    }


    bool parametresEgaux(const ParametresProjet& premier,
                         const ParametresProjet& second)
    {
        if (premier.sensParcours != second.sensParcours) {
            return false;
        }

        if (premier.numeroStationnement != second.numeroStationnement) {
            return false;
        }

        for (uint8_t i = 0; i < NOMBRE_NOTES_MIDI; i++) {
            if (premier.notesMidi[i] != second.notesMidi[i]) {
                return false;
            }
        }

        return true;
    }


    void confirmerStationnement(Del& del, uint8_t numeroStationnement)
    {
        for (uint8_t i = 0; i < numeroStationnement; i++) {
            afficherAmbre(del, DUREE_CONFIRMATION_STATIONNEMENT_MS);
            attendreMillisecondes(DUREE_CONFIRMATION_STATIONNEMENT_MS);
        }
    }


    bool recevoirProgrammeEnRam(uint8_t programme[TAILLE_PROGRAMME_ATTENDUE])
    {
        const uint8_t premierOctetTaille = UART0::recevoirOctet();
        const uint8_t secondOctetTaille = UART0::recevoirOctet();

        const uint16_t tailleFortPuisFaible =
            (uint16_t)(((uint16_t)premierOctetTaille << 8) |
                       secondOctetTaille);
        const uint16_t tailleFaiblePuisFort =
            (uint16_t)(((uint16_t)secondOctetTaille << 8) |
                       premierOctetTaille);

        if (tailleFortPuisFaible == TAILLE_PROGRAMME_ATTENDUE) {
            programme[0] = premierOctetTaille;
            programme[1] = secondOctetTaille;
        }
        else if (tailleFaiblePuisFort == TAILLE_PROGRAMME_ATTENDUE) {
            programme[0] = 0;
            programme[1] = (uint8_t)TAILLE_PROGRAMME_ATTENDUE;
        }
        else {
            return false;
        }

        for (uint16_t i = TAILLE_ENTETE_PROGRAMME;
             i < TAILLE_PROGRAMME_ATTENDUE;
             i++) {
            programme[i] = UART0::recevoirOctet();
        }

        return true;
    }


    bool extraireParametresDepuisRam(
        const uint8_t programme[TAILLE_PROGRAMME_ATTENDUE],
        ParametresProjet& parametres)
    {
        if (programme[INDEX_INSTRUCTION_DEBUT] != JeuInstructions::dbt) {
            return false;
        }

        if (programme[INDEX_INSTRUCTION_SENS] == JeuInstructions::trd) {
            parametres.sensParcours = SensParcours::HORAIRE;
        }
        else if (programme[INDEX_INSTRUCTION_SENS] == JeuInstructions::trg) {
            parametres.sensParcours = SensParcours::ANTI_HORAIRE;
        }
        else {
            return false;
        }

        for (uint8_t i = 0; i < NOMBRE_NOTES_MIDI; i++) {
            const uint16_t indexInstruction =
                INDEX_PREMIERE_NOTE +
                (uint16_t)i * TAILLE_INSTRUCTION_OCTETS;

            if (programme[indexInstruction] != JeuInstructions::sgo) {
                return false;
            }

            parametres.notesMidi[i] = programme[indexInstruction + 1];
        }

        if (programme[INDEX_INSTRUCTION_STATIONNEMENT] !=
            JeuInstructions::att) {
            return false;
        }

        parametres.numeroStationnement =
            programme[INDEX_INSTRUCTION_STATIONNEMENT + 1];

        if ((parametres.numeroStationnement < NUMERO_STATIONNEMENT_MIN) ||
            (parametres.numeroStationnement > NUMERO_STATIONNEMENT_MAX)) {
            parametres.numeroStationnement = NUMERO_STATIONNEMENT_PAR_DEFAUT;
        }

        return programme[INDEX_INSTRUCTION_FIN] == JeuInstructions::fin;
    }


    void jouerNote(Sonorite& sonorite, uint8_t noteMidi)
    {
        sonorite.jouerNoteMidi(noteMidi);
        attendreMillisecondes(DUREE_NOTE_ALERTE_MS);
        sonorite.arreter();
    }


    void confirmerReception(Del& del,
                            Sonorite& sonorite,
                            const ParametresProjet& parametres)
    {
        for (uint8_t i = 0; i < NOMBRE_NOTES_MIDI; i++) {
            jouerNote(sonorite, parametres.notesMidi[i]);
            attendreMillisecondes(PAUSE_ENTRE_NOTES_MS);
        }

        attendreMillisecondes(PAUSE_APRES_ALERTE_MS);

        if (parametres.sensParcours == SensParcours::HORAIRE) {
            del.vert();
        }
        else {
            del.rouge();
        }

        attendreMillisecondes(DUREE_CONFIRMATION_MS);

        del.eteindre();
        attendreMillisecondes(DUREE_CONFIRMATION_MS);

        confirmerStationnement(del, parametres.numeroStationnement);
        del.eteindre();
    }
}

void executerModeInstruction(Memoire24CXXX& memoire,
                             Del& del,
                             Sonorite& sonorite,
                             StockageProjet& stockage)
{
    clignoterCouleur(del,
                     &Del::vert,
                     DUREE_CLIGNOTEMENT_MODE_MS,
                     FREQUENCE_CLIGNOTEMENT_MODE_HZ);

    ParametresProjet parametres =
    {
        SensParcours::HORAIRE,
        {NOTE_PAR_DEFAUT, NOTE_PAR_DEFAUT, NOTE_PAR_DEFAUT},
        NUMERO_STATIONNEMENT_PAR_DEFAUT
    };

    uint8_t programmeRecu[TAILLE_PROGRAMME_ATTENDUE] = {0};

    if (!recevoirProgrammeEnRam(programmeRecu)) {
        DEBUG_PRINT("Mode instruction reception UART echouee\n");
        mettreRobotEnEtatSecuritaire(del, sonorite);
        return;
    }

    if (!extraireParametresDepuisRam(programmeRecu, parametres)) {
        DEBUG_PRINT("Mode instruction extraction parametres echouee\n");
        mettreRobotEnEtatSecuritaire(del, sonorite);
        return;
    }

    ProgrammeBytecode programmeBytecode(memoire);

    if (!programmeBytecode.ecrireProgramme(programmeRecu,
                                           TAILLE_PROGRAMME_ATTENDUE)) {
        DEBUG_PRINT("Mode instruction ecriture programme echouee\n");
        mettreRobotEnEtatSecuritaire(del, sonorite);
        return;
    }

    programmeBytecode.initialiser();

    if (programmeBytecode.obtenirLongueurTotale() != TAILLE_PROGRAMME_ATTENDUE) {
        DEBUG_PRINT("Mode instruction relecture programme echouee\n");
        mettreRobotEnEtatSecuritaire(del, sonorite);
        return;
    }

    stockage.ecrireParametres(parametres);

    ParametresProjet verification = {};

    if (!stockage.lireParametres(verification) ||
        !parametresEgaux(parametres, verification)) {
        DEBUG_PRINT("Mode instruction verification stockage echouee\n");
        mettreRobotEnEtatSecuritaire(del, sonorite);
        return;
    }

    stockage.effacerResultats();
    confirmerReception(del, sonorite, parametres);

    while (true) {
    }
}