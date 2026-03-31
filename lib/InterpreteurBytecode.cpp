/*
 * InterpreteurBytecode.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#define F_CPU 8000000UL

#include "InterpreteurBytecode.h"

#include <util/delay.h>

#include "Debug.h"
#include "JeuInstructions.h"

namespace
{
    constexpr uint8_t COULEUR_DEL_VERTE = 1;
    constexpr uint8_t COULEUR_DEL_ROUGE = 2;

    constexpr uint8_t VITESSE_VIRAGE_POURCENT = 55;
    constexpr uint16_t DUREE_VIRAGE_MS = 760;

    constexpr uint8_t DUREE_ATTENTE_UNITAIRE_MS = 25;

    constexpr uint16_t TAILLE_ENTETE_PROGRAMME_OCTETS = 2;
    constexpr uint16_t TAILLE_INSTRUCTION_OCTETS = 2;

    constexpr uint16_t VALEUR_MAX_OPERANDE_VITESSE = 255;
    constexpr uint16_t POURCENTAGE_MAX = 100;

    void arreterSorties(ControleMoteurs& moteurs,
                        Sonorite& sonorite,
                        Del& del)
    {
        moteurs.arreter();
        sonorite.arreter();
        del.eteindre();
    }
}


InterpreteurBytecode::InterpreteurBytecode(ProgrammeBytecode& programme,
                                           Del& del,
                                           ControleMoteurs& moteurs,
                                           Sonorite& sonorite)
    : programme_(programme),
      del_(del),
      moteurs_(moteurs),
      sonorite_(sonorite),
      estDebutTrouve_(false),
      estBoucleActive_(false),
      adresseDebutBoucle_(0),
      compteurBoucle_(0)
{
}


void InterpreteurBytecode::executer()
{
    programme_.initialiser();

    const uint16_t longueurTotale = programme_.obtenirLongueurTotale();

    if (longueurTotale < TAILLE_ENTETE_PROGRAMME_OCTETS) {
        return;
    }

    uint16_t adresse = programme_.obtenirAdressePremiereInstruction();
    bool estEnCours = true;

    while (estEnCours) {
        uint8_t instruction = 0;
        uint8_t operande = 0;

        if (!programme_.lireInstructionEtOperande(adresse,
                                                  instruction,
                                                  operande)) {
            break;
        }

        uint16_t prochaineAdresse = adresse + TAILLE_INSTRUCTION_OCTETS;

        if (!estDebutTrouve_) {
            if (instruction == JeuInstructions::dbt) {
                estDebutTrouve_ = true;
                DEBUG_PRINT("dbt: debut execution\n");
            }

            adresse = prochaineAdresse;
            continue;
        }

        if (instruction == JeuInstructions::fin) {
            DEBUG_PRINT("fin: fin execution\n");
            arreterSorties(moteurs_, sonorite_, del_);
            estEnCours = false;
            continue;
        }

        if (instruction == JeuInstructions::att) {
            DEBUG_PRINT("att: attendre\n");
            attendreMultiple25ms_(operande);
            adresse = prochaineAdresse;
            continue;
        }

        if (executerBoucle_(instruction, operande, prochaineAdresse)) {
            adresse = prochaineAdresse;
            continue;
        }

        executerDel_(instruction, operande);
        executerSonorite_(instruction, operande);
        executerMoteurs_(instruction, operande);

        adresse = prochaineAdresse;
    }

    arreterSorties(moteurs_, sonorite_, del_);
}


void InterpreteurBytecode::attendreMultiple25ms_(uint8_t facteur)
{
    for (uint8_t i = 0; i < facteur; i++) {
        _delay_ms(DUREE_ATTENTE_UNITAIRE_MS);
    }
}


uint8_t InterpreteurBytecode::convertirVitesseOperandeEnPourcent_(
    uint8_t operande)
{
    constexpr uint16_t ARRONDI =
        VALEUR_MAX_OPERANDE_VITESSE / 2;

    const uint16_t produit =
        (uint16_t)operande * POURCENTAGE_MAX;

    uint16_t pourcent =
        (uint16_t)((produit + ARRONDI) / VALEUR_MAX_OPERANDE_VITESSE);

    if (pourcent > POURCENTAGE_MAX) {
        pourcent = POURCENTAGE_MAX;
    }

    return (uint8_t)pourcent;
}


void InterpreteurBytecode::executerDel_(uint8_t instruction,
                                        uint8_t operande)
{
    if (instruction == JeuInstructions::dal) {
        DEBUG_PRINT("dal: allumer DEL\n");

        if (operande == COULEUR_DEL_VERTE) {
            del_.vert();
        }
        else if (operande == COULEUR_DEL_ROUGE) {
            del_.rouge();
        }

        return;
    }

    if (instruction == JeuInstructions::det) {
        DEBUG_PRINT("det: eteindre DEL\n");
        del_.eteindre();
    }
}


void InterpreteurBytecode::executerSonorite_(uint8_t instruction,
                                             uint8_t operande)
{
    if (instruction == JeuInstructions::sgo) {
        DEBUG_PRINT("sgo: jouer son\n");
        sonorite_.jouerNoteMidi(operande);
        return;
    }

    if (instruction == JeuInstructions::sar) {
        DEBUG_PRINT("sar: arreter son\n");
        sonorite_.arreter();
    }
}


void InterpreteurBytecode::executerMoteurs_(uint8_t instruction,
                                            uint8_t operande)
{
    if ((instruction == JeuInstructions::mar0) ||
        (instruction == JeuInstructions::mar1)) {
        DEBUG_PRINT("mar: arreter moteurs\n");
        moteurs_.arreter();
        return;
    }

    if (instruction == JeuInstructions::mav) {
        DEBUG_PRINT("mav: avancer\n");

        const uint8_t pourcent =
            convertirVitesseOperandeEnPourcent_(operande);

        moteurs_.avancer(pourcent);
        return;
    }

    if (instruction == JeuInstructions::mre) {
        DEBUG_PRINT("mre: reculer\n");

        const uint8_t pourcent =
            convertirVitesseOperandeEnPourcent_(operande);

        moteurs_.reculer(pourcent);
        return;
    }

    if (instruction == JeuInstructions::trd) {
        DEBUG_PRINT("trd: tourner droite\n");
        executerVirageDroite_();
        return;
    }

    if (instruction == JeuInstructions::trg) {
        DEBUG_PRINT("trg: tourner gauche\n");
        executerVirageGauche_();
    }
}


bool InterpreteurBytecode::executerBoucle_(uint8_t instruction,
                                           uint8_t operande,
                                           uint16_t& prochaineAdresse)
{
    if (instruction == JeuInstructions::dbc) {
        DEBUG_PRINT("dbc: repeter sequence\n");

        if (!estBoucleActive_) {
            estBoucleActive_ = true;
            compteurBoucle_ = operande;
            adresseDebutBoucle_ = prochaineAdresse;
        }

        return true;
    }

    if (instruction == JeuInstructions::fbc) {
        DEBUG_PRINT("fbc: fin repetition\n");

        if (estBoucleActive_ && (compteurBoucle_ != 0)) {
            compteurBoucle_--;
            prochaineAdresse = adresseDebutBoucle_;
        }
        else {
            estBoucleActive_ = false;
        }

        return true;
    }

    return false;
}


void InterpreteurBytecode::executerVirageDroite_()
{
    moteurs_.tournerDroite(VITESSE_VIRAGE_POURCENT);
    _delay_ms(DUREE_VIRAGE_MS);
    moteurs_.arreter();
}


void InterpreteurBytecode::executerVirageGauche_()
{
    moteurs_.tournerGauche(VITESSE_VIRAGE_POURCENT);
    _delay_ms(DUREE_VIRAGE_MS);
    moteurs_.arreter();
}