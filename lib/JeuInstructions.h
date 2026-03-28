#ifndef JEU_INSTRUCTIONS_H
#define JEU_INSTRUCTIONS_H

/*
 * JeuInstructions.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Constantes du jeu d'instructions du TP9
 */

#include <stdint.h>

namespace JeuInstructions
{
    constexpr uint8_t dbt = 0x01;
    constexpr uint8_t att = 0x02;

    constexpr uint8_t dal = 0x44;
    constexpr uint8_t det = 0x45;

    constexpr uint8_t sgo = 0x48;
    constexpr uint8_t sar = 0x09;

    constexpr uint8_t mar0 = 0x60;
    constexpr uint8_t mar1 = 0x61;
    constexpr uint8_t mav  = 0x62;
    constexpr uint8_t mre  = 0x63;
    constexpr uint8_t trd  = 0x64;
    constexpr uint8_t trg  = 0x65;

    constexpr uint8_t dbc = 0xC0;
    constexpr uint8_t fbc = 0xC1;

    constexpr uint8_t fin = 0xFF;
}

#endif