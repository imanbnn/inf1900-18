# Programme de test capteurs

Objectif

Afficher sur le PC

1 Les cinq sorties digitales du Maker Line branchees sur PC2 a PC6
2 La lecture ADC du capteur GP2Y0A21 branche sur PA0
3 Une estimation en centimetres a partir d'une table approximative

Branchement

1 DbgEN en place
2 MemEN n'est pas requis
3 Maker Line D1 a D5 sur PC2 a PC6
4 Capteur distance sortie analogique sur PA0

Utilisation

1 Compiler
make

2 Programmer
make install

3 Lire sur le PC
serieViaUSB -l

Resultat attendu

Une ligne periodique du type
ML 1F ADC 345 CM 27
