bin_PROGRAMS = skampi
ACLOCAL_AMFLAGS=-I config 
AM_YFLAGS = -d

BUILT_SOURCES = lex.yy.c parser.c parser.h m.c
CLEANFILES = lex.yy.c parser.c parser.h m.c

MEASUREMENTS = \
	measurements/bug.c measurements/coll.c measurements/datatypes.c measurements/delayed.c measurements/demo.c measurements/helpers.c measurements/io.c \
	measurements/localio.c measurements/mmisc.c measurements/node2node.c measurements/onesided.c measurements/pt2pt.c

if ENABLE_OPENSHMEM
MEASUREMENTS += measurements/openshmem.c        measurements/openshmem_atomic.c  \
		measurements/openshmem_ctx.c    measurements/openshmem_p2p.c \
		measurements/openshmem_coll.c   measurements/openshmem_locks.c \
		measurements/higher_level.c
endif

skampi_SOURCES = \
	accounting.c autorefine.c compile.c debug.c exec.c main.c measurement.c mem.c misc.c output.c print.c symboltable.c synchronize.c \
	m.c lex.yy.c \
	parser.y lexer.lex \
	$(MEASUREMENTS)

skampi_LDADD = -lm @LEXLIB@ 

if ENABLE_OPENSHMEM
skampi_LDADD += -loshmem
endif

if ENABLE_PAPI
skampi_LDADD += -lpapi
endif


m.c: $(MEASUREMENTS) extract.pl
	./extract.pl --mpicc="$(MPICC)" --cflags="$(CFLAGS) -E" --dir=measurements --excl=higher_level.c > m.c

lex.yy.c: lexer.lex
	flex  lexer.lex
