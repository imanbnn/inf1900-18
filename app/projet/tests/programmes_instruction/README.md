# Programmes d'essai pour le mode instruction

Ces fichiers texte sont a compiler avec progmem pour produire des fichiers binaires a envoyer au robot.

Chemin conseille

1 Aller dans le dossier programmes_instruction
2 Compiler avec le progmem du depot
3 Envoyer le fichier binaire au mode instruction via serieViaUSB

Commandes

1 Compilation
../../../progmem/progmem -o instruction_trd_att1.bin instruction_trd_att1.txt

2 Verification
od -v -t x1 instruction_trd_att1.bin

3 Envoi
serieViaUSB -e -f instruction_trd_att1.bin

Fichiers disponibles

1 instruction_trd_att1
2 instruction_trd_att4
3 instruction_trg_att1
4 instruction_trg_att4
