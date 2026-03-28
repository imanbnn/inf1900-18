/*
 * StockageProjet.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "StockageProjet.h"

namespace
{
    constexpr uint8_t TAILLE_BUFFER_PARAMETRES = 7;
    constexpr uint8_t TAILLE_BUFFER_RESULTATS = 8;

    constexpr uint8_t INDEX_OCTET_SIGNATURE_0 = 0;
    constexpr uint8_t INDEX_OCTET_SIGNATURE_1 = 1;

    constexpr uint8_t INDEX_SENS_PARCOURS = 2;
    constexpr uint8_t INDEX_NOTE_1 = 3;
    constexpr uint8_t INDEX_NOTE_2 = 4;
    constexpr uint8_t INDEX_NOTE_3 = 5;
    constexpr uint8_t INDEX_NUMERO_STATIONNEMENT = 6;

    constexpr uint8_t INDEX_PERSONNES_LOCAL_A = 2;
    constexpr uint8_t INDEX_PERSONNES_LOCAL_D = 3;
    constexpr uint8_t INDEX_OBJETS_LOCAL_B = 4;
    constexpr uint8_t INDEX_OBJETS_LOCAL_C = 5;
    constexpr uint8_t INDEX_ZONES_COULOIR_OUEST = 6;
    constexpr uint8_t INDEX_ZONES_COULOIR_EST = 7;

    constexpr uint8_t VALEUR_SENS_HORAIRE = 0;
    constexpr uint8_t VALEUR_SENS_ANTI_HORAIRE = 1;
}

StockageProjet::StockageProjet(Memoire24CXXX& memoire)
    : memoire_(memoire)
{
}

void StockageProjet::ecrireBytes_(uint16_t adresse,
                                  const uint8_t* donnees,
                                  uint8_t longueur)
{
    memoire_.ecriture(adresse, (uint8_t*)donnees, longueur);
}

void StockageProjet::lireBytes_(uint16_t adresse,
                                uint8_t* donnees,
                                uint8_t longueur) const
{
    memoire_.lecture(adresse, donnees, longueur);
}

void StockageProjet::ecrireParametres(const ParametresProjet& parametres)
{
    uint8_t buffer[TAILLE_BUFFER_PARAMETRES] = {0};

    buffer[INDEX_OCTET_SIGNATURE_0] = signatureParametresOctet0_;
    buffer[INDEX_OCTET_SIGNATURE_1] = signatureParametresOctet1_;

    buffer[INDEX_SENS_PARCOURS] =
        (parametres.sensParcours == SensParcours::ANTI_HORAIRE) ?
        VALEUR_SENS_ANTI_HORAIRE :
        VALEUR_SENS_HORAIRE;

    buffer[INDEX_NOTE_1] = parametres.notesMidi[0];
    buffer[INDEX_NOTE_2] = parametres.notesMidi[1];
    buffer[INDEX_NOTE_3] = parametres.notesMidi[2];
    buffer[INDEX_NUMERO_STATIONNEMENT] =
        parametres.numeroStationnement;

    ecrireBytes_(adresseParametres_, buffer, sizeof(buffer));
}

bool StockageProjet::lireParametres(ParametresProjet& parametres) const
{
    uint8_t buffer[TAILLE_BUFFER_PARAMETRES] = {0};

    lireBytes_(adresseParametres_, buffer, sizeof(buffer));

    if ((buffer[INDEX_OCTET_SIGNATURE_0] != signatureParametresOctet0_) ||
        (buffer[INDEX_OCTET_SIGNATURE_1] != signatureParametresOctet1_)) {
        return false;
    }

    parametres.sensParcours =
        (buffer[INDEX_SENS_PARCOURS] == VALEUR_SENS_ANTI_HORAIRE) ?
        SensParcours::ANTI_HORAIRE :
        SensParcours::HORAIRE;

    parametres.notesMidi[0] = buffer[INDEX_NOTE_1];
    parametres.notesMidi[1] = buffer[INDEX_NOTE_2];
    parametres.notesMidi[2] = buffer[INDEX_NOTE_3];

    parametres.numeroStationnement =
        buffer[INDEX_NUMERO_STATIONNEMENT];

    if ((parametres.numeroStationnement < NUMERO_STATIONNEMENT_MIN) ||
        (parametres.numeroStationnement > NUMERO_STATIONNEMENT_MAX)) {
        parametres.numeroStationnement =
            NUMERO_STATIONNEMENT_PAR_DEFAUT;
    }

    return true;
}

void StockageProjet::ecrireResultats(const ResultatsProjet& resultats)
{
    uint8_t buffer[TAILLE_BUFFER_RESULTATS] = {0};

    buffer[INDEX_OCTET_SIGNATURE_0] = signatureResultatsOctet0_;
    buffer[INDEX_OCTET_SIGNATURE_1] = signatureResultatsOctet1_;

    buffer[INDEX_PERSONNES_LOCAL_A] =
        resultats.nombrePersonnesLocalA;
    buffer[INDEX_PERSONNES_LOCAL_D] =
        resultats.nombrePersonnesLocalD;
    buffer[INDEX_OBJETS_LOCAL_B] =
        resultats.nombreObjetsLocalB;
    buffer[INDEX_OBJETS_LOCAL_C] =
        resultats.nombreObjetsLocalC;
    buffer[INDEX_ZONES_COULOIR_OUEST] =
        resultats.nombreZonesCouloirOuest;
    buffer[INDEX_ZONES_COULOIR_EST] =
        resultats.nombreZonesCouloirEst;

    ecrireBytes_(adresseResultats_, buffer, sizeof(buffer));
}

bool StockageProjet::lireResultats(ResultatsProjet& resultats) const
{
    uint8_t buffer[TAILLE_BUFFER_RESULTATS] = {0};

    lireBytes_(adresseResultats_, buffer, sizeof(buffer));

    if ((buffer[INDEX_OCTET_SIGNATURE_0] != signatureResultatsOctet0_) ||
        (buffer[INDEX_OCTET_SIGNATURE_1] != signatureResultatsOctet1_)) {
        return false;
    }

    resultats.nombrePersonnesLocalA =
        buffer[INDEX_PERSONNES_LOCAL_A];
    resultats.nombrePersonnesLocalD =
        buffer[INDEX_PERSONNES_LOCAL_D];
    resultats.nombreObjetsLocalB =
        buffer[INDEX_OBJETS_LOCAL_B];
    resultats.nombreObjetsLocalC =
        buffer[INDEX_OBJETS_LOCAL_C];
    resultats.nombreZonesCouloirOuest =
        buffer[INDEX_ZONES_COULOIR_OUEST];
    resultats.nombreZonesCouloirEst =
        buffer[INDEX_ZONES_COULOIR_EST];

    return true;
}

void StockageProjet::effacerResultats()
{
    uint8_t buffer[TAILLE_BUFFER_RESULTATS] = {0};

    ecrireBytes_(adresseResultats_, buffer, sizeof(buffer));
}