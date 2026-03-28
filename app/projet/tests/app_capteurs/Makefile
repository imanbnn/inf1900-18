########       AVR Project Makefile       ########
#####                                        #####
#####      INF1900 Projet final  Équipe 4148 #####
#####                                        #####
#####        Programme de test capteurs      #####
#####                                        #####
#####   Inspiré de Simon Barrette et Jerome  #####
#####   Collin pour INF1900 et de Pat Deegan #####
#####  Psychogenic Inc (www.psychogenic.com) #####
##################################################

include ../../../Makefile.commun.mk

PROJECTNAME = test_capteurs
TRG = $(PROJECTNAME).elf
HEXROMTRG = $(PROJECTNAME).hex
MAPTRG = $(PROJECTNAME).map

LIBDIR = ../../../lib
LIBNAME = inf1900
LIBFILE = $(LIBDIR)/lib$(LIBNAME).a

OBJCOPY = avr-objcopy
AVRDUDE = avrdude
AVRDUDE_PROGRAMMERID = usbasp
HEXFORMAT = ihex

INC_DIRS = -I. -I$(LIBDIR) -I/usr/include/simavr

CFLAGS = $(CFLAGS_BASE) $(INC_DIRS)
CXXFLAGS = $(CXXFLAGS_BASE)

LDFLAGS = -Wl,-Map,$(MAPTRG) -mmcu=$(MCU) -L$(LIBDIR)
LIBS = -lm -l$(LIBNAME)

CPPFILES = $(wildcard *.cpp)

OBJDEPS = $(CPPFILES:.cpp=.o)
DEPS = $(OBJDEPS:.o=.d)

.PHONY: all clean install lib

all: $(TRG) $(HEXROMTRG)

lib:
	$(MAKE) -C $(LIBDIR)

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

clean:
	$(REMOVE) $(TRG) $(MAPTRG) $(HEXROMTRG) $(OBJDEPS) $(DEPS)
