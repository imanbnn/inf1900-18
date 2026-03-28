/*
 * main.cpp
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Projet final
 *   Sélection du mode instruction, exécution ou rapport
 *
 * Identification matérielle
 *   RS232
 *       PD0 RXD0  cavalier DbgEN
 *       PD1 TXD0  cavalier DbgEN
 *
 *   Mémoire externe I2C
 *       PC0 SDA   cavalier MemEN
 *       PC1 SCL   cavalier MemEN
 *
 *   Moteurs
 *       PB3 OC0A  PWM moteur gauche
 *       PB4 OC0B  PWM moteur droit
 *       PB2       DIR moteur gauche
 *       PB5       DIR moteur droit
 */

#define F_CPU 8000000UL

#include <avr/io.h>

#include "Bouton.h"
#include "BrocheIo.h"
#include "CapteurDistance.h"
#include "ConfigurationProjet.h"
#include "ControleMoteurs.h"
#include "Del.h"
#include "Memoire_24.h"
#include "ModeExecution.h"
#include "ModeInstruction.h"
#include "ModeRapport.h"
#include "Moteur.h"
#include "SelectionMode.h"
#include "Sonorite.h"
#include "StockageProjet.h"
#include "SuiveurLigne.h"
#include "UART0.h"

namespace
{
    constexpr PrescalerTimer0 PRESCALER_MOTEURS =
        PrescalerTimer0::DIV_64;
}

int main()
{
    UART0::initialiser2400bauds();

    Del del;
    Bouton bouton;
    Memoire24CXXX memoire;
    StockageProjet stockage(memoire);

    const ModeProjet mode = selectionnerMode(bouton, del);

    if (mode == ModeProjet::INSTRUCTION) {
        Sonorite sonorite;
        sonorite.initialiser();

        executerModeInstruction(memoire, del, sonorite, stockage);
    }
    else if (mode == ModeProjet::RAPPORT) {
        executerModeRapport(del, stockage);
    }
    else {
        Sonorite sonorite;
        sonorite.initialiser();

        BrocheIo brocheDirectionGauche(DDRB, PORTB, PB2);
        BrocheIo brocheDirectionDroite(DDRB, PORTB, PB5);

        const Moteur::Configuration configurationGauche =
        {
            CanalPwm0::CANAL_A,
            brocheDirectionGauche,
            true
        };

        const Moteur::Configuration configurationDroite =
        {
            CanalPwm0::CANAL_B,
            brocheDirectionDroite,
            true
        };

        Moteur moteurGauche(configurationGauche);
        Moteur moteurDroit(configurationDroite);

        ControleMoteurs moteurs(moteurGauche, moteurDroit);
        moteurs.initialiser(PRESCALER_MOTEURS);

        SuiveurLigne suiveurLigne(BROCHE_DEPART_SUIVEUR_LIGNE);
        CapteurDistance capteurDistance(BROCHE_CAPTEUR_DISTANCE);

        executerModeExecution(del,
                              moteurs,
                              suiveurLigne,
                              capteurDistance,
                              sonorite,
                              stockage);
    }

    while (true) {
    }
}