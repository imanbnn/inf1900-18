# Dossier tests

Ce dossier contient des outils de test qui ne font pas partie du comportement final du robot.

## 1 Programmes mode instruction

Dossier

programmes_instruction

Contenu

1 Fichiers texte a compiler avec progmem
2 Les fichiers binaires a envoyer au robot en mode instruction

Utilisation

1 Aller dans programmes_instruction
2 Compiler un fichier

../../../progmem/progmem -o instruction_trd_att1.bin instruction_trd_att1.txt

3 Envoyer au robot

serieViaUSB -e -f instruction_trd_att1.bin

## 2 Programme test capteurs

Dossier

app_capteurs

But

Voir rapidement sur le PC

1 Les cinq bits du Maker Line
2 La valeur ADC du capteur distance
3 Une estimation en cm

Utilisation

1 make
2 make install
3 serieViaUSB -l

## 3 Programme test RS232

Dossier

app_uart_rs232

But

Verifier que DbgEN et serieViaUSB fonctionnent avant de tester instruction et rapport

Utilisation

1 make
2 make install
3 serieViaUSB -l
