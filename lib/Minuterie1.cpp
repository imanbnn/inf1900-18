/*
 * Minuterie1.cpp
 *
 *   Implémentation de la classe Minuterie1
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Branchement PWM
 *   PD5 : OC1A sortie PWM canal A
 *   PD4 : OC1B sortie PWM canal B
 *
 * Formule pour CTC
 *   OCR1A = F_CPU / (prescaler x frequenceVoulue) - 1
 */

#include "Minuterie1.h"

namespace
{
    constexpr uint8_t MASQUE_BITS_MODE_TCCR1A =
        (uint8_t)((1 << COM1A0) | (1 << COM1B0) | (1 << WGM11) | (1 << WGM10));

    constexpr uint8_t MASQUE_BITS_MODE_TCCR1B =
        (uint8_t)((1 << WGM13) | (1 << WGM12));

    constexpr uint8_t MASQUE_BITS_HORLOGE =
        (uint8_t)((1 << CS12) | (1 << CS11) | (1 << CS10));

    constexpr uint8_t MASQUE_BITS_PWM_SORTIES =
        (uint8_t)((1 << COM1A1) | (1 << COM1B1));

    constexpr uint8_t MASQUE_BROCHES_PWM =
        (uint8_t)((1 << PD5) | (1 << PD4));
}

bool Minuterie1::estInitialisee_ = false;
uint16_t Minuterie1::valeurTop_ = topPwm1::BITS_10;

void Minuterie1::initialiserPwm(ResolutionPwm1 resolution)
{
    if (estInitialisee_) {
        return;
    }

    DDRD |= MASQUE_BROCHES_PWM;

    TCCR1A &= (uint8_t)~MASQUE_BITS_MODE_TCCR1A;
    TCCR1A |= (uint8_t)(MASQUE_BITS_PWM_SORTIES | (uint8_t)resolution);

    TCCR1B &= (uint8_t)~MASQUE_BITS_MODE_TCCR1B;

    OCR1A = 0;
    OCR1B = 0;

    valeurTop_ = obtenirTopDepuisResolution_(resolution);

    estInitialisee_ = true;
}


void Minuterie1::demarrer(PrescalerTimer1 prescaler)
{
    TCCR1B &= (uint8_t)~MASQUE_BITS_HORLOGE;
    TCCR1B |= (uint8_t)prescaler;
}


void Minuterie1::arreter()
{
    TCCR1B &= (uint8_t)~MASQUE_BITS_HORLOGE;
}


bool Minuterie1::estEnMarche()
{
    return (TCCR1B & MASQUE_BITS_HORLOGE) != 0;
}


void Minuterie1::ajusterRapportCycliquePourcent(CanalPwm1 canal,
                                                uint8_t pourcent)
{
    const uint16_t valeurOcr = convertirPourcentEnOcr_(pourcent);

    if (canal == CanalPwm1::CANAL_A) {
        OCR1A = valeurOcr;
    }
    else {
        OCR1B = valeurOcr;
    }
}


void Minuterie1::arreterCanal(CanalPwm1 canal)
{
    if (canal == CanalPwm1::CANAL_A) {
        OCR1A = 0;
    }
    else {
        OCR1B = 0;
    }
}


void Minuterie1::ajusterPwm(uint16_t rapportA, uint16_t rapportB)
{
    OCR1A = rapportA;
    OCR1B = rapportB;
}


void Minuterie1::ajusterPwmPourcent(uint8_t pourcentA, uint8_t pourcentB)
{
    OCR1A = convertirPourcentEnOcr_(pourcentA);
    OCR1B = convertirPourcentEnOcr_(pourcentB);
}


void Minuterie1::initialiserCtc()
{
    reinitialiser();

    TCCR1A &= (uint8_t)~((1 << WGM11) | (1 << WGM10));
    TCCR1B &= (uint8_t)~(1 << WGM13);
    TCCR1B |= (1 << WGM12);
}


void Minuterie1::configurerComparaisonA(uint16_t valeur)
{
    OCR1A = valeur;
}


void Minuterie1::configurerComparaisonB(uint16_t valeur)
{
    OCR1B = valeur;
}


void Minuterie1::reinitialiser()
{
    TCNT1 = 0;
}


void Minuterie1::reinitialiserRegistres()
{
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1C = 0;
    TCNT1  = 0;
    OCR1A  = 0;
    OCR1B  = 0;
    TIMSK1 = 0;

    estInitialisee_ = false;
    valeurTop_ = topPwm1::BITS_10;
}


uint16_t Minuterie1::obtenirCompteur()
{
    return TCNT1;
}


void Minuterie1::activerInterruptionDebordement()
{
    TIMSK1 |= (1 << TOIE1);
}


void Minuterie1::desactiverInterruptionDebordement()
{
    TIMSK1 &= (uint8_t)~(1 << TOIE1);
}


void Minuterie1::activerInterruptionComparaisonA()
{
    TIMSK1 |= (1 << OCIE1A);
}


void Minuterie1::desactiverInterruptionComparaisonA()
{
    TIMSK1 &= (uint8_t)~(1 << OCIE1A);
}


void Minuterie1::activerInterruptionComparaisonB()
{
    TIMSK1 |= (1 << OCIE1B);
}


void Minuterie1::desactiverInterruptionComparaisonB()
{
    TIMSK1 &= (uint8_t)~(1 << OCIE1B);
}


uint16_t Minuterie1::obtenirTopDepuisResolution_(ResolutionPwm1 resolution)
{
    if (resolution == ResolutionPwm1::BITS_8) {
        return topPwm1::BITS_8;
    }

    if (resolution == ResolutionPwm1::BITS_9) {
        return topPwm1::BITS_9;
    }

    return topPwm1::BITS_10;
}


uint16_t Minuterie1::convertirPourcentEnOcr_(uint8_t pourcent)
{
    if (pourcent > pourcentageMax) {
        pourcent = pourcentageMax;
    }

    const uint32_t produit =
        (uint32_t)pourcent * (uint32_t)valeurTop_;

    return (uint16_t)(produit / pourcentageMax);
}