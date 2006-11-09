/*	header.h */
#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/times.h>

#ifdef FTIMER
#  include <sys/timeb.h>
#endif /* FTIMER */

#ifdef ITIMER
#  include SYSTIME
#endif /* ITIMER */

#ifdef TERMIO
#  include <termio.h>
#else
#  include <sys/ioctl.h>
#  include <sys/file.h>
#  include <sgtty.h>
#endif /* TERMIO */

#include <ctype.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#ifdef USG
#  include <strings.h>
#else
#  include <string.h>
#endif

#ifdef VOIDSIG
#  define SIGTYPE void
#else
#  define SIGTYPE int
#endif

#ifndef MAXPATHLEN
#  define MAXPATHLEN 1024
#endif

	/**************** File Names ****************/

#define SCORENAME   "Uscore"
#define HELPNAME    "Uhelp"
#define LEVELSNAME  "Umaps"
#define FORTSNAME   "Ufortune"

	/*************** Defines for max values *************/

#define MAXLEVEL 16 	/*	max # levels in the dungeon + 1	*/
#define MAXVLEVEL 5 	/*	max # of levels in the volcano */
#define NLEVELS (MAXLEVEL+MAXVLEVEL)
#define DBOTTOM (MAXLEVEL-1)
#define VBOTTOM (MAXLEVEL+MAXVLEVEL-1)

#define MAXX 67
#define MAXY 17

#define MAXPLEVEL 100 	/* maximum player level allowed		*/
#define MAXSCROLL 28    /* maximum number of scrolls that are possible	*/
#define MAXPOTION 35 	/* maximum number of potions that are possible	*/
#define MAXOBJ    93 	/* maximum number of objects   n < MAXOBJ */
#define MAXMONST 57 	/* maximum # monsters in the dungeon	*/
	/* 56 monsters, 7 demon lords, 1 demon prince, 1 Lucifer */
#define	IVENSIZE  26	/* max size of inventory */
#define SCORESIZE 25 	/* max number of people on a scoreboard max */
#define DNDSIZE	  92	/* max # items in the dnd store */
#define SPNUM     39 	/* maximum number of spells in existance	*/
#define TIMELIMIT 40000 /* maximum number moves before the game is called*/

	/* create a checkpoint file every CKCOUNT moves */
#define CKCOUNT 400	

#define TAXRATE (0.05) 	/* the tax rate for the LRS = 5%  */

#define LOGNAMESIZE 40	/* max size of the players name */

#define	ESC	'\033'

	/* used by some macros below and in io.c */
#define ST_START 1
#define ST_END   2
#define BOLD     3
#define END_BOLD 4
#define CLEAR    5
#define CL_LINE  6
#define CL_DOWN 14
#define CURSOR  15

/*
 *	------------------- macros --------------------
 */

#define lgetc() ( (ipoint != iepoint) ? inbuffer[ipoint++] : \
			(iepoint != BUFSIZ) ? 0 : lgetc1() )

#define bottomline() 	do { recalc(); bot1f=1; } while (0)
#define bottomhp() 	do { bot2f=1; } while (0)
#define bottomspell() 	do { bot3f=1; } while (0)

/* cursor(x,y)	  Put cursor at specified coordinates staring at [1,1] */
#define cursor(x,y) 			\
	do { if (lpnt >= lpend)		\
		lflush();		\
		*lpnt++ = CURSOR;	\
		*lpnt++ = (x);		\
		*lpnt++ = (y);		\
	} while (0)

	/* turn on bold display for the terminal */
#define setbold() do {if (boldon) *lpnt++ = ST_START;} while (0)

	/* turn off bold display for the terminal */
#define resetbold() do {if (boldon) *lpnt++ = ST_END;} while (0)

	/* clear the screen and home the cursor */
#define clear() do { *lpnt++ =CLEAR; cbak[SPELLS]= -50; } while (0)

	/* clear to end of line */
#define cltoeoln() do { *lpnt++ = CL_LINE; } while (0)

/* cl_line(x,y)  Clear the whole line indicated by 'y', leave cursor at [x,y] */
#define cl_line(x,y) do { cursor(1,(y)); *lpnt++ = CL_LINE; cursor((x),(y)); } while (0)

	/* output one byte to the output buffer */
#define lprc(ch) {(lpnt>=lpend) ? (*lpnt++ = (ch), lflush()) : (*lpnt++ = (ch));}

/* beep()	emit a beep if enabled (see no-beep in .Ularnopts) */
#define beep() do {if (!nobeep) *lpnt++ = '\07';} while (0)

/* setup the scrolling region for the terminal */
#define setscroll() do { enable_scroll=1; } while (0)

/* clear the scrolling region for the terminal */
#define resetscroll() do { enable_scroll=0; } while (0)

/* position cursor at beginning of 24th line */
#define cursors()	do { cursor(1,24); } while (0)

/* create scroll #'s with probability of occurrence */
#define newscroll() (scprob[rund(82)])

/* return a potion # created with probability of occurrence */
#define newpotion() (potprob[rund(42)])

/* return the + points on created leather armor */
#define newleather() (nlpts[rund(c[HARDGAME]?10:13)])

/* return the + points on chain armor */
#define newchain() (nch[rund(11)])

/* return + points on plate armor */
#define newplate() (nplt[rund(c[HARDGAME]?3:11)])

/* return + points on new daggers */
#define newdagger() (ndgg[rund(13)])

/* return + points on new swords */
#define newsword() (nsw[rund(c[HARDGAME]?6:14)])

/* destroy object at present location */
#define forget()                                              \
    do {                                                      \
        item[playerx][playery] = know[playerx][playery] = 0;  \
    } while (0)

/* wipe out a monster at a location */
#define disappear(x,y)                     \
    do {                                   \
        mitem[x][y].mon = know[x][y] = 0;  \
    } while (0)

/* verify coordinates */
#define vxy(x,y)                   \
    do {                           \
	    if(x < 0) x = 0;           \
		if(y < 0) y = 0;           \
		if(x >= MAXX) x = MAXX-1;  \
		if(y >= MAXY) y = MAXY-1;  \
    } while (0)

#ifdef DRAND48
# define srand srand48
# define rand lrand48
#else
# ifdef RANDOM
#   define srand srandom
#   define rand random
# endif
#endif /* RANDOM */

#define rnd(x)  ((int)(rand() % (x)) + 1)
#define rund(x) ((int)(rand() % (x)))
#define rndl(x)  ((long)(rand() % (x)) + 1)
#define rundl(x) ((long)(rand() % (x)))

	/* macros for miscellaneous data conversion */
#define min(x,y) ((int)((x)>(y))?(y):(x))
#define max(x,y) ((int)((x)>(y))?(x):(y))
