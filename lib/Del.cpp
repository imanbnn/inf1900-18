/*
 * Del.cpp
 *
 *   Implémentation de la classe Del
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Branchement par défaut
 *   PB0 : borne négative de la DEL
 *   PB1 : borne positive de la DEL
 */

#define F_CPU 8000000UL

#include <util/delay.h>

#include "Del.h"

namespace
{
    constexpr uint8_t N_ITERATIONS_AMBRE   = 200;
    constexpr uint8_t DELAI_VERT_MS        = 9;
    constexpr uint8_t DELAI_ROUGE_MS       = 2;
}


Del::Del()
    : registrePort_(PORTB),
      registreDirection_(DDRB),
      brochePositive_(PB0),
      brocheNegative_(PB1)
{
    initialiser();
}


Del::Del(volatile uint8_t& registrePort,
         volatile uint8_t& registreDirection,
         uint8_t brochePositive,
         uint8_t brocheNegative)
    : registrePort_(registrePort),
      registreDirection_(registreDirection),
      brochePositive_(brochePositive),
      brocheNegative_(brocheNegative)
{
    initialiser();
}


void Del::initialiser()
{
    registreDirection_ |= (1 << brochePositive_) | (1 << brocheNegative_);
}


void Del::rouge()
{
    registrePort_ |= (1 << brocheNegative_);
    registrePort_ &= (uint8_t)~(1 << brochePositive_);
}


void Del::vert()
{
    registrePort_ |= (1 << brochePositive_);
    registrePort_ &= (uint8_t)~(1 << brocheNegative_);
}


void Del::ambre()
{
    for (uint8_t i = 0; i < N_ITERATIONS_AMBRE; i++) {
        registrePort_ |= (1 << brochePositive_);
        registrePort_ &= (uint8_t)~(1 << brocheNegative_);
        _delay_ms(DELAI_VERT_MS);

        registrePort_ |= (1 << brocheNegative_);
        registrePort_ &= (uint8_t)~(1 << brochePositive_);
        _delay_ms(DELAI_ROUGE_MS);
    }

    eteindre();
}


void Del::eteindre()
{
    registrePort_ &= (uint8_t)~(1 << brochePositive_);
    registrePort_ &= (uint8_t)~(1 << brocheNegative_);
}