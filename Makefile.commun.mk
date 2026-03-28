########       AVR Project Makefile       ########
#####                                        #####
#####      INF1900 TP7  Équipe 4148          #####
#####                                        #####
#####   Variables et options communes        #####
#####                                        #####
#####   Inspiré de Simon Barrette et Jerome  #####
#####   Collin pour INF1900 et de Pat Deegan #####
#####  Psychogenic Inc (www.psychogenic.com) #####
##################################################

# Ce fichier regroupe les variables et les options communes.
# Il est inclus par les Makefiles des sous-répertoires.

# Nom du microcontrôleur cible
MCU = atmega324pa

# Niveau d'optimisation
OPTLEVEL = s

# Compilateur utilisé
CC = avr-gcc

# Suppression des fichiers générés
REMOVE = rm -f

# Options de compilation communes
# -MMD -MP génère automatiquement les dépendances .d
CFLAGS_BASE = -MMD -MP -g -mmcu=$(MCU) -O$(OPTLEVEL) \
    -std=c++14 -fshort-enums -funsigned-bitfields -funsigned-char -Wall

# Option supplémentaire pour retirer le bogue de
# "array subscript 0 is outside array bounds" sur avr-gcc v12
GCCVERSION := $(shell expr `$(CC) -dumpversion | cut -f1 -d.` \>= 12)
ifeq ($(GCCVERSION),1)
    CFLAGS_BASE += --param=min-pagesize=0
endif

# Options supplémentaires pour le C++
CXXFLAGS_BASE = -fno-exceptions

#####                    EOF                   #####