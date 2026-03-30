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

    void avancerJusquaDetectionLigne(ControleMoteurs& moteurs,
                                     SuiveurLigne& suiveurLigne)
    {
        moteurs.avancer(VITESSE_SUIVI_LIGNE);

        while (!suiveurLigne.estSurObjet()) {
            attendreMillisecondes(PERIODE_SUIVI_MS);
        }

        moteurs.arreter();
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
        estHoraire ? CoteSuivi::DROITE : CoteSuivi::GAUCHE;
    const CoteSuivi coteSuiviLocaux =
        estHoraire ? CoteSuivi::GAUCHE : CoteSuivi::DROITE;
    const bool estEntreeLocauxAGauche = estHoraire;

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

    deplacements.demiTour();
    avancerJusquaDetectionLigne(moteurs, suiveurLigne);
    effectuerVirageCoin(deplacements, estHoraire);

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