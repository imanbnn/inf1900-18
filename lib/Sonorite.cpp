/*
 * Sonorite.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#define F_CPU 8000000UL

#include "Sonorite.h"

#include <avr/io.h>

#include "Minuterie2.h"

namespace
{
    constexpr uint8_t MASQUE_OC2A = (1 << PD7);
    constexpr uint8_t MASQUE_MASSE = (1 << PD6);

    constexpr uint8_t MASQUE_COM2A =
        (1 << COM2A1) | (1 << COM2A0);
    constexpr uint8_t MODE_BASCULE_OC2A = (1 << COM2A0);

    constexpr uint8_t NOTE_MIDI_FIN_GRAVES = 59;
    constexpr uint8_t NOTE_MIDI_FIN_MEDIUMS = 71;

    /*
     * Valeurs OCR2A précalculées pour Timer2 en mode CTC.
     *
     * Formule MIDI
     *   f = 440 * 2 ^ ((n - 69) / 12)
     *
     * Datasheet CTC toggle
     *   f_OC2A = F_CPU / (2 * N * (1 + OCR2A))
     *   OCR2A  = F_CPU / (2 * N * f) - 1
     *
     * Paramètres
     *   F_CPU = 8000000
     *
     * Prescaler choisi par plage pour que OCR2A reste dans 0 à 255
     *   Notes 45 à 59  DIV_256  graves
     *   Notes 60 à 71  DIV_128  médiums
     *   Notes 72 à 81  DIV_64   aiguës
     *
     * Plage
     *   Notes MIDI 45 à 81 inclusivement
     */
    constexpr uint8_t TABLE_OCR2A[] =
    {
        141, 133, 126, 118, 112, 105,  99,  94,  88,  83,
         79,  74,  70,  66,  62, 119, 112, 106, 100,  94,
         89,  84,  79,  75,  70,  66,  63, 119, 112, 106,
        100,  94,  89,  84,  79,  75,  70
    };

    PrescalerTimer2 obtenirPrescalerDepuisNote(uint8_t noteMidi)
    {
        if (noteMidi <= NOTE_MIDI_FIN_GRAVES) {
            return PrescalerTimer2::DIV_256;
        }

        if (noteMidi <= NOTE_MIDI_FIN_MEDIUMS) {
            return PrescalerTimer2::DIV_128;
        }

        return PrescalerTimer2::DIV_64;
    }
}


Sonorite::Sonorite()
    : estInitialisee_(false)
{
}


void Sonorite::initialiser()
{
    if (estInitialisee_) {
        return;
    }

    DDRD |= MASQUE_OC2A | MASQUE_MASSE;
    PORTD &= (uint8_t)~MASQUE_MASSE;

    Minuterie2::initialiserCtc();

    arreter();

    estInitialisee_ = true;
}


bool Sonorite::estNoteSupportee_(uint8_t noteMidi)
{
    return (noteMidi >= noteMidiMin) && (noteMidi <= noteMidiMax);
}


uint8_t Sonorite::calculerOcrDepuisNote_(uint8_t noteMidi)
{
    return TABLE_OCR2A[noteMidi - noteMidiMin];
}


void Sonorite::jouerNoteMidi(uint8_t noteMidi)
{
    if (!estInitialisee_) {
        initialiser();
    }

    if (!estNoteSupportee_(noteMidi)) {
        return;
    }

    const uint8_t valeurOcr = calculerOcrDepuisNote_(noteMidi);
    const PrescalerTimer2 prescaler = obtenirPrescalerDepuisNote(noteMidi);

    Minuterie2::arreter();
    TCNT2 = 0;
    OCR2A = valeurOcr;

    TCCR2A =
        (uint8_t)((TCCR2A & (uint8_t)~MASQUE_COM2A) |
                  MODE_BASCULE_OC2A);

    Minuterie2::demarrer(prescaler);
}


void Sonorite::arreter()
{
    Minuterie2::arreter();

    TCCR2A &= (uint8_t)~MASQUE_COM2A;
    PORTD &= (uint8_t)~MASQUE_OC2A;
}