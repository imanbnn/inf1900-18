/*
 * BrocheIo.cpp
 *
 *   Implémentation de la classe BrocheIo
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Registres affectés
 *   DDRx  : configurerEnSortie
 *   PORTx : mettreAUn, mettreAZero, ecrire
 */

#include "BrocheIo.h"

BrocheIo::BrocheIo(volatile uint8_t& registreDirection,
                   volatile uint8_t& registreSortie,
                   uint8_t numeroBit)
    : registreDirection_(registreDirection),
      registreSortie_(registreSortie),
      masque_((uint8_t)(1 << numeroBit))
{
}


void BrocheIo::configurerEnSortie() const
{
    registreDirection_ |= masque_;
}


void BrocheIo::mettreAUn() const
{
    registreSortie_ |= masque_;
}


void BrocheIo::mettreAZero() const
{
    registreSortie_ &= (uint8_t)~masque_;
}


void BrocheIo::ecrire(bool niveauHaut) const
{
    if (niveauHaut) {
        mettreAUn();
    }
    else {
        mettreAZero();
    }
}