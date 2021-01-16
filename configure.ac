 
#
#
# Copyright (c) 2021 LIPN - Universite Sorbonne Paris Nord
#                    All rights reserved.
#
# This file is part of SKaMPI.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of version 2 of the GNU General Public License as
# published by the Free Software Foundation
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */

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

# Use PAPI or not
ENABLE_PAPI=0 # default
AC_ARG_ENABLE([papi],
	AS_HELP_STRING([--enable-papi], [Use PAPI for measurements [default=no]]),
	[ AM_CONDITIONAL(ENABLE_PAPI, true)
       	  CFLAGS+=" -DSKAMPI_USE_PAPI "],
	[ AM_CONDITIONAL(ENABLE_PAPI, false) ] )
AC_ARG_WITH(
	[papi],
	AS_HELP_STRING(
		[--with-papi=FULLPATH],
        	[set full path to your PAPI installation]
    		),
    	[PAPIPATH="$withval"]
    	)
if test -n "$PAPIPATH"; then
       AC_MSG_CHECKING([user-supplied PAPI installation])
       CFLAGS="-I${PAPIPATH}/include ${CFLAGS}"
       LDFLAGS="-L${PAPIPATH}/lib ${LDFLAGS}"
fi

# Tells automake to create a Makefile
AC_CONFIG_FILES([Makefile])

# Generate the output
AC_OUTPUT
