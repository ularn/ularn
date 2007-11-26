#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(Ularn, 1.6.0, josh@bress.net)
AC_CONFIG_SRCDIR([extern.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
AC_CHECK_LIB(z, gzgets)
for termlib in curses termcap termlib; do
    AC_CHECK_LIB(${termlib}, tgetent, [TERMLIBS="-l${termlib}"; break])
done

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h sgtty.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/param.h sys/timeb.h termio.h unistd.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_GETPGRP
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SETPGRP
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([ftime strcspn getopt random drand48 setitimer])

AC_SUBST(TERMLIBS)
AC_SUBST(LIBS)
AC_SUBST(CFLAGS)
AC_SUBST(bindir)
AC_SUBST(prefix)
AC_SUBST(libdir)
AC_SUBST(exec_prefix)
AC_SUBST(datadir)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
