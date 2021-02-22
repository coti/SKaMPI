bin_PROGRAMS = skampi
ACLOCAL_AMFLAGS=-I config 
AM_YFLAGS = -d

BUILT_SOURCES = lex.yy.c parser.c parser.h m.c
CLEANFILES = lex.yy.c parser.c parser.h m.c

MEASUREMENTS = 

MPI_MEASUREMENTS = \
		measurements/bug.c measurements/coll.c measurements/datatypes.c    \
		measurements/delayed.c measurements/demo.c measurements/helpers.c  \
		measurements/io.c measurements/localio.c measurements/mmisc.c      \
		measurements/node2node.c measurements/onesided.c measurements/pt2pt.c


OPENSHMEM_MEASUREMENTS = \
		measurements/openshmem.c        measurements/openshmem_atomic.c   \
		measurements/openshmem_ctx.c    measurements/openshmem_p2p.c      \
		measurements/openshmem_coll.c   measurements/openshmem_locks.c 

if ENABLE_MPI
MEASUREMENTS += $(MPI_MEASUREMENTS)
endif

if ENABLE_OPENSHMEM
MEASUREMENTS += $(OPENSHMEM_MEASUREMENTS)
endif

skampi_SOURCES = \
	accounting.c autorefine.c compile.c debug.c exec.c main.c measurement.c mem.c misc.c output.c print.c symboltable.c synchronize.c \
	m.c lex.yy.c \
	parser.y lexer.lex \
	$(MEASUREMENTS)

skampi_LDADD = -lm @LEXLIB@ 

if ENABLE_MPI
THECC = $(MPICC)
else
if ENABLE_OPENSHMEM
THECC = $(OSHCC)
endif
endif

if ENABLE_MPI
if ENABLE_OPENSHMEM
skampi_LDADD += -loshmem
endif
endif

if ENABLE_PAPI
skampi_LDADD += -lpapi
endif

# This is not exactly elegant
if ENABLE_OPENSHMEM
if ENABLE_MPI
# I don't want to exclude anything at all
EXCLUDE_M =
else
EXCLUDE_M = --excl="$(MPI_MEASUREMENTS)"
endif
else
if !ENABLE_MPI
EXCLUDE_M = "--excl=$(OPENSHMEM_MEASUREMENTS)"
else
EXCLUDE_M = "--excl=$(OPENSHMEM_MEASUREMENTS) $(MPI_MEASUREMENTS)"
endif
endif

$(top_builddir)/m.c: $(MEASUREMENTS) $(srcdir)/extract.pl
	$(srcdir)/extract.pl --mpicc="$(THECC)" --cflags="$(CFLAGS) -E" --dir=$(srcdir)/measurements $(EXCLUDE_M) > m.c

lex.yy.c: $(srcdir)/lexer.lex
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -o $@ $<
