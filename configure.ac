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
# Check for Flex and Bison
AC_PROG_YACC
AC_PROG_LEX
if test "x$LEX" != xflex; then
   LEX="$SHELL $missing_dir/missing flex"
   AC_SUBST([LEX_OUTPUT_ROOT], [lex.yy])
   AC_SUBST([LEXLIB], [''])
fi

# Check for MPI
ACX_MPI([], [AC_MSG_ERROR([Cannot find an MPI compiler.])])
CC="$MPICC"

# Tells automake to create a Makefile
# See https://www.gnu.org/software/automake/manual/html_node/Requirements.html
AC_CONFIG_FILES([Makefile])

# Generate the output
AC_OUTPUT
