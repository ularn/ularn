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
void parse ()
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

