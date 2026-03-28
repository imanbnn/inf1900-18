#ifndef MODE_INSTRUCTION_H
#define MODE_INSTRUCTION_H

/*
 * ModeInstruction.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Mode instruction
 *   Réception du bytecode via RS232
 *   et sauvegarde des paramètres en mémoire externe
 */

class Memoire24CXXX;
class Del;
class Sonorite;
class StockageProjet;

void executerModeInstruction(Memoire24CXXX& memoire,
                             Del& del,
                             Sonorite& sonorite,
                             StockageProjet& stockage);

#endif