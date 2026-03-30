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
 *   Programme de test des capteurs
 *   Affiche en RS232 l'etat du suiveur de ligne Maker Line et la lecture ADC du GP2Y0A21
 *
 * Identification materielle
 *   RS232
 *     PD0 RXD0  cavalier DbgEN
 *     PD1 TXD0  cavalier DbgEN
 *
 *   Maker Line, sorties digitales D1 a D5
 *     PC2 D1
 *     PC3 D2
 *     PC4 D3
 *     PC5 D4
 *     PC6 D5
 *
 *   GP2Y0A21, sortie analogique
 *     PA0 ADC0
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "UART0.h"

namespace
{
    constexpr uint16_t DELAI_AFFICHAGE_MS = 100;

    constexpr uint8_t DECALAGE_CAPTEURS = 2;
    constexpr uint8_t MASQUE_CINQ_BITS = 0x1F;

    constexpr uint8_t ADC_PRESCALER =
        (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    struct PointDistance
    {
        uint16_t adc;
        uint8_t cm;
    };

    const PointDistance TABLE_DISTANCE[] =
{
    { 500, 10 },
    { 255, 20 },
    { 167, 30 },
    { 114, 40 },
    {  87, 50 },
    {  68, 60 },
    {  58, 80 }
};

    constexpr uint8_t TAILLE_TABLE_DISTANCE =
        (uint8_t)(sizeof(TABLE_DISTANCE) / sizeof(TABLE_DISTANCE[0]));

    void envoyerOctetHex(uint8_t valeur)
    {
        const uint8_t haut = (uint8_t)(valeur >> 4);
        const uint8_t bas  = (uint8_t)(valeur & 0x0F);

        const char c1 = (haut < 10) ? (char)('0' + haut)
                                    : (char)('A' + (haut - 10));
        const char c2 = (bas < 10) ? (char)('0' + bas)
                                   : (char)('A' + (bas - 10));

        UART0::envoyerOctet((uint8_t)c1);
        UART0::envoyerOctet((uint8_t)c2);
    }

    void envoyerUint16Decimal(uint16_t valeur)
    {
        char buffer[6];
        uint8_t i = 0;

        if (valeur == 0) {
            UART0::envoyerOctet('0');
            return;
        }

        while ((valeur > 0) && (i < 5)) {
            const uint8_t chiffre = (uint8_t)(valeur % 10);
            buffer[i++] = (char)('0' + chiffre);
            valeur = (uint16_t)(valeur / 10);
        }

        while (i > 0) {
            UART0::envoyerOctet((uint8_t)buffer[--i]);
        }
    }

    void envoyerUint8Decimal(uint8_t valeur)
    {
        envoyerUint16Decimal(valeur);
    }

    void envoyerTexte(const char* texte)
    {
        UART0::envoyerTexte(texte);
    }

    void initialiserMakerLine()
    {
        const uint8_t masque = (uint8_t)(MASQUE_CINQ_BITS << DECALAGE_CAPTEURS);
        DDRC &= (uint8_t)(~masque);
    }

    void initialiserAdc()
    {
        ADMUX = (1 << REFS0);
        ADCSRA = (1 << ADEN) | ADC_PRESCALER;
    }

    uint16_t lireAdc0()
    {
        ADMUX = (uint8_t)((ADMUX & 0xF0) | 0);

        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC)) {
        }

        return ADC;
    }

    uint8_t convertirAdcEnCentimetresApprox(uint16_t adc)
    {
        if (adc >= TABLE_DISTANCE[0].adc) {
            return TABLE_DISTANCE[0].cm;
        }

        const uint8_t dernier = (uint8_t)(TAILLE_TABLE_DISTANCE - 1);
        if (adc <= TABLE_DISTANCE[dernier].adc) {
            return TABLE_DISTANCE[dernier].cm;
        }

        for (uint8_t i = 0; i < dernier; i++) {
            const uint16_t adcHaut = TABLE_DISTANCE[i].adc;
            const uint16_t adcBas  = TABLE_DISTANCE[i + 1].adc;

            if ((adc <= adcHaut) && (adc >= adcBas)) {
                const uint8_t cmHaut = TABLE_DISTANCE[i].cm;
                const uint8_t cmBas  = TABLE_DISTANCE[i + 1].cm;

                const uint16_t intervalle = (uint16_t)(adcHaut - adcBas);
                const uint16_t delta      = (uint16_t)(adcHaut - adc);

                const uint16_t cmDelta =
                    (uint16_t)(cmBas - cmHaut) * delta;

                const uint8_t cm =
                    (uint8_t)(cmHaut + (cmDelta / intervalle));

                return cm;
            }
        }

        return 0;
    }

    uint8_t lireCapteursMakerLine()
    {
        const uint8_t brut = PINC;
        return (uint8_t)((brut >> DECALAGE_CAPTEURS) & MASQUE_CINQ_BITS);
    }

    void afficherUneLigne(uint8_t capteurs, uint16_t adc, uint8_t cm)
    {
        envoyerTexte("ML ");
        envoyerOctetHex(capteurs);

        envoyerTexte(" ADC ");
        envoyerUint16Decimal(adc);

        envoyerTexte(" CM ");
        envoyerUint8Decimal(cm);

        envoyerTexte("\n");
    }
}

int main()
{
    UART0::initialiser2400bauds();
    initialiserMakerLine();
    initialiserAdc();

    envoyerTexte("TEST CAPTEURS\n");

    while (true) {
        const uint8_t capteurs = lireCapteursMakerLine();
        const uint16_t adc = lireAdc0();
        const uint8_t cm = convertirAdcEnCentimetresApprox(adc);

        afficherUneLigne(capteurs, adc, cm);
        _delay_ms(DELAI_AFFICHAGE_MS);
    }
}
