/* scores.c
 *
 *	Functions in this file are:
 *
 *	readboard() 	
 *		Function to read in the scoreboard into a static buffer
 *	writeboard()	
 *		Function to write the scoreboard from readboard()'s buffer
 *	makeboard() 	
 *		Function to create a new scoreboard (wipe out old one)
 *	hashewon()	 
 *		Function to return 1 if player has won a game before, else 0
 *	long paytaxes(x)	 
 *		Function to pay taxes if any are due
 *	winshou()		
 *		Subroutine to print out the winning scoreboard
 *	shou(x)			
 *		Subroutine to print out the non-winners scoreboard
 *	showscores()		
 *		Function to show the scoreboard on the terminal
 *	showallscores()	
 *		Function to show scores and the iven lists that go with them
 *	sortboard()		
 *		Function to sort the scoreboard
 *	newscore(score, whoo, whyded, winner) 	
 *		Function to add entry to scoreboard
 *	new1sub(score,i,whoo,taxes) 		  
 *		Subroutine to put player into a 
 *	new2sub(score,i,whoo,whyded)	 	  
 *		Subroutine to put player into a 
 *	died(x) 	
 *		Subroutine to record who played larn, and what the score was
 *	diedsub(x) 
 *		Subroutine to print out a line showing player when he is killed
 */
#include "header.h"
#include "monst.h"
#include "itm.h"
#include "player.h"
#include "extern.h"
extern long Taxes;

struct scofmt /*This is the structure for the scoreboard 	*/
{
	long	score;		/* the score of the player 	*/
	int	suid;		/* the user id number of the player*/
	short	what;		/* the number of the monster that killedplayer*/
	short	level;		/* the level player was on when he died */
	short	hardlev;	/* the level of difficulty player played at*/
	short	order;		/* the relative ordering place of this entry*/
	char	who[40];	/* the name of the character 		*/
	char    char_class[20]; /* the character class */
	short	sciv[IVENSIZE][2];	/* this is the inventory list of the character*/
};

/* This is the structure for the winning scoreboard */
struct wscofmt {
	long	score;		/* the score of the player	*/
	short	timeused;	/* the time used in mobuls to win the game*/
	long	taxes;		/* taxes he owes to LRS 	*/
	int	suid;		/* the user id number of the player*/
	short	hardlev;	/* the level of difficulty player played at*/
	short	order;		/* the relative ordering place of this entry*/
	char	who[40];	/* the name of the character 		*/
	char    char_class[20]; /* the character class */
};

static struct scofmt sco[SCORESIZE];	/* the structure for the scoreboard  */
static struct wscofmt winr[SCORESIZE];	/* struct for the winning scoreboard */

static char	*whydead[] = {
"quit", 					/* 0 */
"suspended", 
"self - annihilated", 
"shot by an arrow",
"hit by a dart",
"fell into a pit", 				/* 5 */
"fell into a pit to HELL",
"a winner", 
"trapped in solid rock", 
"killed by a missing save file",
"killed by an old save file", 			/* 10 */
"caught by the greedy cheater checker trap",
"killed by a protected save file", 
"killed his family and committed suicide", 	/* 13 */
"erased by a wayward finger",
"fell through a trap door to HELL",		/* 15 */
"fell through a trap door",
"drank some poisonous water",
"fried by an electric shock", 
"slipped in a volcano shaft",
"killed by a stupid act of frustration", 	/* 20 */
"attacked by a revolting demon",
"hit by his own magic",			/* 22 */
"demolished by an unseen attacker",
"fell into the dreadful sleep",
"killed by an exploding chest",			/* 25 */
"died of internal complications",
"annihilated by a sphere",
"died a post mortem death",
"wasted by a malloc() failure", 
"wasted by a pissed off genie",			/* 30 */
"took an elevator straight to HELL"		/* 31 */
};


/*
 *	readboard() 	Function to read in the scoreboard into a static buffer
 *
 *	returns -1 if unable to read in the scoreboard, returns 0 if all is OK
 */
