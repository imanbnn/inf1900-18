#ifndef STOCKAGE_PROJET_H
#define STOCKAGE_PROJET_H

/*
 * StockageProjet.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Stockage des paramètres et des résultats du projet final
 *   en mémoire externe
 */

#include <stdint.h>

#include "Memoire_24.h"
#include "TypesProjet.h"

class StockageProjet
{
public:
    explicit StockageProjet(Memoire24CXXX& memoire);

    void ecrireParametres(const ParametresProjet& parametres);
    bool lireParametres(ParametresProjet& parametres) const;

    void ecrireResultats(const ResultatsProjet& resultats);
    bool lireResultats(ResultatsProjet& resultats) const;

    void effacerResultats();

private:
    void ecrireBytes_(uint16_t adresse,
                      const uint8_t* donnees,
                      uint8_t longueur);

    void lireBytes_(uint16_t adresse,
                    uint8_t* donnees,
                    uint8_t longueur) const;

    Memoire24CXXX& memoire_;

    static constexpr uint16_t adresseParametres_ = 0x0100;
    static constexpr uint16_t adresseResultats_ = 0x0110;

    static constexpr uint8_t signatureParametresOctet0_ = 0xA5;
    static constexpr uint8_t signatureParametresOctet1_ = 0x5A;
    static constexpr uint8_t signatureResultatsOctet0_ = 0xC3;
    static constexpr uint8_t signatureResultatsOctet1_ = 0x3C;
};

#endif