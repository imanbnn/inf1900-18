#ifndef TYPES_PROJET_H
#define TYPES_PROJET_H

/*
 * TypesProjet.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Types utilisés par le projet final
 */

#include <stdint.h>

constexpr uint8_t NOMBRE_NOTES_MIDI = 3;

enum class ModeProjet : uint8_t
{
    INSTRUCTION,
    EXECUTION,
    RAPPORT
};

enum class SensParcours : uint8_t
{
    HORAIRE,
    ANTI_HORAIRE
};

struct ParametresProjet
{
    SensParcours sensParcours;
    uint8_t notesMidi[NOMBRE_NOTES_MIDI];
    uint8_t numeroStationnement;
};

struct ResultatsProjet
{
    uint8_t nombreZonesCouloirOuest;
    uint8_t nombreZonesCouloirEst;

    uint8_t nombrePersonnesLocalA;
    uint8_t nombrePersonnesLocalD;

    uint8_t nombreObjetsLocalB;
    uint8_t nombreObjetsLocalC;
};

#endif