readboard()
{
	FILE *fp;

	if ((fp = fopen(scorefile, "r")) == (FILE *)NULL) {
		fprintf(stderr, "Can't open scorefile for reading\n"); 
		fflush(stderr);
		return(-1); 
	}
	if (fread((char *)sco, sizeof(sco), 1, fp) != 1) {
		fprintf(stderr, "Can't read scoreboard\n");
		fflush(stderr);
		fclose(fp);
		return(-1);
	}
	if (fread((char *)winr, sizeof(winr), 1, fp) != 1) {
		fprintf(stderr, "Can't read scoreboard\n");
		fflush(stderr);
		fclose(fp);
		return(-1);
	}
	fflush(fp);
	fclose(fp);  
	return(0);
}


/*
 *	writeboard()	
 *		Function to write the scoreboard from readboard()'s buffer
 *
 *	returns -1 if unable to write the scoreboard, returns 0 if all is OK
 */
writeboard()
{
	FILE *fp;

	if ((fp = fopen(scorefile, "w")) == (FILE *)NULL) {
		lprcat("Can't open scorefile for writing\n"); 
		lflush();
		return(-1); 
	}
	if (fwrite((char * )sco, sizeof(sco), 1, fp) != 1) {
		lprcat("Can't write scorefile\n"); 
		lflush();
		fclose(fp);
		return(-1); 
	}
	if (fwrite((char * )winr, sizeof(winr), 1, fp) != 1) {
		lprcat("Can't write scorefile\n"); 
		lflush();
		fclose(fp);
		return(-1); 
	}
	fflush(fp);
	fclose(fp);  
	return(0);
}


/*
 *		Function to create a new scoreboard (wipe out old one)
 *	makeboard() 		
 *
 *	returns -1 if unable to write the scoreboard, returns 0 if all is OK
 */
makeboard()
{
	int	i;

	for (i = 0; i < SCORESIZE; i++) {
		winr[i].taxes = winr[i].score = sco[i].score = 0;
		winr[i].order = sco[i].order = i;
	}
	if (writeboard()) 
		return(-1);
	chmod(scorefile, 0666);
	return(0);
}


/*
 *	Function to return 1 if player has won a game before, else 0
 *
 *	hashewon()	 
 *
 *	This function also sets c[HARDGAME] to appropriate value -- 0 if not a
 *	winner, otherwise the next level of difficulty listed in the winners
 *	scoreboard.  This function also sets outstanding_taxes to the value in
 *	the winners scoreboard.
 */
hashewon()
{
	int	i;

	c[HARDGAME] = 0;

	if (readboard() < 0) 
		return(0);	/* can't find scoreboard */

	for (i = 0; i < SCORESIZE; i++)	/* search through winners scoreboard */
		if (winr[i].suid == userid)
			if (winr[i].score > 0) {
				c[HARDGAME] = winr[i].hardlev + 1;
				outstanding_taxes = winr[i].taxes;
				return(1);
			}
	return(0);
}


/*
 *	long paytaxes(x)		 Function to pay taxes if any are due
 *
 *	Enter with the amount (in gp) to pay on the taxes.
 *	Returns amount actually paid.
 */
long	paytaxes(x)
long	x;
{
	int	i;
	long	amt;

	if (x <= 0) 
		return(0L);

	if (readboard() < 0) 
		return(0L);

	for (i = 0; i < SCORESIZE; i++)

		/* look for players winning entry */
		if (winr[i].suid == userid)

			/* search for a winning entry for the player */ {
			if (winr[i].score > 0) 
				amt = winr[i].taxes;
				if (x < amt) 
					amt = x;
				winr[i].taxes -= amt;
				outstanding_taxes -= amt;
				if (writeboard() < 0) 
					return(0);
				return(amt);
			}
	return(0L);	/* couldn't find user on winning scoreboard */
}


/*
 *	winshou()		Subroutine to print out the winning scoreboard
 *
 *	Returns the number of players on scoreboard that were shown 
 */
