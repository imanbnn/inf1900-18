/*
 * SuiviLigneParcours.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "SuiviLigneParcours.h"

#include "BrocheIo.h"
#include "ConfigurationProjet.h"
#include "ControleMoteurs.h"
#include "OutilsProjet.h"
#include "SuiveurLigne.h"

namespace
{
    int16_t bornerValeur(int16_t valeur, int16_t minimum, int16_t maximum)
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
                                       BrocheIo& delLibre)
    : suiveurLigne_(suiveurLigne),
      moteurs_(moteurs),
      delLibre_(delLibre)
{
}

uint8_t SuiviLigneParcours::obtenirMasqueSuivi_(CoteSuivi cote) const
{
    return (cote == CoteSuivi::GAUCHE) ?
        MASQUE_SUIVI_GAUCHE :
        MASQUE_SUIVI_DROITE;
}

uint8_t SuiviLigneParcours::obtenirMasqueMur_(CoteSuivi cote) const
{
    return (cote == CoteSuivi::GAUCHE) ?
        MASQUE_MUR_GAUCHE :
        MASQUE_MUR_DROITE;
}

bool SuiviLigneParcours::estIntersection_(uint8_t capteurs) const
{
    return compterBitsActifsSur5(capteurs) >=
           SEUIL_INTERSECTION_CAPTEURS;
}

bool SuiviLigneParcours::estPerteLigneEntree_(uint8_t capteurs,
                                              CoteSuivi cote) const
{
    const uint8_t masqueSuivi = obtenirMasqueSuivi_(cote);
    return (capteurs & masqueSuivi) == 0;
}

int16_t SuiviLigneParcours::calculerPositionCapteurs_(uint8_t capteurs,
                                                      CoteSuivi cote) const
{
    const uint8_t masqueSuivi = obtenirMasqueSuivi_(cote);
    uint16_t sommePositions = 0;
    uint16_t nombreCapteursActifs = 0;

    for (uint8_t i = 0; i < NOMBRE_CAPTEURS_SUIVEUR_LIGNE; i++) {
        const uint8_t bitCapteur = (uint8_t)(1 << i);

        if ((capteurs & masqueSuivi & bitCapteur) != 0) {
            sommePositions +=
                (uint16_t)i * FACTEUR_POSITION_CAPTEUR;
            nombreCapteursActifs++;
        }
    }

    if (nombreCapteursActifs == 0) {
        return POSITION_NON_DETECTEE;
    }

    return (int16_t)(sommePositions / nombreCapteursActifs);
}

void SuiviLigneParcours::ajusterMoteursSelonLigne(uint8_t capteurs,
                                                  CoteSuivi cote)
{
    const int16_t position = calculerPositionCapteurs_(capteurs, cote);
    const int16_t positionVoulue =
        (cote == CoteSuivi::GAUCHE) ?
        POSITION_VOULUE_GAUCHE :
        POSITION_VOULUE_DROITE;

    int16_t vitesseGauche = VITESSE_SUIVI_LIGNE;
    int16_t vitesseDroite = VITESSE_SUIVI_LIGNE;

    if (position == POSITION_NON_DETECTEE) {
        if (cote == CoteSuivi::GAUCHE) {
            vitesseGauche = VITESSE_SUIVI_LIGNE -
                            AJUSTEMENT_PERTE_LIGNE;
            vitesseDroite = VITESSE_SUIVI_LIGNE +
                            AJUSTEMENT_PERTE_LIGNE;
        }
        else {
            vitesseGauche = VITESSE_SUIVI_LIGNE +
                            AJUSTEMENT_PERTE_LIGNE;
            vitesseDroite = VITESSE_SUIVI_LIGNE -
                            AJUSTEMENT_PERTE_LIGNE;
        }
    }
    else {
        const int16_t erreur = position - positionVoulue;
        const int16_t correction =
            (KP_SUIVI * erreur) / FACTEUR_POSITION_CAPTEUR;

        vitesseGauche = VITESSE_SUIVI_LIGNE + correction;
        vitesseDroite = VITESSE_SUIVI_LIGNE - correction;
    }

    vitesseGauche = bornerValeur(vitesseGauche,
                                 VITESSE_MIN,
                                 VITESSE_MAX);
    vitesseDroite = bornerValeur(vitesseDroite,
                                 VITESSE_MIN,
                                 VITESSE_MAX);

    moteurs_.ajusterVitesses((int8_t)(-vitesseGauche),
                             (int8_t)(-vitesseDroite));
}

void SuiviLigneParcours::ajusterPourZoneSol_(uint8_t capteurs,
                                             bool estComptageZonesActif,
                                             uint8_t* compteurZones)
{
    const bool estZoneSol =
        compterBitsActifsSur5(capteurs) >= SEUIL_OBJET_CAPTEURS;

    if (estZoneSol) {
        delLibre_.mettreAUn();
    }
    else {
        delLibre_.mettreAZero();
    }

    (void)estComptageZonesActif;
    (void)compteurZones;
}

EvenementSuivi SuiviLigneParcours::suivreJusquaEvenement(
    CoteSuivi cote,
    const OptionsSuivi& options)
{
    uint16_t dureePerteLigneMs = 0;
    uint16_t tempsEcouleMillisecondes = 0;
    bool etaitZoneSol = false;

    while (tempsEcouleMillisecondes < options.timeoutMillisecondes) {
        const uint8_t capteurs = suiveurLigne_.lireCapteurs();
        const uint8_t masqueMur = obtenirMasqueMur_(cote);

        if ((capteurs & masqueMur) == masqueMur) {
            delLibre_.mettreAZero();
            return EvenementSuivi::INTERSECTION;
        }

        ajusterPourZoneSol_(capteurs,
                            options.estComptageZonesActif,
                            options.compteurZones);

        if (options.estComptageZonesActif &&
            (options.compteurZones != nullptr)) {
            const bool estZoneSol =
                compterBitsActifsSur5(capteurs) >=
                SEUIL_OBJET_CAPTEURS;

            if (estZoneSol && !etaitZoneSol) {
                if (*(options.compteurZones) < NOMBRE_ZONES_SOL_MAX) {
                    (*(options.compteurZones))++;
                }
            }

            etaitZoneSol = estZoneSol;
        }

        if (options.estDetectionEntreeActive) {
            if (estPerteLigneEntree_(capteurs, cote)) {
                dureePerteLigneMs += PERIODE_SUIVI_MS;
            }
            else {
                dureePerteLigneMs = 0;
            }

            if (dureePerteLigneMs >= DUREE_PERTE_LIGNE_ENTREE_MS) {
                delLibre_.mettreAZero();
                return EvenementSuivi::ENTREE_LOCAL;
            }
        }

        if (estIntersection_(capteurs)) {
            delLibre_.mettreAZero();
            return EvenementSuivi::INTERSECTION;
        }

        ajusterMoteursSelonLigne(capteurs, cote);
        attendreMillisecondes(PERIODE_SUIVI_MS);

        tempsEcouleMillisecondes += PERIODE_SUIVI_MS;
    }

    delLibre_.mettreAZero();
    return EvenementSuivi::TIMEOUT;
}