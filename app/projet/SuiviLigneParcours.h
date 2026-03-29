#ifndef SUIVI_LIGNE_PARCOURS_H
#define SUIVI_LIGNE_PARCOURS_H

/*
 * SuiviLigneParcours.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Encapsulation du suivi de ligne et de la détection d'événements
 *   Intersection, entrée de local, timeout
 */

#include <stdint.h>

class BrocheIo;
class ControleMoteurs;
class SuiveurLigne;

enum class CoteSuivi : uint8_t
{
    GAUCHE,
    DROITE
};

enum class EvenementSuivi : uint8_t
{
    INTERSECTION,
    ENTREE_LOCAL,
    TIMEOUT
};

struct OptionsSuivi
{
    bool estDetectionEntreeActive;
    bool estComptageZonesActif;
    uint16_t timeoutMillisecondes;
    uint8_t* compteurZones;
};

class SuiviLigneParcours
{
public:
    SuiviLigneParcours(SuiveurLigne& suiveurLigne,
                       ControleMoteurs& moteurs,
                       BrocheIo& delLibre);

    EvenementSuivi suivreJusquaEvenement(
        CoteSuivi cote,
        const OptionsSuivi& options);

    void ajusterMoteursSelonLigne(uint8_t capteurs,
                                  CoteSuivi cote);

private:
    uint8_t obtenirMasqueSuivi_(CoteSuivi cote) const;
    uint8_t obtenirMasqueMur_(CoteSuivi cote) const;

    bool estIntersection_(uint8_t capteurs) const;
    bool estPerteLigneEntree_(uint8_t capteurs,
                              CoteSuivi cote) const;

    int16_t calculerPositionCapteurs_(uint8_t capteurs,
                                      CoteSuivi cote) const;

    void ajusterPourZoneSol_(uint8_t capteurs,
                             bool estComptageZonesActif,
                             uint8_t* compteurZones);

    SuiveurLigne& suiveurLigne_;
    ControleMoteurs& moteurs_;
    BrocheIo& delLibre_;
};

#endif