winshou()
{
	struct wscofmt *p;
	int	i, j, count;

	/* is there anyone on the scoreboard? */
	for (count = j = i = 0; i < SCORESIZE; i++)
		if (winr[i].score != 0) { 
			j++; 
			break; 
		}

	if (j) {

	      /*12345678901234567890123456789012345678901234567890123*/
	puts("\n   Score       Diff   Time Needed  Ularn Winners List");

		/* needed to print out the */
		/* winners in order */
		for (i = 0; i < SCORESIZE; i++)
		  for (j = 0; j < SCORESIZE; j++) {
			p = &winr[j];/*pointer to the scoreboard entry*/
			if (p->order == i) {
			    if (p->score) {
			      count++;
			      printf("%-10ld%8d%8d Mobuls  (%s) %s\n",
				    (long)p->score,
				    p->hardlev, 
				    p->timeused, 
				    p->char_class,
				    p->who);
			      fflush(stdout);
			    }
			    break;
			 }
		  }
	}
	return(count);	/* return number of people on scoreboard */
}


/*
 *	shou(x)			
 *	int x;
 *		Subroutine to print out the non-winners scoreboard
 *
 *	Enter with 0 to list the scores, enter with 1 to list inventories too
 *	Returns the number of players on scoreboard that were shown 
 */
shou(x)
int	x;
{
	int	i, j, n;
	int	count;

	/* is the scoreboard empty? */
	for (count = j = i = 0; i < SCORESIZE; i++)	
		if (sco[i].score != 0) { 
			j++; 
			break; 
		}

	if (j) {
		puts("\n  Score Diff  Ularn Visitor Log");
		for (i = 0; i < SCORESIZE; i++)
			for (j = 0; j < SCORESIZE; j++)
			    if (sco[j].order == i) {
				if (sco[j].score) {
				   count++;
				   printf("%7ld %3ld   (%s) %s ",
				    	(long)sco[j].score, 
					(long)sco[j].hardlev, 
					sco[j].char_class,
					sco[j].who);
				   if (sco[j].what < 256)
				       printf("killed by a %s", 
					monster[sco[j].what].name);
				   else {
					int foo = sco[j].what - 256;
				       	if (!sex && (foo == 13 || foo == 22))
						switch(foo) {
						case 13 : 
			printf("killed her family and committed suicide");
							break;
						case 22 :
			printf("hit by her own magic");
							break;
						}
					else
						printf("%s", whydead[foo]);
				   }
				   if (x != 263)
				 	printf(" on %s",
						levelname[sco[j].level]);
				   if (x) {
					for (n = 0; n < IVENSIZE; n++) { 
					   iven[n] = sco[j].sciv[n][0]; 
					   ivenarg[n] = sco[j].sciv[n][1]; 
					}
				  	for (n = 0; n < IVENSIZE; n++)
						if (iven[n])  {
						    srcount = 0; 
						    showscore3(n); 
						}
				    }
				    printf("\n");
				    fflush(stdout);
				}
				j = SCORESIZE;
			}
	}
	return(count);	/* return the number of players just shown */
}


/*
 *	showscores()		Function to show the scoreboard on the terminal
 *
 *	Returns nothing of value
 */
static char	esb[] = "\nThe scoreboard is empty.\n";

showscores()
{
	int	i, j;

	if (readboard() < 0)
		return;
	i = winshou();
	j = shou(0);

	if (i + j == 0) 
		puts(esb); 
	else 
		printf("\n");
	lflush();
}


/*
 *	showallscores()	
 *		Function to show scores and the iven lists that go with them
 *
 *	Returns nothing of value
 */
showallscores()
{
	int	i, j;


	if (readboard() < 0) 
		return;

	c[WEAR] = c[WIELD] = c[SHIELD] = -1;

	for (i = 0; i < MAXPOTION; i++) 
	        potionknown[i] = 1;
/*		potionname[i][0] = ' '; */
	for (i = 0; i < MAXSCROLL; i++) 
	        scrollknown[i] = 1;
/*		scrollname[i][0] = ' '; */

	i = winshou();  
	j = shou(1);

	if (i + j == 0)
		puts(esb); 
	else
		printf("\n");

}


