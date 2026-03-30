#ifndef OUTILS_PROJET_H
#define OUTILS_PROJET_H

/*
 * OutilsProjet.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Fonctions utilitaires pour le projet final
 */

#include <stdint.h>

class Del;

void initialiserBaseTempsProjet();
uint32_t obtenirTempsProjetMs();
bool tempsEcouleDepuis(uint32_t departMillisecondes,
                       uint16_t dureeMillisecondes);

void attendreMillisecondes(uint16_t dureeMillisecondes);

uint8_t compterBitsActifsSur5(uint8_t valeur);

void afficherAmbre(Del& del, uint16_t dureeMillisecondes);

void clignoterCouleur(Del& del,
                      void (Del::*allumerCouleur)(),
                      uint16_t dureeMillisecondes,
                      uint8_t frequenceHertz);

#endif