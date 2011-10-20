/*	main.c		*/
/* This game is bad for you. It is evil. It will rot your brain. */
#include "header.h"
#include "player.h"
#include "itm.h"
#include "extern.h"

static char copyright[]=
"\nUlarn created by Phil Cordier -- based on Larn by Noah Morgan\n  Updated by Josh Brandt and David Richerby\n";

char  srcount=0;	/* line counter for showstr()	*/
char rmst=80;	/* random monster creation counter		*/
int userid;	/* the players login user id number */

char dropflag=0; 	/* if 1 then don't lookforobject() next round */
char restorflag=0;		/* 1 means restore has been done	*/
char nowelcome=0, nomove=0; 	/* if nomove no count next iteration as move */

static short viewflag=0;
/*	if viewflag then we have done a 99 stay here and
	don't showcell in the main loop */

static char cmdhelp[] = "\
Cmd line format: Ularn [-sicnh] [-o <optsfile>] [-d #] [-r]\n\
  -s   show the scoreboard\n\
  -i   show scoreboard with inventories\n\
  -c   create new scoreboard (wizard only)\n\
  -n   suppress welcome message on starting game\n\
  -h   print this help text\n\
  -o <optsfile> specify .Ularnopts file to be used instead of \"~/.Ularnopts\"\n\
  -d # specify level of difficulty (example: Ularn -d 5)\n\
  -r   restore checkpoint (.ckp) file\n";

static char *optstring = "sicnhro:d:";

/*
	************
	MAIN PROGRAM
	************
 */

