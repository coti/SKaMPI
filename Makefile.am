bin_PROGRAMS = skampi
ACLOCAL_AMFLAGS=-I config 

#BUILT_SOURCES = lexer.c parser.c parser.h m.c
#CLEANFILES = lexer.c parser.c parser.h m.c

#SUBDIRS = measurements

MEASUREMENTS = \
	measurements/bug.c measurements/coll.c measurements/datatypes.c measurements/delayed.c measurements/demo.c measurements/helpers.c measurements/io.c \
	measurements/localio.c measurements/mmisc.c measurements/node2node.c measurements/onesided.c measurements/pt2pt.c \
	measurements/openshmem.c
#MEASUREMENTS = \
#	bug.c coll.c datatypes.c delayed.c demo.c helpers.c io.c \
#	localio.c mmisc.c node2node.c onesided.c pt2pt.c \
#	openshmem.c

skampi_SOURCES = \
	accounting.c autorefine.c compile.c debug.c exec.c main.c measurement.c mem.c misc.c output.c print.c symboltable.c synchronize.c \
	m.c \
	parser.y lexer.lex \
	$(MEASUREMENTS)

#lexer.c parser.c 

skampi_LDADD = -lm @LEXLIB@

m.c: $(MEASUREMENTS) extract.pl
	./extract.pl --mpicc="$(MPICC)" --cflags="$(CFLAGS) -E" --dir=measurements > m.c

