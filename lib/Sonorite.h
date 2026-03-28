#ifndef SONORITE_H
#define SONORITE_H

/*
 * Sonorite.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Génération de son par le Timer2 en mode CTC.
 *   Le signal OC2A est basculé à chaque comparaison.
 *
 * Identifications matérielles
 *   PD7 OC2A  signal vers le fil rouge du piezo
 *   PD6       mise à la masse vers le fil noir du piezo
 */

#include <stdint.h>

class Sonorite
{
public:
    Sonorite();

    void initialiser();
    void jouerNoteMidi(uint8_t noteMidi);
    void arreter();

private:
    static bool estNoteSupportee_(uint8_t noteMidi);
    static uint8_t calculerOcrDepuisNote_(uint8_t noteMidi);

    static constexpr uint8_t noteMidiMin = 45;
    static constexpr uint8_t noteMidiMax = 81;

    bool estInitialisee_;
};

#endif