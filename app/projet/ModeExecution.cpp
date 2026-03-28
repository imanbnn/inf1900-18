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

#include "ModeExecution.h"

#include "BrocheIo.h"
#include "CapteurDistance.h"
#include "ConfigurationProjet.h"
#include "ControleMoteurs.h"
#include "Debug.h"
#include "Del.h"
#include "OutilsProjet.h"
#include "Sonorite.h"
#include "StockageProjet.h"
#include "SuiveurLigne.h"

namespace
{
    enum class CoteSuivi : uint8_t
    {
        GAUCHE,
        DROITE
    };

    enum class EvenementSuivi : uint8_t
    {
        INTERSECTION,
        ENTREE_LOCAL,
        TIMEOUT
    };

    constexpr uint8_t MASQUE_CINQ_BITS = 0x1F;
    constexpr uint8_t NOMBRE_CAPTEURS = 5;

    constexpr uint8_t MASQUE_SUIVI_GAUCHE = 0b00111;
    constexpr uint8_t MASQUE_SUIVI_DROITE = 0b11100;

    constexpr uint8_t MASQUE_MUR_GAUCHE = 0b00011;
    constexpr uint8_t MASQUE_MUR_DROITE = 0b11000;

    constexpr uint8_t POSITION_VOULUE_GAUCHE = 50;
    constexpr uint16_t POSITION_VOULUE_DROITE = 350;
    constexpr uint16_t FACTEUR_POSITION_CAPTEUR = 100;
    constexpr int16_t POSITION_NON_DETECTEE = -1;

    constexpr uint8_t AJUSTEMENT_PERTE_LIGNE = 15;
    constexpr uint8_t VITESSE_MIN = 0;
    constexpr uint8_t VITESSE_MAX = 100;

    constexpr uint8_t NOMBRE_LECTURES_INTERSECTION = 3;
    constexpr uint16_t DELAI_AVANCE_APRES_ENTREE_MS = 300;
    constexpr uint8_t VITESSE_APPROCHE_COIN = 30;
    constexpr uint16_t DUREE_APPROCHE_COIN_MS = 150;
    constexpr uint16_t TIMEOUT_SEGMENT_SUD_MS = 12000;
    constexpr uint16_t TIMEOUT_SEGMENT_PRINCIPAL_MS = 20000;
    constexpr uint16_t TIMEOUT_STATIONNEMENT_MS = 4000;
    constexpr uint16_t PAS_STATIONNEMENT_MS = 10;
    constexpr uint8_t NOMBRE_NOTES_ALERTE = 3;
    constexpr uint8_t NOTE_PAR_DEFAUT_1 = 60;
    constexpr uint8_t NOTE_PAR_DEFAUT_2 = 62;
    constexpr uint8_t NOTE_PAR_DEFAUT_3 = 64;

    uint8_t obtenirMasquePertinentPourSuivi(CoteSuivi cote)
    {
        return (cote == CoteSuivi::GAUCHE) ?
            MASQUE_SUIVI_GAUCHE :
            MASQUE_SUIVI_DROITE;
    }

    uint8_t obtenirMasqueMur(CoteSuivi cote)
    {
        return (cote == CoteSuivi::GAUCHE) ?
            MASQUE_MUR_GAUCHE :
            MASQUE_MUR_DROITE;
    }