main (argc, argv)
int argc;
char *argv[];
{
	int i, hard;
	char buf[BUFSIZ];
	char *getenv(), *getlogin(), *ptr=0;
	struct passwd *pwe,*getpwuid();
	extern char *optarg;
	extern int optind, opterr;


/*
 *	first task is to identify the player
 */
	if ((ptr = getlogin()) == (char *)NULL) {
	  	if (pwe=getpwuid(geteuid()))
			ptr = pwe->pw_name;
	  	else if ((ptr = getenv("LOGNAME")) == (char *)NULL)
	  	if ((ptr = getenv("USER")) == (char *)NULL) {
noone:		    	fprintf(stderr,"Who *are* you?\n");
			exit(1);
		 }
	}
	if (ptr==(char *)NULL)
		goto noone;
	if (strlen(ptr)==0)
		goto noone;

	if ( ( lpbuf = malloc ((5* BUFSIZ)>>2) ) == (char *)NULL)
		died(-285);

	if ( ( inbuffer = malloc( (5*BUFSIZ) >>2) ) == (char *)NULL)
		died(-285);

/*
 *	second task is to prepare the pathnames the player will need
 */
	strcpy(loginname,ptr);

	/* save loginname of the user for logging purposes */
	strcpy(logname,ptr);	

	/* this will be overwritten with the players name */
	if ((ptr = getenv("HOME")) == (char *)NULL)
		if ((ptr = pwe->pw_dir) == (char *)NULL)
			ptr = ".";

	/* save file name in home directory */
	sprintf(savefilename, "%s/Ularn.sav", ptr);

	sprintf(buf, "%s/%s",ptr, optsfile); 	/* the .Ularnopts filename */
	strcpy(optsfile, buf);

	sprintf(buf, "%s/%s", ptr, ckpfile);	/* the checkpoint file */
	strcpy(ckpfile, buf);

	sprintf(scorefile, "%s/%s", libdir, SCORENAME); /* the Ularn scoreboard filename */

	sprintf(helpfile, "%s/%s", libdir, HELPNAME); 	/* the Ularn on-line help file */

	sprintf(larnlevels, "%s/%s", libdir, LEVELSNAME); /* the pre-made cave level data file */

	sprintf(fortfile, "%s/%s", libdir, FORTSNAME); 	/* the fortune data file name */

	init_term(); /*setup the terminal (find out what type) for termcap */

	init_cells();	/* initialize dungeon storage */

	lcreat((char*)0);	
	newgame();		/*	set the initial clock  */
	hard= -1;

	sigsetup();	/* trap all needed signals	*/

/*
 *	now make scoreboard if it is not there (don't clear)
 */
	if (access(scorefile,0) == -1) /* not there */
		if (makeboard() == -1) {
			fprintf(stderr, "I can't create the scoreboard.\n");
			fprintf(stderr, "Check permissions on %s\n",libdir);
			exit(1);
		}
	
/*
 *	now process the command line arguments
 */
/*  	opterr = 0; */
	while ((i = getopt(argc, argv, optstring)) != -1) {
		  switch(i) {
			case 's': showscores();
				  exit(0);  /* show scoreboard   */

			case 'i': showallscores();
				  exit(0);  /* show all scoreboard */

			case 'c': /*anyone with password can create scoreboard*/
				setupvt100();	
			lprcat("Preparing to initialize the scoreboard.\n");
				  if (getpassword() != 0) {
					if (makeboard() == -1) {
				lprcat("Scoreboard creation failed!!\n");
						clearvt100();
						exit(1);
					}
					showscores();
					lprcat("\nScoreboard initialized.\n");
				  }
				clearvt100();	
				exit(0);

			case 'n':
				nowelcome=1;
				break;

			case 'd': 	/* specify hardness */
				hard = atoi(optarg);
				if (hard > 100)
					hard = 100;
				if (hard < 0) {
			fprintf(stderr,"difficulty level must be > 0\n");
					puts(cmdhelp);
					exit(1);
				}
				break;

			case 'h':	/* print out command line arguments */
				fprintf(stderr,"%s", copyright);
				fprintf(stderr,"  Version %s",
					PACKAGE_VERSION);
				fprintf(stderr,"\n%s", cmdhelp);
				exit(0);

			case 'o':	/* specify a .Ularnopts filename */
				strcpy(optsfile,optarg);
				break;

			case 'r':
				/* restore checkpointed game */
				if (access(ckpfile,0) == -1)	{
				    printf("Cannot find checkpoint file %s\n",
					ckpfile);
				   exit(1);
				}
				printf("Restoring..."); fflush(stdout);
				restorflag = 1;
				hitflag=1;
				setupvt100();
				restoregame(ckpfile);
				break;

			case '?':
			default:
				if (!opterr)
				    fprintf(stderr,"Unknown option <%s>\n",
					argv[optind-1]);
				fprintf(stderr,"%s\n", copyright);
				fprintf(stderr,"Version %s\n",
					PACKAGE_VERSION);
				fprintf(stderr,"%s", cmdhelp);
				exit(1);
		} /* end switch */
	} /* end while */


	userid = geteuid();	/* obtain the user's effective id number */

	readopts();	/* read the options file if there is one */
	sethard(hard);	/* set up the desired difficulty	*/

	/* restore game if need to, and haven't restored ckpfile */
	if (!restorflag && access(savefilename,0)==0)	{
		restorflag = 1;
		hitflag=1;	
		printf("Restoring...");
		fflush(stdout);
		setupvt100();	/* setup the terminal special mode	*/
		restoregame(savefilename);
	} else {
		setupvt100();	/* setup the terminal special mode	*/
	}

	/* create new game */
	if (restorflag == 0) {
		makeplayer();	/*	make the character that will play*/
		newcavelevel(0);/*	make the dungeon */
		predostuff = 1;	

		/* tell signals that we are in the welcome screen */
		if (nowelcome==0)
			welcome();	 /* welcome the player to the game */
	}

	drawscreen();	/*	show the initial dungeon */

	/* tell the trap functions that they must do a showplayer()
	   from here on */
	predostuff = 2;	

	yrepcount = hit2flag = 0;
	/*
	 *	M A I N  L O O P
	 */
	while (1) {
		if (dropflag==0)
			lookforobject(); /* see if there is an object here*/
		else
			dropflag=0; /* don't show it just dropped an item */

		if (hitflag==0) {
			if (c[HASTEMONST])
				movemonst();
			movemonst();
		}	/* move the monsters	*/

		if (viewflag==0)
			showcell(playerx,playery);
		else viewflag=0;	/* show stuff around player */

		if (hit3flag)
			flushall();
		hitflag = hit3flag = 0;	
		nomove=1;
		bot_linex();	/* update bottom line */

		/*	get commands and make moves	*/
		while (nomove) {
			if (hit3flag)
				flushall();
			nomove=0;
			parse();	/* may reset nomove=1 */
		}	

		/* regenerate hp and spells */
		regen();

		if (c[TIMESTOP]==0)
		  	if (--rmst <= 0) {
				rmst = 120-(level<<2);
				fillmonst(makemonst(level));
		  	}
	}	/* end main loop */
	/* NOTREACHED */
}	/* end main */


