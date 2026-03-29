/*
 * ModeExecution.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#define F_CPU 8000000UL

#include "ModeExecution.h"

#include "ConfigurationProjet.h"
#include "Deplacements.h"
#include "LocalRangement.h"
#include "LocalTravail.h"
#include "OutilsProjet.h"
#include "Stationnement.h"
#include "SuiviLigneParcours.h"

namespace
{
    void jouerSonFinStationnement(Sonorite& sonorite)
    {
        sonorite.jouerNoteMidi(NOTE_GRAVE_STATIONNEMENT);
        attendreMillisecondes(DUREE_NOTE_STATIONNEMENT_MS);
        sonorite.arreter();
    }

    void avancerJusquaDetectionLigne(SuiveurLigne& suiveurLigne,
                                     ControleMoteurs& moteurs)
    {
        moteurs.avancer(VITESSE_ENTREE_LOCAL);

        for (uint8_t i = 0; i < NOMBRE_LECTURES_DEPART_PARKING; i++) {
            if (suiveurLigne.estSurObjet()) {
                break;
            }

            attendreMillisecondes(PAS_DEPART_PARKING_MS);
        }

        moteurs.arreter();
        attendreMillisecondes(DELAI_POST_PARKING_MS);
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
    parametres.sensParcours = SensParcours::HORAIRE;
    parametres.numeroStationnement = NUMERO_STATIONNEMENT_PAR_DEFAUT;

    for (uint8_t i = 0; i < NOMBRE_NOTES_MIDI; i++) {
        parametres.notesMidi[i] = NOTE_PAR_DEFAUT;
    }

    (void)stockage.lireParametres(parametres);

    ResultatsProjet resultats = {};

    BrocheIo delLibre(DDRA, PORTA, BROCHE_DEL_LIBRE);
    delLibre.configurerEnSortie();
    delLibre.mettreAZero();

    SuiviLigneParcours suiviLigneParcours(suiveurLigne,
                                          moteurs,
                                          delLibre);

    Deplacements deplacements(moteurs);

    LocalTravail localTravail(capteurDistance,
                              sonorite,
                              del,
                              deplacements);

    LocalRangement localRangement(suiveurLigne,
                                  moteurs,
                                  delLibre,
                                  deplacements);

    Stationnement stationnement(suiveurLigne,
                                moteurs,
                                deplacements,
                                suiviLigneParcours);

    const bool estEntreeADroite =
        (parametres.sensParcours == SensParcours::HORAIRE);

    deplacements.demiTour();
    avancerJusquaDetectionLigne(suiveurLigne, moteurs);

    if (estEntreeADroite) {
        deplacements.tournerDroite90();
    }
    else {
        deplacements.tournerGauche90();
    }

    const CoteSuivi coteSuivi =
        estEntreeADroite ? CoteSuivi::DROITE : CoteSuivi::GAUCHE;

    OptionsSuivi optionsCouloir =
    {
        false,
        true,
        TIMEOUT_SEGMENT_SUD_MS,
        &resultats.nombreZonesCouloirOuest
    };

    EvenementSuivi evenement =
        suiviLigneParcours.suivreJusquaEvenement(coteSuivi,
                                                 optionsCouloir);

    if (evenement == EvenementSuivi::INTERSECTION) {
        if (estEntreeADroite) {
            deplacements.tournerGauche90();
        }
        else {
            deplacements.tournerDroite90();
        }
    }

    OptionsSuivi optionsEntreeLocal =
    {
        true,
        false,
        TIMEOUT_SEGMENT_PRINCIPAL_MS,
        nullptr
    };

    evenement =
        suiviLigneParcours.suivreJusquaEvenement(coteSuivi,
                                                 optionsEntreeLocal);

    if (evenement == EvenementSuivi::ENTREE_LOCAL) {
        deplacements.avancerPendant(VITESSE_SUIVI_LIGNE,
                                    DUREE_AVANCE_CENTRE_ENTREE_MS);

        resultats.nombrePersonnesLocalD =
            localTravail.gerer(!estEntreeADroite,
                               parametres.notesMidi);

        moteurs.avancer(VITESSE_SORTIE_LOCAL);
        attendreMillisecondes(DELAI_AVANCE_APRES_ENTREE_MS);
        moteurs.arreter();
    }

    OptionsSuivi optionsCouloirHaut =
    {
        false,
        false,
        TIMEOUT_SEGMENT_PRINCIPAL_MS,
        nullptr
    };

    evenement =
        suiviLigneParcours.suivreJusquaEvenement(coteSuivi,
                                                 optionsCouloirHaut);

    if (evenement == EvenementSuivi::INTERSECTION) {
        if (estEntreeADroite) {
            deplacements.tournerGauche90();
        }
        else {
            deplacements.tournerDroite90();
        }
    }

    OptionsSuivi optionsLocalRangement =
    {
        true,
        false,
        TIMEOUT_SEGMENT_PRINCIPAL_MS,
        nullptr
    };

    evenement =
        suiviLigneParcours.suivreJusquaEvenement(coteSuivi,
                                                 optionsLocalRangement);

    if (evenement == EvenementSuivi::ENTREE_LOCAL) {
        deplacements.avancerPendant(VITESSE_SUIVI_LIGNE,
                                    DUREE_AVANCE_CENTRE_ENTREE_MS);

        resultats.nombreObjetsLocalC =
            localRangement.gerer(!estEntreeADroite);

        moteurs.avancer(VITESSE_SORTIE_LOCAL);
        attendreMillisecondes(DELAI_AVANCE_APRES_ENTREE_MS);
        moteurs.arreter();
    }

    OptionsSuivi optionsCouloirEst =
    {
        false,
        true,
        TIMEOUT_SEGMENT_PRINCIPAL_MS,
        &resultats.nombreZonesCouloirEst
    };

    evenement =
        suiviLigneParcours.suivreJusquaEvenement(coteSuivi,
                                                 optionsCouloirEst);

    if (evenement == EvenementSuivi::INTERSECTION) {
        if (estEntreeADroite) {
            deplacements.tournerGauche90();
        }
        else {
            deplacements.tournerDroite90();
        }
    }

    OptionsSuivi optionsLocalA =
    {
        true,
        false,
        TIMEOUT_SEGMENT_PRINCIPAL_MS,
        nullptr
    };

    evenement =
        suiviLigneParcours.suivreJusquaEvenement(coteSuivi,
                                                 optionsLocalA);

    if (evenement == EvenementSuivi::ENTREE_LOCAL) {
        deplacements.avancerPendant(VITESSE_SUIVI_LIGNE,
                                    DUREE_AVANCE_CENTRE_ENTREE_MS);

        resultats.nombrePersonnesLocalA =
            localTravail.gerer(!estEntreeADroite,
                               parametres.notesMidi);

        moteurs.avancer(VITESSE_SORTIE_LOCAL);
        attendreMillisecondes(DELAI_AVANCE_APRES_ENTREE_MS);
        moteurs.arreter();
    }

    OptionsSuivi optionsCouloirBas =
    {
        false,
        false,
        TIMEOUT_SEGMENT_PRINCIPAL_MS,
        nullptr
    };

    evenement =
        suiviLigneParcours.suivreJusquaEvenement(coteSuivi,
                                                 optionsCouloirBas);

    if (evenement == EvenementSuivi::INTERSECTION) {
        if (estEntreeADroite) {
            deplacements.tournerGauche90();
        }
        else {
            deplacements.tournerDroite90();
        }
    }

    OptionsSuivi optionsLocalB =
    {
        true,
        false,
        TIMEOUT_SEGMENT_PRINCIPAL_MS,
        nullptr
    };

    evenement =
        suiviLigneParcours.suivreJusquaEvenement(coteSuivi,
                                                 optionsLocalB);

    if (evenement == EvenementSuivi::ENTREE_LOCAL) {
        deplacements.avancerPendant(VITESSE_SUIVI_LIGNE,
                                    DUREE_AVANCE_CENTRE_ENTREE_MS);

        resultats.nombreObjetsLocalB =
            localRangement.gerer(!estEntreeADroite);

        moteurs.avancer(VITESSE_SORTIE_LOCAL);
        attendreMillisecondes(DELAI_AVANCE_APRES_ENTREE_MS);
        moteurs.arreter();
    }

    if (evenement == EvenementSuivi::INTERSECTION) {
        if (estEntreeADroite) {
            deplacements.tournerGauche90();
        }
        else {
            deplacements.tournerDroite90();
        }
    }

    stationnement.executer(estEntreeADroite,
                           parametres.numeroStationnement);

    jouerSonFinStationnement(sonorite);

    delLibre.mettreAZero();

    (void)stockage.ecrireResultats(resultats);
}