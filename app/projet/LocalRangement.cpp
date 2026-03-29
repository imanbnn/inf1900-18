/*
 * LocalRangement.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "LocalRangement.h"

#include "BrocheIo.h"
#include "ConfigurationProjet.h"
#include "ControleMoteurs.h"
#include "Deplacements.h"
#include "OutilsProjet.h"
#include "SuiveurLigne.h"

LocalRangement::LocalRangement(SuiveurLigne& suiveurLigne,
                               ControleMoteurs& moteurs,
                               BrocheIo& delLibre,
                               Deplacements& deplacements)
    : suiveurLigne_(suiveurLigne),
      moteurs_(moteurs),
      delLibre_(delLibre),
      deplacements_(deplacements)
{
}

uint8_t LocalRangement::gerer(bool estEntreeAGauche)
{
    if (estEntreeAGauche) {
        deplacements_.tournerGauche90();
    }
    else {
        deplacements_.tournerDroite90();
    }

    const uint8_t nombreObjetsTrouves = compterObjetsAuSol_();

    delLibre_.mettreAZero();

    deplacements_.reculerPendant(VITESSE_SORTIE_LOCAL,
                                 DUREE_RECUL_LOCAL_RANGEMENT_MS);

    if (estEntreeAGauche) {
        deplacements_.tournerDroite90();
    }
    else {
        deplacements_.tournerGauche90();
    }

    return nombreObjetsTrouves;
}

uint8_t LocalRangement::compterObjetsAuSol_()
{
    uint8_t nombreObjetsTrouves = 0;
    bool estSurObjetPrecedemment = false;

    moteurs_.avancer(VITESSE_ENTREE_LOCAL);

    uint16_t tempsEcouleMillisecondes = 0;

    while (tempsEcouleMillisecondes <
           DUREE_AVANCE_LOCAL_RANGEMENT_MS) {
        const uint8_t capteurs = suiveurLigne_.lireCapteurs();

        const bool estSurObjet =
            compterBitsActifsSur5(capteurs) >=
            SEUIL_OBJET_CAPTEURS;

        if (estSurObjet) {
            delLibre_.mettreAUn();
        }
        else {
            delLibre_.mettreAZero();
        }

        if (estSurObjet && !estSurObjetPrecedemment) {
            if (nombreObjetsTrouves <
                NOMBRE_OBJETS_MAX_LOCAL_RANGEMENT) {
                nombreObjetsTrouves++;
            }
        }

        estSurObjetPrecedemment = estSurObjet;

        attendreMillisecondes(PERIODE_SUIVI_MS);
        tempsEcouleMillisecondes += PERIODE_SUIVI_MS;
    }

    moteurs_.arreter();

    return nombreObjetsTrouves;
}