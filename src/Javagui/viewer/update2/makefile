# file: viewer/update2/makefile
#
# last change: 05-2003; Thomas Behr
# very last change: 01-2005; Uwe Hartmann
#

include ../../makefile.inc


JAVAFILES =$(shell find -name "*.java")
TMPFILES = $(subst .java,.class,$(JAVAFILES))
CLASSFILES = $(subst ./,,$(TMPFILES))

.PHONY: all
all: $(CLASSFILES)
	make -C format
	make -C gui

%.class: %.java
	$(JAVAC) -Xlint:unchecked -classpath $(CLASSPATH) $<


.PHONY: clean
clean:
	make -C format clean
	make -C gui clean
	rm -f *.class
