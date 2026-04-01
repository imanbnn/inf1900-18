/*
 * ModeExecution.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Implémentation du mode exécution
 */

#include "ModeExecution.h"

#include "CapteurDistance.h"
#include "ConfigurationProjet.h"
#include "ControleMoteurs.h"
#include "Deplacements.h"
#include "Del.h"
#include "LocalRangement.h"
#include "LocalTravail.h"
#include "OutilsProjet.h"
#include "Sonorite.h"
#include "Stationnement.h"
#include "StockageProjet.h"
#include "SuiveurLigne.h"
#include "SuiviLigneParcours.h"

namespace
{
    constexpr uint8_t NOMBRE_LOCAUX = 4;
    constexpr uint8_t INDICE_LOCAL_A = 0;
    constexpr uint8_t INDICE_LOCAL_B = 1;
    constexpr uint8_t INDICE_LOCAL_C = 2;
    constexpr uint8_t INDICE_LOCAL_D = 3;

    constexpr uint16_t DUREE_AVANCE_15_CM_MS = 2000;
    constexpr uint16_t TIMEOUT_RECHERCHE_LIGNE_LATERALE_MS = 4000;
    constexpr uint8_t MASQUE_CAPTEUR_EXTERIEUR_GAUCHE = 0b00001;
    constexpr uint8_t MASQUE_CAPTEUR_EXTERIEUR_DROITE = 0b10000;


    bool ligneCompleteDetectee(SuiveurLigne& suiveurLigne)
    {
        return suiveurLigne.lireCapteurs() == MASQUE_CINQ_BITS;
    }


    uint8_t compterBitsActifs(uint8_t valeur)
    {
        uint8_t compteur = 0;

        while (valeur != 0) {
            compteur += valeur & 1;
            valeur >>= 1;
        }

        return compteur;
    }


    void reculerJusquaLigneComplete(ControleMoteurs& moteurs,
                                    SuiveurLigne& suiveurLigne)
    {
        moteurs.reculer(VITESSE_SUIVI_LIGNE);

        while (!ligneCompleteDetectee(suiveurLigne)) {
            attendreMillisecondes(PERIODE_SUIVI_MS);
        }

        moteurs.arreter();
    }


    bool ligneLateraleTrouvee(SuiveurLigne& suiveurLigne,
                              bool chercherADroite)
    {
        const uint8_t lectureCapteurs = suiveurLigne.lireCapteurs();

        if (chercherADroite) {
            const bool capteurExterieurActif =
                (lectureCapteurs & MASQUE_CAPTEUR_EXTERIEUR_DROITE) != 0;
            const uint8_t nombreCapteursDroite =
                compterBitsActifs(lectureCapteurs & MASQUE_SUIVI_DROITE);

            return capteurExterieurActif && (nombreCapteursDroite == 1);
        }

        const bool capteurExterieurActif =
            (lectureCapteurs & MASQUE_CAPTEUR_EXTERIEUR_GAUCHE) != 0;
        const uint8_t nombreCapteursGauche =
            compterBitsActifs(lectureCapteurs & MASQUE_SUIVI_GAUCHE);

        return capteurExterieurActif && (nombreCapteursGauche == 1);
    }


    bool rechercherLigneLateraleApresVirage(ControleMoteurs& moteurs,
                                            SuiveurLigne& suiveurLigne,
                                            bool chercherADroite)
    {
        if (chercherADroite) {
            moteurs.tournerDroite(VITESSE_TOURNER);
        }
        else {
            moteurs.tournerGauche(VITESSE_TOURNER);
        }

        uint16_t tempsEcouleMs = 0;

        while (tempsEcouleMs < TIMEOUT_RECHERCHE_LIGNE_LATERALE_MS) {
            if (ligneLateraleTrouvee(suiveurLigne, chercherADroite)) {
                moteurs.arreter();
                return true;
            }

            attendreMillisecondes(PERIODE_SUIVI_MS);
            tempsEcouleMs += PERIODE_SUIVI_MS;
        }

        moteurs.arreter();
        return false;
    }


    void jouerSonGraveFin(Sonorite& sonorite)
    {
        sonorite.jouerNoteMidi(NOTE_GRAVE_FIN_MIDI);
        attendreMillisecondes(DUREE_SON_GRAVE_FIN_MS);
        sonorite.arreter();
    }


    void effectuerVirageCoin(Deplacements& deplacements,
                             bool estHoraire)
    {
        if (estHoraire) {
            deplacements.tournerDroite90();
        }
        else {
            deplacements.tournerGauche90();
        }
    }


    void gererLocalTravail(Deplacements& deplacements,
                           LocalTravail& localTravail,
                           bool estEntreeAGauche,
                           const uint8_t notesMidi[],
                           uint8_t& nombrePersonnes)
    {
        deplacements.avancerPendant(VITESSE_ENTREE_LOCAL,
                                    DUREE_AVANCE_CENTRE_ENTREE_MS);

        nombrePersonnes = localTravail.gerer(estEntreeAGauche,
                                             notesMidi);

        deplacements.avancerPendant(VITESSE_ENTREE_LOCAL,
                                    DUREE_AVANCE_CENTRE_ENTREE_MS);
    }


    void gererLocalRangement(Deplacements& deplacements,
                             LocalRangement& localRangement,
                             bool estEntreeAGauche,
                             uint8_t& nombreObjets)
    {
        deplacements.avancerPendant(VITESSE_ENTREE_LOCAL,
                                    DUREE_AVANCE_CENTRE_ENTREE_MS);

        nombreObjets = localRangement.gerer(estEntreeAGauche);

        deplacements.avancerPendant(VITESSE_ENTREE_LOCAL,
                                    DUREE_AVANCE_CENTRE_ENTREE_MS);
    }


