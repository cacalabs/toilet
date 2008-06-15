# $Id$

AC_INIT(toilet, 0.1)

AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(src/main.c)
AC_CONFIG_AUX_DIR(.auto)
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(toilet, 0.1)
AM_CONFIG_HEADER(config.h)

AM_PROG_CC_C_O
AC_PROG_CPP

dnl AC_PROG_EGREP only exists in autoconf 2.54+, so we use AC_EGREP_CPP right
dnl now otherwise it might be set in an obscure if statement. Same thing for
dnl PKG_PROG_PKG_CONFIG which needs to be called first.
AC_EGREP_CPP(yes, foo)
PKG_PROG_PKG_CONFIG()

dnl Don't let pkg-config fuck our cross-compilation environment
m4_pattern_allow([^PKG_CONFIG_LIBDIR$])
if test "$build" != "$host" -a "${PKG_CONFIG_LIBDIR}" = ""; then
  export PKG_CONFIG_LIBDIR=/dev/null
fi

AC_CHECK_HEADERS(getopt.h sys/ioctl.h zlib.h)

AC_CHECK_FUNCS(getopt_long,
 [AC_DEFINE(HAVE_GETOPT_LONG, 1, Define to 1 if you have the `getopt_long' function.)],
 [AC_CHECK_LIB(gnugetopt, getopt_long,
   [AC_DEFINE(HAVE_GETOPT_LONG, 1, Define to 1 if you have the `getopt_long' function.)
    GETOPT_LIBS="${GETOPT_LIBS} -lgnugetopt"])])
AC_SUBST(GETOPT_LIBS)

AC_CHECK_LIB(z, gzopen,
 [ZLIB_LIBS="${ZLIB_LIBS} -lz"])
AC_SUBST(ZLIB_LIBS)

AC_CACHE_CHECK([for TIOCGWINSZ],
  [ac_cv_have_tiocgwinsz],
  [AC_TRY_COMPILE(
    [#include <sys/ioctl.h>],
    [struct winsize ws;
     ioctl(1, TIOCGWINSZ, &ws);],
    ac_cv_have_tiocgwinsz=yes,
    ac_cv_have_tiocgwinsz=no)])
if test "${ac_cv_have_tiocgwinsz}" = "yes"; then
  AC_DEFINE(HAVE_TIOCGWINSZ, 1, [Define if you have TIOCGWINSZ])
fi

CUCUL="no"
PKG_CHECK_MODULES(CUCUL, cucul >= 0.99.beta15,
 [CUCUL="yes"],
 [AC_MSG_RESULT(no)
  AC_MSG_ERROR([you need libcucul version 0.99.beta15 or later])])

AC_MSG_CHECKING([for release date])
stamp="$(sed -ne 's/# \$Id: .*\(....-..-..\).*/\1/p;q' configure.ac)"
 if date +"%e %B %Y" -d "$stamp" >/dev/null 2>&1; then
   ac_build_date="$(LANG=C date +"%e %B %Y" -d "$stamp" | sed 's/^ //')"
 else
   tmp="${stamp%-*}"
   ac_build_date="${stamp##*-} ${tmp#*-} ${stamp%%-*}"
 fi
AC_MSG_RESULT([$ac_build_date])
AC_DEFINE_UNQUOTED(DATE, "$ac_build_date", [Define the version date])

# Optimizations
CFLAGS="${CFLAGS} -g -O2 -fno-strength-reduce -fomit-frame-pointer"
# Code qui fait des warnings == code de porc == deux baffes dans ta gueule
CFLAGS="${CFLAGS} -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-prototypes -Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs -Wsign-compare"

AC_OUTPUT([
  Makefile
  src/Makefile
  tools/Makefile
  fonts/Makefile
  doc/Makefile
])

