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

echo "Extracting Makefile.u (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >Makefile.u <<!GROK!THIS!
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
SYS_LIBRARIES=	$termlib $bsdlibs $zlib
##########################################################################

##########################################################################
#	link flags
LDFLAGS= $ldflags
##########################################################################

##########################################################################
#	compile flags
CFLAGS= $ccflags 
##########################################################################
!GROK!THIS!

if test $hpux = "define" ; then
echo "" >> Makefile.u
echo "## HPUX gets to use gcc" >> Makefile.u
echo "CC=gcc" >> Makefile.u 
echo "" >> Makefile.u
fi

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>Makefile.u <<'!NO!SUBS!'
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

all:	$(PROGRAM)

$(PROGRAM): $(OBJS1)
	$(CC) -o $@ $(LDFLAGS) $(OBJS1)  $(SYS_LIBRARIES)

.c.o:	
	$(CC) -c $(CFLAGS) $<

##########################################################################
install: $(PROGRAM) $(LIBFILES)
	if test ! -d $(LIBDIR) ; then mkdir -p $(LIBDIR) ; fi
	-cp Umaps Ufortune Uhelp $(LIBDIR)
	-chmod 644 $(LIBDIR)/Umaps $(LIBDIR)/Ufortune \
		$(LIBDIR)/Uhelp
	if test ! -d $(BINDIR) ; then mkdir -p $(BINDIR) ; fi
	-cp $(PROGRAM) $(BINDIR)


lint:
	lint $(SRCS1) $(LINTLIBS)

clean:
	rm -f $(PROGRAM) $(OBJS1) core Ularn.exe

distclean: clean
	rm -f config.h Makefile.u config.sh Makefile Imakefile
	rm -f *~ .#*

tags:	nulldep

nulldep:
	ctags -w $(SRCS1) $(HDRS)
	rm -f TAGS
	ctags -xw $(SRCS1) $(HDRS) > TAGS
##########################################################################

# DO NOT DELETE
action.o: header.h config.h extern.h
bill.o: header.h config.h 
create.o: header.h config.h itm.h extern.h
data.o: header.h config.h 
diag.o: header.h config.h 
display.o: header.h config.h player.h itm.h monst.h extern.h
fortune.o: header.h config.h extern.h
player.o: header.h config.h itm.h extern.h
help.o: header.h config.h 
io.o: header.h config.h extern.h
main.o: header.h config.h patchlevel.h
monster.o: header.h config.h itm.h player.h monst.h extern.h
moreobj.o: header.h config.h player.h itm.h monst.h extern.h
movem.o: header.h config.h player.h itm.h extern.h
object.o: header.h config.h player.h extern.h
regen.o: header.h config.h extern.h
savelev.o: header.h config.h sphere.h player.h itm.h monst.h extern.h
scores.o: header.h config.h player.h extern.h
show.o: header.h config.h 
signal.o: header.h config.h 
sphere.o: header.h config.h itm.h sphere.h extern.h
store.o: header.h config.h extern.h
tok.o: header.h config.h 
nap.o: header.h config.h 
tty.o: header.h config.h 
!NO!SUBS!
chmod 755 Makefile.u
$eunicefix Makefile.u
