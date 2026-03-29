#ifndef LOCAL_TRAVAIL_H
#define LOCAL_TRAVAIL_H

/*
 * LocalTravail.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Gestion d'un local de travail
 *   Balayage au capteur de distance et évacuation des poteaux
 */

#include <stdint.h>

class CapteurDistance;
class Del;
class Deplacements;
class Sonorite;

class LocalTravail
{
public:
    LocalTravail(CapteurDistance& capteurDistance,
                 Sonorite& sonorite,
                 Del& del,
                 Deplacements& deplacements);

    uint8_t gerer(bool estEntreeAGauche,
                  const uint8_t notesMidi[]);

private:
    bool evacuerPersonneSiPresente_(const uint8_t notesMidi[]);
    void jouerSequenceAlerte_(const uint8_t notesMidi[]);
    void confirmerEvacuation_();

    CapteurDistance& capteurDistance_;
    Sonorite& sonorite_;
    Del& del_;
    Deplacements& deplacements_;
};

#endif