/*
 *	parse()
 *
 *	get and execute a command
 */
parse ()
{
	int i,j;
	int k,flag;

	while	(1) {
		k = yylex();
		/* get the token from the input and switch on it	*/
		switch(k)	{
			case 'A':	if (wizard)
						diag();
					yrepcount=0;
					return;
			case 'h':	moveplayer(4);	
					return;	/* west	*/
			case 'H':	run(4);
					return;	/* west	*/
			case 'l':	moveplayer(2);	
					return;	/* east	*/
			case 'L':	run(2);		
					return;	/* east	*/
			case 'j':	moveplayer(1);	
					return;	/* south */
			case 'J':	run(1);			
					return;	/* south */
			case 'k':	moveplayer(3);	
					return;	/* north */
			case 'K':	run(3);			
					return;	/* north */
			case 'u':	moveplayer(5);	
					return;	/* northeast */
			case 'U':	run(5);			
					return;	/* northeast */
			case 'y':	moveplayer(6);
					return;	/* northwest */
			case 'Y':	run(6);			
					return;	/* northwest */
			case 'n':	moveplayer(7);	
					return;	/* southeast */
			case 'N':	run(7);			
					return;	/* southeast */
			case 'b':	moveplayer(8);	
					return;	/* southwest */
			case 'B':	run(8);			
					return;	/* southwest */

			case '.':	if (yrepcount)
						viewflag=1;
					return;	/* do nothing, stay here */

			case ' ':	yrepcount=0;	
					nomove=1;
					return;

			case 'w':	yrepcount=0;	
					wield();	
					return;	/* wield a weapon */

			case 'W':	yrepcount=0;	
					wear();		
					return;	/* wear armor	*/

			case 'r':	yrepcount=0;
					if (c[BLINDCOUNT]) {
						cursors();
			lprcat("\nYou can't read anything when you're blind!");
					} else if (c[TIMESTOP]==0)
						readscr();
					return;	/* to read a scroll */

			case 'q':	yrepcount=0;	
					if (c[TIMESTOP]==0)
						quaff();	
					return;	/* quaff a potion */

			case 'd':	yrepcount=0;	
					if (c[TIMESTOP]==0)
					dropobj();
					return;	/* to drop an object */

			case 'c':	yrepcount=0;	
					cast();		
					return;	/* cast a spell	*/

			case 'C':	yrepcount=0;
					closedoor();
					return;

			case 'i':	yrepcount=0;	
					nomove=1;
					showstr();	
					return;	/* status */

			case 'e':	yrepcount=0;
					if (c[TIMESTOP]==0)
						eatcookie();
					return;	/* to eat a fortune cookie */

			case 'D':	yrepcount=0;	
					seemagic(0);	
					nomove=1;
					return;	/*list spells and scrolls */

			case '?':	yrepcount=0;	
					help();
					nomove=1;
					return;	/*give the help screen*/

			case 'S':	clear();
					lprcat("Saving . . .");
					lflush();
					savegame(savefilename);
					wizard=1;		/* so not show scores */
					died(-257);
					/* NOTREACHED */

			case 'Z':	yrepcount=0;	
				if (wizard) {
					int t;

					cursors();
			lprcat("\nWhich level do you wish to teleport to? ");
					t = (int) readnum(20);
					if (t > 20 || t < 0) {
						lprcat(" sorry!");
						return;
					}
					playerx = rnd(MAXX-2);	
					playery = rnd(MAXY-2);
					newcavelevel(t);
					positionplayer();
					draws(0,MAXX,0,MAXY);
					bot_linex();
					return;
				}
					if (c[LEVEL]>9) {
						oteleport(1);
						return;
					}
					cursors();
					lprcat("\nYou don't know how to teleport yet.");
					return;	/* teleport yourself	*/

			case '^':	/* identify traps */
					flag=yrepcount=0;
					cursors();
					lprc('\n');
					for (j=playery-1; j<playery+2; j++) {
					  if (j < 0) j=0;		
					  if (j >= MAXY) break;
					  for (i=playerx-1; i<playerx+2; i++) {
					    if (i < 0) i=0;	
					    if (i >= MAXX) break;
					    switch(item[i][j]) {
						case OTRAPDOOR:		
						case ODARTRAP:
						case OTRAPARROW:
						case OTELEPORTER:
						case OELEVATORUP:
						case OELEVATORDOWN:
						 lprcat("\nIt's ");
						 lprcat(objectname[item[i][j]]);
						flag++;
					    };
					}
				      }
				      if (flag==0)
						  lprcat("\nNo traps are visible.");
				      return;

			case '#':		/* become creator */
				yrepcount=0;	
				cursors();
				nomove=1;
				if (!wizard)
					if (getpassword()==0) {
						scbr();
						return;
					}
				scbr();
				raiseexperience(370 * 1000000);
				/* c[GOLD]=10000; */
				bottomline();
				drawscreen();
				return;

				/* create an item */
			case '@':
				yrepcount=0;
				if (wizard)
					do_create();
				return;

				/* toggle wizard flag */
			case '=':
				yrepcount=0;	
				cursors();
				nomove=1;
				if (wizard) {
					lprcat("You are no longer a wizard.\n");
					wizard = 0;
					return;
				}
				if (getpassword()) {
					lprcat("You are now a wizard.\n");
					wizard = 1;
				} else
					lprcat("Sorry.\n");
				return;

			case '_': /* debug mode */
				yrepcount=0;	
				cursors();
				nomove=1;
				if (!wizard) {
					if (getpassword()==0)
						return;
					wizard=1;
				}
				scbr();
				for (i=0; i<6; i++)
					c[i]=70;
				iven[0]=iven[1]=0;
				take(OPROTRING,50);
				take(OLANCE,25);
				for (i=0; i<IVENSIZE; i++)
				  if (iven[i]==OLANCE && ivenarg[i]==25) {
						c[WIELD]=i;
						break;
				  }
				c[LANCEDEATH]=1;
				c[WEAR] = c[SHIELD] = -1;
				raiseexperience(370*1000000);
				c[AWARENESS] += 25000;
				{
					int i,j;
					for (i=0; i<MAXY; i++)
					  for (j=0; j<MAXX; j++)
						know[j][i]=1;
					for (i=0; i<SPNUM; i++)	
						spelknow[i]=1;
					for (i=0; i<MAXSCROLL; i++)
						potionknown[i]=1;
					for (i=0; i<MAXPOTION; i++)
						potionknown[i]=1;
				}
				for (i=0; i<MAXSCROLL; i++)
				  if (strlen(scrollname[i])>2) {
					item[i][0]=OSCROLL;
					iarg[i][0]=i;
				  }
				for (i=MAXX-1; i>MAXX-1-MAXPOTION; i--)
					/* no null items */
				  if (strlen(potionname[i-MAXX+MAXPOTION])>2) {
					item[i][0]=OPOTION;
					iarg[i][0]=i-MAXX+MAXPOTION;
				  }
				for (i=1; i<MAXY; i++) {
					item[0][i]=i;
					iarg[0][i]=0;
				}
				for (i=MAXY; i<MAXY+MAXX; i++) {
					item[i-MAXY][MAXY-1]=i;
					iarg[i-MAXY][MAXY-1]=0;
				}
				for (i=MAXX+MAXY; i<MAXX+MAXY+MAXY; i++) {
					item[MAXX-1][i-MAXX-MAXY]=i;
					iarg[MAXX-1][i-MAXX-MAXY]=0;
				}
				c[GOLD]+=250000;
				bottomline();
				drawscreen();	
				return;

			case 'T':	yrepcount=0;	
					cursors();
					if (c[SHIELD] != -1) {
						c[SHIELD] = -1;
						lprcat("\nYour shield is off.");
 						bottomline();
					} else
					if (c[WEAR] != -1) {
						c[WEAR] = -1;
						lprcat("\nYour armor is off.");
						bottomline();
					}
					else lprcat("\nYou aren't wearing anything.");
					return;

			case 'g':	cursors();
				lprintf("\nThe stuff you are carrying presently weighs %d "
						"pound%s.",(long)packweight(), packweight()==1?"":"s");
					nomove=1;
					yrepcount=0;
					return;

			case 'v':	yrepcount=0;	
					cursors();
lprintf( "\nThe Addiction of Ularn -- Version %s\nDifficulty level %d",
	PACKAGE_VERSION, (long)c[HARDGAME]);
					if (wizard) lprcat(" (WIZARD)");
					nomove=1;
					if (cheat) lprcat(" (Cheater)");
					lprcat(copyright);
					return;

			case 'Q':	yrepcount=0;
					quit();
					nomove=1;	
					return;	/* quit	*/

			case 12:	/* ^R */
			case 18:	/* ^L */
					yrepcount=0;	
					drawscreen();
					nomove=1;
					return;	/*	look		*/

			case 'P':	cursors();
				if (outstanding_taxes>0)
					lprintf("\nYou presently owe %d gp in taxes.",
							(long)outstanding_taxes);
				else
					lprcat("\nYou do not owe any taxes.");
				return;
		};
	}
}

