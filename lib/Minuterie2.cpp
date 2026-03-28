/*
 * Minuterie2.cpp
 *
 *   Implémentation de la classe Minuterie2
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Identifications materielles
 *   OC2A : PD7 (canal A)
 *   OC2B : PD6 (canal B)
 *
 * Registres affectes
 *   TCCR2A, TCCR2B, OCR2A, OCR2B, TCNT2, TIMSK2
 *
 * Frequence PWM phase correcte
 *   F_CPU / (prescaler x 510)
 *
 * Exemples a 8 MHz
 *   DIV_8   -> ~1.96 kHz
 *   DIV_32  -> ~490 Hz
 *   DIV_64  -> ~245 Hz
 *   DIV_128 -> ~122 Hz
 */

#include "Minuterie2.h"

namespace
{
    constexpr uint8_t MASQUE_BROCHES_PWM =
        (uint8_t)((1 << PD7) | (1 << PD6));

    constexpr uint8_t MASQUE_TCCR2A_MODE =
        (uint8_t)((1 << WGM21) | (1 << WGM20) |
                  (1 << COM2A1) | (1 << COM2A0) |
                  (1 << COM2B1) | (1 << COM2B0));

    constexpr uint8_t MASQUE_TCCR2B_MODE =
        (uint8_t)((1 << CS22) | (1 << CS21) |
                  (1 << CS20) | (1 << WGM22));

    constexpr uint8_t MASQUE_PRESCALER =
        (uint8_t)((1 << CS22) | (1 << CS21) | (1 << CS20));

    constexpr uint8_t MASQUE_SORTIES_PWM =
        (uint8_t)((1 << WGM20) | (1 << COM2A1) | (1 << COM2B1));
}


void Minuterie2::initialiserPwm()
{
    DDRD |= MASQUE_BROCHES_PWM;

    OCR2A = 0;
    OCR2B = 0;

    TCCR2A &= (uint8_t)~MASQUE_TCCR2A_MODE;
    TCCR2A |= MASQUE_SORTIES_PWM;

    TCCR2B &= (uint8_t)~MASQUE_TCCR2B_MODE;

    TCNT2 = 0;
}


void Minuterie2::initialiserCtc()
{
    TCCR2A &= (uint8_t)~MASQUE_TCCR2A_MODE;
    TCCR2A |= (1 << WGM21);

    TCCR2B &= (uint8_t)~MASQUE_TCCR2B_MODE;

    TCNT2 = 0;
    OCR2A = 0;
    OCR2B = 0;
}


void Minuterie2::demarrer(PrescalerTimer2 prescaler)
{
    if (prescaler == PrescalerTimer2::ARRET) {
        arreter();
        return;
    }

    TCCR2B &= (uint8_t)~MASQUE_PRESCALER;
    TCCR2B |= (uint8_t)prescaler;
}


void Minuterie2::arreter()
{
    TCCR2B &= (uint8_t)~MASQUE_PRESCALER;
}


bool Minuterie2::estEnMarche()
{
    return (TCCR2B & MASQUE_PRESCALER) != 0;
}


void Minuterie2::ajusterRapportCycliquePourcent(CanalPwm2 canal,
                                                uint8_t pourcent)
{
    const uint8_t valeurOcr = convertirPourcentEnOcr_(pourcent);

    if (canal == CanalPwm2::CANAL_A) {
        OCR2A = valeurOcr;
    }
    else {
        OCR2B = valeurOcr;
    }
}


void Minuterie2::arreterCanal(CanalPwm2 canal)
{
    if (canal == CanalPwm2::CANAL_A) {
        OCR2A = 0;
    }
    else {
        OCR2B = 0;
    }
}


void Minuterie2::ajusterPwm(uint8_t rapportA, uint8_t rapportB)
{
    OCR2A = rapportA;
    OCR2B = rapportB;
}


void Minuterie2::ajusterPwmPourcent(uint8_t pourcentA, uint8_t pourcentB)
{
    OCR2A = convertirPourcentEnOcr_(pourcentA);
    OCR2B = convertirPourcentEnOcr_(pourcentB);
}


void Minuterie2::configurerComparaisonA(uint8_t valeur)
{
    OCR2A = valeur;
}


void Minuterie2::configurerComparaisonB(uint8_t valeur)
{
    OCR2B = valeur;
}


void Minuterie2::activerInterruptionComparaisonA()
{
    TIMSK2 |= (1 << OCIE2A);
}


void Minuterie2::desactiverInterruptionComparaisonA()
{
    TIMSK2 &= (uint8_t)~(1 << OCIE2A);
}


void Minuterie2::activerInterruptionComparaisonB()
{
    TIMSK2 |= (1 << OCIE2B);
}


void Minuterie2::desactiverInterruptionComparaisonB()
{
    TIMSK2 &= (uint8_t)~(1 << OCIE2B);
}


void Minuterie2::activerInterruptionDebordement()
{
    TIMSK2 |= (1 << TOIE2);
}


void Minuterie2::desactiverInterruptionDebordement()
{
    TIMSK2 &= (uint8_t)~(1 << TOIE2);
}


void Minuterie2::reinitialiser()
{
    TCNT2 = 0;
}


void Minuterie2::reinitialiserRegistres()
{
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2  = 0;
    OCR2A  = 0;
    OCR2B  = 0;
    TIMSK2 = 0;
}


uint8_t Minuterie2::convertirPourcentEnOcr_(uint8_t pourcent)
{
    if (pourcent > pourcentageMax) {
        pourcent = pourcentageMax;
    }

    const uint16_t produit =
        (uint16_t)pourcent * (uint16_t)pwmTop;

    return (uint8_t)(produit / pourcentageMax);
}