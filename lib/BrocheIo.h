#ifndef BROCHE_IO_H
#define BROCHE_IO_H

/*
 * BrocheIo.h
 *
 *   Encapsulation d'une broche de sortie numérique
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Constructeur
 *   registreDirection : registre de direction  exemple DDRB, DDRD
 *   registreSortie    : registre de sortie     exemple PORTB, PORTD
 *   numeroBit         : numéro de la broche    exemple PB2, PD5
 *
 * Opérations disponibles
 *   configurerEnSortie() : met le bit DDR à 1
 *   mettreAUn()          : met le bit PORT à 1
 *   mettreAZero()        : met le bit PORT à 0
 *   ecrire(bool)         : écrit un niveau logique
 *
 * Seul le bit ciblé est affecté via |= et &=.
 * Dépendances            : aucune
 * Interruptions requises : aucune
 */

#include <stdint.h>
#include <avr/io.h>

class BrocheIo
{
public:
    BrocheIo(volatile uint8_t& registreDirection,
             volatile uint8_t& registreSortie,
             uint8_t numeroBit);

    void configurerEnSortie() const;
    void mettreAUn() const;
    void mettreAZero() const;
    void ecrire(bool niveauHaut) const;

private:
    volatile uint8_t& registreDirection_;
    volatile uint8_t& registreSortie_;
    uint8_t masque_;
};

#endif