parse2 ()
{
	if (c[HASTEMONST])
		movemonst();
	movemonst(); /*	move the monsters */
	randmonst();	
	regen();
}

#ifndef HAVE_GETOPT

/* Getopt for GNU.
   Copyright (C) 1987, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifdef __STDC__
#define CONST const
#else
#define CONST
#endif

/* This version of `getopt' appears to the caller like standard Unix `getopt'
   but it behaves differently for the user, since it allows the user
   to intersperse the options with the other arguments.

   As `getopt' works, it permutes the elements of `argv' so that,
   when it is done, all the options precede everything else.  Thus
   all application programs are extended to handle flexible argument order.

   Setting the environment variable _POSIX_OPTION_ORDER disables permutation.
   Then the behavior is completely standard.

   GNU application programs can use a third alternative mode in which
   they can distinguish the relative order of options and other arguments.  */

/* For communication from `getopt' to the caller.
   When `getopt' finds an option that takes an argument,
   the argument value is returned here.
   Also, when `ordering' is RETURN_IN_ORDER,
   each non-option ARGV-element is returned here.  */

char *optarg = 0;

/* Index in ARGV of the next element to be scanned.
   This is used for communication to and from the caller
   and for communication between successive calls to `getopt'.

   On entry to `getopt', zero means this is the first call; initialize.

   When `getopt' returns EOF, this is the index of the first of the
   non-option elements that the caller should itself scan.

   Otherwise, `optind' communicates from one call to the next
   how much of ARGV has been scanned so far.  */

