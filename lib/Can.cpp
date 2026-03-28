/*
 * Can.cpp - Implementation de la classe Can.

 * Ecole Polytechnique de Montreal
 * Departement de genie informatique
 * Cours inf1995
 *
 * Matthew Khouzam et Jerome Collin
 * 2005-2006
 *
 * Code qui n'est sous aucune license.
 *
 */

#include "Can.h"

namespace
{
    constexpr uint8_t MASQUE_SELECTION_ENTREE =
        (uint8_t)((1 << MUX4) | (1 << MUX3) | (1 << MUX2) |
                  (1 << MUX1) | (1 << MUX0));

    constexpr uint8_t MASQUE_CANAL = 0x07;
}


Can::Can()
{
    ADMUX &= (uint8_t)~((1 << REFS1) | (1 << REFS0) | (1 << ADLAR));
    ADMUX &= (uint8_t)~MASQUE_SELECTION_ENTREE;

    ADCSRA &= (uint8_t)~((1 << ADSC) | (1 << ADATE) | (1 << ADIF) |
                         (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) |
                         (1 << ADPS0));
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
}


Can::~Can()
{
    ADCSRA &= (uint8_t)~(1 << ADEN);
}


uint16_t Can::lecture(uint8_t position)
{
    uint16_t resultatConversion = 0;

    ADMUX &= (uint8_t)~MASQUE_SELECTION_ENTREE;

    ADMUX |= (uint8_t)((position & MASQUE_CANAL) << MUX0);

    ADCSRA |= (1 << ADSC);

    while (!(ADCSRA & (1 << ADIF))) {
    }

    ADCSRA |= (1 << ADIF);

    resultatConversion = ADCL;
    resultatConversion |= (uint16_t)ADCH << 8;

    return resultatConversion;
}