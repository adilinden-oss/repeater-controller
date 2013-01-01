#
# Common includes for our project
#
# Copyright (c) 2013, Adi Linden <adi@adis.ca>
# All rights reserved.
#

# The absolute path to the installation directory
PREFIX      = /home/controller

# User we run as
EXECUSER    = controller

SHELL       = /bin/sh
BINDIR      = $(PREFIX)/bin
SCRIPTDIR   = $(PREFIX)/script

CC          = gcc
MKDIR       = /bin/mkdir -p
INSTALL     = /usr/bin/install
RM          = /bin/rm -f
LN          = /bin/ln

LIBS        =
INCLUDES    =
CFLAGS      = -O2 -Wall
LDFLAGS     =

COMPILE     = $(CC) $(INCLUDES) $(CFLAGS)
LINK        = $(CC) $(LDFLAGS) -o $@
REPLACE     = sed -e 's;@@USER@@;$(EXECUSER);g' \
                  -e 's;@@BIN@@;$(BINDIR);g' \
                  -e 's;@@SCRIPT@@;$(SCRIPTDIR);g'