int optind = 0;

/* The next char to be scanned in the option-element
   in which the last option character we returned was found.
   This allows us to pick up the scan where we left off.

   If this is zero, or a null string, it means resume the scan
   by advancing to the next ARGV-element.  */

static char *nextchar;

/* Callers store zero here to inhibit the error message
   for unrecognized options.  */

int opterr = 1;

/* Describe how to deal with options that follow non-option ARGV-elements.

   If the caller did not specify anything,
   the default is REQUIRE_ORDER if the environment variable
   _POSIX_OPTION_ORDER is defined, PERMUTE otherwise.

   REQUIRE_ORDER means don't recognize them as options;
   stop option processing when the first non-option is seen.
   This is what Unix does.
   This mode of operation is selected by either setting the environment
   variable _POSIX_OPTION_ORDER, or using `+' as the first character
   of the list of option characters.

   PERMUTE is the default.  We permute the contents of ARGV as we scan,
   so that eventually all the non-options are at the end.  This allows options
   to be given in any order, even with programs that were not written to
   expect this.

   RETURN_IN_ORDER is an option available to programs that were written
   to expect options and other ARGV-elements in any order and that care about
   the ordering of the two.  We describe each non-option ARGV-element
   as if it were the argument of an option with character code 1.
   Using `-' as the first character of the list of option characters
   selects this mode of operation.

   The special argument `--' forces an end of option-scanning regardless
   of the value of `ordering'.  In the case of RETURN_IN_ORDER, only
   `--' can cause `getopt' to return EOF with `optind' != ARGC.  */

