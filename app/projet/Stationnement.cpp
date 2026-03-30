/*
 * Stationnement.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Implémentation de la classe responsable du stationnement
 */

#include "Stationnement.h"

#include "ConfigurationProjet.h"
#include "ControleMoteurs.h"
#include "Deplacements.h"
#include "OutilsProjet.h"
#include "SuiveurLigne.h"
#include "SuiviLigneParcours.h"

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
    uint8_t numeroStationnementCorrige = numeroStationnement;

    if (numeroStationnementCorrige < NUMERO_STATIONNEMENT_MIN) {
        numeroStationnementCorrige = NUMERO_STATIONNEMENT_MIN;
    }

    if (numeroStationnementCorrige > NUMERO_STATIONNEMENT_MAX) {
        numeroStationnementCorrige = NUMERO_STATIONNEMENT_MAX;
    }

    const CoteSuivi coteSuivi =
        estCoinEst ? CoteSuivi::GAUCHE : CoteSuivi::DROITE;

    uint8_t nombreReperesPasses = 0;
    bool etaitSurRepere = false;

    while (nombreReperesPasses < numeroStationnementCorrige) {
        const uint8_t capteurs = suiveurLigne_.lireCapteurs();
        const bool estRepere =
            compterBitsActifsSur5(capteurs) >=
            SEUIL_REPERE_STATIONNEMENT_CAPTEURS;

        suiviLigne_.ajusterSuiviSelonCapteurs(capteurs, coteSuivi);

        if (estRepere && !etaitSurRepere) {
            nombreReperesPasses++;
        }

        etaitSurRepere = estRepere;
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
    bool estRepereStationnementConfirme = false;

    while ((tempsEcouleMillisecondes < TIMEOUT_STATIONNEMENT_MS) &&
           !estRepereStationnementConfirme) {
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

        estRepereStationnementConfirme =
            nombreLecturesConsecutives >=
            NOMBRE_LECTURES_REPERE_STATIONNEMENT;

        if (!estRepereStationnementConfirme) {
            attendreMillisecondes(PAS_STATIONNEMENT_MS);
            tempsEcouleMillisecondes += PAS_STATIONNEMENT_MS;
        }
    }

    moteurs_.arreter();
}