#ifndef SELECTION_MODE_H
#define SELECTION_MODE_H

/*
 * SelectionMode.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Sélection du mode de fonctionnement du robot
 */

#include "TypesProjet.h"

class Bouton;
class Del;

ModeProjet selectionnerMode(Bouton& bouton, Del& del);

#endif