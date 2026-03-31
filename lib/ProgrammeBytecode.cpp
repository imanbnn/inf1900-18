/*
 * ProgrammeBytecode.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#define F_CPU 8000000UL

#include "ProgrammeBytecode.h"

#include <util/delay.h>

namespace
{
    constexpr uint16_t DECALAGE_OCTET_FAIBLE = 1;
    constexpr uint8_t DECALAGE_OCTET_FORT = 8;
    constexpr uint8_t DELAI_ECRITURE_MEMOIRE_MS = 5;

    void ecrireOctetAvecDelai(Memoire24CXXX& memoire,
                              uint16_t adresse,
                              uint8_t octet)
    {
        memoire.ecriture(adresse, octet);
        _delay_ms(DELAI_ECRITURE_MEMOIRE_MS);
    }
}

ProgrammeBytecode::ProgrammeBytecode(Memoire24CXXX& memoire)
    : memoire_(memoire),
      longueurTotale_(0)
{
}


void ProgrammeBytecode::initialiser()
{
    uint8_t octetFort = 0;
    uint8_t octetFaible = 0;

    memoire_.lecture(adresseDepartMemoire_, &octetFort);
    memoire_.lecture(adresseDepartMemoire_ + DECALAGE_OCTET_FAIBLE,
                     &octetFaible);

    longueurTotale_ =
        (uint16_t)(((uint16_t)octetFort << DECALAGE_OCTET_FORT) |
                   octetFaible);
}


uint16_t ProgrammeBytecode::obtenirLongueurTotale() const
{
    return longueurTotale_;
}


uint16_t ProgrammeBytecode::obtenirAdressePremiereInstruction() const
{
    return adresseDepartMemoire_ + tailleEntete_;
}


bool ProgrammeBytecode::lireInstructionEtOperande(uint16_t adresseInstruction,
                                                  uint8_t& instruction,
                                                  uint8_t& operande) const
{
    if (longueurTotale_ < tailleEntete_) {
        return false;
    }

    const uint16_t adresseFin = adresseDepartMemoire_ + longueurTotale_;

    if ((adresseInstruction + DECALAGE_OCTET_FAIBLE) >= adresseFin) {
        return false;
    }

    memoire_.lecture(adresseInstruction, &instruction);
    memoire_.lecture(adresseInstruction + DECALAGE_OCTET_FAIBLE,
                     &operande);

    return true;
}


bool ProgrammeBytecode::ecrireProgramme(const uint8_t* programme,
                                        uint16_t longueurTotale)
{
    if ((programme == nullptr) || (longueurTotale < tailleEntete_)) {
        return false;
    }

    const uint8_t octetFort =
        (uint8_t)(longueurTotale >> DECALAGE_OCTET_FORT);
    const uint8_t octetFaible = (uint8_t)longueurTotale;

    ecrireOctetAvecDelai(memoire_, adresseDepartMemoire_, octetFort);
    ecrireOctetAvecDelai(memoire_,
                         adresseDepartMemoire_ + DECALAGE_OCTET_FAIBLE,
                         octetFaible);

    for (uint16_t adresse = tailleEntete_;
         adresse < longueurTotale;
         adresse++) {
        ecrireOctetAvecDelai(memoire_,
                             adresseDepartMemoire_ + adresse,
                             programme[adresse]);
    }

    longueurTotale_ = longueurTotale;

    return verifierProgramme(programme, longueurTotale);
}


bool ProgrammeBytecode::verifierProgramme(const uint8_t* programme,
                                          uint16_t longueurTotale) const
{
    if ((programme == nullptr) || (longueurTotale < tailleEntete_)) {
        return false;
    }

    uint8_t octetLu = 0;

    memoire_.lecture(adresseDepartMemoire_, &octetLu);
    if (octetLu != (uint8_t)(longueurTotale >> DECALAGE_OCTET_FORT)) {
        return false;
    }

    memoire_.lecture(adresseDepartMemoire_ + DECALAGE_OCTET_FAIBLE,
                     &octetLu);
    if (octetLu != (uint8_t)longueurTotale) {
        return false;
    }

    for (uint16_t adresse = tailleEntete_;
         adresse < longueurTotale;
         adresse++) {
        memoire_.lecture(adresseDepartMemoire_ + adresse, &octetLu);

        if (octetLu != programme[adresse]) {
            return false;
        }
    }

    return true;
}