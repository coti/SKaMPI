AC_INIT([skampi], [6.0])

# Safety checks in case user overwritten --srcdir
AC_CONFIG_SRCDIR([main.c])

# Store the auxiliary build tools (e.g., install-sh, config.sub, config.guess)
# in this dir (build-aux)
AC_CONFIG_AUX_DIR([build-aux])

# Init automake, and specify this program use relaxed structures.
# i.e. this program doesn't follow the gnu coding standards, and doesn't have
# ChangeLog, COPYING, AUTHORS, INSTALL, README etc. files.
#AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_INIT_AUTOMAKE([subdir-objects])

# The macros are here
AC_CONFIG_MACRO_DIR([m4])

# Check for C compiler
AC_PROG_CC
AM_PROG_CC_C_O
# Check for Flex and Bison
AC_PROG_YACC
AC_PROG_LEX

# Check for MPI
ACX_MPI([], [AC_MSG_ERROR([Cannot find an MPI compiler.])])
CC="$MPICC"

# Enable OpenSHMEM or not
ENABLE_OPENSHMEM=0 # default
AC_ARG_ENABLE([openshmem],
	AS_HELP_STRING([--enable-openshmem], [Enable OpenSHMEM operations [default=no]]),
	[ AM_CONDITIONAL(ENABLE_OPENSHMEM, true)
       	  CFLAGS+=" -DSKAMPI_OPENSHMEM "],
	[ AM_CONDITIONAL(ENABLE_OPENSHMEM, false) ] )

# use PAPI or not
ENABLE_PAPI=0 # default
AC_ARG_ENABLE([papi],
	AS_HELP_STRING([--enable-papi], [Use PAPI for measurements [default=no]]),
	[ AM_CONDITIONAL(ENABLE_PAPI, true)
       	  CFLAGS+=" -DSKAMPI_USE_PAPI "],
	[ AM_CONDITIONAL(ENABLE_PAPI, false) ] )

# Tells automake to create a Makefile
AC_CONFIG_FILES([Makefile])

# Generate the output
AC_OUTPUT