static enum
{
  REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER
} ordering;

/* Describe the long-named options requested by the application.
   _GETOPT_LONG_OPTIONS is a vector of `struct option' terminated by an
   element containing a name which is zero.
   The field `has_arg' is 1 if the option takes an argument,
   2 if it takes an optional argument.  */

struct option
{
  char *name;
  int has_arg;
  int *flag;
  int val;
};

CONST struct option *_getopt_long_options;

int _getopt_long_only = 0;

/* Index in _GETOPT_LONG_OPTIONS of the long-named option actually found.
   Only valid when a long-named option was found. */

int option_index;

/* Handle permutation of arguments.  */

/* Describe the part of ARGV that contains non-options that have
   been skipped.  `first_nonopt' is the index in ARGV of the first of them;
   `last_nonopt' is the index after the last of them.  */

static int first_nonopt;
static int last_nonopt;

/* Exchange two adjacent subsequences of ARGV.
   One subsequence is elements [first_nonopt,last_nonopt)
    which contains all the non-options that have been skipped so far.
   The other is elements [last_nonopt,optind), which contains all
    the options processed since those non-options were skipped.

   `first_nonopt' and `last_nonopt' are relocated so that they describe
    the new indices of the non-options in ARGV after they are moved.  */

static void
exchange (argv)
     char **argv;
{
  int nonopts_size = (last_nonopt - first_nonopt) * sizeof (char *);
  char **temp = (char **) malloc (nonopts_size);

  /* Interchange the two blocks of data in ARGV.  */

  bcopy (&argv[first_nonopt], temp, nonopts_size);
  bcopy (&argv[last_nonopt], &argv[first_nonopt],
	 (optind - last_nonopt) * sizeof (char *));
  bcopy (temp, &argv[first_nonopt + optind - last_nonopt], nonopts_size);

  /* Update records for the slots the non-options now occupy.  */

  first_nonopt += (optind - last_nonopt);
  last_nonopt = optind;
}

/* Scan elements of ARGV (whose length is ARGC) for option characters
   given in OPTSTRING.

   If an element of ARGV starts with '-', and is not exactly "-" or "--",
   then it is an option element.  The characters of this element
   (aside from the initial '-') are option characters.  If `getopt'
   is called repeatedly, it returns successively each of the option characters
   from each of the option elements.

   If `getopt' finds another option character, it returns that character,
   updating `optind' and `nextchar' so that the next call to `getopt' can
   resume the scan with the following option character or ARGV-element.

   If there are no more option characters, `getopt' returns `EOF'.
   Then `optind' is the index in ARGV of the first ARGV-element
   that is not an option.  (The ARGV-elements have been permuted
   so that those that are not options now come last.)

   OPTSTRING is a string containing the legitimate option characters.
   If an option character is seen that is not listed in OPTSTRING,
   return '?' after printing an error message.  If you set `opterr' to
   zero, the error message is suppressed but we still return '?'.

   If a char in OPTSTRING is followed by a colon, that means it wants an arg,
   so the following text in the same ARGV-element, or the text of the following
   ARGV-element, is returned in `optarg'.  Two colons mean an option that
   wants an optional arg; if there is text in the current ARGV-element,
   it is returned in `optarg', otherwise `optarg' is set to zero.

   If OPTSTRING starts with `-' or `+', it requests different methods of
   handling the non-option ARGV-elements.
   See the comments about RETURN_IN_ORDER and REQUIRE_ORDER, above.

   Long-named options begin with `+' instead of `-'.
   Their names may be abbreviated as long as the abbreviation is unique
   or is an exact match for some defined option.  If they have an
   argument, it follows the option name in the same ARGV-element, separated
   from the option name by a `=', or else the in next ARGV-element.
   When `getopt' finds a long-named option, it returns 0 if that option's
   `flag' field is nonzero, the value of the option's `val' field
   otherwise.  */