/*
 *	sortboard()		Function to sort the scoreboard
 *
 *	Returns 0 if no sorting done, else returns 1
 */
sortboard()
{
	int	i, j, pos;
	long	jdat;

	for (i = 0; i < SCORESIZE; i++) 
		sco[i].order = winr[i].order = -1;
	pos = 0;
	while (pos < SCORESIZE) {
		jdat = 0;
		for (i = 0; i < SCORESIZE; i++)
			if ((sco[i].order < 0) && (sco[i].score >= jdat)) { 
				j = i;  
				jdat = sco[i].score; 
			}
		sco[j].order = pos++;
	}
	pos = 0;
	while (pos < SCORESIZE) {
		jdat = 0;
		for (i = 0; i < SCORESIZE; i++)
			if ((winr[i].order < 0) && (winr[i].score >= jdat)) { 
				j = i;  
				jdat = winr[i].score; 
			}
		winr[j].order = pos++;
	}
	return(1);
}


/*
 *		Function to add entry to scoreboard
 *	newscore(score, whoo, whyded, winner) 	
 *	int score, winner, whyded;
 *	char *whoo;
 *
 *	Enter with the total score in gp in score,  players name in whoo,
 *		died() reason # in whyded, and TRUE/FALSE in winner if a winner
 *	ex.		newscore(1000, "player 1", 32, 0);
 */
newscore(score, whoo, whyded, winner)
long	score;
int	winner, whyded;
char	*whoo;
{
	int	i;
	long	taxes;

	if (readboard() < 0) 
		return; 	/*	do the scoreboard	*/

	/* if a winner then delete all non-winning scores */
	if (cheat) 
		winner = 0;	/* if he cheated, don't let him win */

	if (winner) {
		for (i = 0; i < SCORESIZE; i++)
			if (sco[i].suid == userid)
				sco[i].score = 0;
 		taxes = (long)(outstanding_taxes + (score * TAXRATE));
		Taxes = taxes;
		score += 100000 * c[HARDGAME];	/* bonus for winning */

/* if he has a slot on the winning scoreboard update it if greater score*/
		for (i = 0; i < SCORESIZE; i++) 
			if (winr[i].suid == userid) { 
				new1sub(score, i, whoo, taxes); 
				return; 
			}

/* he had no entry. look for last entry and see if he has a greater score */
		for (i = 0; i < SCORESIZE; i++) 
			if (winr[i].order == SCORESIZE - 1) { 
				new1sub(score, i, whoo, taxes); 
				return; 
			}
	} else if (!cheat) /* for not winning scoreboard */ {
	/* if he has a slot on the scoreboard update it if greater score */
		for (i = 0; i < SCORESIZE; i++) 
			if (sco[i].suid == userid) { 
				new2sub(score, i, whoo, whyded); 
				return; 
			}

/* he had no entry. look for last entry and see if he has a greater score */
		for (i = 0; i < SCORESIZE; i++)
			if (sco[i].order == SCORESIZE - 1) { 
				new2sub(score, i, whoo, whyded); 
				return; 
			}
	}
}


/*
 *	new1sub(score,i,whoo,taxes) 	  Subroutine to put player into a 
 *	int score,i,whyded,taxes;	  winning scoreboard entry if his score
 *	char *whoo; 		  	  is high enough
 *
 *	Enter with the total score in gp in score,  players name in whoo,
 *		died() reason # in whyded, and TRUE/FALSE in winner if a winner
 *		slot in scoreboard in i, and the tax bill in taxes.
 *	Returns nothing of value
 */
new1sub(score, i, whoo, taxes)
long	score, taxes;
int	i;
char	*whoo;
{
	struct wscofmt *p;

	p = &winr[i];
	p->taxes += taxes;
	if ((score >= p->score) || (c[HARDGAME] > p->hardlev)) {
		strcpy(p->who, whoo);  		
		strcpy(p->char_class, char_class);
		p->score = score;
		p->hardlev = c[HARDGAME];		
		p->suid = userid;
		p->timeused = gtime / 100;
	}
}


