#ifndef DEL_H
#define DEL_H

/*
 * Del.h
 *
 *   Classe pour contrôler la DEL bicolore
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Constructeur par défaut
 *   PB0 : borne négative de la DEL
 *   PB1 : borne positive de la DEL
 *
 * Constructeur avec broches configurables
 *   registrePort       : PORT  exemple PORTB, PORTA
 *   registreDirection  : DDR   exemple DDRB, DDRA
 *   brochePositive     : broche de la borne positive  exemple PB1
 *   brocheNegative     : broche de la borne négative  exemple PB0
 *
 * Les broches et le port sont configurables via le
 * constructeur.
 */

#include <stdint.h>
#include <avr/io.h>

class Del
{
public:
    Del();

    Del(volatile uint8_t& registrePort,
        volatile uint8_t& registreDirection,
        uint8_t brochePositive,
        uint8_t brocheNegative);

    void initialiser();

    void rouge();
    void vert();
    void ambre();
    void eteindre();

private:
    volatile uint8_t& registrePort_;
    volatile uint8_t& registreDirection_;
    uint8_t brochePositive_;
    uint8_t brocheNegative_;
};

#endif