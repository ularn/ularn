case $CONFIG in
'')
    if test ! -f config.sh; then
  ln ../config.sh . || \
  ln ../../config.sh . || \
  ln ../../../config.sh . || \
  (echo "Can't find config.sh."; exit 1)
  echo "Using config.sh from above..."
    fi
    . ./config.sh
    ;;
esac

if test $hpux = "define"; then
   d_voidsig="define"
fi

echo "Extracting config.h (with variable substitutions)"
sed <<!GROK!THIS! >config.h -e 's!^#undef!/\*#undef!'
/* config.h
 * This file was produced by running the config.h.SH script, which
 * gets its values from config.sh, which is generally produced by
 * running Configure.
 *
 * Feel free to modify any of this as the need arises.  Note, however,
 * that running config.h.SH again will wipe out any changes you've made.
 * For a more permanent change edit config.sh and rerun config.h.SH.
 */


/* EUNICE:
 *  This symbol, if defined, indicates that the program is being compiled
 *  under the EUNICE package under VMS.  The program will need to handle
 *  things like files that don't go away the first time you unlink them,
 *  due to version numbering.  It will also need to compensate for lack
 *  of a respectable link() command.
 */
/* VMS:
 *  This symbol, if defined, indicates that the program is running under
 *  VMS.  It is currently only set in conjunction with the EUNICE symbol.
 */
#$d_eunice  EUNICE    /**/
#$d_eunice  VMS    /**/

/* CPPSTDIN:
 *  This symbol contains the first part of the string which will invoke
 *  the C preprocessor on the standard input and produce to standard
 *  output.   Typical value of "cc -E" or "/lib/cpp".
 */
/* CPPMINUS:
 *  This symbol contains the second part of the string which will invoke
 *  the C preprocessor on the standard input and produce to standard
 *  output.  This symbol will have the value "-" if CPPSTDIN needs a minus
 *  to specify standard input, otherwise the value is "".
 */
#define CPPSTDIN "$cppstdin"
#define CPPMINUS "$cppminus"

/* FTIMER:
 *  This symbol, if defined, indicates that the ftime() routine exists.
 */
#$d_ftime  FTIMER    /**/

/* GETOPT:
 *  This symbol, if defined, indicates that the getopt() routine exists.
 */
#$d_getopt  GETOPT    /**/

/* ZLIB:
 * This symbox, if defined, indicates that the zlib library exists
 */
#$d_zlib ZLIB   /**/

/* index:
 *  This preprocessor symbol is defined, along with rindex, if the system
 *  uses the strchr and strrchr routines instead.
 */
/* rindex:
 *  This preprocessor symbol is defined, along with index, if the system
 *  uses the strchr and strrchr routines instead.
 */
#$d_index  index strchr  /* cultural */
#$d_index  rindex strrchr  /*  differences? */

/* TERMIO:
 *  This symbol, if defined, indicates that the program should include
 *  termio.h rather than sgtty.h.  There are also differences in the
 *  ioctl() calls that depend on the value of this symbol.
 */
#$d_termio  TERMIO    /**/

/* VOIDSIG:
 *  This symbol is defined if this system declares "void (*signal())()" in
 *  signal.h.  The old way was to declare it as "int (*signal())()".  It
 *  is up to the package author to declare things correctly based on the
 *  symbol.
 */
#$d_voidsig  VOIDSIG   /**/

/* BINDIR:
 *     This symbol indicates where the executable binaries will reside.
 */
#define BINDIR "$bindir"          /**/

/* DRAND48:
 *  This symbol,if defined, indicates that the drand48 routines
 *  are availible.
 */
#$d_drand48     DRAND48    /**/

/* HAVETERMLIB:
 *  This symbol, when defined, indicates that termlib-style routines
 *  are available.  There is nothing to include.
 */
#$d_havetlib     HAVETERMLIB  /**/

/* ITIMER:
 *  This symbol, if defined, indicates that the setitimer() routine exists.
 */
/* SYSTIME:
 *  This symbol is the include file which contains the struct timeval and
 *  itimer definitions.
 */
#$d_itimer  ITIMER    /**/

#define  SYSTIME  $d_systime  /**/

/* RANDOM:
 *  This symbol,if defined, indicates that the random routine is availible.
 */
#$d_random  RANDOM    /**/

/* LIBDIR:
 *     This symbol indicates where the data files will reside.
 */
#define LIBDIR "$libdir"          /**/

/* LINUX 
 *   we're compiling on linux.
 */
#$linux LINUX			/**/

/* BSD
 *   we're compiling on FreeBSD or OpenBSD.
 */
#$bsd BSDCOMPILE		/**/

/* HPUX
 *   we're compiling on HPUX
 */
#$hpux HPUX 			/**/

/* CYGWIN
 *   we're compiling under the cygwin unix emulation environment thing
 *   that runs under Win98.
 */
#$cygwin CYGWIN

/* SYSERRLISTDEFINED
 *   sys_errlist is defined somewhere outside of this code, so we don't
 *   want to redefine it.
 */
#$syserr SYSERRLISTDEFINED	/**/

!GROK!THIS!