/*
 *	new2sub(score,i,whoo,whyded)	 	  
 *	int score,i,whyded,taxes;		  
 *	char *whoo; 					  
 *				Subroutine to put player into a 
 *				non-winning scoreboard entry if his
 *				score is high enough
 *
 *	Enter with the total score in gp in score,  players name in whoo,
 *		died() reason # in whyded, and slot in scoreboard in i.
 *	Returns nothing of value
 */
new2sub(score, i, whoo, whyded)
long	score;
int	i, whyded;
char	*whoo;
{
	int	j;
	struct scofmt *p;

	p = &sco[i];
	if ((score >= p->score) || (c[HARDGAME] > p->hardlev)) {
		strcpy(p->who, whoo);  
		strcpy(p->char_class, char_class);
		p->score = score;
		p->what = whyded;       
		p->hardlev = c[HARDGAME];
		p->suid = userid;		  
		p->level = level;
		for (j = 0; j < IVENSIZE; j++) { 
			p->sciv[j][0] = iven[j]; 
			p->sciv[j][1] = ivenarg[j]; 
		}
	}
}


/*
 *		Subroutine to record who played larn, and what the score was
 *	died(x) 	
 *	int x;
 *
 *	if x < 0 then don't show scores
 *	died() never returns! (unless c[LIFEPROT] and a reincarnatable death!)
 *
 *	256 - <number> gives index into whydead[] array
 *
 *		< 256	killed by the monster number
 *		256		quit
 *		257		suspended
 *		258		self - annihilated
 *		259		shot by an arrow
 *		260		hit by a dart
 *		261		fell into a pit
 *		262		fell into a bottomless pit to HELL
 *		263		a winner
 *		264		trapped in solid rock
 *		265		killed by a missing save file
 *		266		killed by an old save file
 *		267		caught by the greedy cheater checker trap
 *		268		killed by a protected save file
 *		269		killed his family and killed himself
 *		270		erased by a wayward finger
 *		271		fell through a bottomless trap door to HELL
 *		272		fell through a trap door
 *		273		drank some poisonous water
 *		274		fried by an electric shock
 *		275		slipped on a volcano shaft
 *		276		killed by a stupid act of frustration
 *		277		attacked by a revolting demon
 *		278		hit by his own magic
 *		279		demolished by an unseen attacker
 *		280		fell into the dreadful sleep
 *		281		killed by an exploding chest
 *		282		died of internal complications (save file sum error)
 *		283		killed by a sphere of annihilation
 *		284		died a post mortem death
 *		285		malloc() failure
 *		286		pissed off genie
 *		287		took an elevator straight to HELL
 *		300		quick quit -- don't put on scoreboard
 */

static int	scorerror;

died(x)
int	x;
{
	int	f, win;
	if (c[LIFEPROT] > 0) /* if life protection */ {
		switch ((x > 0) ? x : -x) {
		case 256: 
		case 257:
		case 262: 
		case 263:
		case 265: 
		case 266:
		case 267: 
		case 268:
		case 269: 
		case 271:
		case 282: 
		case 284:
		case 285: 
		case 286: 
		case 300:  
			goto invalid; /* can't be saved */
		};
		--c[LIFEPROT]; 
		c[HP] = 1; 
		--c[CONSTITUTION];
		lprcat("\nYou feel wiiieeeeerrrrrd all over! ");
		lflush();
		beep();
		sleep(4);
		return; /* only case where died() returns */
	}
invalid:
	if (ckpflag) 
		unlink(ckpfile);	/* remove checkpoint file if used */
	if (tempfilename)
		unlink(tempfilename);

	f = 0;
	if (x < 0) { 
		f++; 
		x = -x; 
	}     /* if we are not to display the scores */

	if ((x == 300) || (x == 257))   {
		clearvt100();
		printf("\n");
		exit(0);	/* for quick exit or saved game */
	}

	if (x == 263)  
		win = 1;  
	else 
		win = 0;

	c[GOLD] += c[BANKACCOUNT];   
	c[BANKACCOUNT] = 0;

	/*	now enter the player at the end of the scoreboard */
	newscore(c[GOLD] + c[BANKACCOUNT], logname, x, win);

	clear();
	lflush();
	diedsub(x);	/* print out the score line */

	/*	wizards can't score*/
	if ( !wizard ) {

	/*now for the scoreboard maintenance -- not for a suspended game*/
		if (x != 257) {
			if (sortboard())  
				scorerror = writeboard();
		}
	}
	if ((x == 256) || (x == 257) || (f != 0))
		exit(0);

	if (scorerror == 0) 
		showscores();  /* if we updated the scoreboard*/

	if (x == 263 && mail)
		mailbill();

	clearvt100();
	printf("\n");
	exit(0);
}


