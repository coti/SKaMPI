dnl @synopsis ACX_OPENSHMEM([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
dnl @summary figure out how to compile/link code with OpenSHMEM
dnl @category InstalledPackages
dnl
dnl Mostly inspired by ACX_MPI
dnl @author Camille Coti <coti@lipn.fr>

AC_DEFUN([ACX_OPENSHMEM], [
AC_PREREQ(2.50) dnl for AC_LANG_CASE not sure)
AC_LANG_CASE([C], [
		  AC_REQUIRE([AC_PROG_CC])
		  AC_ARG_VAR(OSHCC,[OpenSHMEM C compiler command])
		  AC_CHECK_PROGS(OSHCC, oshcc mpicc, $CC) dnl TODO others?
		  acx_openshmem_save_CC="$CC"
		  CC="$OSHCC"
		  AC_SUBST(OSHCC)
],
[C++], [
       AC_REQUIRE([AC_PROG_CXX])
       AC_ARG_VAR(CXX,[OpenSHMEM C++ compiler command])
       AC_CHECK_PROGS(OSCXX, oshc++ oshCC oshcxx mpic++ mpiCC mpicxx mpCC, $CXX) dnl TODO others?
       acx_openshmem_save_CXX="$CXX"
       CXX="$OSHCXX"
       AC_SUBST(OSHCXX)
],
[F77], [
       AC_REQUIRE([AC_PROG_FC])
       AC_ARG_VAR(FC,[OpenSHMEM Fortran compiler command])
       AC_CHECK_PROGS(OSFORT, oshfort mpifort, $FC) dnl TODO others?
       acx_openshmem_save_FC="$FC"
       FC="$OSHFORT"
       AC_SUBST(OSHFORT)
])
if test x = x"$OPENSHMEMLIBS"; then
   AC_LANG_CASE([C], [AC_CHECK_FUNC(shmem_init, [SHMEMLIBS=" "])],
   		     [C++], [AC_CHECK_FUNC(shmem_init, [SHMEMLIBS=" "])],
		     	    [Fortran 77], [AC_MSG_CHECKING([for shmem_init])
			    	     	  AC_TRY_LINK([],[      call shmem_init], [SHMEMLIBS=" "
					  			     		AC_MSG_RESULT(yes)], [AC_MSG_RESULT(no)])])
fi
if test x = x"OPENSHMEMLIBS"; then
   AC_CHECK_LIB(shmem, shmem_init, [SHMEMLIBS="-lshmem"])
fi
#if test x = x"$MPILIBS"; then
#   AC_CHECK_LIB(mpich, MPI_Init, [MPILIBS="-lmpich"])
#fi
dnl We have to use AC_TRY_COMPILE and not AC_CHECK_HEADER because the
dnl latter uses $CPP, not $CC (which may be mpicc).
AC_LANG_CASE([C], [if test x != x"$OPENSHMEMLIBS"; then
		  AC_MSG_CHECKING([for shmem.h])
		  AC_TRY_COMPILE([#include <shmem.h>],[],[AC_MSG_RESULT(yes)], [OPENSHMEMLIBS=""
		  			   AC_MSG_RESULT(no)])
fi],
[C++], [if test x != x"OPENSHMEMLIBS"; then
       AC_MSG_CHECKING([for shmem.h])
       AC_TRY_COMPILE([#include <shmem.h>],[],[AC_MSG_RESULT(yes)], [MPILIBS=""
       				AC_MSG_RESULT(no)])
fi])
#AC_LANG_CASE([C], [CC="$acx_mpi_save_CC"],
#		   [C++], [CXX="$acx_mpi_save_CXX"])
AC_SUBST(OPENSHMEMLIBS)
# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x = x"OPENSHMEMLIBS"; then
        $2
        :
else
        ifelse([$1],,[AC_DEFINE(HAVE_OPENSHMEM,1,[Define if you have the OpenSHMEM library.])],[$1])
        :
fi
])dnl ACX_OPENSHMEM
