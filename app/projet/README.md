# INF1900 Projet final

Ce dossier contient le programme du robot pour le projet final.

## Où mettre ce fichier

Place ce dossier `projet` dans le répertoire demandé par le cours

1  Racine du dépôt
2  Dossier `projet`
3  Dossiers `lib` et `app` selon votre structure de remise

Si vous gardez la structure TP9, tu peux aussi copier ce dossier dans `app/projet`.

## Branchement et cavaliers

### Mémoire externe

1  Cavalier MemEN en place
2  PC0 SDA
3  PC1 SCL

### RS232

1  Cavalier DbgEN en place pour les modes instruction et rapport
2  PD0 RXD0
3  PD1 TXD0

### Suiveur de ligne Maker Line

1  D1 sur PC2
2  D2 sur PC3
3  D3 sur PC4
4  D4 sur PC5
5  D5 sur PC6

### Capteur de distance Sharp

1  Sortie analogique sur PA0 ADC0
2  Alimentation correcte et masse commune

### DEL libre pour le survol des zones et objets

Par défaut la DEL libre est sur PA1.

### Moteurs

1  PB3 OC0A PWM moteur gauche
2  PB4 OC0B PWM moteur droit
3  PB2 direction moteur gauche
4  PB5 direction moteur droit

### Sonorité

1  PD7 OC2A fil rouge du piezo
2  PD6 fil noir du piezo, PD6 doit rester à 0 volt

## Compilation et installation

Depuis le dossier du programme

```bash
make clean
make
make install
```

## Tests complets

L’objectif est de valider chaque mode séparément puis l’intégration.

### Préparation commune

1  Piles chargées ou alimentation de labo
2  Cavaliers MemEN et DbgEN selon le mode
3  Robot centré dans un stationnement
4  Nez du robot vers le nord

### Sélection du mode

But

Vérifier la séquence de sélection et la réaction au bouton Interrupt

Scénario 1  Aucun appui

Résultat attendu

La séquence se termine et le robot passe en mode exécution

Scénario 2  Appui pendant vert

Résultat attendu

Le robot interrompt la séquence et passe au mode instruction

Scénario 3  Appui pendant rouge

Résultat attendu

Le robot interrompt la séquence et passe au mode rapport

Scénario 4  Appui pendant DEL éteinte

Résultat attendu

Aucun effet

### Mode instruction

But

Recevoir le bytecode, l’écrire en mémoire externe, puis confirmer la réception

Étape PC

Dans un terminal

```bash
serieViaUSB -e -f chemin/vers/programme.bin
```

Scénario 1  trd et att 1

Résultat attendu

1  Trois notes
2  Pause
3  DEL de confirmation selon le sens
4  DEL ambre 1 clignotement à 1 Hz
5  Fin du mode, le robot ne fait plus rien

Scénario 2  trg et att 4

Résultat attendu

Même séquence mais avec 4 clignotements ambre

Scénario 3  Réception incomplète

Résultat attendu

Le robot reste en état sûr

1  Moteurs arrêtés
2  Son arrêté
3  DEL éteinte ou indication prévue

### Mode exécution

But

Faire le parcours, compter, évacuer, stationner, sauvegarder les résultats

Ordre recommandé

1  Parcours sans ajout
2  Ajout zones endommagées
3  Ajout poteaux
4  Ajout objets
5  Stationnement final

#### Départ du stationnement

Scénario 1  Départ normal

Résultat attendu

Le robot sort sans toucher les lignes, rejoint la paroi sud, puis part du bon côté selon le sens

Scénario 2  Départ depuis 1 2 3 4

Résultat attendu

Même comportement pour chaque place

#### Couloirs et zones endommagées

Scénario 1  0 zone

Résultat attendu

1  Compteur du couloir à 0
2  DEL libre jamais rouge

Scénario 2  1 zone

Résultat attendu

1  Compteur à 1
2  DEL libre rouge uniquement pendant le survol

Scénario 3  2 zones

Résultat attendu

1  Compteur à 2
2  DEL libre rouge deux fois

#### Locaux de travail et poteaux

Le local peut avoir 0 à 3 poteaux sur trois côtés possibles.

Principe

1  Le robot entre
2  Il fait un balayage centre puis gauche puis droite
3  S’il voit un poteau il joue l’alerte en boucle
4  Il attend le retrait
5  Il confirme par clignotement vert à 4 Hz pendant 2 secondes
6  Il recommence le balayage jusqu’à ce que le local soit vide

Scénario 1  0 poteau

Résultat attendu

1  Aucune alerte
2  Compteur personnes à 0

Scénario 2  1 poteau côté bas

Résultat attendu

1  Alerte jouée jusqu’au retrait
2  Confirmation DEL verte 4 Hz 2 secondes
3  Compteur à 1

Scénario 3  1 poteau côté gauche

Résultat attendu

Même logique, compteur à 1

Scénario 4  1 poteau côté droite

Résultat attendu

Même logique, compteur à 1

Scénario 5  2 ou 3 poteaux

Résultat attendu

Après chaque retrait le robot rescane et continue jusqu’à ce que tout soit vide

#### Locaux de rangement et objets

Scénario 1  0 objet

Résultat attendu

1  Compteur à 0
2  DEL libre jamais rouge

Scénario 2  1 objet

Résultat attendu

1  Compteur à 1
2  DEL libre rouge pendant le survol

Scénario 3  2 objets

Résultat attendu

1  Compteur à 2
2  DEL libre rouge deux fois

#### Stationnement

Scénario att 1 2 3 4

Résultat attendu

1  Le robot se gare dans la case demandée
2  Il ne touche pas les lignes
3  Il joue un son grave environ 1 seconde

### Mode rapport

But

Envoyer le rapport via RS232 avec serieViaUSB

Étape PC

Dans un terminal

```bash
serieViaUSB -l
```

Scénario 1  Tout conforme

Résultat attendu

Toutes les lignes affichent oui

Scénario 2  Une section non conforme

Résultat attendu

La ligne correspondante affiche non et le détail contient le nombre trouvé

Scénario 3  Combinaison complète

Résultat attendu

Le rapport reflète exactement les compteurs enregistrés en exécution

## Debug optionnel

Le debug sert à afficher des messages par RS232 pendant les tests.

Dans le dossier du programme

```bash
make debug
make install
```

Puis sur le PC

```bash
serieViaUSB -l
```

Résultat attendu

Des messages apparaissent dans le terminal pendant l’exécution.

## Calibration rapide

Pour stabiliser le comportement sur table réelle

1  Ajuster les vitesses et durées dans `ConfigurationProjet.h`
2  Ajuster le seuil de détection des poteaux
3  Ajuster le temps des petits pivots gauche et droite pour viser les trois côtés
4  Ajuster les temps de stationnement pour chaque case

## Outils de test

Le dossier tests contient des programmes et des fichiers utiles pendant la mise au point

1 tests programmes_instruction pour generer des bytecodes de test
2 tests app_capteurs pour lire Maker Line et capteur distance en RS232
3 tests app_uart_rs232 pour valider rapidement la liaison serie
