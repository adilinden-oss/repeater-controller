#
# Makefile for repeater-controller
#
# Copyright (c) 2013, Adi Linden <adi@adis.ca>
# All rights reserved.
#

all install uninstall clean:
	$(MAKE) --directory=cwid $@
	$(MAKE) --directory=repeater $@

