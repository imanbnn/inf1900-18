########       AVR Project Makefile       ########
#####                                        #####
#####      INF1900  Projet Final 4148       #####
#####                                        #####
#####            Application robot           #####
#####                                        #####
#####   Inspire de Simon Barrette et Jerome  #####
#####   Collin pour INF1900 et de Pat Deegan #####
#####  Psychogenic Inc (www.psychogenic.com) #####
##################################################

# Ce Makefile compile un executable .elf puis .hex pour AVR.
# L'executable est lie avec la bibliotheque statique dans ../../lib.

include ../../Makefile.commun.mk

PROJECTNAME = projet
TRG = $(PROJECTNAME).elf
HEXROMTRG = $(PROJECTNAME).hex
MAPTRG = $(PROJECTNAME).map

# Dossier de la bibliotheque
LIBDIR = ../../lib
LIBNAME = inf1900
LIBFILE = $(LIBDIR)/lib$(LIBNAME).a

# Outils utilises
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# Identifiant du programmateur
AVRDUDE_PROGRAMMERID = usbasp

# Format du fichier hex
HEXFORMAT = ihex

# Inclusions pour le code d'application
INC_DIRS = -I. -I$(LIBDIR) -I/usr/include/simavr

CFLAGS = $(CFLAGS_BASE) $(INC_DIRS)
CXXFLAGS = $(CXXFLAGS_BASE)

# Compilation en mode debug (activer via `make debug`)
ifdef DEBUG
CFLAGS += -DDEBUG
CXXFLAGS += -DDEBUG
endif

# Edition de liens
LDFLAGS = -Wl,-Map,$(MAPTRG) -mmcu=$(MCU) -L$(LIBDIR)
LIBS = -lm -l$(LIBNAME)

# Sources du repertoire courant
CPPFILES = $(wildcard *.cpp)

# Fichiers objets et dependances
OBJDEPS = $(CPPFILES:.cpp=.o)
DEPS = $(OBJDEPS:.o=.d)

.PHONY: all clean install lib cleanall debug

all: $(TRG) $(HEXROMTRG)

lib:
	$(MAKE) -C $(LIBDIR) $(if $(DEBUG),debug,all)

$(TRG): lib $(OBJDEPS)
	$(CC) $(LDFLAGS) -o $(TRG) $(OBJDEPS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O $(HEXFORMAT) $< $@

install: $(HEXROMTRG)
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) \
		-p $(MCU) -P usb -e \
		-U flash:w:$(HEXROMTRG)

debug:
	$(MAKE) DEBUG=1 clean all

clean:
	$(REMOVE) $(TRG) $(MAPTRG) $(HEXROMTRG) $(OBJDEPS) $(DEPS)

cleanall:
	$(MAKE) -C $(LIBDIR) clean
	$(MAKE) clean

#####                    EOF                   #####
