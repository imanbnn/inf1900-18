/*
 * SuiviLigneParcours.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Implémentation de la classe responsable du suivi de ligne
 *   pendant le parcours
 */

#include "SuiviLigneParcours.h"

#include "ConfigurationProjet.h"
#include "ControleMoteurs.h"
#include "Del.h"
#include "OutilsProjet.h"
#include "SuiveurLigne.h"

namespace
{
    int16_t bornerInt16(int16_t valeur,
                        int16_t minimum,
                        int16_t maximum)
    {
        if (valeur < minimum) {
            return minimum;
        }

        if (valeur > maximum) {
            return maximum;
        }

        return valeur;
    }
}

SuiviLigneParcours::SuiviLigneParcours(SuiveurLigne& suiveurLigne,
                                       ControleMoteurs& moteurs,
                                       Del& del)
    : suiveurLigne_(suiveurLigne),
      moteurs_(moteurs),
      del_(del)
{
}


EvenementSuivi SuiviLigneParcours::suivreJusquaEvenement(
    CoteSuivi coteSuivi,
    const OptionsSuivi& options)
{
    uint16_t tempsEcouleMs = 0;
    uint16_t dureePerteLigneMs = 0;
    bool etaitZoneSol = false;

    while (true) {
        const uint8_t capteurs = suiveurLigne_.lireCapteurs();

        ajusterPourZoneSol_(capteurs,
                            options.estComptageZonesActif,
                            options.compteurZones,
                            etaitZoneSol);

        if (estIntersection_(capteurs)) {
            del_.eteindre();
            moteurs_.arreter();
            return EvenementSuivi::INTERSECTION;
        }

        if (options.estDetectionEntreeActive) {
            if (estPerteLigneEntree_(capteurs, coteSuivi)) {
                dureePerteLigneMs += PERIODE_SUIVI_MS;

                if (dureePerteLigneMs >= DUREE_PERTE_LIGNE_ENTREE_MS) {
                    del_.eteindre();
                    moteurs_.arreter();
                    return EvenementSuivi::ENTREE_LOCAL;
                }
            }
            else {
                dureePerteLigneMs = 0;
            }
        }

        ajusterSuiviSelonCapteurs(capteurs, coteSuivi);
        attendreMillisecondes(PERIODE_SUIVI_MS);

        tempsEcouleMs += PERIODE_SUIVI_MS;

        if ((options.timeoutMillisecondes > 0) &&
            (tempsEcouleMs >= options.timeoutMillisecondes)) {
            del_.eteindre();
            moteurs_.arreter();
            return EvenementSuivi::TIMEOUT;
        }
    }
}


void SuiviLigneParcours::ajusterSuiviSelonCapteurs(uint8_t capteurs,
                                                   CoteSuivi coteSuivi)
{
    const int16_t positionLigne = calculerPositionLigne_(capteurs);
    const int8_t correction = calculerCorrection_(positionLigne, coteSuivi);

    const int16_t vitesseGauche =
        bornerInt16(VITESSE_SUIVI_LIGNE + correction,
                    VITESSE_MIN,
                    VITESSE_MAX);
    const int16_t vitesseDroite =
        bornerInt16(VITESSE_SUIVI_LIGNE - correction,
                    VITESSE_MIN,
                    VITESSE_MAX);

    const int8_t vitesseGaucheSignee = (int8_t)(-vitesseGauche);
    const int8_t vitesseDroiteSignee = (int8_t)(-vitesseDroite);

    moteurs_.ajusterVitesses(vitesseGaucheSignee,
                             vitesseDroiteSignee);
}


int16_t SuiviLigneParcours::calculerPositionLigne_(uint8_t capteurs) const
{
    uint16_t sommePositions = 0;
    uint8_t nombreCapteursActifs = 0;

    for (uint8_t i = 0; i < NOMBRE_CAPTEURS; i++) {
        if (capteurs & (1 << i)) {
            sommePositions += (uint16_t)i * FACTEUR_POSITION_CAPTEUR;
            nombreCapteursActifs++;
        }
    }

    if (nombreCapteursActifs == 0) {
        return POSITION_NON_DETECTEE;
    }

    return (int16_t)(sommePositions / nombreCapteursActifs);
}


int8_t SuiviLigneParcours::calculerCorrection_(int16_t positionLigne,
                                               CoteSuivi coteSuivi) const
{
    if (positionLigne == POSITION_NON_DETECTEE) {
        if (coteSuivi == CoteSuivi::GAUCHE) {
            return -AJUSTEMENT_PERTE_LIGNE;
        }

        return AJUSTEMENT_PERTE_LIGNE;
    }

    const int16_t positionVoulue =
        (coteSuivi == CoteSuivi::GAUCHE) ? POSITION_VOULUE_GAUCHE
                                         : POSITION_VOULUE_DROITE;
    const int16_t erreur = positionVoulue - positionLigne;
    int16_t correction = erreur / KP_SUIVI;

    correction = bornerInt16(correction,
                             -AJUSTEMENT_PERTE_LIGNE,
                             AJUSTEMENT_PERTE_LIGNE);

    return (int8_t)correction;
}


bool SuiviLigneParcours::estIntersection_(uint8_t capteurs) const
{
    return compterBitsActifsSur5(capteurs) >= SEUIL_INTERSECTION_CAPTEURS;
}


bool SuiviLigneParcours::estPerteLigneEntree_(uint8_t capteurs,
                                              CoteSuivi coteSuivi) const
{
    const uint8_t nombreBitsActifs = compterBitsActifsSur5(capteurs);
    const uint8_t masqueSuivi =
        (coteSuivi == CoteSuivi::GAUCHE) ? MASQUE_SUIVI_GAUCHE
                                         : MASQUE_SUIVI_DROITE;

    if (capteurs == 0) {
        return true;
    }

    return ((capteurs & masqueSuivi) == 0) && (nombreBitsActifs <= 1);
}


void SuiviLigneParcours::ajusterPourZoneSol_(uint8_t capteurs,
                                             bool estComptageZonesActif,
                                             uint8_t* compteurZones,
                                             bool& etaitZoneSol)
{
    const uint8_t nombreCapteursActifs = compterBitsActifsSur5(capteurs);
    const bool estZoneSol =
        !estIntersection_(capteurs) &&
        (nombreCapteursActifs >= SEUIL_OBJET_CAPTEURS);

    if (estZoneSol) {
        del_.rouge();
    }
    else {
        del_.eteindre();
    }

    if (estComptageZonesActif && (compteurZones != nullptr)) {
        if (estZoneSol && !etaitZoneSol &&
            (*compteurZones < NOMBRE_ZONES_SOL_MAX)) {
            (*compteurZones)++;
        }
    }

    etaitZoneSol = estZoneSol;
}