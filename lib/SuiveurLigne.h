#ifndef SUIVEUR_LIGNE_H
#define SUIVEUR_LIGNE_H

/*
 * SuiveurLigne.h
 *
 *   Encapsulation du capteur de suivi de ligne Cytron Maker Line
 *   Lecture numérique des 5 capteurs via le port C, ATmega324
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Constructeur
 *   numeroBrocheDepart : broche de départ sur le port C, ex. PC2
 *                        les 5 capteurs occupent numeroBrocheDepart
 *                        à numeroBrocheDepart + 4
 *
 * Opérations disponibles
 *   lirePosition() : retourne une enum Position selon les capteurs actifs
 *   lireCapteurs() : retourne un uint8_t de 5 bits,
 *                    bit i = 1 si ligne détectée
 *   estSurObjet()  : retourne true si au moins un capteur détecte une ligne
 *
 * Branchement, numeroBrocheDepart = PC2
 *   PC2 -> S1 capteur 1, extrême gauche
 *   PC3 -> S2 capteur 2
 *   PC4 -> S3 capteur 3, centre
 *   PC5 -> S4 capteur 4
 *   PC6 -> S5 capteur 5, extrême droite
 *   VCC -> 5 V
 *   GND -> GND
 *
 * Convention logique, switch Maker Line sur DARK
 *   1 = ligne noire détectée
 *   0 = fond blanc
 *
 * Calibration physique requise, bouton CAL sur la carte Maker Line
 *   Voir section 6 de la fiche technique Cytron
 *   avant la première utilisation
 *
 * Cas détectés et validés au labo, parcours projet
 *   LIGNE_NON_DETECTEE : fond blanc, aucun capteur actif
 *   EXTREME_GAUCHE     : S1 seul actif
 *   GAUCHE             : S1 + S2 actifs
 *   CENTRE             : S2 + S3 + S4 actifs
 *   DROITE             : S4 + S5 actifs
 *   EXTREME_DROITE     : S5 seul actif
 *   CROISEMENT         : tous les capteurs actifs
 *
 * Dépendances
 *   avr/io.h
 *
 * Interruptions requises
 *   Aucune
 */

#include <stdint.h>
#include <avr/io.h>

class SuiveurLigne
{
public:
    enum Position
    {
        LIGNE_NON_DETECTEE,
        EXTREME_GAUCHE,
        GAUCHE,
        CENTRE,
        DROITE,
        EXTREME_DROITE,
        CROISEMENT
    };

    explicit SuiveurLigne(uint8_t numeroBrocheDepart);

    Position lirePosition();
    uint8_t lireCapteurs();
    bool estSurObjet();

private:
    static const uint8_t nombreCapteurs_ = 5;
    static const uint8_t masqueCapteur1_ = 0b00001;
    static const uint8_t masqueCapteur5_ = 0b10000;
    static const uint8_t masqueCapteursGauche_ = 0b00011;
    static const uint8_t masqueCapteursDroite_ = 0b11000;
    static const uint8_t masqueTousCapteurs_ = 0b11111;

    uint8_t numeroBrocheDepart_;
};

#endif