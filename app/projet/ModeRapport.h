#ifndef MODE_RAPPORT_H
#define MODE_RAPPORT_H

/*
 * ModeRapport.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Mode rapport
 *   Lecture des résultats en mémoire externe
 *   et envoi du rapport via RS232
 */

class Del;
class StockageProjet;

void executerModeRapport(Del& del,
                         const StockageProjet& stockage);

#endif