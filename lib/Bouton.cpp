/*
 * Bouton.cpp
 *
 *   Implémentation de la classe Bouton
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Branchement par défaut
 *   PD2 : bouton-poussoir (entrée, INT0)
 *
 * Compatible avec le bouton de la carte mère et le bouton
 * du breadboard. Les deux donnent 0 quand appuyé,
 * 1 quand relâché.
 */

#define F_CPU 8000000UL

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "Bouton.h"

constexpr uint8_t DELAI_ANTIREBOND_MS = 10;


Bouton::Bouton()
    : registreEntree_(PIND),
      registrePort_(PORTD),
      registreDirection_(DDRD),
      numeroBroche_(PD2),
      estPullupActif_(false)
{
    initialiser();
}


Bouton::Bouton(volatile uint8_t& registreEntree,
               volatile uint8_t& registrePort,
               volatile uint8_t& registreDirection,
               uint8_t numeroBroche,
               bool estPullupActif)
    : registreEntree_(registreEntree),
      registrePort_(registrePort),
      registreDirection_(registreDirection),
      numeroBroche_(numeroBroche),
      estPullupActif_(estPullupActif)
{
    initialiser();
}


void Bouton::initialiser()
{
    const uint8_t masqueBroche = (1 << numeroBroche_);

    registreDirection_ &= (uint8_t)~masqueBroche;

    if (estPullupActif_) {
        registrePort_ |= masqueBroche;
    }
}


bool Bouton::estAppuye() const
{
    const uint8_t masqueBroche = (1 << numeroBroche_);
    uint8_t premiereLecture = registreEntree_ & masqueBroche;

    _delay_ms(DELAI_ANTIREBOND_MS);

    uint8_t deuxiemeLecture = registreEntree_ & masqueBroche;

    return (premiereLecture == 0) && (deuxiemeLecture == 0);
}


void Bouton::configurerInterruption(
    NumeroInterruption numeroInterruption,
    ModeInterruption modeInterruption) const
{
    if (numeroInterruption == NumeroInterruption::EXTERNE_0) {
        const uint8_t masqueConfigurationInt0 =
            (1 << ISC01) | (1 << ISC00);

        EIMSK |= (1 << INT0);
        EICRA &= (uint8_t)~masqueConfigurationInt0;
        EICRA |= (uint8_t)modeInterruption;
    }
    else {
        const uint8_t masqueConfigurationInt1 =
            (1 << ISC11) | (1 << ISC10);
        const uint8_t bitsModeInterruption = (uint8_t)modeInterruption;

        EIMSK |= (1 << INT1);
        EICRA &= (uint8_t)~masqueConfigurationInt1;
        EICRA |= (uint8_t)(bitsModeInterruption << 2);
    }
}


void Bouton::desactiverInterruption(
    NumeroInterruption numeroInterruption) const
{
    if (numeroInterruption == NumeroInterruption::EXTERNE_0) {
        EIMSK &= (uint8_t)~(1 << INT0);
    }
    else {
        EIMSK &= (uint8_t)~(1 << INT1);
    }
}