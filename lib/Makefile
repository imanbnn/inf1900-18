########       AVR Project Makefile       ########
#####                                        #####
#####      INF1900 TP7  Équipe 4148          #####
#####                                        #####
#####          Librairie statique            #####
#####                                        #####
#####   Inspiré de Simon Barrette et Jerome  #####
#####   Collin pour INF1900 et de Pat Deegan #####
#####  Psychogenic Inc (www.psychogenic.com) #####
##################################################

# Ce Makefile compile une librairie statique .a pour AVR.
# Il compile tous les fichiers .c et .cpp du dossier lib.

include ../Makefile.commun.mk

# Nom de la librairie produite
LIBNAME = inf1900
TRG = lib$(LIBNAME).a

# Archiveur utilisé pour créer la bibliothèque
AR = avr-ar

# Inclusions locales
INC_DIRS = -I.

CFLAGS = $(CFLAGS_BASE) $(INC_DIRS)
CXXFLAGS = $(CXXFLAGS_BASE)

# Sources du répertoire courant
CPPFILES = $(wildcard *.cpp)

# Fichiers objets et dépendances
OBJDEPS = $(CPPFILES:.cpp=.o)
DEPS = $(OBJDEPS:.o=.d)

.PHONY: all clean

all: $(TRG)

$(TRG): $(OBJDEPS)
	$(AR) rcs $(TRG) $(OBJDEPS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	$(REMOVE) $(TRG) $(OBJDEPS) $(DEPS)

debug: CFLAGS += -DDEBUG
debug: CXXFLAGS += -DDEBUG
debug: clean all

#####                    EOF                   #####
