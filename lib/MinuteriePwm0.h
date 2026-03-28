#ifndef MINUTERIE_PWM0_H
#define MINUTERIE_PWM0_H

/*
 * MinuteriePwm0.h
 *
 *   PWM phase correcte via Timer0 pour la motricité
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Identifications matérielles
 *   OC0A       PB3  canal A
 *              partagé ISP, débrancher avant make install
 *   OC0B       PB4  canal B
 *              partagé ISP, débrancher avant make install
 *   Minuterie  Timer0
 *
 * Mode
 *   PWM phase correcte 8 bits  WGM00 = 1, WGM01 = 0
 *   Fréquence = F_CPU / (prescaler x 510)
 *   Exemple à 8 MHz avec DIV_8  ~1.96 kHz
 *
 * initialiser() est idempotente
 *   Peut être appelée sans risque plusieurs fois
 *
 * Timer0 est réservé à la motricité
 */

#include <stdint.h>
#include <avr/io.h>

enum class CanalPwm0 : uint8_t
{
    CANAL_A = 0,
    CANAL_B = 1
};

enum class PrescalerTimer0 : uint8_t
{
    ARRET    = 0,
    DIV_1    = (1 << CS00),
    DIV_8    = (1 << CS01),
    DIV_64   = (1 << CS01) | (1 << CS00),
    DIV_256  = (1 << CS02),
    DIV_1024 = (1 << CS02) | (1 << CS00)
};

class MinuteriePwm0
{
public:
    static void initialiser();
    static void demarrer(PrescalerTimer0 prescaler);
    static void arreter();
    static bool estEnMarche();

    static void ajusterRapportCycliquePourcent(CanalPwm0 canal,
                                               uint8_t pourcent);
    static void arreterCanal(CanalPwm0 canal);
    static void reinitialiserRegistres();

private:
    static uint8_t convertirPourcentEnOcr_(uint8_t pourcent);

    static bool estInitialisee_;
    static constexpr uint8_t pwmTop = 0xFF;
    static constexpr uint8_t pourcentageMax = 100;

    MinuteriePwm0() = default;
};

#endif