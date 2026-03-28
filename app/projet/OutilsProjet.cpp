/*
 * OutilsProjet.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#define F_CPU 8000000UL

#include "OutilsProjet.h"

#include <util/delay.h>

#include "Del.h"

namespace
{
    constexpr uint8_t MASQUE_CINQ_BITS = 0x1F;
    constexpr uint8_t NOMBRE_BITS_SUIVEUR = 5;

    constexpr uint16_t DUREE_PAS_ATTENTE_MS = 1;

    constexpr uint16_t DUREE_CYCLE_AMBRE_MS = 10;
    constexpr uint16_t DUREE_ROUGE_AMBRE_MS = 2;
    constexpr uint16_t DUREE_VERT_AMBRE_MS = 8;

    constexpr uint16_t MILLISECONDES_PAR_SECONDE = 1000;
}


void attendreMillisecondes(uint16_t dureeMillisecondes)
{
    while (dureeMillisecondes > 0) {
        _delay_ms(DUREE_PAS_ATTENTE_MS);
        dureeMillisecondes--;
    }
}


uint8_t compterBitsActifsSur5(uint8_t valeur)
{
    valeur &= MASQUE_CINQ_BITS;

    uint8_t nombreBitsActifs = 0;

    for (uint8_t i = 0; i < NOMBRE_BITS_SUIVEUR; i++) {
        if (valeur & (1 << i)) {
            nombreBitsActifs++;
        }
    }

    return nombreBitsActifs;
}


void afficherAmbre(Del& del, uint16_t dureeMillisecondes)
{
    for (uint16_t tempsEcoule = 0;
         tempsEcoule < dureeMillisecondes;
         tempsEcoule += DUREE_CYCLE_AMBRE_MS) {
        del.rouge();
        _delay_ms(DUREE_ROUGE_AMBRE_MS);

        del.vert();
        _delay_ms(DUREE_VERT_AMBRE_MS);
    }

    del.eteindre();
}


void clignoterCouleur(Del& del,
                      void (Del::*allumerCouleur)(),
                      uint16_t dureeMillisecondes,
                      uint8_t frequenceHertz)
{
    if (frequenceHertz == 0) {
        return;
    }

    const uint16_t periodeMillisecondes =
        MILLISECONDES_PAR_SECONDE / frequenceHertz;
    const uint16_t demiPeriodeMillisecondes =
        periodeMillisecondes / 2;

    uint16_t dureeRestanteMillisecondes = dureeMillisecondes;

    while (dureeRestanteMillisecondes >= periodeMillisecondes) {
        (del.*allumerCouleur)();
        attendreMillisecondes(demiPeriodeMillisecondes);

        del.eteindre();
        attendreMillisecondes(demiPeriodeMillisecondes);

        dureeRestanteMillisecondes -= periodeMillisecondes;
    }
}