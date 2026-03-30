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
#include "Sonorite.h"
#include "StockageProjet.h"
#include "UART0.h"

namespace
{
    constexpr uint16_t ADRESSE_DEPART_PROGRAMME = 0x0000;

    constexpr uint16_t TAILLE_MINIMALE_PROGRAMME = 2;
    constexpr uint16_t TAILLE_INSTRUCTION_OCTETS = 2;

    constexpr uint16_t DECALAGE_INSTRUCTION_DEBUT = 0;
    constexpr uint16_t DECALAGE_INSTRUCTION_SENS = 2;
    constexpr uint16_t DECALAGE_PREMIERE_NOTE = 4;
    constexpr uint16_t DECALAGE_INSTRUCTION_STATIONNEMENT = 10;
    constexpr uint16_t DECALAGE_INSTRUCTION_FIN = 12;

    constexpr uint8_t DECALAGE_OCTET_FAIBLE = 1;
    constexpr uint8_t DECALAGE_OCTET_FORT = 8;

    constexpr uint16_t DUREE_CONFIRMATION_STATIONNEMENT_MS = 500;
    constexpr uint16_t DUREE_CLIGNOTEMENT_MODE_MS = 2000;

    void mettreRobotEnEtatSecuritaire(Del& del, Sonorite& sonorite)
    {
        sonorite.arreter();
        del.eteindre();
    }


    void confirmerStationnement(Del& del, uint8_t numeroStationnement)
    {
        for (uint8_t i = 0; i < numeroStationnement; i++) {
            afficherAmbre(del, DUREE_CONFIRMATION_STATIONNEMENT_MS);
            attendreMillisecondes(DUREE_CONFIRMATION_STATIONNEMENT_MS);
        }
    }


    bool recevoirProgrammeEtSauvegarder(Memoire24CXXX& memoire,
                                        uint16_t adresseDepart)
    {
        const uint8_t octetFaible = UART0::recevoirOctet();
        const uint8_t octetFort = UART0::recevoirOctet();

        const uint16_t tailleProgramme =
            ((uint16_t)octetFort << DECALAGE_OCTET_FORT) |
            (uint16_t)octetFaible;

        if (tailleProgramme < TAILLE_MINIMALE_PROGRAMME) {
            return false;
        }

        memoire.ecriture(adresseDepart, octetFaible);
        memoire.ecriture(adresseDepart + DECALAGE_OCTET_FAIBLE, octetFort);

        for (uint16_t i = TAILLE_MINIMALE_PROGRAMME;
             i < tailleProgramme;
             i++) {
            const uint8_t octet = UART0::recevoirOctet();
            memoire.ecriture(adresseDepart + i, octet);
        }

        return true;
    }


    void lireInstructionEtOperande(Memoire24CXXX& memoire,
                                   uint16_t adresseInstruction,
                                   uint8_t& instruction,
                                   uint8_t& operande)
    {
        memoire.lecture(adresseInstruction, &instruction);
        memoire.lecture(adresseInstruction + DECALAGE_OCTET_FAIBLE,
                        &operande);
    }


    bool extraireParametres(Memoire24CXXX& memoire,
                            uint16_t adresseDepart,
                            ParametresProjet& parametres)
    {
        const uint16_t adressePremiereInstruction =
            adresseDepart + TAILLE_INSTRUCTION_OCTETS;

        uint8_t instruction = 0;
        uint8_t operande = 0;

        lireInstructionEtOperande(memoire,
                                  adressePremiereInstruction +
                                      DECALAGE_INSTRUCTION_DEBUT,
                                  instruction,
                                  operande);

        if (instruction != JeuInstructions::dbt) {
            return false;
        }

        lireInstructionEtOperande(memoire,
                                  adressePremiereInstruction +
                                      DECALAGE_INSTRUCTION_SENS,
                                  instruction,
                                  operande);

        if (instruction == JeuInstructions::trd) {
            parametres.sensParcours = SensParcours::HORAIRE;
        }
        else if (instruction == JeuInstructions::trg) {
            parametres.sensParcours = SensParcours::ANTI_HORAIRE;
        }
        else {
            return false;
        }

        for (uint8_t i = 0; i < NOMBRE_NOTES_MIDI; i++) {
            const uint16_t adresseInstructionNote =
                adressePremiereInstruction +
                DECALAGE_PREMIERE_NOTE +
                (uint16_t)i * TAILLE_INSTRUCTION_OCTETS;

            lireInstructionEtOperande(memoire,
                                      adresseInstructionNote,
                                      instruction,
                                      operande);

            if (instruction != JeuInstructions::sgo) {
                return false;
            }

            parametres.notesMidi[i] = operande;
        }

        lireInstructionEtOperande(memoire,
                                  adressePremiereInstruction +
                                      DECALAGE_INSTRUCTION_STATIONNEMENT,
                                  instruction,
                                  operande);

        if (instruction != JeuInstructions::att) {
            return false;
        }

        if ((operande < NUMERO_STATIONNEMENT_MIN) ||
            (operande > NUMERO_STATIONNEMENT_MAX)) {
            operande = NUMERO_STATIONNEMENT_PAR_DEFAUT;
        }

        parametres.numeroStationnement = operande;

        lireInstructionEtOperande(memoire,
                                  adressePremiereInstruction +
                                      DECALAGE_INSTRUCTION_FIN,
                                  instruction,
                                  operande);

        return instruction == JeuInstructions::fin;
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

    if (!recevoirProgrammeEtSauvegarder(memoire,
                                        ADRESSE_DEPART_PROGRAMME)) {
        DEBUG_PRINT("Mode instruction reception echouee\n");
        mettreRobotEnEtatSecuritaire(del, sonorite);
        return;
    }

    if (!extraireParametres(memoire,
                            ADRESSE_DEPART_PROGRAMME,
                            parametres)) {
        DEBUG_PRINT("Mode instruction extraction parametres echouee\n");
        mettreRobotEnEtatSecuritaire(del, sonorite);
        return;
    }

    stockage.ecrireParametres(parametres);
    stockage.effacerResultats();

    confirmerReception(del, sonorite, parametres);

    while (true) {
    }
}