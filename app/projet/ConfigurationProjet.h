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
 *   Constantes et paramètres de calibration du projet final
 *
 * Identification matérielle
 *   Suiveur de ligne Maker Line
 *       PC2 à PC6
 *
 *   Capteur de distance infrarouge
 *       PA0, ADC0
 *
 *   DEL libre
 *       PA0
 */

#include <avr/io.h>
#include <stdint.h>

constexpr uint8_t BROCHE_DEPART_SUIVEUR_LIGNE = PC2;
constexpr uint8_t NOMBRE_CAPTEURS_SUIVEUR_LIGNE = 5;

constexpr uint8_t BROCHE_CAPTEUR_DISTANCE = 0;
constexpr uint8_t BROCHE_DEL_LIBRE = PA0;

constexpr uint8_t MASQUE_CINQ_BITS = 0x1F;
constexpr uint8_t NOMBRE_CAPTEURS = NOMBRE_CAPTEURS_SUIVEUR_LIGNE;

constexpr uint8_t MASQUE_SUIVI_GAUCHE = 0b00111;
constexpr uint8_t MASQUE_SUIVI_DROITE = 0b11100;

constexpr int16_t POSITION_VOULUE_GAUCHE = 50;
constexpr int16_t POSITION_VOULUE_DROITE = 350;

constexpr uint8_t FACTEUR_POSITION_CAPTEUR = 100;
constexpr int16_t POSITION_NON_DETECTEE = -1;

constexpr int16_t AJUSTEMENT_PERTE_LIGNE = 15;
constexpr uint16_t DUREE_PERTE_LIGNE_ENTREE_MS = 120;
constexpr uint8_t KP_SUIVI = 18;

constexpr int16_t VITESSE_MIN = 0;
constexpr int16_t VITESSE_MAX = 100;

constexpr uint8_t MASQUE_MUR_GAUCHE = 0b00011;
constexpr uint8_t MASQUE_MUR_DROITE = 0b11000;

constexpr uint8_t SEUIL_INTERSECTION_CAPTEURS = 4;
constexpr uint8_t SEUIL_OBJET_CAPTEURS = 3;

constexpr uint8_t NOMBRE_ZONES_SOL_MAX = 2;
constexpr uint8_t NOMBRE_LECTURES_CONFIRMATION_OBJET = 2;
constexpr uint8_t NOMBRE_OBJETS_MAX_LOCAL_RANGEMENT = 2;
constexpr uint8_t NOMBRE_LECTURES_INTERSECTION = 3;

constexpr uint8_t FREQUENCE_CLIGNOTEMENT_MODE_HZ = 4;
constexpr uint16_t PERIODE_SUIVI_MS = 20;

constexpr uint8_t VITESSE_SUIVI_LIGNE = 50;
constexpr uint8_t VITESSE_APPROCHE_COIN = 35;
constexpr uint8_t VITESSE_TOURNER = 60;
constexpr uint8_t VITESSE_ENTREE_LOCAL = 40;
constexpr uint8_t VITESSE_SORTIE_LOCAL = 40;

constexpr uint16_t DUREE_APPROCHE_COIN_MS = 150;
constexpr uint16_t DUREE_TOURNER_90_MS = 700;
constexpr uint16_t DUREE_AVANCE_CENTRE_ENTREE_MS = 250;
constexpr uint16_t DUREE_RECUL_LOCAL_TRAVAIL_MS = 350;
constexpr uint16_t DUREE_AVANCE_LOCAL_RANGEMENT_MS = 1400;
constexpr uint16_t DUREE_RECUL_LOCAL_RANGEMENT_MS = 800;

constexpr uint8_t NOMBRE_BALAYAGES_LOCAL_TRAVAIL = 2;
constexpr uint16_t DUREE_TOURNER_15_MS =
    (DUREE_TOURNER_90_MS * 15) / 90;
constexpr uint16_t DUREE_TOURNER_30_MS =
    (DUREE_TOURNER_90_MS * 30) / 90;

constexpr uint8_t DISTANCE_POTEAU_CM = 35;
constexpr uint8_t HYSTERESIS_DISTANCE_POTEAU_CM = 5;

constexpr uint16_t DUREE_NOTE_ALERTE_MS = 250;
constexpr uint16_t PAUSE_ENTRE_NOTES_MS = 125;
constexpr uint16_t PAUSE_APRES_ALERTE_MS = 2000;
constexpr uint16_t DUREE_CONFIRMATION_MS = 2000;
constexpr uint8_t NOMBRE_NOTES_ALERTE = 3;

constexpr uint16_t DUREE_ATTENTE_BOUCLE_MS = 125;

constexpr uint16_t TIMEOUT_SEGMENT_SUD_MS = 12000;
constexpr uint16_t TIMEOUT_SEGMENT_PRINCIPAL_MS = 20000;

constexpr uint16_t TIMEOUT_STATIONNEMENT_MS = 4000;
constexpr uint16_t PAS_STATIONNEMENT_MS = 10;

constexpr uint16_t TIMEOUT_SORTIE_STATIONNEMENT_MS = 5000;
constexpr uint16_t PAS_SORTIE_STATIONNEMENT_MS = 10;

constexpr uint16_t DELAI_AVANCE_APRES_ENTREE_MS = 300;

constexpr uint8_t SEUIL_REPERE_STATIONNEMENT_CAPTEURS = 4;
constexpr uint8_t NOMBRE_LECTURES_REPERE_STATIONNEMENT = 2;
constexpr uint16_t DUREE_DECALAGE_CENTRE_STATIONNEMENT_MS = 250;

constexpr uint8_t NOTE_PAR_DEFAUT = 60;

#endif