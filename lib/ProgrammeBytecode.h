#ifndef PROGRAMME_BYTECODE_H
#define PROGRAMME_BYTECODE_H

/*
 * ProgrammeBytecode.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Accès au bytecode du TP9 stocké en mémoire I2C externe
 *   Cette classe est une couche au-dessus de Memoire24CXXX
 *   Elle connaît le format TP9
 *     2 octets de taille totale
 *     ensuite instruction puis opérande sur 2 octets
 */

#include <stdint.h>

#include "Memoire_24.h"

class ProgrammeBytecode
{
public:
    explicit ProgrammeBytecode(Memoire24CXXX& memoire);

    void initialiser();

    uint16_t obtenirLongueurTotale() const;
    uint16_t obtenirAdressePremiereInstruction() const;

    bool lireInstructionEtOperande(uint16_t adresseInstruction,
                                   uint8_t& instruction,
                                   uint8_t& operande) const;

    bool ecrireProgramme(const uint8_t* programme,
                         uint16_t longueurTotale);

    bool verifierProgramme(const uint8_t* programme,
                           uint16_t longueurTotale) const;

private:
    static constexpr uint16_t adresseDepartMemoire_ = 0x0000;
    static constexpr uint16_t tailleEntete_ = 2;

    Memoire24CXXX& memoire_;
    uint16_t longueurTotale_;
};

#endif