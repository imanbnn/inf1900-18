/*
 * UART0.cpp
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
 *   Implémentation de l'encapsulation du USART0
 */

#include "UART0.h"

namespace
{
    constexpr uint8_t UBRR0H_2400_BAUDS_8_MHZ = 0x00;
    constexpr uint8_t UBRR0L_2400_BAUDS_8_MHZ = 0xCF;

    constexpr uint8_t MASQUE_RECEPTION = (1 << RXEN0);
    constexpr uint8_t MASQUE_TRANSMISSION = (1 << TXEN0);

    constexpr uint8_t MASQUE_RECEPTION_ET_TRANSMISSION =
        MASQUE_RECEPTION | MASQUE_TRANSMISSION;

    constexpr uint8_t MASQUE_DONNEES_PRETES = (1 << UDRE0);
    constexpr uint8_t MASQUE_RECEPTION_COMPLETEE = (1 << RXC0);

    constexpr uint8_t MASQUE_MODE_8_BITS =
        (1 << UCSZ01) | (1 << UCSZ00);

    constexpr uint8_t BASE_DECIMALE = 10;
    constexpr uint8_t NOMBRE_CHIFFRES_MAX_UINT16 = 5;
    constexpr uint8_t TAILLE_TAMPON_UINT16 = 6;

    constexpr uint8_t MASQUE_NIBBLE = 0x0F;
    constexpr uint8_t VALEUR_LIMITE_NIBBLE_DECIMAL = 10;
}


void UART0::initialiser2400bauds()
{
    UBRR0H = UBRR0H_2400_BAUDS_8_MHZ;
    UBRR0L = UBRR0L_2400_BAUDS_8_MHZ;

    UCSR0A = 0;
    UCSR0B = MASQUE_RECEPTION_ET_TRANSMISSION;
    UCSR0C = MASQUE_MODE_8_BITS;
}


void UART0::envoyerOctet(uint8_t octet)
{
    while (!(UCSR0A & MASQUE_DONNEES_PRETES)) {
    }

    UDR0 = octet;
}


uint8_t UART0::recevoirOctet()
{
    while (!(UCSR0A & MASQUE_RECEPTION_COMPLETEE)) {
    }

    return UDR0;
}


void UART0::envoyerTexte(const char* texte)
{
    if (texte == nullptr) {
        return;
    }

    for (uint16_t i = 0; texte[i] != '\0'; i++) {
        envoyerOctet((uint8_t)texte[i]);
    }
}


void UART0::envoyerLigne(const char* texte)
{
    envoyerTexte(texte);
    envoyerOctet('\n');
}


void UART0::envoyerUint16(uint16_t valeur)
{
    char tampon[TAILLE_TAMPON_UINT16] = {0};
    uint8_t i = 0;

    if (valeur == 0) {
        envoyerOctet('0');
        return;
    }

    while ((valeur > 0) && (i < NOMBRE_CHIFFRES_MAX_UINT16)) {
        tampon[i] = (char)('0' + (valeur % BASE_DECIMALE));
        i++;
        valeur /= BASE_DECIMALE;
    }

    while (i > 0) {
        i--;
        envoyerOctet((uint8_t)tampon[i]);
    }
}


void UART0::envoyerNibbleHex(uint8_t nibble)
{
    uint8_t caractere = '0';

    nibble &= MASQUE_NIBBLE;

    if (nibble < VALEUR_LIMITE_NIBBLE_DECIMAL) {
        caractere = (uint8_t)('0' + nibble);
    }
    else {
        caractere =
            (uint8_t)('A' + (nibble - VALEUR_LIMITE_NIBBLE_DECIMAL));
    }

    envoyerOctet(caractere);
}


void UART0::envoyerHex8(uint8_t valeur)
{
    envoyerNibbleHex((uint8_t)(valeur >> 4));
    envoyerNibbleHex(valeur);
}


void UART0::envoyerHex16(uint16_t valeur)
{
    envoyerHex8((uint8_t)(valeur >> 8));
    envoyerHex8((uint8_t)valeur);
}