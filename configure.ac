
AC_INIT(toilet, 0.3)

AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(src/main.c)
AC_CONFIG_AUX_DIR(.auto)
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(toilet, 0.3)
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

AC_CHECK_HEADERS(sys/ioctl.h)

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

PKG_CHECK_MODULES(CACA, caca >= 0.99.beta18,
 [:],
 [AC_MSG_RESULT(no)
  AC_MSG_ERROR([you need libcaca version 0.99.beta18 or later])])

AC_MSG_CHECKING([for release date])
tmp="${stamp%-*}"
ac_build_date="${stamp##*-} ${tmp#*-} ${stamp%%-*}"
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
  test/Makefile
])

