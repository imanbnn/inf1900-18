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

#include "ProgrammeBytecode.h"

namespace
{
    constexpr uint16_t DECALAGE_OCTET_FAIBLE = 1;
    constexpr uint8_t DECALAGE_OCTET_FORT = 8;
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
        (uint16_t)((octetFort << DECALAGE_OCTET_FORT) | octetFaible);
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