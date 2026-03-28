/*
 * main.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Test RS232
 *   Envoie un message en boucle pour valider DbgEN et serieViaUSB
 *
 * Identification materielle
 *   RS232
 *     PD0 RXD0  cavalier DbgEN
 *     PD1 TXD0  cavalier DbgEN
 */

#define F_CPU 8000000UL

#include <util/delay.h>
#include <stdint.h>

#include "UART0.h"

namespace
{
    constexpr uint16_t DELAI_MS = 500;
}

int main()
{
    UART0::initialiser2400bauds();

    while (true) {
        UART0::envoyerTexte("RS232 OK\n");
        _delay_ms(DELAI_MS);
    }
}
