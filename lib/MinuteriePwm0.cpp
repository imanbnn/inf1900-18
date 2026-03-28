/*
 * MinuteriePwm0.cpp
 *
 *   Implémentation de la classe MinuteriePwm0
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Registres affectés
 *   DDRB   PB3 et PB4 en sortie
 *   TCCR0A mode PWM et bits COM
 *   TCCR0B prescaler et WGM02
 *   OCR0A, OCR0B rapport cyclique
 *   TCNT0  compteur
 */

#include "MinuteriePwm0.h"

namespace
{
    constexpr uint8_t MASQUE_BROCHES_PWM =
        (uint8_t)((1 << PB3) | (1 << PB4));

    constexpr uint8_t MASQUE_TCCR0A_MODE =
        (uint8_t)((1 << WGM01) | (1 << WGM00) |
                  (1 << COM0A1) | (1 << COM0A0) |
                  (1 << COM0B1) | (1 << COM0B0));

    constexpr uint8_t MASQUE_TCCR0B_MODE =
        (uint8_t)((1 << CS02) | (1 << CS01) |
                  (1 << CS00) | (1 << WGM02));

    constexpr uint8_t MASQUE_PRESCALER =
        (uint8_t)((1 << CS02) | (1 << CS01) | (1 << CS00));

    constexpr uint8_t MASQUE_SORTIES_PWM =
        (uint8_t)((1 << WGM00) | (1 << COM0A1) | (1 << COM0B1));
}

bool MinuteriePwm0::estInitialisee_ = false;

void MinuteriePwm0::initialiser()
{
    if (estInitialisee_) {
        return;
    }

    DDRB |= MASQUE_BROCHES_PWM;

    OCR0A = 0;
    OCR0B = 0;

    TCCR0A &= (uint8_t)~MASQUE_TCCR0A_MODE;
    TCCR0A |= MASQUE_SORTIES_PWM;

    TCCR0B &= (uint8_t)~MASQUE_TCCR0B_MODE;

    TCNT0 = 0;

    estInitialisee_ = true;
}


void MinuteriePwm0::demarrer(PrescalerTimer0 prescaler)
{
    if (!estInitialisee_) {
        initialiser();
    }

    if (prescaler == PrescalerTimer0::ARRET) {
        arreter();
        return;
    }

    TCCR0B &= (uint8_t)~MASQUE_PRESCALER;
    TCCR0B |= (uint8_t)prescaler;
}


void MinuteriePwm0::arreter()
{
    if (!estInitialisee_) {
        initialiser();
    }

    TCCR0B &= (uint8_t)~MASQUE_PRESCALER;
}


bool MinuteriePwm0::estEnMarche()
{
    if (!estInitialisee_) {
        initialiser();
    }

    return (TCCR0B & MASQUE_PRESCALER) != 0;
}


void MinuteriePwm0::ajusterRapportCycliquePourcent(CanalPwm0 canal,
                                                   uint8_t pourcent)
{
    if (!estInitialisee_) {
        initialiser();
    }

    const uint8_t valeurOcr = convertirPourcentEnOcr_(pourcent);

    if (canal == CanalPwm0::CANAL_A) {
        OCR0A = valeurOcr;
    }
    else {
        OCR0B = valeurOcr;
    }
}


uint8_t MinuteriePwm0::convertirPourcentEnOcr_(uint8_t pourcent)
{
    if (pourcent > pourcentageMax) {
        pourcent = pourcentageMax;
    }

    const uint16_t produit =
        (uint16_t)pourcent * (uint16_t)pwmTop;

    return (uint8_t)(produit / pourcentageMax);
}


void MinuteriePwm0::arreterCanal(CanalPwm0 canal)
{
    if (canal == CanalPwm0::CANAL_A) {
        OCR0A = 0;
    }
    else {
        OCR0B = 0;
    }
}


void MinuteriePwm0::reinitialiserRegistres()
{
    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0  = 0;
    OCR0A  = 0;
    OCR0B  = 0;
    TIMSK0 = 0;

    estInitialisee_ = false;
}