#ifndef BOUTON_H
#define BOUTON_H

/*
 * Bouton.h
 *
 *   Classe pour lire un bouton-poussoir avec antirebond
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Branchement par défaut
 *   PD2 : bouton-poussoir (entrée, INT0)
 *
 * Compatible avec le bouton de la carte mère et le bouton
 * du breadboard. Les deux donnent 0 quand appuyé,
 * 1 quand relâché.
 *
 * Constructeur par défaut
 *   PIND, PORTD, DDRD, PD2, sans pullup (carte mère)
 *
 * Constructeur avec broches configurables
 *   registreEntree     registre PIN  exemple PINB, PINA
 *   registrePort       registre PORT exemple PORTB, PORTA
 *   registreDirection  registre DDR  exemple DDRB, DDRA
 *   numeroBroche       numéro de la broche  exemple PD2, PD3
 *   estPullupActif     true  = breadboard
 *                      false = carte mère
 *
 * Exemple de constructeur pour le breadboard
 *   Bouton b(PIND, PORTD, DDRD, PD3, true)
 *   brancher le bouton entre PD3 et GND
 *
 * Deux boutons peuvent être utilisés simultanément
 * sur INT0 et INT1.
 *
 * Configuration par défaut pour l'ISR
 *   INT0, FRONT_DESCENDANT
 *
 * ISR requises selon le mode choisi
 *   INT0_vect  si NumeroInterruption::EXTERNE_0
 *   INT1_vect  si NumeroInterruption::EXTERNE_1
 *
 * sei() doit être appelé dans le code applicatif.
 */

#include <stdint.h>
#include <avr/io.h>

enum class ModeInterruption : uint8_t
{
    TOUT_CHANGEMENT  = (1 << ISC00),
    FRONT_DESCENDANT = (1 << ISC01),
    FRONT_MONTANT    = (1 << ISC01) | (1 << ISC00)
};

enum class NumeroInterruption : uint8_t
{
    EXTERNE_0 = 0,
    EXTERNE_1 = 1
};

class Bouton
{
public:
    Bouton();

    Bouton(volatile uint8_t& registreEntree,
           volatile uint8_t& registrePort,
           volatile uint8_t& registreDirection,
           uint8_t numeroBroche,
           bool estPullupActif = false);

    void initialiser();
    bool estAppuye() const;

    void configurerInterruption(
        NumeroInterruption numeroInterruption =
            NumeroInterruption::EXTERNE_0,
        ModeInterruption modeInterruption =
            ModeInterruption::FRONT_DESCENDANT) const;

    void desactiverInterruption(
        NumeroInterruption numeroInterruption =
            NumeroInterruption::EXTERNE_0) const;

private:
    volatile uint8_t& registreEntree_;
    volatile uint8_t& registrePort_;
    volatile uint8_t& registreDirection_;
    uint8_t numeroBroche_;
    bool estPullupActif_;
};

#endif