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
 *   Déclaration de la classe responsable du suivi de ligne pendant le parcours
 */

#include <stdint.h>

class ControleMoteurs;
class Del;
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
    uint16_t timeoutMillisecondes;
    bool estComptageZonesActif;
    uint8_t* compteurZones;
};

class SuiviLigneParcours
{
public:
    SuiviLigneParcours(SuiveurLigne& suiveurLigne,
                       ControleMoteurs& moteurs,
                       Del& del);

    EvenementSuivi suivreJusquaEvenement(CoteSuivi coteSuivi,
                                         const OptionsSuivi& options);

    void ajusterSuiviSelonCapteurs(uint8_t capteurs,
                                   CoteSuivi coteSuivi);

private:
    int16_t calculerPositionLigne_(uint8_t capteurs) const;
    int8_t calculerCorrection_(int16_t positionLigne,
                               CoteSuivi coteSuivi) const;
    bool estIntersection_(uint8_t capteurs) const;
    bool estPerteLigneEntree_(uint8_t capteurs,
                              CoteSuivi coteSuivi) const;
    void ajusterPourZoneSol_(uint8_t capteurs,
                             bool estComptageZonesActif,
                             uint8_t* compteurZones,
                             bool& etaitZoneSol);

    SuiveurLigne& suiveurLigne_;
    ControleMoteurs& moteurs_;
    Del& del_;
};

#endif