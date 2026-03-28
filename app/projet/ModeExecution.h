#ifndef MODE_EXECUTION_H
#define MODE_EXECUTION_H

/*
 * ModeExecution.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Mode exécution
 *   Parcours de l'étage, détections
 *   et sauvegarde des résultats
 */

class CapteurDistance;
class ControleMoteurs;
class Del;
class Sonorite;
class StockageProjet;
class SuiveurLigne;

void executerModeExecution(Del& del,
                           ControleMoteurs& moteurs,
                           SuiveurLigne& suiveur,
                           CapteurDistance& capteurDistance,
                           Sonorite& sonorite,
                           StockageProjet& stockage);

#endif