#
# Makefile for repeater
#
# Copyright (c) 2013, Adi Linden <adi@adis.ca>
# All rights reserved.
#

# Source the common variables
include ../Common.mk

#CFLAGS      += -g
LDFLAGS     += -lm

PROGRAMS    = repeater portctl
SCRIPTS     = repeater_cronjob repeater_crontab repeater_init courtesy ider

# Objects portctl
lib_obj     = portctl_lib.o irlpdev.o
repeat_obj  = $(lib_obj) repeater.o
portctl_obj = $(lib_obj) portctl.o

# Build rules
all:        $(PROGRAMS)

repeater:   $(repeat_obj)
	$(LINK) $(repeat_obj)

portctl:    $(portctl_obj)
	$(LINK) $(portctl_obj)

# Source the common install scripts
include ../Install.mk

# Manipulate the sources
clean:
	$(RM) *.o *.core core $(PROGRAMS)