int
getopt (argc, argv, optstring)
     int argc;
     char **argv;
     CONST char *optstring;
{
  optarg = 0;

  /* Initialize the internal data when the first call is made.
     Start processing options with ARGV-element 1 (since ARGV-element 0
     is the program name); the sequence of previously skipped
     non-option ARGV-elements is empty.  */

  if (optind == 0)
    {
      first_nonopt = last_nonopt = optind = 1;

      nextchar = 0;

      /* Determine how to handle the ordering of options and nonoptions.  */

      if (optstring[0] == '-')
	{
	  ordering = RETURN_IN_ORDER;
	  ++optstring;
	}
      else if (optstring[0] == '+')
	{
	  ordering = REQUIRE_ORDER;
	  ++optstring;
	}
      else if (getenv ("_POSIX_OPTION_ORDER") != 0)
	ordering = REQUIRE_ORDER;
      else
	ordering = PERMUTE;
    }

  if (nextchar == 0 || *nextchar == 0)
    {
      if (ordering == PERMUTE)
	{
	  /* If we have just processed some options following some non-options,
	     exchange them so that the options come first.  */

	  if (first_nonopt != last_nonopt && last_nonopt != optind)
	    exchange (argv);
	  else if (last_nonopt != optind)
	    first_nonopt = optind;

	  /* Now skip any additional non-options
	     and extend the range of non-options previously skipped.  */

	  while (optind < argc
		 && (argv[optind][0] != '-'
		     || argv[optind][1] == 0)
		 && (_getopt_long_options == 0
		     || argv[optind][0] != '+'
		     || argv[optind][1] == 0))
	    optind++;
	  last_nonopt = optind;
	}

      /* Special ARGV-element `--' means premature end of options.
	 Skip it like a null option,
	 then exchange with previous non-options as if it were an option,
	 then skip everything else like a non-option.  */

      if (optind != argc && !strcmp (argv[optind], "--"))
	{
	  optind++;

	  if (first_nonopt != last_nonopt && last_nonopt != optind)
	    exchange (argv);
	  else if (first_nonopt == last_nonopt)
	    first_nonopt = optind;
	  last_nonopt = argc;

	  optind = argc;
	}

      /* If we have done all the ARGV-elements, stop the scan
	 and back over any non-options that we skipped and permuted.  */

      if (optind == argc)
	{
	  /* Set the next-arg-index to point at the non-options
	     that we previously skipped, so the caller will digest them.  */
	  if (first_nonopt != last_nonopt)
	    optind = first_nonopt;
	  return EOF;
	}

      /* If we have come to a non-option and did not permute it,
	 either stop the scan or describe it to the caller and pass it by.  */

      if ((argv[optind][0] != '-' || argv[optind][1] == 0)
	  && (_getopt_long_options == 0
	      || argv[optind][0] != '+' || argv[optind][1] == 0))
	{
	  if (ordering == REQUIRE_ORDER)
	    return EOF;
	  optarg = argv[optind++];
	  return 1;
	}

      /* We have found another option-ARGV-element.
	 Start decoding its characters.  */

      nextchar = argv[optind] + 1;
    }

  if (_getopt_long_options != 0
      && (argv[optind][0] == '+'
	  || (_getopt_long_only && argv[optind][0] == '-'))
    )
    {
      CONST struct option *p;
      char *s = nextchar;
      int exact = 0;
      int ambig = 0;
      CONST struct option *pfound = 0;
      int indfound;

      while (*s && *s != '=')
	s++;

      /* Test all options for either exact match or abbreviated matches.  */
      for (p = _getopt_long_options, option_index = 0; p->name;
	   p++, option_index++)
	if (!strncmp (p->name, nextchar, s - nextchar))
	  {
	    if (s - nextchar == strlen (p->name))
	      {
		/* Exact match found.  */
		pfound = p;
		indfound = option_index;
		exact = 1;
		break;
	      }
	    else if (pfound == 0)
	      {
		/* First nonexact match found.  */
		pfound = p;
		indfound = option_index;
	      }
	    else
	      /* Second nonexact match found.  */
	      ambig = 1;
	  }

      if (ambig && !exact)
	{
	  fprintf (stderr, "%s: option `%s' is ambiguous\n",
		   argv[0], argv[optind]);
	  nextchar += strlen (nextchar);
	  optind++;
	  return '?';
	}

      if (pfound != 0)
	{
	  option_index = indfound;
	  optind++;
	  if (*s)
	    {
	      if (pfound->has_arg > 0)
		optarg = s + 1;
	      else
		{
		  fprintf (stderr,
			   "%s: option `%c%s' doesn't allow an argument\n",
			   argv[0], argv[optind - 1][0], pfound->name);
		  nextchar += strlen (nextchar);
		  return '?';
		}
	    }
	  else if (pfound->has_arg == 1)
	    {
	      if (optind < argc)
		optarg = argv[optind++];
	      else
		{
		  fprintf (stderr, "%s: option `%s' requires an argument\n",
			   argv[0], argv[optind - 1]);
		  nextchar += strlen (nextchar);
		  return '?';
		}
	    }
	  nextchar += strlen (nextchar);
	  if (pfound->flag)
	    {
	      *(pfound->flag) = pfound->val;
	      return 0;
	    }
	  return pfound->val;
	}
      /* Can't find it as a long option.  If this is getopt_long_only,
	 and the option starts with '-' and is a valid short
	 option, then interpret it as a short option.  Otherwise it's
	 an error.  */
      if (_getopt_long_only == 0 || argv[optind][0] == '+' ||
	  index (optstring, *nextchar) == 0)
	{
	  if (opterr != 0)
	    fprintf (stderr, "%s: unrecognized option `%c%s'\n",
		     argv[0], argv[optind][0], nextchar);
	  nextchar += strlen (nextchar);
	  optind++;
	  return '?';
	}
    }

  /* Look at and handle the next option-character.  */

  {
    char c = *nextchar++;
    char *temp = index (optstring, c);

    /* Increment `optind' when we start to process its last character.  */
    if (*nextchar == 0)
      optind++;

    if (temp == 0 || c == ':')
      {
	if (opterr != 0)
	  {
	    if (c < 040 || c >= 0177)
	      fprintf (stderr, "%s: unrecognized option, character code 0%o\n",
		       argv[0], c);
	    else
	      fprintf (stderr, "%s: unrecognized option `-%c'\n",
		       argv[0], c);
	  }
	return '?';
      }
    if (temp[1] == ':')
      {
	if (temp[2] == ':')
	  {
	    /* This is an option that accepts an argument optionally.  */
	    if (*nextchar != 0)
	      {
		optarg = nextchar;
		optind++;
	      }
	    else
	      optarg = 0;
	    nextchar = 0;
	  }
	else
	  {
	    /* This is an option that requires an argument.  */
	    if (*nextchar != 0)
	      {
		optarg = nextchar;
		/* If we end this ARGV-element by taking the rest as an arg,
		   we must advance to the next element now.  */
		optind++;
	      }
	    else if (optind == argc)
	      {
		if (opterr != 0)
		  fprintf (stderr, "%s: option `-%c' requires an argument\n",
			   argv[0], c);
		c = '?';
	      }
	    else
	      /* We already incremented `optind' once;
		 increment it again when taking next ARGV-elt as argument.  */
	      optarg = argv[optind++];
	    nextchar = 0;
	  }
      }
    return c;
  }
}
#endif /* HAVE_GETOPT */
