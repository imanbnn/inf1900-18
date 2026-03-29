/*
 * Stationnement.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "Stationnement.h"

#include "ConfigurationProjet.h"
#include "ControleMoteurs.h"
#include "Deplacements.h"
#include "OutilsProjet.h"
#include "SuiveurLigne.h"
#include "SuiviLigneParcours.h"

namespace
{
    constexpr uint8_t NUMERO_STATIONNEMENT_MIN = 1;
    constexpr uint8_t NUMERO_STATIONNEMENT_MAX = NOMBRE_STATIONNEMENTS;
}

Stationnement::Stationnement(SuiveurLigne& suiveurLigne,
                             ControleMoteurs& moteurs,
                             Deplacements& deplacements,
                             SuiviLigneParcours& suiviLigne)
    : suiveurLigne_(suiveurLigne),
      moteurs_(moteurs),
      deplacements_(deplacements),
      suiviLigne_(suiviLigne)
{
}

void Stationnement::executer(bool estCoinEst,
                             uint8_t numeroStationnement)
{
    if (numeroStationnement < NUMERO_STATIONNEMENT_MIN) {
        numeroStationnement = NUMERO_STATIONNEMENT_MIN;
    }

    if (numeroStationnement > NUMERO_STATIONNEMENT_MAX) {
        numeroStationnement = NUMERO_STATIONNEMENT_MAX;
    }

    uint8_t nombreReperesPasses = 0;
    bool etaitSurReperePrecedent = false;

    const CoteSuivi coteSuivi =
        estCoinEst ? CoteSuivi::GAUCHE : CoteSuivi::DROITE;

    while (nombreReperesPasses < numeroStationnement) {
        const uint8_t capteurs = suiveurLigne_.lireCapteurs();

        suiviLigne_.ajusterMoteursSelonLigne(capteurs,
                                             coteSuivi);

        const bool estSurRepere =
            compterBitsActifsSur5(capteurs) >=
            SEUIL_INTERSECTION_CAPTEURS;

        if (estSurRepere && !etaitSurReperePrecedent) {
            nombreReperesPasses++;
        }

        etaitSurReperePrecedent = estSurRepere;

        attendreMillisecondes(PERIODE_SUIVI_MS);
    }

    moteurs_.arreter();

    deplacements_.avancerPendant(VITESSE_SUIVI_LIGNE,
                                 DUREE_DECALAGE_CENTRE_STATIONNEMENT_MS);

    if (estCoinEst) {
        deplacements_.tournerGauche90();
    }
    else {
        deplacements_.tournerDroite90();
    }

    moteurs_.avancer(VITESSE_ENTREE_LOCAL);

    uint16_t tempsEcouleMillisecondes = 0;
    uint8_t nombreLecturesConsecutives = 0;

    while (tempsEcouleMillisecondes < TIMEOUT_STATIONNEMENT_MS) {
        const uint8_t capteurs = suiveurLigne_.lireCapteurs();

        const bool estRepereStationnement =
            compterBitsActifsSur5(capteurs) >=
            SEUIL_REPERE_STATIONNEMENT_CAPTEURS;

        if (estRepereStationnement) {
            if (nombreLecturesConsecutives <
                NOMBRE_LECTURES_REPERE_STATIONNEMENT) {
                nombreLecturesConsecutives++;
            }
        }
        else {
            nombreLecturesConsecutives = 0;
        }

        if (nombreLecturesConsecutives >=
            NOMBRE_LECTURES_REPERE_STATIONNEMENT) {
            break;
        }

        attendreMillisecondes(PAS_STATIONNEMENT_MS);
        tempsEcouleMillisecondes += PAS_STATIONNEMENT_MS;
    }

    moteurs_.arreter();
}