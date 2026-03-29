/*
 * LocalTravail.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "LocalTravail.h"

#include "CapteurDistance.h"
#include "ConfigurationProjet.h"
#include "Deplacements.h"
#include "Del.h"
#include "OutilsProjet.h"
#include "Sonorite.h"

namespace
{
    constexpr int8_t ANGLE_BALAYAGE_INITIAL_DEGRES = 15;
    constexpr int8_t ANGLE_BALAYAGE_CENTRAL_DEGRES = -30;
    constexpr int8_t ANGLE_BALAYAGE_FINAL_DEGRES = 15;
}

LocalTravail::LocalTravail(CapteurDistance& capteurDistance,
                           Sonorite& sonorite,
                           Del& del,
                           Deplacements& deplacements)
    : capteurDistance_(capteurDistance),
      sonorite_(sonorite),
      del_(del),
      deplacements_(deplacements)
{
}

uint8_t LocalTravail::gerer(bool estEntreeAGauche,
                            const uint8_t notesMidi[])
{
    if (estEntreeAGauche) {
        deplacements_.tournerGauche90();
    }
    else {
        deplacements_.tournerDroite90();
    }

    uint8_t nombrePersonnesEvacuees = 0;

    for (uint8_t i = 0; i < NOMBRE_BALAYAGES_LOCAL_TRAVAIL; i++) {
        deplacements_.tournerAngleBalayage(estEntreeAGauche,
                                           ANGLE_BALAYAGE_INITIAL_DEGRES);
        if (evacuerPersonneSiPresente_(notesMidi)) {
            nombrePersonnesEvacuees++;
        }

        deplacements_.tournerAngleBalayage(estEntreeAGauche,
                                           ANGLE_BALAYAGE_CENTRAL_DEGRES);
        if (evacuerPersonneSiPresente_(notesMidi)) {
            nombrePersonnesEvacuees++;
        }

        deplacements_.tournerAngleBalayage(estEntreeAGauche,
                                           ANGLE_BALAYAGE_FINAL_DEGRES);
        if (evacuerPersonneSiPresente_(notesMidi)) {
            nombrePersonnesEvacuees++;
        }
    }

    if (estEntreeAGauche) {
        deplacements_.tournerDroite90();
    }
    else {
        deplacements_.tournerGauche90();
    }

    return nombrePersonnesEvacuees;
}

bool LocalTravail::evacuerPersonneSiPresente_(const uint8_t notesMidi[])
{
    if (!capteurDistance_.objetDetecte(DISTANCE_MAX_POTEAU_CM)) {
        return false;
    }

    const uint8_t distanceSortieCm =
        DISTANCE_MAX_POTEAU_CM + HYSTERESIS_DISTANCE_POTEAU_CM;

    while (capteurDistance_.objetDetecte(distanceSortieCm)) {
        jouerSequenceAlerte_(notesMidi);
        attendreMillisecondes(PAUSE_APRES_ALERTE_MS);
    }

    confirmerEvacuation_();

    return true;
}

void LocalTravail::jouerSequenceAlerte_(const uint8_t notesMidi[])
{
    if (notesMidi == nullptr) {
        return;
    }

    for (uint8_t i = 0; i < NOMBRE_NOTES_ALERTE; i++) {
        sonorite_.jouerNoteMidi(notesMidi[i]);
        attendreMillisecondes(DUREE_NOTE_ALERTE_MS);
        sonorite_.arreter();
        attendreMillisecondes(PAUSE_ENTRE_NOTES_MS);
    }
}

void LocalTravail::confirmerEvacuation_()
{
    clignoterCouleur(del_,
                     &Del::vert,
                     DUREE_CONFIRMATION_MS,
                     FREQUENCE_CLIGNOTEMENT_MODE_HZ);
}