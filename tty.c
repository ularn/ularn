/* tty.c */
#include "header.h"
#include "player.h"
#include "extern.h"

#ifndef HAVE_TERMIO_H /* sgtty stuff */
#    define ECHOMASK		ECHO
#    define termstruct		sgttyb
#    define kill_sym		sg_kill
#    define erase_sym		sg_erase
#    define intr_sym		t_intrc
#    define EXTABS		XTABS
#    define tabflgs		sg_flags
#    define echoflgs		sg_flags
#    define cbrkflgs		sg_flags
#    define inputflags		sg_flags /* don't know how enabling meta bits */
#    define CBRKMASK		CBREAK
#    define STRIPHI		0	/* should actually be done on BSD */
#    define O_SPEED(x)		(x).sg_ospeed
#    define GTTY(x)		gtty(0, x)
#    define STTY(x)		stty(0, x)
#    define GTTY2(x)		(ioctl(0, TIOCGETC, (char *)x)) /* for tchars */
#    define STTY2(x)		(ioctl(0, TIOCSETC, (char *)x)) /* for tchars */
#    define vmin		t_eofc
#    define vtime		t_brkc
struct tchars inittyb2, curttyb2;
#else
#    define ECHOMASK		(ECHO|ECHOE|ECHOK|ECHONL)
#    define termstruct		termio
#    define kill_sym		c_cc[VKILL]
#    define erase_sym		c_cc[VERASE]
#    define intr_sym		c_cc[VINTR]
#    define EXTABS		TAB3
#    define tabflgs		c_oflag
#    define echoflgs		c_lflag
#    define cbrkflgs		c_lflag
#    define inputflags		c_iflag
#    define CBRKMASK		ICANON
#    define O_SPEED(x)		((x).c_cflag & CBAUD)
#    define STRIPHI		ISTRIP
#    define GTTY(x)		(ioctl(0, TCGETA, (char *)x))
#    define STTY(x)		(ioctl(0, TCSETAW, (char *)x))
#    define GTTY2(x)		(1)
#    define STTY2(x)		(1)
#    define vmin		c_cc[VMIN]
#    define vtime	    	c_cc[VTIME]
#    define inittyb2		inittyb
#    define curttyb2		curttyb
#endif /* HAVE_TERMIO_H */

extern short ospeed;

static struct termstruct inittyb, curttyb;

/*
 *	set current tty
 */
setctty()
{
	if (STTY(&curttyb) == -1 )
		perror("setctty");
	if (STTY2(&curttyb2) == -1)
		perror("setctty 2");
}

/*
 * Get initial state of terminal, set ospeed (for termcap routines)
 * and switch off tab expansion if necessary.
 * Called by startup() in termcap.c and after returning from ! or ^Z
 */
gettty()
{
	if (GTTY(&inittyb) == -1) 
		perror("gettty");
	if (GTTY2(&inittyb2) == -1)
		perror("gettty 2");

	curttyb = inittyb;
	ospeed = O_SPEED(inittyb);

	setuptty();

	/* do not expand tabs */
	if(curttyb.tabflgs & EXTABS) {
		curttyb.tabflgs &= ~EXTABS;
		setctty();
	}
}

/* reset terminal to original state */
settty()
{
	if (STTY(&inittyb) == -1)
		perror("settty");
	if (STTY2(&inittyb2) == -1)
		perror("settty 2");

	curttyb.inputflags |= STRIPHI;
}

setuptty()
{
	curttyb.echoflgs &= ~ECHOMASK;
#ifdef HAVE_TERMIO_H
	curttyb.cbrkflgs &= ~CBRKMASK;
#else
	curttyb.cbrkflgs |= CBRKMASK;
#endif
	/* be satisfied with one character; no timeout */
	curttyb2.vmin = 1;
	curttyb2.vtime = 0;
	curttyb.inputflags &= ~STRIPHI;
	setctty();
}

/*
 *	scbr()		Function to set cbreak -echo for the terminal
 *
 *	like: system("stty cbreak -echo")
 */
scbr ()
{
	curttyb.cbrkflgs &= ~ECHOMASK;
#ifdef HAVE_TERMIO_H
	curttyb.cbrkflgs &= ~CBRKMASK;
#else
	curttyb.cbrkflgs |= CBRKMASK;
#endif
	setctty();
}

/*
 *	sncbr()		Function to set -cbreak echo for the terminal
 *
 */
sncbr ()
{
	curttyb.cbrkflgs |= ECHOMASK;
#ifdef HAVE_TERMIO_H
 	curttyb.cbrkflgs |= CBRKMASK;
#else
 	curttyb.cbrkflgs &= ~CBRKMASK;
#endif
	setctty();
}

/*
 *	setupvt100() 		
 *		Subroutine to set up terminal in correct mode for game
 *	Attributes off, clear screen, set scrolling region, set tty mode 
 */
setupvt100 ()
{
	clear();  
	setscroll();  
	gettty();
}

/*
 *	clearvt100() 	 	
 *		Subroutine to clean up terminal when the game is over
 *	Attributes off, clear screen, unset scrolling region, restore tty mode 
 */
clearvt100 ()
{
	resetscroll();  
	settty();
}
