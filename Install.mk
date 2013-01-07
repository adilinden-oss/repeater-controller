#
# Makefile for repeater
# Written by Adi Linden
#

# Installation rules
install:    all install-bin install-scripts

install-bin:
	@echo "Installing binaries in $(BINDIR) ..."
	@[ -d $(BINDIR) ] || $(MKDIR) $(BINDIR) ;\
    for file in $(PROGRAMS); do \
        echo "$(INSTALL) -m 755 $${file} $(BINDIR)/$${file}"; \
        $(INSTALL) -m 755 $${file} $(BINDIR)/$${file}; \
    done; \
    echo "Setting ownership and permissions ..."; \
    for file in $(PROGRAMS); do \
        echo "chown $(USER).$(USER) $(BINDIR)/$${file}"; \
        chown $(USER).$(USER) $(BINDIR)/$${file}; \
    done; \

install-scripts:
	@echo "Installing scripts in $(SCRIPTDIR) ..."
	[ -d $(SCRIPTDIR) ] || $(MKDIR) $(SCRIPTDIR) ;\
    for file in $(SCRIPTS); do \
        $(REPLACE) < $${file}.in > $(SCRIPTDIR)/$${file}; \
        echo "chown $(USER).$(USER) $(SCRIPTDIR)/$${file}"; \
        chown $(USER).$(USER) $(SCRIPTDIR)/$${file}; \
        echo "chmod 755 $(SCRIPTDIR)/$${file}"; \
        chmod 755 $(SCRIPTDIR)/$${file}; \
    done

# Uninstallation rules
uninstall:  uninstall-bin uninstall-scripts

uninstall-bin:
	@echo "Uninstalling binaries in $(BINDIR) ..."
	@for file in $(PROGRAMS); do \
        echo "$(RM) $(BINDIR)/$$file"; \
        $(RM) $(BINDIR)/$$file; \
    done

uninstall-scripts:
	@echo "Uninstalling scripts in $(SCRIPTDIR) ..."
	@for file in $(SCRIPTS); do \
        echo "$(RM) $(SCRIPTDIR)/$$file"; \
        $(RM) $(SCRIPTDIR)/$$file; \
    done