    bool suivreSegment(SuiviLigneParcours& suiviParcours,
                       CoteSuivi coteSuivi,
                       const OptionsSuivi& options,
                       EvenementSuivi evenementAttendu)
    {
        return suiviParcours.suivreJusquaEvenement(coteSuivi,
                                                   options) ==
               evenementAttendu;
    }


    uint8_t obtenirIndiceLocalResultat(uint8_t indiceParcours,
                                       bool estHoraire)
    {
        if (estHoraire) {
            return indiceParcours;
        }

        return (NOMBRE_LOCAUX - 1) - indiceParcours;
    }
}

void executerModeExecution(Del& del,
                           ControleMoteurs& moteurs,
                           SuiveurLigne& suiveurLigne,
                           CapteurDistance& capteurDistance,
                           Sonorite& sonorite,
                           StockageProjet& stockage)
{
    ParametresProjet parametres = {};

    if (!stockage.lireParametres(parametres)) {
        return;
    }

    ResultatsProjet resultats = {};

    const bool estHoraire =
        (parametres.sensParcours == SensParcours::HORAIRE);

    const CoteSuivi coteSuiviPerimetre =
        estHoraire ? CoteSuivi::GAUCHE : CoteSuivi::DROITE;
    const CoteSuivi coteSuiviLocaux =
        estHoraire ? CoteSuivi::DROITE : CoteSuivi::GAUCHE;
    const bool estEntreeLocauxAGauche = estHoraire;

    const bool chercherLigneADroiteApresPremierVirage = !estHoraire;

    Deplacements deplacements(moteurs);
    SuiviLigneParcours suiviParcours(suiveurLigne, moteurs, del);
    LocalTravail localTravail(capteurDistance, sonorite, del, deplacements);
    LocalRangement localRangement(suiveurLigne, moteurs, del, deplacements);
    Stationnement stationnement(suiveurLigne,
                                moteurs,
                                deplacements,
                                suiviParcours);

    const OptionsSuivi optionsIntersection =
    {
        false,
        TIMEOUT_SEGMENT_PRINCIPAL_MS,
        false,
        nullptr
    };

    const OptionsSuivi optionsEntreeLocal =
    {
        true,
        TIMEOUT_SEGMENT_PRINCIPAL_MS,
        false,
        nullptr
    };

    OptionsSuivi optionsCouloir =
    {
        false,
        TIMEOUT_SEGMENT_PRINCIPAL_MS,
        true,
        nullptr
    };

    reculerJusquaLigneComplete(moteurs, suiveurLigne);

    deplacements.avancerPendant(VITESSE_SUIVI_LIGNE,
                                DUREE_AVANCE_15_CM_MS);

    effectuerVirageCoin(deplacements, estHoraire);

    if (!rechercherLigneLateraleApresVirage(
            moteurs,
            suiveurLigne,
            chercherLigneADroiteApresPremierVirage)) {
        return;
    }

    if (!suivreSegment(suiviParcours,
                       coteSuiviPerimetre,
                       optionsIntersection,
                       EvenementSuivi::INTERSECTION)) {
        return;
    }

    effectuerVirageCoin(deplacements, estHoraire);

    optionsCouloir.compteurZones =
        estHoraire ? &resultats.nombreZonesCouloirOuest
                   : &resultats.nombreZonesCouloirEst;

    if (!suivreSegment(suiviParcours,
                       coteSuiviPerimetre,
                       optionsCouloir,
                       EvenementSuivi::INTERSECTION)) {
        return;
    }

    effectuerVirageCoin(deplacements, estHoraire);

    for (uint8_t i = 0; i < NOMBRE_LOCAUX; i++) {
        if (!suivreSegment(suiviParcours,
                           coteSuiviLocaux,
                           optionsEntreeLocal,
                           EvenementSuivi::ENTREE_LOCAL)) {
            return;
        }

        const uint8_t indiceLocalResultat =
            obtenirIndiceLocalResultat(i, estHoraire);

        if (indiceLocalResultat == INDICE_LOCAL_A) {
            gererLocalTravail(deplacements,
                              localTravail,
                              estEntreeLocauxAGauche,
                              parametres.notesMidi,
                              resultats.nombrePersonnesLocalA);
        }
        else if (indiceLocalResultat == INDICE_LOCAL_B) {
            gererLocalRangement(deplacements,
                                localRangement,
                                estEntreeLocauxAGauche,
                                resultats.nombreObjetsLocalB);
        }
        else if (indiceLocalResultat == INDICE_LOCAL_C) {
            gererLocalRangement(deplacements,
                                localRangement,
                                estEntreeLocauxAGauche,
                                resultats.nombreObjetsLocalC);
        }
        else {
            gererLocalTravail(deplacements,
                              localTravail,
                              estEntreeLocauxAGauche,
                              parametres.notesMidi,
                              resultats.nombrePersonnesLocalD);
        }
    }

    if (!suivreSegment(suiviParcours,
                       coteSuiviLocaux,
                       optionsIntersection,
                       EvenementSuivi::INTERSECTION)) {
        return;
    }

    effectuerVirageCoin(deplacements, estHoraire);

    optionsCouloir.compteurZones =
        estHoraire ? &resultats.nombreZonesCouloirEst
                   : &resultats.nombreZonesCouloirOuest;

    if (!suivreSegment(suiviParcours,
                       coteSuiviPerimetre,
                       optionsCouloir,
                       EvenementSuivi::INTERSECTION)) {
        return;
    }

    effectuerVirageCoin(deplacements, estHoraire);
    stationnement.executer(estHoraire, parametres.numeroStationnement);
    jouerSonGraveFin(sonorite);
    del.eteindre();

    stockage.ecrireResultats(resultats);
}