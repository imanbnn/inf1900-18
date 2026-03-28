#ifndef CONFIGURATION_PROJET_H
#define CONFIGURATION_PROJET_H

/*
 * ConfigurationProjet.h
 *
 *   Auteur
 *       Équipe 4148
 *       Iman Badakhshan
 *       Kaouthar Nouadir
 *       Paul-Erwin Koffi
 *       Sandra Timma
 *
 * Description
 *   Constantes de configuration du projet final
 *
 * Identification matérielle
 *   Suiveur de ligne Maker Line
 *       PC2 à PC6
 *
 *   Capteur de distance Sharp GP2Y0A21YK0F
 *       PA0, ADC0
 *
 *   DEL libre rouge
 *       PA1
 *
 * Détection de poteau
 *   Les poteaux sont autour des cadres rouges
 *   La distance mesurée peut varier selon la position et l'angle
 */

#include <stdint.h>
#include <avr/io.h>

constexpr uint8_t BROCHE_DEPART_SUIVEUR_LIGNE = PC2;
constexpr uint8_t NOMBRE_CAPTEURS_SUIVEUR_LIGNE = 5;

constexpr uint8_t BROCHE_CAPTEUR_DISTANCE = 0;
constexpr uint8_t BROCHE_DEL_LIBRE = PA1;

constexpr uint8_t VITESSE_SUIVI_LIGNE = 45;
constexpr uint8_t VITESSE_TOURNER = 50;
constexpr uint8_t VITESSE_ENTREE_LOCAL = 40;
constexpr uint8_t VITESSE_SORTIE_LOCAL = 40;

constexpr int16_t KP_SUIVI = 18;

constexpr uint16_t PERIODE_SUIVI_MS = 10;
constexpr uint16_t DUREE_AVANCE_CENTRE_ENTREE_MS = 250;
constexpr uint16_t TIMEOUT_TOURNER_MS = 1500;

constexpr uint16_t DUREE_AVANCE_LOCAL_RANGEMENT_MS = 1400;
constexpr uint16_t DUREE_AVANCE_LOCAL_TRAVAIL_MS = 1200;

constexpr uint16_t DUREE_RECUL_LOCAL_RANGEMENT_MS = 1400;
constexpr uint16_t DUREE_RECUL_LOCAL_TRAVAIL_MS = 1200;

constexpr uint8_t SEUIL_INTERSECTION_CAPTEURS = 4;
constexpr uint16_t DUREE_PERTE_LIGNE_ENTREE_MS = 120;
constexpr uint8_t SEUIL_OBJET_CAPTEURS = 2;

constexpr uint8_t DISTANCE_MAX_POTEAU_CM = 40;

constexpr uint16_t DUREE_NOTE_ALERTE_MS = 250;
constexpr uint16_t PAUSE_ENTRE_NOTES_MS = 125;
constexpr uint16_t PAUSE_APRES_ALERTE_MS = 2000;

constexpr uint16_t DUREE_CONFIRMATION_MS = 2000;
constexpr uint8_t FREQUENCE_CLIGNOTEMENT_MODE_HZ = 4;

constexpr uint16_t DUREE_TOURNER_90_MS = 760;

constexpr uint8_t NOMBRE_STATIONNEMENTS = 4;

constexpr uint16_t TEMPS_STATIONNEMENT_DEPUIS_OUEST_MS[
    NOMBRE_STATIONNEMENTS] = {700, 1100, 1500, 1900};

constexpr uint16_t TEMPS_STATIONNEMENT_DEPUIS_EST_MS[
    NOMBRE_STATIONNEMENTS] = {1900, 1500, 1100, 700};

#endif