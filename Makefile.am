# SKaMPI   MPI-Benchmark

# Copyright 2021 Camille Coti,
# Laboratoire d'Informatique de Paris Nord
# Universite Sorbonne Paris Nord.

# This program is free software; you can redistribute it and/or modify
# it under the terms of version 2 of the GNU General Public License as
# published by the Free Software Foundation

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 

subdirs = src 

ACLOCAL_AMFLAGS=-I m4 
AM_YFLAGS = -d
AM_CFLAGS = -I $(srcdir)/src -I src

bin_PROGRAMS = skampi
BUILT_SOURCES = lex.yy.c src/parser.c src/parser.h m.c
CLEANFILES = lex.yy.c src/parser.c src/parser.h m.c

MEASUREMENTS = src/measurements/helpers.c

MPI_MEASUREMENTS = \
		src/measurements/bug.c       src/measurements/coll.c     src/measurements/datatypes.c \
		src/measurements/delayed.c   src/measurements/demo.c                              \
		src/measurements/io.c        src/measurements/localio.c  src/measurements/mmisc.c     \
		src/measurements/node2node.c src/measurements/onesided.c src/measurements/pt2pt.c

OPENSHMEM_MEASUREMENTS = \
		src/measurements/openshmem.c        src/measurements/openshmem_atomic.c   \
		src/measurements/openshmem_ctx.c    src/measurements/openshmem_p2p.c      \
		src/measurements/openshmem_coll.c   src/measurements/openshmem_locks.c 

if ENABLE_MPI
MEASUREMENTS += $(MPI_MEASUREMENTS)
endif

if ENABLE_OPENSHMEM
MEASUREMENTS += $(OPENSHMEM_MEASUREMENTS)
endif

skampi_SOURCES = \
	src/accounting.c src/autorefine.c src/compile.c src/debug.c src/exec.c src/main.c src/measurement.c src/mem.c src/misc.c src/output.c src/print.c src/symboltable.c src/synchronize.c \
	m.c lex.yy.c \
	src/parser.y src/lexer.lex \
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
if ENABLE_MPI
EXCLUDE_M = --excl="$(OPENSHMEM_MEASUREMENTS)"
else
EXCLUDE_M = --excl="$(OPENSHMEM_MEASUREMENTS) $(MPI_MEASUREMENTS)"
endif
endif

$(top_builddir)/m.c: $(MEASUREMENTS) $(srcdir)/src/extract.pl
	$(srcdir)/src/extract.pl --mpicc="$(THECC)" --cflags="$(CFLAGS) -E" --dir=$(srcdir)/src/measurements $(EXCLUDE_M) > m.c

lex.yy.c: $(srcdir)/src/lexer.lex
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -o $@ $<
