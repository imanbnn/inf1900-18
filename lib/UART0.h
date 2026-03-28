#ifndef UART0_H
#define UART0_H

/*
 * UART0.h
 *
 *   Module UART0
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Encapsulation du USART0 de l'ATmega324PA
 *   Support de l'envoi binaire et de l'envoi texte ASCII
 *
 * Identifications matérielles
 *   Cavalier DbgEN activé
 *   PD0  RXD0  relié au pont série de la carte
 *   PD1  TXD0  relié au pont série de la carte
 */

#include <stdint.h>
#include <avr/io.h>

class UART0
{
public:
    static void initialiser2400bauds();

    static void envoyerOctet(uint8_t octet);
    static uint8_t recevoirOctet();

    static void envoyerTexte(const char* texte);
    static void envoyerLigne(const char* texte);

    static void envoyerUint16(uint16_t valeur);

    static void envoyerHex8(uint8_t valeur);
    static void envoyerHex16(uint16_t valeur);

private:
    static void envoyerNibbleHex(uint8_t nibble);
};

#endif