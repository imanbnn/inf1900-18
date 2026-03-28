/*
 * ModeRapport.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "ModeRapport.h"

#include "ConfigurationProjet.h"
#include "Del.h"
#include "OutilsProjet.h"
#include "StockageProjet.h"
#include "UART0.h"

namespace
{
    constexpr uint16_t DUREE_CLIGNOTEMENT_MODE_MS = 2000;
    constexpr uint8_t LARGEUR_COLONNE_EMPLACEMENT = 16;
    constexpr uint8_t NOMBRE_ESPACES_DETAIL = 5;
    constexpr uint8_t NOMBRE_ESPACES_SEPARATION = 9;

    uint8_t calculerLongueurTexte(const char* texte)
    {
        uint8_t longueur = 0;

        while (texte[longueur] != '\0') {
            longueur++;
        }

        return longueur;
    }

    void envoyerEspaces(uint8_t nombreEspaces)
    {
        for (uint8_t i = 0; i < nombreEspaces; i++) {
            UART0::envoyerOctet(' ');
        }
    }

    void envoyerDetail(uint8_t compte,
                       const char* singulier,
                       const char* pluriel)
    {
        if (compte == 0) {
            return;
        }

        envoyerEspaces(NOMBRE_ESPACES_DETAIL);
        UART0::envoyerUint16(compte);
        UART0::envoyerTexte(" ");
        UART0::envoyerTexte((compte == 1) ? singulier : pluriel);
    }

    void envoyerLigne(const char* emplacement,
                      uint8_t compte,
                      const char* singulier,
                      const char* pluriel)
    {
        UART0::envoyerTexte(emplacement);

        uint8_t longueurEmplacement = calculerLongueurTexte(emplacement);

        while (longueurEmplacement < LARGEUR_COLONNE_EMPLACEMENT) {
            UART0::envoyerOctet(' ');
            longueurEmplacement++;
        }

        UART0::envoyerTexte((compte == 0) ? "oui" : "non");
        envoyerEspaces(NOMBRE_ESPACES_SEPARATION);
        envoyerDetail(compte, singulier, pluriel);
        UART0::envoyerTexte("\n");
    }
}

void executerModeRapport(Del& del, const StockageProjet& stockage)
{
    clignoterCouleur(del,
                     &Del::rouge,
                     DUREE_CLIGNOTEMENT_MODE_MS,
                     FREQUENCE_CLIGNOTEMENT_MODE_HZ);
    del.eteindre();

    ResultatsProjet resultats = {0};
    const bool estLectureValide = stockage.lireResultats(resultats);

    UART0::envoyerTexte("Rapport de conformite\n\n");

    if (!estLectureValide) {
        UART0::envoyerTexte("Aucun resultat disponible.\n");

        while (true) {
        }
    }

    UART0::envoyerTexte("Emplacement       conformite   detail\n");
    UART0::envoyerTexte("-----------------------------------------------\n");

    envoyerLigne("Local A",
                 resultats.nombrePersonnesLocalA,
                 "personne",
                 "personnes");
    envoyerLigne("Local B",
                 resultats.nombreObjetsLocalB,
                 "objet",
                 "objets");
    envoyerLigne("Local C",
                 resultats.nombreObjetsLocalC,
                 "objet",
                 "objets");
    envoyerLigne("Local D",
                 resultats.nombrePersonnesLocalD,
                 "personne",
                 "personnes");
    envoyerLigne("Couloir OUEST",
                 resultats.nombreZonesCouloirOuest,
                 "zone endommagee",
                 "zones endommagees");
    envoyerLigne("Couloir EST",
                 resultats.nombreZonesCouloirEst,
                 "zone endommagee",
                 "zones endommagees");

    while (true) {
    }
}