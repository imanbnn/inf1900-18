/*
 * OutilsProjet.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Implémentation des fonctions utilitaires du projet final
 */

#include "OutilsProjet.h"

#include <avr/interrupt.h>

#include "Del.h"
#include "Minuterie1.h"

namespace
{
    constexpr uint8_t MASQUE_CINQ_BITS = 0x1F;
    constexpr uint8_t NOMBRE_BITS_SUIVEUR = 5;

    constexpr uint16_t DUREE_CYCLE_AMBRE_MS = 10;
    constexpr uint16_t DUREE_ROUGE_AMBRE_MS = 2;
    constexpr uint16_t DUREE_VERT_AMBRE_MS = 8;

    constexpr uint16_t MILLISECONDES_PAR_SECONDE = 1000;
    constexpr uint16_t VALEUR_COMPARAISON_BASE_TEMPS_MS = 124;

    volatile uint32_t tempsProjetMs = 0;
    bool estBaseTempsInitialisee = false;
}

ISR(TIMER1_COMPA_vect)
{
    tempsProjetMs++;
}

void initialiserBaseTempsProjet()
{
    if (estBaseTempsInitialisee) {
        return;
    }

    Minuterie1::reinitialiserRegistres();
    Minuterie1::initialiserCtc();
    Minuterie1::configurerComparaisonA(VALEUR_COMPARAISON_BASE_TEMPS_MS);
    Minuterie1::activerInterruptionComparaisonA();
    Minuterie1::demarrer(PrescalerTimer1::DIV_64);

    sei();

    estBaseTempsInitialisee = true;
}


uint32_t obtenirTempsProjetMs()
{
    const uint8_t etatInterruptions = SREG;
    cli();

    const uint32_t tempsCourant = tempsProjetMs;

    SREG = etatInterruptions;

    return tempsCourant;
}


bool tempsEcouleDepuis(uint32_t departMillisecondes,
                       uint16_t dureeMillisecondes)
{
    const uint32_t dureeEcouleeMs =
        obtenirTempsProjetMs() - departMillisecondes;

    return dureeEcouleeMs >= dureeMillisecondes;
}


void attendreMillisecondes(uint16_t dureeMillisecondes)
{
    const uint32_t departMillisecondes = obtenirTempsProjetMs();

    while (!tempsEcouleDepuis(departMillisecondes, dureeMillisecondes)) {
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
    const uint32_t debutAffichageMs = obtenirTempsProjetMs();

    while (!tempsEcouleDepuis(debutAffichageMs, dureeMillisecondes)) {
        del.rouge();
        attendreMillisecondes(DUREE_ROUGE_AMBRE_MS);

        del.vert();
        attendreMillisecondes(DUREE_VERT_AMBRE_MS);
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

    const uint32_t debutClignotementMs = obtenirTempsProjetMs();

    while (!tempsEcouleDepuis(debutClignotementMs, dureeMillisecondes)) {
        (del.*allumerCouleur)();
        attendreMillisecondes(demiPeriodeMillisecondes);

        del.eteindre();
        attendreMillisecondes(demiPeriodeMillisecondes);
    }
}