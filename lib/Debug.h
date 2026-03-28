#ifndef DEBUG_H
#define DEBUG_H

/*
 * Debug.h
 *
 *   Macros de débogage par communication RS232
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Fonctionnement du module
 *   Mode actif  make debug
 *       L'option -DDEBUG est passée au compilateur.
 *       La macro DEBUG_PRINT transmet les chaînes de
 *       caractères au PC via RS232.
 *
 *   Mode inactif  make
 *       DEBUG n'est pas défini.
 *       La macro DEBUG_PRINT est remplacée par du code
 *       mort  do {} while (0).
 *
 * Utilisation
 *   Appeler Debug::initialiser() avant tout DEBUG_PRINT
 *   pour activer la communication RS232.
 */

#include "UART0.h"

#ifdef DEBUG
    #define DEBUG_PRINT(texte) UART0::envoyerTexte(texte)
#else
    #define DEBUG_PRINT(texte) do { } while (0)
#endif

class Debug
{
public:
    static void initialiser();
};

#endif