#ifndef MINUTERIE2_H
#define MINUTERIE2_H

/*
 * Minuterie2.h
 *
 *   Encapsulation du Timer2 8 bits de l'ATmega324PA
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Identifications materielles
 *   OC2A       PD7  canal A
 *   OC2B       PD6  canal B
 *   Minuterie  Timer2
 *
 * Modes disponibles
 *   PWM phase correcte 8 bits  TOP = 0xFF
 *   CTC  Clear Timer on Compare Match
 *   Mode normal par defaut si ni initialiserPwm()
 *   ni initialiserCtc() ne sont appeles
 *
 * Avantage du Timer2
 *   Prescalers DIV_32 et DIV_128 exclusifs,
 *   permettant un controle plus fin de la frequence
 */

#include <stdint.h>
#include <avr/io.h>

enum class PrescalerTimer2 : uint8_t
{
    ARRET    = 0,
    DIV_1    = (1 << CS20),
    DIV_8    = (1 << CS21),
    DIV_32   = (1 << CS21) | (1 << CS20),
    DIV_64   = (1 << CS22),
    DIV_128  = (1 << CS22) | (1 << CS20),
    DIV_256  = (1 << CS22) | (1 << CS21),
    DIV_1024 = (1 << CS22) | (1 << CS21) | (1 << CS20)
};

enum class CanalPwm2 : uint8_t
{
    CANAL_A = 0,
    CANAL_B = 1
};

class Minuterie2
{
public:
    static void initialiserPwm();
    static void initialiserCtc();

    static void demarrer(PrescalerTimer2 prescaler);
    static void arreter();
    static bool estEnMarche();

    static void ajusterRapportCycliquePourcent(CanalPwm2 canal,
                                               uint8_t pourcent);
    static void arreterCanal(CanalPwm2 canal);
    static void ajusterPwm(uint8_t rapportA, uint8_t rapportB);
    static void ajusterPwmPourcent(uint8_t pourcentA, uint8_t pourcentB);

    static void configurerComparaisonA(uint8_t valeur);
    static void configurerComparaisonB(uint8_t valeur);

    static void activerInterruptionComparaisonA();
    static void desactiverInterruptionComparaisonA();
    static void activerInterruptionComparaisonB();
    static void desactiverInterruptionComparaisonB();
    static void activerInterruptionDebordement();
    static void desactiverInterruptionDebordement();

    static void reinitialiser();
    static void reinitialiserRegistres();

private:
    Minuterie2() = default;

    static uint8_t convertirPourcentEnOcr_(uint8_t pourcent);

    static constexpr uint8_t pwmTop = 0xFF;
    static constexpr uint8_t pourcentageMax = 100;
};

#endif