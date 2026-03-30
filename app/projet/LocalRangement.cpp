/*
 * LocalRangement.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Implémentation de la classe responsable du traitement du local de rangement
 */

#include "LocalRangement.h"

#include "ConfigurationProjet.h"
#include "ControleMoteurs.h"
#include "Deplacements.h"
#include "Del.h"
#include "OutilsProjet.h"
#include "SuiveurLigne.h"

LocalRangement::LocalRangement(SuiveurLigne& suiveurLigne,
                               ControleMoteurs& moteurs,
                               Del& del,
                               Deplacements& deplacements)
    : suiveurLigne_(suiveurLigne),
      moteurs_(moteurs),
      del_(del),
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

    const uint8_t nombreObjets = compterObjets_();

    deplacements_.reculerPendant(VITESSE_SORTIE_LOCAL,
                                 DUREE_RECUL_LOCAL_RANGEMENT_MS);

    if (estEntreeAGauche) {
        deplacements_.tournerDroite90();
    }
    else {
        deplacements_.tournerGauche90();
    }

    del_.eteindre();

    return nombreObjets;
}


uint8_t LocalRangement::compterObjets_()
{
    uint8_t nombreObjets = 0;
    bool etaitSurObjet = false;

    moteurs_.avancer(VITESSE_ENTREE_LOCAL);

    const uint32_t debutBalayageMs = obtenirTempsProjetMs();

    while (!tempsEcouleDepuis(debutBalayageMs,
                              DUREE_AVANCE_LOCAL_RANGEMENT_MS)) {
        const uint8_t capteurs = suiveurLigne_.lireCapteurs();
        const bool estSurObjet =
            compterBitsActifsSur5(capteurs) >= SEUIL_OBJET_CAPTEURS;

        if (estSurObjet) {
            del_.rouge();

            if (!etaitSurObjet &&
                (nombreObjets < NOMBRE_OBJETS_MAX_LOCAL_RANGEMENT)) {
                nombreObjets++;
            }
        }
        else {
            del_.eteindre();
        }

        etaitSurObjet = estSurObjet;
        attendreMillisecondes(PERIODE_SUIVI_MS);
    }

    moteurs_.arreter();
    del_.eteindre();

    return nombreObjets;
}