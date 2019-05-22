VERSION = 2
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION = 0

ACA_RELEASE=$(VERSION).$(PATCHLEVEL).$(SUBLEVEL).$(EXTRAVERSION)

include makefile.inc


SUBDIRS=util driver case main
APP=odbctest

all:$(APP)
$(APP):	
	@for d in $(SUBDIRS); do (cd $$d && $(MAKE)); done

clean:
	@for d in $(SUBDIRS); do (cd $$d && $(MAKE) clean); done	
	rm -rf lib/*.a

help:
	@echo "Auth Server makefile."
	@echo "version : $(ACA_RELEASE)"
	@echo
	@echo "Syntax:"
	@echo "	aca	-- make all auth source code. It is default make."
	@echo "	clean	-- make clean all auth source code."
	@echo "	install	-- make all auth source code, and then install programs to DESDIR."
	@echo "	uninstall -- uninstall programs from DESDIR."
	@echo "	help	-- print this help."

