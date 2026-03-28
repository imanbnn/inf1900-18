#ifndef MINUTERIE1_H
#define MINUTERIE1_H

/*
 * Minuterie1.h
 *
 *   PWM et CTC via Timer1 16 bits de l'ATmega324PA
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Identifications materielles
 *   OC1A       PD5  sortie PWM canal A
 *   OC1B       PD4  sortie PWM canal B
 *   Minuterie  Timer1
 *
 * Deux modes disponibles
 *   CTC  minuterie avec interruption sur comparaison
 *        OCR1A est utilise comme TOP
 *   PWM  signal PWM phase correcte sur OC1A et OC1B
 *   Mode normal par defaut si on ne choisit ni PWM ni CTC
 */

#include <stdint.h>
#include <avr/io.h>

enum class PrescalerTimer1 : uint8_t
{
    ARRET    = 0,
    DIV_1    = (1 << CS10),
    DIV_8    = (1 << CS11),
    DIV_64   = (1 << CS11) | (1 << CS10),
    DIV_256  = (1 << CS12),
    DIV_1024 = (1 << CS12) | (1 << CS10)
};

enum class CanalPwm1 : uint8_t
{
    CANAL_A = 0,
    CANAL_B = 1
};

enum class ResolutionPwm1 : uint8_t
{
    BITS_8  = (1 << WGM10),
    BITS_9  = (1 << WGM11),
    BITS_10 = (1 << WGM11) | (1 << WGM10)
};

namespace topPwm1
{
    constexpr uint16_t BITS_8  = 0x00FF;
    constexpr uint16_t BITS_9  = 0x01FF;
    constexpr uint16_t BITS_10 = 0x03FF;
}

class Minuterie1
{
public:
    static void initialiserPwm(
        ResolutionPwm1 resolution = ResolutionPwm1::BITS_10);
    static void demarrer(PrescalerTimer1 prescaler);
    static void arreter();
    static bool estEnMarche();
    static void ajusterRapportCycliquePourcent(
        CanalPwm1 canal,
        uint8_t pourcent);
    static void arreterCanal(CanalPwm1 canal);
    static void ajusterPwm(uint16_t rapportA, uint16_t rapportB);
    static void ajusterPwmPourcent(uint8_t pourcentA, uint8_t pourcentB);

    static void initialiserCtc();
    static void configurerComparaisonA(uint16_t valeur);
    static void configurerComparaisonB(uint16_t valeur);
    static void reinitialiser();
    static void reinitialiserRegistres();
    static uint16_t obtenirCompteur();

    static void activerInterruptionDebordement();
    static void desactiverInterruptionDebordement();
    static void activerInterruptionComparaisonA();
    static void desactiverInterruptionComparaisonA();
    static void activerInterruptionComparaisonB();
    static void desactiverInterruptionComparaisonB();

private:
    Minuterie1() = default;

    static uint16_t obtenirTopDepuisResolution_(
        ResolutionPwm1 resolution);
    static uint16_t convertirPourcentEnOcr_(uint8_t pourcent);

    static bool estInitialisee_;
    static uint16_t valeurTop_;
    static constexpr uint8_t pourcentageMax = 100;
};

#endif