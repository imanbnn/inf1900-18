/*
 * Debug.cpp
 *
 *   Implémentation de la classe Debug
 *
 *   Auteur
 *       Equipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 */

#include "Debug.h"

void Debug::initialiser()
{
    UART0::initialiser2400bauds();
}