    int16_t calculerPositionCapteurs(uint8_t capteurs, uint8_t masque)
    {
        capteurs &= masque;

        uint16_t sommePositions = 0;
        uint8_t nombreCapteursActifs = 0;

        for (uint8_t i = 0; i < NOMBRE_CAPTEURS; i++) {
            if (capteurs & (1 << i)) {
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

    void appliquerSuiviLigne(ControleMoteurs& moteurs,
                             uint8_t capteurs,
                             CoteSuivi cote)
    {
        const uint8_t masque =
            obtenirMasquePertinentPourSuivi(cote);
        const int16_t position =
            calculerPositionCapteurs(capteurs, masque);

        const int16_t positionVoulue =
            (cote == CoteSuivi::GAUCHE) ?
            POSITION_VOULUE_GAUCHE :
            POSITION_VOULUE_DROITE;

        int16_t vitesseGauche = VITESSE_SUIVI_LIGNE;
        int16_t vitesseDroite = VITESSE_SUIVI_LIGNE;

        if (position < 0) {
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

        vitesseGauche =
            bornerValeur(vitesseGauche, VITESSE_MIN, VITESSE_MAX);
        vitesseDroite =
            bornerValeur(vitesseDroite, VITESSE_MIN, VITESSE_MAX);

        moteurs.ajusterVitesses((int8_t)(-vitesseGauche),
                                (int8_t)(-vitesseDroite));
    }

    bool estIntersection(uint8_t capteurs)
    {
        return compterBitsActifsSur5(capteurs) >=
               SEUIL_INTERSECTION_CAPTEURS;
    }

    void avancerPendant(ControleMoteurs& moteurs,
                        uint8_t vitesse,
                        uint16_t dureeMillisecondes)
    {
        moteurs.avancer(vitesse);
        attendreMillisecondes(dureeMillisecondes);
        moteurs.arreter();
    }

    void reculerPendant(ControleMoteurs& moteurs,
                        uint8_t vitesse,
                        uint16_t dureeMillisecondes)
    {
        moteurs.reculer(vitesse);
        attendreMillisecondes(dureeMillisecondes);
        moteurs.arreter();
    }

    void tournerGauche90(ControleMoteurs& moteurs)
    {
        moteurs.tournerGauche(VITESSE_TOURNER);
        attendreMillisecondes(DUREE_TOURNER_90_MS);
        moteurs.arreter();
    }

    void tournerDroite90(ControleMoteurs& moteurs)
    {
        moteurs.tournerDroite(VITESSE_TOURNER);
        attendreMillisecondes(DUREE_TOURNER_90_MS);
        moteurs.arreter();
    }

    void demiTour(ControleMoteurs& moteurs)
    {
        tournerGauche90(moteurs);
        tournerGauche90(moteurs);
    }

    EvenementSuivi suivreJusquaEvenement(SuiveurLigne& suiveurLigne,
                                         ControleMoteurs& moteurs,
                                         CoteSuivi cote,
                                         bool detecterEntree,
                                         bool compterZones,
                                         uint8_t& compteurZones,
                                         BrocheIo& delLibre,
                                         uint16_t timeoutMs)
    {
        const uint8_t masqueMur = obtenirMasqueMur(cote);

        bool enZone = false;
        uint16_t tempsPerteLigne = 0;
        uint8_t compteurIntersection = 0;

        for (uint16_t tempsEcoule = 0;
             tempsEcoule < timeoutMs;
             tempsEcoule += PERIODE_SUIVI_MS) {
            const uint8_t capteurs = suiveurLigne.lireCapteurs();

            appliquerSuiviLigne(moteurs, capteurs, cote);

            if (estIntersection(capteurs)) {
                compteurIntersection++;

                if (compteurIntersection >=
                    NOMBRE_LECTURES_INTERSECTION) {
                    moteurs.arreter();
                    delLibre.mettreAZero();
                    return EvenementSuivi::INTERSECTION;
                }
            }
            else {
                compteurIntersection = 0;
            }

            if (compterZones) {
                const uint8_t capteursSansMur =
                    (capteurs & MASQUE_CINQ_BITS) &
                    (uint8_t)(~masqueMur);

                const bool detectionZone =
                    compterBitsActifsSur5(capteursSansMur) >=
                    SEUIL_OBJET_CAPTEURS;

                if (detectionZone) {
                    delLibre.mettreAUn();

                    if (!enZone) {
                        compteurZones++;
                        enZone = true;
                    }
                }
                else {
                    delLibre.mettreAZero();
                    enZone = false;
                }
            }

            if (detecterEntree) {
                if ((capteurs & masqueMur) == 0) {
                    tempsPerteLigne += PERIODE_SUIVI_MS;

                    if (tempsPerteLigne >=
                        DUREE_PERTE_LIGNE_ENTREE_MS) {
                        moteurs.arreter();
                        delLibre.mettreAZero();
                        return EvenementSuivi::ENTREE_LOCAL;
                    }
                }
                else {
                    tempsPerteLigne = 0;
                }
            }

            attendreMillisecondes(PERIODE_SUIVI_MS);
        }

        moteurs.arreter();
        delLibre.mettreAZero();

        return EvenementSuivi::TIMEOUT;
    }

    void jouerSequenceAlerte(Sonorite& sonorite,
                             const uint8_t notesMidi[NOMBRE_NOTES_MIDI])
    {
        for (uint8_t i = 0; i < NOMBRE_NOTES_ALERTE; i++) {
            sonorite.jouerNoteMidi(notesMidi[i]);
            attendreMillisecondes(DUREE_NOTE_ALERTE_MS);
            sonorite.arreter();
            attendreMillisecondes(PAUSE_ENTRE_NOTES_MS);
        }
    }

    bool evacuerPersonneSiPresente(
        CapteurDistance& capteurDistance,
        Sonorite& sonorite,
        Del& del,
        const uint8_t notesMidi[NOMBRE_NOTES_MIDI])
    {
        if (!capteurDistance.objetDetecte(DISTANCE_POTEAU_CM)) {
            return false;
        }

        while (capteurDistance.objetDetecte(DISTANCE_POTEAU_CM)) {
            jouerSequenceAlerte(sonorite, notesMidi);
            attendreMillisecondes(PAUSE_APRES_ALERTE_MS);
        }

        clignoterCouleur(del,
                         &Del::vert,
                         DUREE_CONFIRMATION_MS,
                         FREQUENCE_CLIGNOTEMENT_MODE_HZ);
        del.eteindre();

        return true;
    }

    uint8_t gererLocalTravail(
        bool entreeAGauche,
        CapteurDistance& capteurDistance,
        Sonorite& sonorite,
        Del& del,
        ControleMoteurs& moteurs,
        const uint8_t notesMidi[NOMBRE_NOTES_MIDI])
    {
        if (entreeAGauche) {
            tournerGauche90(moteurs);
        }
        else {
            tournerDroite90(moteurs);
        }

        avancerPendant(moteurs,
                       VITESSE_ENTREE_LOCAL,
                       DUREE_AVANCE_LOCAL_TRAVAIL_MS);

        uint8_t nombrePersonnes = 0;

        if (evacuerPersonneSiPresente(capteurDistance,
                                      sonorite,
                                      del,
                                      notesMidi)) {
            nombrePersonnes++;
        }

        tournerGauche90(moteurs);

        if (evacuerPersonneSiPresente(capteurDistance,
                                      sonorite,
                                      del,
                                      notesMidi)) {
            nombrePersonnes++;
        }

        tournerDroite90(moteurs);
        tournerDroite90(moteurs);

        if (evacuerPersonneSiPresente(capteurDistance,
                                      sonorite,
                                      del,
                                      notesMidi)) {
            nombrePersonnes++;
        }

        tournerGauche90(moteurs);

        reculerPendant(moteurs,
                       VITESSE_SORTIE_LOCAL,
                       DUREE_RECUL_LOCAL_TRAVAIL_MS);

        if (entreeAGauche) {
            tournerDroite90(moteurs);
        }
        else {
            tournerGauche90(moteurs);
        }

        del.eteindre();

        return nombrePersonnes;
    }

    uint8_t gererLocalRangement(bool entreeAGauche,
                                SuiveurLigne& suiveurLigne,
                                ControleMoteurs& moteurs,
                                BrocheIo& delLibre)
    {
        if (entreeAGauche) {
            tournerGauche90(moteurs);
        }
        else {
            tournerDroite90(moteurs);
        }

        uint8_t nombreObjets = 0;
        bool enObjet = false;

        moteurs.avancer(VITESSE_ENTREE_LOCAL);

        for (uint16_t tempsEcoule = 0;
             tempsEcoule < DUREE_AVANCE_LOCAL_RANGEMENT_MS;
             tempsEcoule += PERIODE_SUIVI_MS) {
            const uint8_t capteurs = suiveurLigne.lireCapteurs();
            const bool objetDetecte =
                compterBitsActifsSur5(capteurs) >= 3;

            if (objetDetecte) {
                delLibre.mettreAUn();

                if (!enObjet) {
                    nombreObjets++;
                    enObjet = true;
                }
            }
            else {
                delLibre.mettreAZero();
                enObjet = false;
            }

            attendreMillisecondes(PERIODE_SUIVI_MS);
        }

        moteurs.arreter();
        delLibre.mettreAZero();

        reculerPendant(moteurs,
                       VITESSE_SORTIE_LOCAL,
                       DUREE_RECUL_LOCAL_RANGEMENT_MS);

        if (entreeAGauche) {
            tournerDroite90(moteurs);
        }
        else {
            tournerGauche90(moteurs);
        }

        return nombreObjets;
    }

    void stationnerRobot(bool arriveDuCoinEst,
                         uint8_t numeroStationnement,
                         SuiveurLigne& suiveurLigne,
                         ControleMoteurs& moteurs)
    {
        if (numeroStationnement < NUMERO_STATIONNEMENT_MIN ||
            numeroStationnement > NUMERO_STATIONNEMENT_MAX) {
            numeroStationnement = NUMERO_STATIONNEMENT_PAR_DEFAUT;
        }

        const uint16_t dureeStationnement =
            arriveDuCoinEst ?
            TEMPS_STATIONNEMENT_DEPUIS_EST_MS[
                numeroStationnement -
                NUMERO_STATIONNEMENT_MIN] :
            TEMPS_STATIONNEMENT_DEPUIS_OUEST_MS[
                numeroStationnement -
                NUMERO_STATIONNEMENT_MIN];

        const CoteSuivi coteSud =
            arriveDuCoinEst ?
            CoteSuivi::DROITE :
            CoteSuivi::GAUCHE;

        for (uint16_t tempsEcoule = 0;
             tempsEcoule < dureeStationnement;
             tempsEcoule += PERIODE_SUIVI_MS) {
            const uint8_t capteurs = suiveurLigne.lireCapteurs();
            appliquerSuiviLigne(moteurs, capteurs, coteSud);
            attendreMillisecondes(PERIODE_SUIVI_MS);
        }

        moteurs.arreter();

        if (arriveDuCoinEst) {
            tournerDroite90(moteurs);
        }
        else {
            tournerGauche90(moteurs);
        }

        moteurs.avancer(VITESSE_ENTREE_LOCAL);

        for (uint16_t tempsEcoule = 0;
             tempsEcoule < TIMEOUT_STATIONNEMENT_MS;
             tempsEcoule += PAS_STATIONNEMENT_MS) {
            if (suiveurLigne.estSurObjet()) {
                break;
            }

            attendreMillisecondes(PAS_STATIONNEMENT_MS);
        }

        moteurs.arreter();
    }
}

void executerModeExecution(Del& del,
                           ControleMoteurs& moteurs,
                           SuiveurLigne& suiveurLigne,
                           CapteurDistance& capteurDistance,
                           Sonorite& sonorite,
                           StockageProjet& stockage)
{
    ParametresProjet parametres =
    {
        SensParcours::HORAIRE,
        {
            NOTE_PAR_DEFAUT_1,
            NOTE_PAR_DEFAUT_2,
            NOTE_PAR_DEFAUT_3
        },
        NUMERO_STATIONNEMENT_PAR_DEFAUT
    };

    const bool parametresValides =
        stockage.lireParametres(parametres);

    if (!parametresValides) {
        DEBUG_PRINT("Mode execution  parametres par defaut\n");
    }

    BrocheIo delLibre(DDRA, PORTA, BROCHE_DEL_LIBRE);
    delLibre.configurerEnSortie();
    delLibre.mettreAZero();

    ResultatsProjet resultats = {0};

    stockage.effacerResultats();

    demiTour(moteurs);

    moteurs.avancer(VITESSE_ENTREE_LOCAL);

    for (uint16_t tempsEcoule = 0;
         tempsEcoule < 5000;
         tempsEcoule += 10) {
        if (suiveurLigne.estSurObjet()) {
            break;
        }

        attendreMillisecondes(10);
    }

    moteurs.arreter();

    avancerPendant(moteurs,
                   VITESSE_APPROCHE_COIN,
                   DUREE_APPROCHE_COIN_MS);

    if (parametres.sensParcours == SensParcours::HORAIRE) {
        tournerDroite90(moteurs);

        suivreJusquaEvenement(suiveurLigne,
                              moteurs,
                              CoteSuivi::DROITE,
                              false,
                              false,
                              resultats.nombreZonesCouloirOuest,
                              delLibre,
                              TIMEOUT_SEGMENT_SUD_MS);

        avancerPendant(moteurs,
                       VITESSE_APPROCHE_COIN,
                       DUREE_APPROCHE_COIN_MS);
        tournerDroite90(moteurs);

        suivreJusquaEvenement(suiveurLigne,
                              moteurs,
                              CoteSuivi::GAUCHE,
                              false,
                              true,
                              resultats.nombreZonesCouloirOuest,
                              delLibre,
                              TIMEOUT_SEGMENT_PRINCIPAL_MS);

        avancerPendant(moteurs,
                       VITESSE_APPROCHE_COIN,
                       DUREE_APPROCHE_COIN_MS);
        tournerDroite90(moteurs);

        const bool entreeAGauche = true;

        if (suivreJusquaEvenement(suiveurLigne,
                                  moteurs,
                                  CoteSuivi::GAUCHE,
                                  true,
                                  false,
                                  resultats.nombreZonesCouloirOuest,
                                  delLibre,
                                  TIMEOUT_SEGMENT_PRINCIPAL_MS) ==
            EvenementSuivi::ENTREE_LOCAL) {
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DUREE_AVANCE_CENTRE_ENTREE_MS);
            resultats.nombrePersonnesLocalA =
                gererLocalTravail(entreeAGauche,
                                  capteurDistance,
                                  sonorite,
                                  del,
                                  moteurs,
                                  parametres.notesMidi);
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DELAI_AVANCE_APRES_ENTREE_MS);
        }

        if (suivreJusquaEvenement(suiveurLigne,
                                  moteurs,
                                  CoteSuivi::GAUCHE,
                                  true,
                                  false,
                                  resultats.nombreZonesCouloirOuest,
                                  delLibre,
                                  TIMEOUT_SEGMENT_PRINCIPAL_MS) ==
            EvenementSuivi::ENTREE_LOCAL) {
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DUREE_AVANCE_CENTRE_ENTREE_MS);
            resultats.nombreObjetsLocalB =
                gererLocalRangement(entreeAGauche,
                                    suiveurLigne,
                                    moteurs,
                                    delLibre);
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DELAI_AVANCE_APRES_ENTREE_MS);
        }

        if (suivreJusquaEvenement(suiveurLigne,
                                  moteurs,
                                  CoteSuivi::GAUCHE,
                                  true,
                                  false,
                                  resultats.nombreZonesCouloirOuest,
                                  delLibre,
                                  TIMEOUT_SEGMENT_PRINCIPAL_MS) ==
            EvenementSuivi::ENTREE_LOCAL) {
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DUREE_AVANCE_CENTRE_ENTREE_MS);
            resultats.nombreObjetsLocalC =
                gererLocalRangement(entreeAGauche,
                                    suiveurLigne,
                                    moteurs,
                                    delLibre);
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DELAI_AVANCE_APRES_ENTREE_MS);
        }

        if (suivreJusquaEvenement(suiveurLigne,
                                  moteurs,
                                  CoteSuivi::GAUCHE,
                                  true,
                                  false,
                                  resultats.nombreZonesCouloirOuest,
                                  delLibre,
                                  TIMEOUT_SEGMENT_PRINCIPAL_MS) ==
            EvenementSuivi::ENTREE_LOCAL) {
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DUREE_AVANCE_CENTRE_ENTREE_MS);
            resultats.nombrePersonnesLocalD =
                gererLocalTravail(entreeAGauche,
                                  capteurDistance,
                                  sonorite,
                                  del,
                                  moteurs,
                                  parametres.notesMidi);
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DELAI_AVANCE_APRES_ENTREE_MS);
        }

        suivreJusquaEvenement(suiveurLigne,
                              moteurs,
                              CoteSuivi::GAUCHE,
                              false,
                              false,
                              resultats.nombreZonesCouloirOuest,
                              delLibre,
                              TIMEOUT_SEGMENT_PRINCIPAL_MS);

        avancerPendant(moteurs,
                       VITESSE_APPROCHE_COIN,
                       DUREE_APPROCHE_COIN_MS);
        tournerDroite90(moteurs);

        suivreJusquaEvenement(suiveurLigne,
                              moteurs,
                              CoteSuivi::GAUCHE,
                              false,
                              true,
                              resultats.nombreZonesCouloirEst,
                              delLibre,
                              TIMEOUT_SEGMENT_PRINCIPAL_MS);

        avancerPendant(moteurs,
                       VITESSE_APPROCHE_COIN,
                       DUREE_APPROCHE_COIN_MS);
        tournerDroite90(moteurs);

        stationnerRobot(true,
                        parametres.numeroStationnement,
                        suiveurLigne,
                        moteurs);
    }
    else {
        tournerGauche90(moteurs);

        suivreJusquaEvenement(suiveurLigne,
                              moteurs,
                              CoteSuivi::GAUCHE,
                              false,
                              false,
                              resultats.nombreZonesCouloirEst,
                              delLibre,
                              TIMEOUT_SEGMENT_SUD_MS);

        avancerPendant(moteurs,
                       VITESSE_APPROCHE_COIN,
                       DUREE_APPROCHE_COIN_MS);
        tournerGauche90(moteurs);

        suivreJusquaEvenement(suiveurLigne,
                              moteurs,
                              CoteSuivi::DROITE,
                              false,
                              true,
                              resultats.nombreZonesCouloirEst,
                              delLibre,
                              TIMEOUT_SEGMENT_PRINCIPAL_MS);

        avancerPendant(moteurs,
                       VITESSE_APPROCHE_COIN,
                       DUREE_APPROCHE_COIN_MS);
        tournerGauche90(moteurs);

        const bool entreeADroite = true;

        if (suivreJusquaEvenement(suiveurLigne,
                                  moteurs,
                                  CoteSuivi::DROITE,
                                  true,
                                  false,
                                  resultats.nombreZonesCouloirEst,
                                  delLibre,
                                  TIMEOUT_SEGMENT_PRINCIPAL_MS) ==
            EvenementSuivi::ENTREE_LOCAL) {
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DUREE_AVANCE_CENTRE_ENTREE_MS);
            resultats.nombrePersonnesLocalD =
                gererLocalTravail(!entreeADroite,
                                  capteurDistance,
                                  sonorite,
                                  del,
                                  moteurs,
                                  parametres.notesMidi);
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DELAI_AVANCE_APRES_ENTREE_MS);
        }

        if (suivreJusquaEvenement(suiveurLigne,
                                  moteurs,
                                  CoteSuivi::DROITE,
                                  true,
                                  false,
                                  resultats.nombreZonesCouloirEst,
                                  delLibre,
                                  TIMEOUT_SEGMENT_PRINCIPAL_MS) ==
            EvenementSuivi::ENTREE_LOCAL) {
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DUREE_AVANCE_CENTRE_ENTREE_MS);
            resultats.nombreObjetsLocalC =
                gererLocalRangement(!entreeADroite,
                                    suiveurLigne,
                                    moteurs,
                                    delLibre);
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DELAI_AVANCE_APRES_ENTREE_MS);
        }

        if (suivreJusquaEvenement(suiveurLigne,
                                  moteurs,
                                  CoteSuivi::DROITE,
                                  true,
                                  false,
                                  resultats.nombreZonesCouloirEst,
                                  delLibre,
                                  TIMEOUT_SEGMENT_PRINCIPAL_MS) ==
            EvenementSuivi::ENTREE_LOCAL) {
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DUREE_AVANCE_CENTRE_ENTREE_MS);
            resultats.nombreObjetsLocalB =
                gererLocalRangement(!entreeADroite,
                                    suiveurLigne,
                                    moteurs,
                                    delLibre);
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DELAI_AVANCE_APRES_ENTREE_MS);
        }

        if (suivreJusquaEvenement(suiveurLigne,
                                  moteurs,
                                  CoteSuivi::DROITE,
                                  true,
                                  false,
                                  resultats.nombreZonesCouloirEst,
                                  delLibre,
                                  TIMEOUT_SEGMENT_PRINCIPAL_MS) ==
            EvenementSuivi::ENTREE_LOCAL) {
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DUREE_AVANCE_CENTRE_ENTREE_MS);
            resultats.nombrePersonnesLocalA =
                gererLocalTravail(!entreeADroite,
                                  capteurDistance,
                                  sonorite,
                                  del,
                                  moteurs,
                                  parametres.notesMidi);
            avancerPendant(moteurs,
                           VITESSE_SUIVI_LIGNE,
                           DELAI_AVANCE_APRES_ENTREE_MS);
        }

        suivreJusquaEvenement(suiveurLigne,
                              moteurs,
                              CoteSuivi::DROITE,
                              false,
                              false,
                              resultats.nombreZonesCouloirEst,
                              delLibre,
                              TIMEOUT_SEGMENT_PRINCIPAL_MS);

        avancerPendant(moteurs,
                       VITESSE_APPROCHE_COIN,
                       DUREE_APPROCHE_COIN_MS);
        tournerGauche90(moteurs);

        suivreJusquaEvenement(suiveurLigne,
                              moteurs,
                              CoteSuivi::DROITE,
                              false,
                              true,
                              resultats.nombreZonesCouloirOuest,
                              delLibre,
                              TIMEOUT_SEGMENT_PRINCIPAL_MS);

        avancerPendant(moteurs,
                       VITESSE_APPROCHE_COIN,
                       DUREE_APPROCHE_COIN_MS);
        tournerGauche90(moteurs);

        stationnerRobot(false,
                        parametres.numeroStationnement,
                        suiveurLigne,
                        moteurs);
    }

    stockage.ecrireResultats(resultats);

    sonorite.jouerNoteMidi(45);
    attendreMillisecondes(1000);
    sonorite.arreter();

    del.eteindre();
    delLibre.mettreAZero();

    while (true) {
    }
}