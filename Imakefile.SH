case $CONFIG in
'')
    if test ! -f config.sh; then
	ln ../config.sh . || \
	ln ../../config.sh . || \
	ln ../../../config.sh . || \
	(echo "Can't find config.sh."; exit 1)
    fi
    . config.sh
    ;;
esac
: This forces SH files to create target in same directory as SH file.
: This is so that make depend always knows where to find SH derivatives.
case "$0" in
*/*) cd `expr X$0 : 'X\(.*\)/'` ;;
esac
echo "Extracting Imakefile (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >Imakefile <<!GROK!THIS!

##########################################################################
#  	the location of data files
LIBDIR	= 	$libdir
##########################################################################

##########################################################################
#	final location of binary (for 'make install')
BINDIR = 	$bindir
##########################################################################

##########################################################################
#  	name of program
PROGRAM	= 	$package
##########################################################################

##########################################################################
# 	for termcap functions
SYS_LIBRARIES=	$termlib
##########################################################################

##########################################################################
#	link flags
LDOPTIONS=	$ldflags
##########################################################################

##########################################################################
#	link flags
CCOPTIONS=	$cflags
##########################################################################

!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>Imakefile <<'!NO!SUBS!'

##########################################################################
#	the source files 
SRCS1	= action.c bill.c create.c data.c diag.c display.c fortune.c\
	player.c help.c io.c main.c monster.c moreobj.c movem.c object.c\
	regen.c savelev.c scores.c show.c signal.c sphere.c store.c tok.c \
	nap.c tty.c
##########################################################################

##########################################################################
#	the object files
OBJS1	= action.o bill.o create.o data.o diag.o display.o fortune.o\
	player.o help.o io.o main.o monster.o moreobj.o movem.o object.o\
	regen.o savelev.o scores.o show.o signal.o sphere.o store.o tok.o \
	nap.o tty.o
##########################################################################

##########################################################################
#	header files
HDRS	= header.h itm.h monst.h sphere.h config.h player.h
##########################################################################

##########################################################################
#	data files
LIBFILES=	Uhelp Umaps Ufortune
##########################################################################

all::	$(PROGRAM)

depend:: $(HDRS)

install:: $(PROGRAM) $(LIBFILES)
	if test ! -d $(LIBDIR) ; then mkdir $(LIBDIR) ; fi
	-cp Umaps Ufortune Uhelp $(LIBDIR)
	-chmod 644 $(LIBDIR)/Umaps $(LIBDIR)/Ufortune \
		$(LIBDIR)/Uhelp
	if test ! -d $(BINDIR) ; then mkdir $(BINDIR) ; fi
	-cp $(PROGRAM) $(BINDIR)

##########################################################################
ComplexProgramTarget_1($(PROGRAM),,)
##########################################################################

##########################################################################
#      archive the source
##########################################################################
SHAR	= shar -a
SHAR1 	= README monster.c sphere.h
SHAR2	= object.c extern.h Ularnopts
SHAR3	= Configure create.c
SHAR4	= data.c main.c help.c
SHAR5	= store.c io.c
SHAR6	= Umaps scores.c Imakefile 
SHAR7	= player.c display.c savlev.c config.h.SH
SHAR8	= movem.c moreobj.c action.c tok.c diag.c tty.c
SHAR9	= bill.c sphere.c header.h show.c Uhelp signal.c Makefile.u
SHAR10	= regen.c player.h itm.h nap.c monst.h MANIFEST Ufortune

ALLSHAR = $(SHAR1) $(SHAR2) $(SHAR3) $(SHAR4) $(SHAR5) \
	$(SHAR6) $(SHAR7) $(SHAR8) $(SHAR9) $(SHAR10)

shar:: $(ALLSHAR)
	$(SHAR) -m 1 -n 10 $(SHAR1) > part01
	$(SHAR) -m 2 -n 10 $(SHAR2) > part02
	$(SHAR) -m 3 -n 10 $(SHAR3) > part03
	$(SHAR) -m 4 -n 10 $(SHAR4) > part04
	$(SHAR) -m 5 -n 10 $(SHAR5) > part05
	$(SHAR) -m 6 -n 10 $(SHAR6) > part06
	$(SHAR) -m 7 -n 10 $(SHAR7) > part07
	$(SHAR) -m 8 -n 10 $(SHAR8) > part08
	$(SHAR) -m 9 -n 10 $(SHAR8) > part09
	$(SHAR) -m 10 -n 10 $(SHAR8) > part10

tar:: $(ALLSHAR)
	tar cvf Ularn.tar $(ALLSHAR)
!NO!SUBS!
chmod 755 Imakefile
$eunicefix Imakefile