/*
 *	Subroutine to print out the line showing the player when he is killed
 *	diedsub(x) 
 *	int x;
 */
diedsub(x)
int	x;
{
	char	ch, *mod, *cls;
	cls = class[c[LEVEL]];
	if (!char_class[0])
		strcpy(char_class, "<unknown>");
	puts("---------------------------------------------------------------");
	puts("                 U L A R N    S C O R E S");
	puts("---------------------------------------------------------------");
	printf("Score: %ld  Diff: %d  ", c[GOLD], c[HARDGAME]);
	printf("Level: %s  Char: %s\n", cls, char_class);
	printf("\t%s", logname);
	if (wizard) 
		printf(" (wizard)");
	if (x < 256) {
		ch = *monster[x].name;
		if(ch== 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
			mod = "an";  
		else 
			mod = "a";
		printf(" killed by %s %s", mod, monster[x].name);
	} else {
		int foo = x - 256;
	       	if (!sex && (foo == 13 || foo == 22))
			switch(foo) {
			case 13 : 
			printf(" killed her family and committed suicide");
				break;
			case 22 :
			printf(" hit by her own magic");
				break;
			}
		else
			printf(" %s", whydead[foo]);
	   }

	if (x != 263)
		printf(" on %s\n", levelname[level]);  
	else 
		printf("\n");
	puts("---------------------------------------------------------------");
	fflush(stdout);
}

showscore3(index)
int	index;
{
	switch (iven[index]) {
	case OPOTION:	
		showscore1(index, potionname);  
		break;
	case OSCROLL:	
		showscore1(index, scrollname);  
		break;
	case OLARNEYE:
	case OBOOK:
	case OSPIRITSCARAB:
	case ODIAMOND:
	case ORUBY:
	case OCUBEofUNDEAD:
	case OEMERALD:
	case OCHEST:
	case OCOOKIE:
	case OSAPPHIRE:
	case OORB:
	case OHANDofFEAR:
	case OBRASSLAMP:
	case OURN:
	case OWWAND:
	case OSPHTALISMAN:
	case ONOTHEFT:		
		showscore1(index, (char **)0);  
		break;
	default:
		printf("\n%c)   %s", index + 'a' , objectname[iven[index]]);
		if (ivenarg[index] > 0)
			printf(" + %d", (long)ivenarg[index]);
		else if (ivenarg[index] < 0)
			printf(" %d", (long)ivenarg[index]);
		break;
	}
	if (c[WIELD] == index) 
		puts(" (weapon in hand)");
	if ((c[WEAR] == index) || (c[SHIELD] == index))  
		puts(" (being worn)");
	fflush(stdout);
}

showscore1(idx, str2)
int	idx;
char	*str2[];
{
	if (str2 == 0)
		printf("\n%c)   %s", idx + 'a' , objectname[iven[idx]]);
	else if (*str2[ivenarg[idx]] == 0)
		printf("\n%c)   %s", idx + 'a' , objectname[iven[idx]]);
	else
		printf("\n%c)   %s of%s", idx  + 'a' , 
			objectname[iven[idx]], str2[ivenarg[idx]]);
}
