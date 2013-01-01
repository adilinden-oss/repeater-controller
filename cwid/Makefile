#
# Makefile for cwid
#
# Copyright (c) 2013, Adi Linden <adi@adis.ca>
# All rights reserved.
#

# Source the common variables
include ../Common.mk

#CFLAGS      += -g
LDFLAGS     += -lm -lasound

PROGRAMS    = cw tones test
SCRIPTS     = 

# Objects
lib_obj     = wave.o stdout.o dsp.o alsa.o sound.o
cw_obj      = $(lib_obj) cw.o
tones_obj   = $(lib_obj) tones.o
test_obj    = $(lib_obj) test.o

# Build rules
all:        $(PROGRAMS)

cw:         $(cw_obj)
	$(LINK) $(cw_obj)

tones:      $(tones_obj)
	$(LINK) $(tones_obj)

test:       $(test_obj)
	$(LINK) $(test_obj)

# Source the common install scripts
include ../Install.mk

# Manipulate the sources
clean:
	$(RM) *.o *.core core $(PROGRAMS)

