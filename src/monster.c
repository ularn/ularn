/* monster.c */
#include "header.h"
#include "itm.h"
#include "player.h"
#include "monst.h"
#include "extern.h"

/*
 * This file contains the following functions:
 * ------------------------------------------------------------------------ 
 *
 * createmonster(monstno) create a monster next to the player int monstno; 
 *
 * int cgood(x,y,itm,monst) check location for emptiness int
 * x,y,itm,monst; 
 *
 * createitem(it,arg) 	place an item next to the player int it,arg; 
 *
 * cast() 		cast a spell for the user 
 *
 * speldamage(x) 	perform spell functions cast by the player int x; 
 *
 * loseint()		decrement your int (intelligence) if > 3 
 *
 * isconfuse() 		check to see if player is confused 
 *
 * nospell(x,monst) 	return 1 if a spell doesn't affect a monster int
 * x,monst; 
 *
 * fullhit(xx)		return full damage against a monst (aka web) int xx; 
 *
 * direct(spnum,dam,str,arg)	int - direct spell damage 1 square in 1 dir 
 * spnum,dam,arg; char *str; 
 *
 * godirect(spnum,dam,str,delay,cshow)		- perform missile attacks 
 * int spnum,dam,delay; char *str,cshow; 
 *
 * ifblind(x,y)	int x,y; 
 * put "monster" or the monster name into lastmosnt 
 *
 * tdirect(spnum)	teleport away a monster int spnum; 
 *
 * damage all monsters 1 square from player omnidirect(sp,dam,str)  int sp,dam;
 * char *str; 
 *
 * dirsub(x,y)		ask for direction, then modify x,y for it int *x,*y; 
 *
 * verifyxy(x,y)  	verify/fix (*x,*y) for being within bounds int x,y; 
 *
 * dirpoly(spnum)	ask for a direction and polymorph a monst int spnum; 
 *
 * hitmonster(x,y)	hit a monster at the designated coordinates int x,y; 
 *
 * hitm(x,y,amt)	just hit a monster at a given coordinates int
 * x,y,amt; 
 *
 * hitplayer(x,y) 	for the monster to hit the player from (x,y) int x,y; 
 *
 * dropsomething(monst) create an object when a monster dies int monst; 
 *
 * dropgold(amount)	drop some gold around player int amount; 
 *
 * something(level) 	create a random item around player int level; 
 *
 * newobject(lev,i) 	return a randomly selected new object int lev,*i; 
 *
 * spattack(atckno,xx,yy)  process special attacks from monsters int
 * atckno,xx,yy; 
 *
 * subtract hp from user and flag bottomline display checkloss(x) 	int x; 
 *
 * annihilate monsters around player annihilate() 
 *
 * genmonst()		ask for monster and genocide from game 
 *
 * makemonst(lev)		function to return monster number for a randomly
 * selected monster 
 */
struct isave {			/* used for alter reality spell */
	char            type;	/* 0=item,  1=monster */
	short           id;	/* item number or monster number */
	short           arg;	/* the type of item or hitpoints of monster */
};

/*
 * Function to create a monster next to the player createmonster(monstno) 
 * nt monstno; 
 *
 * Enter with the monster number (1 to MAXMONST+8) Returns no value. 
 */
createmonster(mon)
int             mon;
{
	int             x, y, k, i;

	if (mon < 1 || mon > MAXMONST + 8) {
		beep();
		lprintf("\ncan't createmonst(%d)\n", (long) mon);
		nap(3000);
		return;
	}
	while ((monster[mon].flags & FL_GENOCIDED) != 0 && mon < MAXMONST)
		mon++;		/* genocided? */
	/* choose direction, then try all */
	for (k = rnd(8), i = -8; i < 0; i++, k++) {
		if (k > 8)
			k = 1;	/* wraparound the diroff arrays */
		x = playerx + diroffx[k];
		y = playery + diroffy[k];
		/* if we can create here */
		if (cgood(x, y, 0, 1)) {
			mitem[x][y].mon = mon;
			hitp[x][y] = monster[mon].hitpoints;
			stealth[x][y] = know[x][y] = 0;
			switch (mon) {
			case ROTHE:
			case POLTERGEIST:
			case VAMPIRE:
				stealth[x][y] = 1;
			};
			return;
		}
	}
}

/*
 * int cgood(x,y,itm,monst)	Function to check location for emptiness int
 * x,y,itm,monst; 
 *
 * Routine to return TRUE if a location does not have itm or monst there returns
 * FALSE (0) otherwise Enter with itm or monst TRUE or FALSE if checking it
 * Example:  if itm==TRUE check for no item at this location if monst==TRUE
 * check for no monster at this location This routine will return FALSE if at
 * a wall or the dungeon exit on level 1 
 */
cgood(x, y, itm, monst)
	int             x, y, itm, monst;
{
	if ((y >= 0) && (y <= MAXY - 1)
	    && (x >= 0) && (x <= MAXX - 1))	/* within bounds? */
		if (item[x][y] != OWALL && item[x][y] != OCLOSEDDOOR)	/* can't make on walls */
			if (itm == 0 || (item[x][y] == 0))	/* is it free of items? */
				if (monst == 0 || (mitem[x][y].mon == 0))	/* is it free of
									 * monsters? */
					if ((level != 1) || (x != 33) || (y != MAXY - 1))
						/* not exit to level 1 */
						return (1);
	return (0);
}

/*
 * createitem(it,arg) 	Routine to place an item next to the player int
 * it,arg; 
 *
 * Enter with the item number and its argument (iven[], ivenarg[]) Returns no
 * value, thus we don't know about createitem() failures. 
 *
 */
createitem(x, y, it, arg)
int             x,y, it, arg;
{
	int             k, i;

	if (it >= MAXOBJ)
		return;		/* no such object */

	for (k = rnd(8), i = -8; i < 0; i++, k++) {	/* choose direction, try
							 * all */
		if (k > 8)
			k = 1;	/* wraparound the diroff arrays */
		x = x + diroffx[k];
		y = y + diroffy[k];
		if ((it != OMAXGOLD && it != OGOLDPILE)) {
			if (cgood(x, y, 1, 0)) { /* if we can create here */
				item[x][y] = it;
				know[x][y] = 0;
				iarg[x][y] = arg;
				return;
			}
		} else {
			int             i;
			switch (item[x][y]) {
			case OGOLDPILE:
				if ((iarg[x][y] + arg) < 32767) {
					iarg[x][y] += arg;
					return;
				}
			case ODGOLD:
				if ((10L * iarg[x][y] + arg) < 327670L) {
					i = iarg[x][y];
					iarg[x][y]=(int)((10L * i + arg) / 10);
					item[x][y] = ODGOLD;
					return;
				}
			case OMAXGOLD:
				if ((100L * iarg[x][y] + arg) < 3276700L) {
					i = (int) ((100L * iarg[x][y]) + arg);
					iarg[x][y] = i / 100;
					item[x][y] = OMAXGOLD;
					return;
				}
			case OKGOLD:
				if ((1000L * iarg[x][y] + arg) <= 32767000L) {
					i = iarg[x][y];
					iarg[x][y]=(int)((1000L*i+arg) / 1000);
					item[x][y] = OKGOLD;
					return;
				} else
					iarg[x][y] = 32767;
				return;
			default:
				if (cgood(x, y, 1, 0)) {
					item[x][y] = it;
					know[x][y] = 0;
					if (it == OMAXGOLD)
						iarg[x][y] = arg / 100;
					else
						iarg[x][y] = arg;
					return;
				}
			}	/* end switch */
		}		/* end else */
	}			/* end for */
}

/*
 * cast() 		Subroutine called by parse to cast a spell for the
 * user 
 *
 */
static char     eys[] = "\nEnter your spell: ";

cast()
{
	int             i, j, a, b, d;

	cursors();
	if (c[SPELLS] <= 0) {
		lprcat("\nYou don't have any spells!");
		return;
	}
	do {
		lprcat(eys);
		while ((a = getcharacter()) == 'D') {
			seemagic(-1);
			cursors();
			lprcat(eys);
		}
		if (a == ESC)
			goto over;	/* to escape casting a spell	 */
		lprc(a);
		lflush;
		if ((b = getcharacter()) == ESC)
			goto over;	/* to escape casting a spell */
		lprc(b);
		lflush;
		if ((d = getcharacter()) == ESC) {
over:
			lprcat("aborted.");
			return;
		}			/* to escape casting a spell */
		--c[SPELLS];
		/* seq search for his spell, hash? */
		lprc(d);
		for (j = -1, i = 0; i < SPNUM; i++)
			if (spelcode[i][0]==a && spelcode[i][1]==b && spelcode[i][2]==d)
				if (spelknow[i]) {
					lprintf(" (%s)",spelname[i]);
					speldamage(i);
					j = 1;
					i = SPNUM;
				}
		bottomline();
	} while (j == -1);
}

/*
 * speldamage(x)	Function to perform spell functions cast by the
 * player int x; 
 *
 * Enter with the spell number, returns no value. Please insure that there are 2
 * spaces before all messages here 
 */
speldamage(x)
int             x;
{
	int             i, j, clev;
	int             xl, xh, yl, yh;
	char          *it, *kn, *pm;
	char           *s;

	if (x >= SPNUM)
		return;		/* no such spell */

	if (c[TIMESTOP]) {
		lprcat("\n  It didn't seem to work.");
		return;
	}			/* not if time stopped */
	if ((rnd(23) == 7) || (rnd(18) > c[INTELLIGENCE])) {
		lprcat("\n  It didn't work!");
		return;
	}
	clev = c[LEVEL];
	if (clev * 3 + 2 < x) {
		lprcat("\n  Nothing happens.  You seem inexperienced.");
		return;
	}
	switch (x) {
		/* ----- LEVEL 1 SPELLS ----- */

	case 0:
		if (c[PROTECTIONTIME] == 0)
			c[MOREDEFENSES] += 2;	/* protection field +2 */
		c[PROTECTIONTIME] += 250;
		return;

	case 1:		/* magic missile */
		i = rnd(((clev + 1) << 1)) + clev + 3;
		godirect(x, i, (clev >= 2) ?
			 "  Your missiles hit the %s." :
			 "  Your missile hit the %s.", 100, '+');
		return;

	case 2:		/* dexterity	 */
		if (c[DEXCOUNT] == 0)
			c[DEXTERITY] += 3;
		c[DEXCOUNT] += 400;
		return;

	case 3:		/* sleep	 */
		i = rnd(3) + 1;
		s = "  While the %s slept, you smashed it %d times.";
ws:
		direct(x, fullhit(i), s, i);
		return;

	case 4:		/* charm monster */
		c[CHARMCOUNT] += c[CHARISMA] << 1;
		return;

	case 5:		/* sonic spear */
		godirect(x, rnd(10) + 15 + clev, 
			"  The sound damages the %s.", 70, '@');
		return;

		/* ----- LEVEL 2 SPELLS ----- */
	case 6:
		i = rnd(3) + 2;
		s = "  While the %s is entangled, you hit it %d times.";
		goto ws;	/* web */

	case 7:
		if (c[STRCOUNT] == 0)
			c[STREXTRA] += 3;	/* strength */
		c[STRCOUNT] += 150 + rnd(100);
		return;

	case 8:
		yl = playery - 5;	/* enlightenment */
		yh = playery + 6;
		xl = playerx - 15;
		xh = playerx + 16;
		vxy(xl, yl);
		vxy(xh, yh);	/* check bounds */
		for (i = yl; i <= yh; i++)	/* enlightenment	 */
			for (j = xl; j <= xh; j++)
				know[j][i] = 1;
		draws(xl, xh + 1, yl, yh + 1);
		return;

	case 9:		/* healing */
		raisehp(20 + (clev << 1));
		return;

	case 10:		/* cure blindness	 */
		c[BLINDCOUNT] = 0;
		return;

	case 11:
		if (wizard) {
			lprintf("Number: ");
			if ((i = readnum((long) -1)) != -1) {
				createmonster(i);
				return;
			}
		}
		createmonster(makemonst(level + 1) + 8);
		return;

	case 12:
		if (rnd(11) + 7 <= c[WISDOM])
			direct(x, rnd(20) + 20 + clev, "  The %s believed!", 0);
		else
			lprcat("  It didn't believe the illusions!");
		return;


	case 13:		/* if he has the amulet of invisibility then
				 * add more time */
		for (j = i = 0; i < IVENSIZE; i++)
			if (iven[i] == OAMULET)
				j += 1 + ivenarg[i];
		c[INVISIBILITY] += (j << 7) + 12;
		return;

		/* ----- LEVEL 3 SPELLS ----- */
	case 14:
		godirect(x, rnd(25 + clev) + 25 + clev,
			 "  The fireball hits the %s.", 40, '*');
		return;		/* fireball */

	case 15:		/* cold */
		godirect(x, rnd(25) + 20 + clev,
			 "  The cone of cold strikes the %s.", 60, 'O');
		return;

	case 16:		/* polymorph */
		dirpoly(x);
		return;

	case 17:		/* cancellation	 */
		c[CANCELLATION] += 5 + clev;
		return;

	case 18:		/* haste self	 */
		c[HASTESELF] += 7 + clev;
		return;

	case 19:		/* cloud kill */
		omnidirect(x, 30 + rnd(10), "  The %s gasps for air!");
		return;

	case 20:		/* vaporize rock */
		xh = min(playerx + 1, MAXX - 2);
		yh = min(playery + 1, MAXY - 2);
		for (i = max(playerx - 1, 1); i <= xh; i++)
			for (j = max(playery - 1, 1); j <= yh; j++) {
				kn = &know[i][j];
				pm = &mitem[i][j].mon;
				switch (*(it = &item[i][j])) {
				case OWALL:
					/* can't vpr below V2 */
					if (level < VBOTTOM-2)
						*it = *kn = 0;
					break;
				case OSTATUE:
					if (c[HARDGAME] > 3 && rnd(60) < 30)
						break;
					*it = OBOOK;
					iarg[i][j] = level;
					*kn = 0;
					break;
				case OTHRONE:
					*pm = GNOMEKING;
					*kn = 0;
					*it = OTHRONE2;
					hitp[i][j] = monster[GNOMEKING].hitpoints;
					break;
				case OALTAR:
					*pm = DEMONPRINCE;
					*kn = 0;
					hitp[i][j] = monster[DEMONPRINCE].hitpoints;
					createmonster(DEMONPRINCE);
					createmonster(DEMONPRINCE);
					createmonster(DEMONPRINCE);
					createmonster(DEMONPRINCE);
					break;
				};
				switch (*pm) {
					/* Xorn takes damage from vpr */
				case XORN:
					ifblind(i, j);
					hitm(i, j, 200);
					break;
				case TROLL:
					ifblind(i, j);
					hitm(i, j, 200);
					break;
				}
			}
		return;

		/* ----- LEVEL 4 SPELLS ----- */
	case 21:		/* dehydration */
		direct(x, 100 + clev, "  The %s shrivels up.", 0);
		return;

	case 22:		/* lightning */
		godirect(x, rnd(25) + 20 + (clev << 1),
			 "  A lightning bolt hits the %s.", 1, '~');
		return;

	case 23:		/* drain life */
		i = min(c[HP] - 1, c[HPMAX] / 2);
		direct(x, i + i, "", 0);
		c[HP] -= i;
		return;

	case 24:
		if (c[GLOBE] == 0)
			c[MOREDEFENSES] += 10;
		c[GLOBE] += 200;
		loseint();	/* globe of invulnerability */
		return;

	case 25:		/* flood */
		omnidirect(x, 32 + clev,
			   "  The %s struggles for air in your flood!");
		return;

	case 26:		/* finger of death */
		if (rnd(151) == 63) {
			beep();
			lprcat("\nYour heart stopped!\n");
			nap(4000);
			died(270);
			return;
		}
		if (c[WISDOM] > rnd(10) + 10)
			direct(x, 2000,
			       "  The %s's heart stopped.", 0);
		else
			lprcat("  It didn't work.");
		return;

		/* ----- LEVEL 5 SPELLS ----- */

	case 27:		/* scare monster */
		c[SCAREMONST] += rnd(10) + clev;

		/* if have HANDofFEAR make last longer */
		for (j = i = 0; i < IVENSIZE; i++)
			if (iven[i] == OHANDofFEAR) {
				c[SCAREMONST] *= 3;
				break;
			}
		return;

	case 28:		/* hold monster */
		c[HOLDMONST] += rnd(10) + clev;
		return;

	case 29:
		c[TIMESTOP] += rnd(20) + (clev << 1);
		return;		/* time stop */

	case 30:		/* teleport */
		tdirect(x);
		return;

	case 31:		/* magic fire */
		omnidirect(x, 35 + rnd(10) + clev, "  The %s cringes from the flame.");
		return;

		/* ----- LEVEL 6 SPELLS ----- */
	case 32:		/* make wall */
		makewall(x);
		return;

	case 33:		/* sphere of annihilation */
		if ((rnd(23) == 5) && (wizard == 0)) {
			beep();
			lprcat("\nYou have been enveloped by the zone of nothingness!\n");
			lflush();
			nap(4000);
			died(258);
			return;
		}
		xl = playerx;
		yl = playery;
		loseint();
		i = dirsub(&xl, &yl);	/* get direction of sphere */
		newsphere(xl, yl, i, rnd(20) + 11);	/* make a sphere */
		return;

	case 34:		/* genocide */
		genmonst();
		spelknow[34] = 0;
		loseint();
		return;

	case 35:		/* summon demon */
		if (rnd(100) > 30) {
			direct(x - 1, 150, "  The demon strikes at the %s.", 0);
			return;
		}
		if (rnd(100) > 15) {
			lprcat("  Nothing seems to have happened.");
			return;
		}
		lprcat("  The demon turned on you and then vanished!");
		beep();
		i = rnd(40) + 30;
		lastnum = 277;
		losehp(i);	/* must say killed by a demon */
		return;

	case 36:		/* walk through walls */
		c[WTW] += rnd(10) + 5;
		return;

	case 37:		/* alter reality */
		{
			struct isave   *save;
			/* pointer to item save structure */
			int             sc;
			sc = 0;	/* # items saved */

			if ((save = (struct isave *) malloc(sizeof(struct isave) * MAXX * MAXY * 2))
			    == (struct isave *) NULL) {
				died(285);
			}
			/* save all items and monsters */
			for (j = 0; j < MAXY; j++)
				for (i = 0; i < MAXX; i++) {
					xl = item[i][j];
					if (xl && xl != OWALL
					    && xl != OANNIHILATION) {
						save[sc].type = 0;
						save[sc].id = item[i][j];
						save[sc++].arg = iarg[i][j];
					}
					if (mitem[i][j].mon) {
						save[sc].type = 1;
						save[sc].id = mitem[i][j].mon;
						save[sc++].arg = hitp[i][j];
					}
					item[i][j] = OWALL;
					mitem[i][j].mon = 0;
					if (wizard)
						know[i][j] = 1;
					else
						know[i][j] = 0;
				}
			eat(1, 1);
			if (level == 1)
				item[33][MAXY - 1] = 0;
			for (j = rnd(MAXY - 2), i = 1; i < MAXX - 1; i++)
				item[i][j] = 0;
			/* put objects back in level */
			while (sc > 0) {
				--sc;
				if (save[sc].type == 0) {
					int             trys;
					for (trys = 100, i = j = 1;
					     --trys > 0 && item[i][j];
					     i = rnd(MAXX - 1), j = rnd(MAXY - 1));
					if (trys) {
						item[i][j] = save[sc].id;
						iarg[i][j] = save[sc].arg;
					}
				} else {	/* put monsters back in */
					int             trys;
					for (trys = 100, i = j = 1;
					  --trys > 0 && (item[i][j] == OWALL
							 || mitem[i][j].mon);
					     i = rnd(MAXX - 1), j = rnd(MAXY - 1));
					if (trys) {
						mitem[i][j].mon = save[sc].id;
						hitp[i][j] = save[sc].arg;
					}
				}
			}
			loseint();
			draws(0, MAXX, 0, MAXY);
			if (wizard == 0)
				spelknow[37] = 0;
			if (save)
				free((char *) save);
			positionplayer();
			return;
		}

	case 38:		/* permanence */
		adjusttime(-99999L);
		spelknow[38] = 0;	/* forget */
		loseint();
		return;

	default:
		lprintf("  spell %d not available!", (long) x);
		beep();
		return;
	};
}

/*
 * loseint()		Routine to subtract 1 from your int (intelligence) if
 * > 3 
 *
 */
loseint()
{
	if (--c[INTELLIGENCE] < 3)
		c[INTELLIGENCE] = 3;
}

/*
 * isconfuse() 		Routine to check to see if player is confused 
 *
 * This routine prints out a message saying "You can't aim your magic!" returns
 * 0 if not confused, non-zero (time remaining confused) if confused 
 */
isconfuse()
{
	if (c[CONFUSE]) {
		lprcat(" You can't aim your magic!");
		beep();
	}
	return (c[CONFUSE]);
}

/*
 * Routine to return 1 if a spell doesn't affect a monster nospell(x,monst)
 * int x,monst; 
 *
 * Subroutine to return 1 if the spell can't affect the monster otherwise
 * returns 0 Enter with the spell number in x, and the monster number in
 * monst. 
 */
nospell(x, monst)
	int             x, monst;
{
	int             tmp;

	/* bad spell or monst */
	if (x >= SPNUM || monst > MAXMONST + 8 || monst < 0 || x < 0)
		return (0);

	if ((tmp = spelweird[monst - 1][x]) == 0)
		return (0);
	cursors();
	lprc('\n');
	lprintf(spelmes[tmp], monster[monst].name);
	return (1);
}

/*
 * Function to return full damage against a monster (aka web) fullhit(xx)
 * nt xx; 
 *
 * Function to return hp damage to monster due to a number of full hits Enter
 * with the number of full hits being done 
 */
fullhit(xx)
	int             xx;
{
	int             i;

	if (xx < 0 || xx > 20)
		return (0);	/* fullhits are out of range */
	if (c[LANCEDEATH])
		return (10000);	/* lance of death */

	i = xx * ((c[WCLASS] >> 1) + c[STRENGTH] + c[STREXTRA] - c[HARDGAME] - 12 + c[MOREDAM]);

	return ((i >= 1) ? i : xx);
}

/*
 * Routine to direct spell damage 1 square in 1 dir direct(spnum,dam,str,arg)
 * int spnum,dam,arg; char *str; 
 *
 * Routine to ask for a direction to a spell and then hit the monster Enter with
 * the spell number in spnum, the damage to be done in dam, lprintf format
 * string in str, and lprintf's argument in arg. Returns no value. 
 */
direct(spnum, dam, str, arg)
	int             spnum, dam, arg;
	char           *str;
{
	int             x, y;
	int             m;

	if (spnum < 0 || spnum >= SPNUM || str == 0)
		return;		/* bad arguments */
	if (isconfuse())
		return;
	dirsub(&x, &y);
	m = mitem[x][y].mon;
	if (item[x][y] == OMIRROR) {
		if (spnum == 3) {	/* sleep */
			lprcat("You fall asleep! ");
			beep();
	fool:
			arg += 2;
			while (arg-- > 0) {
				parse2();
				nap(1000);
			}
			return;
		}
		/* web */
		else if (spnum == 6) {
			lprcat("You get stuck in your own web! ");
			beep();
			goto fool;
		} else {
			lastnum = 278;
			lprintf(str, "spell caster (that's you)", (long) arg);
			beep();
			losehp(dam);
			return;
		}
	}
	if (m == 0) {
		lprcat("  There wasn't anything there!");
		return;
	}
	ifblind(x, y);
	if (nospell(spnum, m)) {
		lasthx = x;
		lasthy = y;
		return;
	}
	lprintf(str, lastmonst, (long) arg);
	hitm(x, y, dam);
}

/*
 * Function to perform missile attacks godirect(spnum,dam,str,delay,cshow)
 * nt spnum,dam,delay; char *str,cshow; 
 *
 * Function to hit in a direction from a missile weapon and have it keep on
 * going in that direction until its power is exhausted Enter with the spell
 * number in spnum, the power of the weapon in hp, lprintf format string in
 * str, the # of milliseconds to delay between locations in delay, and the
 * character to represent the weapon in cshow. Returns no value. 
 */
godirect(spnum, dam, str, delay, cshow)
int             spnum, dam, delay;
char           *str, cshow;
{
	char          *it;
	int             x, y, m;
	int             dx, dy;

	if (spnum < 0 || spnum >= SPNUM || str == 0 || delay < 0)
		return;		/* bad args */

	if (isconfuse())
		return;

	dirsub(&dx, &dy);
	x = dx;
	y = dy;
	dx = x - playerx;
	dy = y - playery;
	x = playerx;
	y = playery;

	while (dam > 0) {
		x += dx;
		y += dy;
		if ((x > MAXX - 1) || (y > MAXY - 1) || (x < 0) || (y < 0)) {
			dam = 0;
			break;	/* out of bounds */
		}
		/* if energy hits player */
		if ((x == playerx) && (y == playery)) {
			cursors();
			lprcat("\nYou are hit by your own magic!");
			beep();
			lastnum = 278;
			losehp(dam);
			return;
		}
		/* if not blind show effect */
		if (c[BLINDCOUNT] == 0) {
			cursor(x + 1, y + 1);
			lprc(cshow);
			nap(delay);
			show1cell(x, y);
		}
		/* is there a monster there? */
		if ((m = mitem[x][y].mon) != 0) {
			ifblind(x, y);
			/* cannot cast a missile spell at lucifer!! */
		    	if (m == LUCIFER || (m >= DEMONLORD && rnd(100) < 10)){
				dx *= -1;
				dy *= -1;
				cursors();
				lprc('\n');
				lprintf("\nthe %s returns your puny missile!", 
					monster[m].name);
			} else {
				if (nospell(spnum, m)) {
					lasthx = x;
					lasthy = y;
					return;
				}
				cursors();
				lprc('\n');
				lprintf(str, lastmonst);
				dam -= hitm(x, y, dam);
				show1cell(x, y);
				nap(1000);
				x -= dx;
				y -= dy;
			}
		} else
			switch (*(it = &item[x][y])) {
			case OWALL:
				cursors();
				lprc('\n');
				lprintf(str, "wall");
				if (dam >= 50 + c[HARDGAME]) /*enough damage?*/
					/* can't break wall below V2 */
					if (level < VBOTTOM-2)
					  if ((x < MAXX - 1) && (y < MAXY - 1) 
					      && (x) && (y)) {
						lprcat("  The wall crumbles.");
					god3:
							*it = 0;
					god:
							know[x][y] = 0;
							show1cell(x, y);
						}
		god2:
				dam = 0;
				break;

			case OCLOSEDDOOR:
				cursors();
				lprc('\n');
				lprintf(str, "door");
				if (dam >= 40) {
					lprcat("  The door is blasted apart.");
					goto god3;
				}
				goto god2;

			case OSTATUE:
				cursors();
				lprc('\n');
				lprintf(str, "statue");
				if (dam > 44) {
					if (c[HARDGAME] > 3 && rnd(60) < 30)
						goto god2;
					lprcat("  The statue crumbles.");
					*it = OBOOK;
					iarg[x][y] = level;
					goto god;
				}
				goto god2;

			case OTHRONE:
				cursors();
				lprc('\n');
				lprintf(str, "throne");
				if (dam > 33) {
					mitem[x][y].mon = GNOMEKING;
				       hitp[x][y]=monster[GNOMEKING].hitpoints;
					*it = OTHRONE2;
					goto god;
				}
				goto god2;

			case OMIRROR:
				dx *= -1;
				dy *= -1;
				break;
			};
		dam -= 3 + (int) (c[HARDGAME] >> 1);
	}
}

/*
 * Routine to put "monster" or the monster name into lastmosnt ifblind(x,y)
 * int x,y; 
 *
 * Subroutine to copy the word "monster" into lastmonst if the player is blind.
 * Enter with the coordinates (x,y) of the monster Returns no value. 
 */
ifblind(x, y)
	int             x, y;
{
	char           *s;

	vxy(x, y);		/* verify correct x,y coordinates */
	if (c[BLINDCOUNT]) {
		lastnum = 279;
		s = "monster";
	} else {
		lastnum = mitem[x][y].mon;
		s = monster[lastnum].name;
	}
	strcpy(lastmonst, s);
}

/*
 * tdirect(spnum)		Routine to teleport away a monster int spnum; 
 *
 * Routine to ask for a direction to a spell and then teleport away monster
 * Enter with the spell number that wants to teleport away Returns no value. 
 */
tdirect(spnum)
	int             spnum;
{
	int             x, y;
	int             m;

	if (spnum < 0 || spnum >= SPNUM)
		return;		/* bad args */
	if (isconfuse())
		return;
	dirsub(&x, &y);
	if ((m = mitem[x][y].mon) == 0) {
		lprcat("  There wasn't anything there!");
		return;
	}
	ifblind(x, y);
	if (nospell(spnum, m)) {
		lasthx = x;
		lasthy = y;
		return;
	}
	teleportmonst(x,y,m);
}


makewall(spnum)
	int             spnum;
{
	int             x, y;

	if (spnum < 0 || spnum >= SPNUM)
		return;		/* bad args */
	if (isconfuse())
		return;
	dirsub(&x, &y);

	if ((y >= 0) && (y <= MAXY - 1) && (x >= 0) && (x <= MAXX - 1))	/* within bounds? */
		if (item[x][y] != OWALL) {	/* can't make anything on
						 * walls */
			if (item[x][y] == 0) {	/* is it free of items? */
				if (mitem[x][y].mon == 0) {	/* is it free of
							 * monsters? */
					if ((level != 1) || (x != 33) || (y != MAXY - 1)) {
						item[x][y] = OWALL;
						know[x][y] = 1;
						show1cell(x, y);
					} else
						lprcat("\nyou can't make a wall there!");
				} else
					lprcat("\nthere's a monster there!");
			} else
				lprcat("\nthere's something there already!");
		} else
			lprcat("\nthere's a wall there already!");
}

/*
 * Routine to damage all monsters 1 square from player 
 *
 * omnidirect(sp,dam,str)   int sp,dam; char *str; 
 *
 * Routine to cast a spell and then hit the monster in all directions Enter with
 * the spell number in sp, the damage done to wach square in dam, and the
 * lprintf string to identify the spell in str. Returns no value. 
 */
omnidirect(spnum, dam, str)
	int             spnum, dam;
	char           *str;
{
	int             x, y, m;

	if (spnum < 0 || spnum >= SPNUM || str == 0)
		return;		/* bad args */
	for (x = playerx - 1; x < playerx + 2; x++)
		for (y = playery - 1; y < playery + 2; y++) {
			if ((m = mitem[x][y].mon) != 0)
				if (nospell(spnum, m) == 0) {
					ifblind(x, y);
					cursors();
					lprc('\n');
					lprintf(str, lastmonst);
					hitm(x, y, dam);
					nap(800);
				} else {
					lasthx = x;
					lasthy = y;
				}
		}
}

/*
 * Routine to ask for direction, then modify x,y for it static dirsub(x,y)
 * nt *x,*y; 
 *
 * Function to ask for a direction and modify an x,y for that direction Enter
 * with the origination coordinates in (x,y). Returns index into diroffx[]
 * (0-8). 
 */
dirsub(x, y)
	int            *x, *y;
{
	int             i;

	lprcat("\nIn What Direction? ");
	for (i = 0;;)
		switch (getcharacter()) {
		case 'b':
			i++;
		case 'n':
			i++;
		case 'y':
			i++;
		case 'u':
			i++;
		case 'h':
			i++;
		case 'k':
			i++;
		case 'l':
			i++;
		case 'j':
			i++;
			goto out;
		};
out:
	*x = playerx + diroffx[i];
	*y = playery + diroffy[i];
	vxy(*x, *y);
	return (i);
}

/*
 * Routine to verify/fix coordinates for being within bounds 
 *
 * vxy(x,y)	   int *x,*y; 
 *
 * Function to verify x & y are within the bounds for a level If *x or *y is not
 * within the absolute bounds for a level, fix them so that they are on the
 * level. Returns TRUE if it was out of bounds, and the *x & *y in the
 * calling routine are affected. 
 */
verifyxy(x, y)
int            *x, *y;
{
	int             flag = 0;

	if (*x < 0) {
		*x = 0;
		flag++;
	}
	if (*y < 0) {
		*y = 0;
		flag++;
	}
	if (*x >= MAXX) {
		*x = MAXX - 1;
		flag++;
	}
	if (*y >= MAXY) {
		*y = MAXY - 1;
		flag++;
	}
	return (flag);
}

/*
 * Routine to ask for a direction and polymorph a monst 
 *
 * dirpoly(spnum)		int spnum; 
 *
 * Subroutine to polymorph a monster and ask for the direction its in Enter with
 * the spell number in spmun. Returns no value. 
 */
dirpoly(spnum)
	int             spnum;
{
	int             x, y, m;

	if (spnum < 0 || spnum >= SPNUM)
		return;		/* bad args */
	if (isconfuse())
		return;		/* if he is confused, he can't aim his magic */
	dirsub(&x, &y);
	if (mitem[x][y].mon == 0) {
		lprcat("  There wasn't anything there!");
		return;
	}
	ifblind(x, y);
	if (nospell(spnum, mitem[x][y].mon)) {
		lasthx = x;
		lasthy = y;
		return;
	}
	while ((monster[m = mitem[x][y].mon = rnd(MAXMONST + 7)]
			  .flags & FL_GENOCIDED) != 0)
        /* EMPTY */;
	hitp[x][y] = monster[m].hitpoints;
	show1cell(x, y);	/* show the new monster */
}

/*
 * Function to hit a monster at the designated coordinates hitmonster(x,y) 
 * int x,y; 
 *
 * This routine is used for a bash & slash type attack on a monster Enter with
 * the coordinates of the monster in (x,y). Returns no value. 
 */
hitmonster(x, y)
int             x, y;
{
	int             tmp, monst, damag, flag;

	if (c[TIMESTOP])
		return;		/* not if time stopped */

	vxy(x, y);		/* verify coordinates are within range */

	if ((monst = mitem[x][y].mon) == 0)
		return;

	hit3flag = 1;
	ifblind(x, y);

	tmp = monster[monst].armorclass + c[LEVEL] + c[DEXTERITY] +
		c[WCLASS] / 4 - 12 - c[HARDGAME];
	cursors();

	/* need at least random chance to hit */
	if ((rnd(20) < tmp) || (rnd(71) < 5)) {
		lprcat("\nYou hit");
		flag = 1;
		damag = fullhit(1);
		if (damag < 9999)
			damag = rnd(damag) + 1;
	} else {
		lprcat("\nYou missed");
		flag = 0;
	}
	lprintf(" the %s.", lastmonst);

	/*
	 *  If the monser was hit, deal with weapon dulling.
	 */
	if (flag && (monst==RUSTMONSTER || monst==DISENCHANTRESS || monst==CUBE)
		&& c[WIELD] > 0)
		/* if it's not already dulled to hell */
		if (((ivenarg[c[WIELD]] > -10) &&
			 ((iven[c[WIELD]] == OSLAYER) ||
			  (iven[c[WIELD]] == ODAGGER) ||
			  (iven[c[WIELD]] == OSPEAR) ||
			  (iven[c[WIELD]] == OFLAIL) ||
			  (iven[c[WIELD]] == OBATTLEAXE) ||
			  (iven[c[WIELD]] == OLONGSWORD) ||
			  (iven[c[WIELD]] == O2SWORD) ||
			  (iven[c[WIELD]] == OLANCE) ||
			  (iven[c[WIELD]] == OHAMMER) ||
			  (iven[c[WIELD]] == OVORPAL) ||
			  (iven[c[WIELD]] == OBELT)))
			|| (ivenarg[c[WIELD]] > 0)) {
			lprintf("\nYour weapon is dulled by the %s.", lastmonst);
			beep();
			--ivenarg[c[WIELD]];
		} else if (ivenarg[c[WIELD]] <= -10) {
			lprintf("\nYour weapon disintegrates!");
			iven[c[WIELD]] = ivenarg[c[WIELD]] = 0;
			c[WIELD] = -1;
			flag = 0; /* Didn't hit after all... */
		}

	if (flag) {
		hitm(x, y, damag);
		if ((monst >= DEMONLORD) && (c[LANCEDEATH]) && (hitp[x][y]))
			lprintf("\nYour lance of death tickles the %s!", lastmonst);
	}
	if (monst == METAMORPH)
		if (hitp[x][y] < 25 && hitp[x][y] > 0) {
			mitem[x][y].mon = BRONZEDRAGON + rund(9);
			show1cell(x, y);
		}
	if (mitem[x][y].mon == LEMMING)
		if (rnd(100) <= 40)
			createmonster(LEMMING);
}

/*
 * Function to just hit a monster at a given coordinates 
 * hitm(x,y,amt)
 * int x,y,amt
 *
 * Returns the number of hitpoints the monster absorbed This routine is used to
 * specifically damage a monster at a location (x,y) Called by
 * hitmonster(x,y) 
 */
hitm(x, y, amt)
int x, y, amt;
{
	int             monst;
	int             hpoints, amt2;

	vxy(x, y);		/* verify coordinates are within range */
	amt2 = amt;		/* save initial damage so we can return it */
	monst = mitem[x][y].mon;

	/* if half damage curse adjust damage points */
	if (c[HALFDAM])
		amt >>= 1;
	if (amt <= 0)
		amt2 = amt = 1;

	lasthx = x;
	lasthy = y;

	/* make sure hitting monst breaks stealth condition */
	stealth[x][y] = 1;
	c[HOLDMONST] = 0;	/* hit a monster breaks hold monster spell */

	/* if a dragon and orb(s) of dragon slaying	 */
	if (c[SLAYING])
		switch (monst) {
		case WHITEDRAGON:
		case REDDRAGON:
		case GREENDRAGON:
		case BRONZEDRAGON:
		case PLATINUMDRAGON:
		case SILVERDRAGON:
			amt *= 3;
			break;
		}

	/* Deal with Vorpy */
	if (c[WIELD] > 0 && iven[c[WIELD]] == OVORPAL && rnd(20) == 1 &&
		(monster[monst].flags & FL_HEAD) != 0 &&
		(monster[monst].flags & FL_NOBEHEAD) == 0) {
		lprintf("\nThe Vorpal Blade beheads the %s!", lastmonst);
		amt = hitp[x][y];
	}

	/* invincible monster fix is here */
	if (hitp[x][y] > monster[monst].hitpoints)
		hitp[x][y] = monster[monst].hitpoints;

	if (monst >= DEMONLORD) {
		if (c[LANCEDEATH])
			amt = 300;
		if (iven[c[WIELD]] == OSLAYER)
			amt = 10000;
	}
	if ((hpoints = hitp[x][y]) <= amt) {
		int i;

		lprintf("\nThe %s died!", lastmonst);
		raiseexperience((long) monster[monst].experience);
		disappear(x, y);

		if (mitem[x][y].n > 0) {
			for (i=0;i<mitem[x][y].n;i++) {
				createitem(x, y, 
					mitem[x][y].it[i].item, 
					mitem[x][y].it[i].itemarg);
			}
			beenhere[level] -= mitem[x][y].n;
			if (beenhere[level] < 1) beenhere[level]=1;
			mitem[x][y].n = 0;
			if ((amt = monster[monst].gold) > 0)
				dropgold(rnd(amt) + amt);
		} else {
			if ((amt = monster[monst].gold) > 0)
				dropgold(rnd(amt) + amt);
			dropsomething(x,y,monst);
		}
		show1cell(x,y);
		showcell(playerx,playery);
		bottomline();
		hitp[x][y] = 0;
		return (hpoints);
	}
	hitp[x][y] = hpoints - amt;
	return (amt2);
}

/*
 *			Function for the monster to hit the player from (x,y)
 *	hitplayer(x,y) 		
 *		int x,y;
 *
 *	Function for the monster to hit the player with monster at location x,y
 *	Returns nothing of value.
 */
hitplayer (x, y)
int x, y;
{
	register int dam,tmp,mster,bias;

	vxy(x,y);	/* verify coordinates are within range */

	lastnum = mster = mitem[x][y].mon;

	if ((know[x][y]&1) == 0) {
		know[x][y]=1; 
		show1cell(x,y);
	}

	bias = (c[HARDGAME]) + 1;
	hitflag = hit2flag = hit3flag = 1;
	yrepcount=0;
	cursors();	
	ifblind(x,y);

	if (mster==LEMMING) 
		return;

	if (mster < DEMONLORD)
		if (c[INVISIBILITY]) if (rnd(33)<20) {
			lprintf("\nThe %s misses wildly!",lastmonst);	
			return;
		}

	if ( (mster < DEMONLORD) && (mster != PLATINUMDRAGON) ) 
		if (c[CHARMCOUNT]) 
			if (rnd(30)+5*monster[mster].level-c[CHARISMA]<30) {
	lprintf("\nThe %s is awestruck by your magnificence!",lastmonst);
				return;
			}

	dam = monster[mster].damage;
	dam += rnd( (int) ((dam<1)?1:dam) ) + monster[mster].level;

	/* demon lords/prince/god of hellfire damage is reduced if wielding
			Slayer */
	if (mster >= DEMONLORD)
		if (iven[c[WIELD]]==OSLAYER)
			dam=(int) (1 - (0.1 * rnd(5)) * dam);

	/*	spirit naga's and poltergeist's damage is halved if scarab of 
	  	negate spirit	*/
	if (c[NEGATESPIRIT] || c[SPIRITPRO])  
		if ((mster ==POLTERGEIST) || (mster ==SPIRITNAGA)) 
			dam = (int) dam/2; 

	/*	halved if undead and cube of undead control	*/
	if (c[CUBEofUNDEAD] || c[UNDEADPRO]) 
		if ((mster ==VAMPIRE) || (mster ==WRAITH) || (mster ==ZOMBIE)) 
			dam = (int) dam/2;

	tmp = 0;
	if (monster[mster].attack>0)
		if (((dam + bias + 8) > c[AC]) 
	 	    || (rnd((int)((c[AC]>0)?c[AC]:1))==1)) { 	
			if (spattack(monster[mster].attack,x,y)) { 	
				flushall(); 
				return; 
			}
			tmp = 1;  
			bias -= 2; 
			cursors(); 
		}

	if (((dam + bias) > c[AC]) || (rnd((int)((c[AC]>0)?c[AC]:1))==1)) {
		lprintf("\n  The %s hit you.",lastmonst);	
		tmp = 1;
		if ((dam -= c[AC]) < 0) 
			dam=0;

		if (dam > 0) { 
			losehp(dam); 
			bottomhp(); 
			flushall(); 
		}
	}

	if (tmp == 0)  
		lprintf("\n  The %s missed.",lastmonst);
}

/*
 *			Function to create an object when a monster dies
 *	dropsomething(monst) 
 *		int monst;
 *
 *	Function to create an object near the player when 
 *	certain monsters are killed
 *	Enter with the monster number 
 *	Returns nothing of value.
 */
dropsomething (x,y,monst)
int x,y;
int monst;
{
	switch(monst) {
	case ORC:  
	case NYMPH:
	case ELF:
	case TROGLODYTE:
	case TROLL:
	case ROTHE:
	case VIOLETFUNGI:
	case PLATINUMDRAGON:
	case GNOMEKING:
	case REDDRAGON:
		something(x,y,level);
		return;
	case LEPRECHAUN: 
		if (rnd(101)>=75) creategem();
		if (rnd(5)==1) dropsomething(x,y,LEPRECHAUN);
	case LEMMING:
		return;
	}
}

/*
 * dropgold(amount) 	Function to drop some gold around player int amount; 
 *
 * Enter with the number of gold pieces to drop Returns nothing of value. 
 */
dropgold(amount)
int amount;
{
	if (amount > 250)
		createitem(playerx, playery, OMAXGOLD, (long) amount);
	else
		createitem(playerx, playery, OGOLDPILE, (long) amount);
}

/*
 * something(x, y, level) Function to create a random item around coords x y
 * int x, y, level; 
 *
 * Function to create an item from a designed probability around player Enter
 * with the cave level on which something is to be dropped Returns nothing of
 * value. 
 */
something(x,y,lev)
int x,y,lev;
{
	int j, i;

	if (lev < 0 || lev > VBOTTOM)
		return;		/* correct level? */
	if (rnd(101) < 8)
		something(x,y,lev);/* possibly more than one item */
	j = newobject(lev, &i);
	createitem(x, y, j, (long) i);
}

/*
 * newobject(lev,i) 	Routine to return a randomly selected new object
 * int lev,*i; 
 *
 * Routine to return a randomly selected object to be created Returns the object
 * number created, and sets *i for its argument Enter with the cave level and
 * a pointer to the items arg 
 */
static char     nobjtab[] = {
			     0, OSCROLL, OSCROLL, OSCROLL, OSCROLL,
			     OPOTION, OPOTION, OPOTION, OPOTION,
			     OGOLDPILE, OGOLDPILE, OGOLDPILE, OGOLDPILE,
			     OBOOK, OBOOK, OBOOK, OBOOK,
			     ODAGGER, ODAGGER, ODAGGER,
			     OLEATHER, OLEATHER, OLEATHER,
		     OREGENRING, OPROTRING, OENERGYRING, ODEXRING, OSTRRING,
		OSPEAR, OBELT, ORING, OSTUDLEATHER, OSHIELD, OFLAIL, OCHAIN,
			     O2SWORD, OPLATE, OLONGSWORD};	/* 38 */

newobject(lev, i)
int             lev, *i;
{
int             tmp = 32, j;

	if (level < 0 || level > VBOTTOM)
		return (0);	/* correct level? */

	if (lev > 6)
		tmp = 37;
	else if (lev > 4)
		tmp = 35;

	j = nobjtab[(tmp = rnd(tmp))];	/* the object type */
	switch (tmp) {
	case 1:
	case 2:
	case 3:
	case 4:
		*i = newscroll();
		break;
	case 5:
	case 6:
	case 7:
	case 8:
		*i = newpotion();
		break;
	case 9:
	case 10:
	case 11:
	case 12:
		*i = rnd((lev + 1) * 10) + lev * 10 + 10;
		break;
	case 13:
	case 14:
	case 15:
	case 16:
		*i = lev;
		break;
	case 17:
	case 18:
	case 19:
		if ((*i = newdagger()) == 0)
			return (0);
		break;
	case 20:
	case 21:
	case 22:
		if ((*i = newleather()) == 0)
			return (0);
		break;
	case 23:
	case 32:
	case 35:
		*i = rund(lev / 3 + 1);
		break;
	case 24:
	case 26:
		*i = rnd(lev / 4 + 1);
		break;
	case 25:
		*i = rund(lev / 4 + 1);
		break;
	case 27:
		*i = rnd(lev / 2 + 1);
		break;
	case 28:
		*i = rund(lev / 3 + 1);
		if (*i == 0)
			return (0);
		break;
	case 29:
	case 31:
		*i = rund(lev / 2 + 1);
		if (*i == 0)
			return (0);
		break;
	case 30:
	case 33:
		*i = rund(lev / 2 + 1);
		break;
	case 34:
		*i = newchain();
		break;
	case 36:
		*i = newplate();
		break;
	case 37:
		*i = newsword();
		break;
	}
	return (j);
}

/*
 * Function to process special attacks from monsters spattack(atckno,xx,yy) 
 * int atckno,xx,yy; 
 *
 * Enter with the special attack number, and the coordinates (xx,yy) of the
 * monster that is special attacking Returns 1 if must do a show1cell(xx,yy)
 * upon return, 0 otherwise 
 *
 * atckno   monster     effect
 * --------------------------------------------------- 
 * 0	none 
 * 1	rust 			eat armor 
 * 2	hell hound		breathe light fire 
 * 3	dragon			breathe fire 
 * 4	giant centipede 	weakening strength
 * 5	white dragon		cold breath 
 * 6	wraith 			drain level 
 * 7	waterlord		water gusher 
 * 8	leprechaun 		steal gold 
 * 9	disenchantress		disenchant weapon or armor 
 * 10	ice lizard		hits with barbed tail 
 * 11	umber hulk 		confusion 
 * 12	spirit naga		cast spells taken from special attacks 
 * 13	platinum dragon		psionics 
 * 14	nymph 			steal objects 
 * 15	bugbear			bite 
 * 16	osequip
 *
 * short int rustarm[ARMORTYPES][2]; special array for maximum rust damage to
 * armor from rustmonster. format is: { armor type , minimum attribute }
 */
#define ARMORTYPES 6

static short    rustarm[ARMORTYPES][2] = {
					  OSTUDLEATHER, -2,
					  ORING, -4,
					  OCHAIN, -5,
					  OSPLINT, -6,
					  OPLATE, -8,
					  OPLATEARMOR, -9
};

static char     spsel[] = {1, 2, 3, 5, 6, 8, 9, 11, 13, 14};

spattack(x, xx, yy)
int             x, xx, yy;
{
	int             i, j = 0, k, m;
	char           *p = 0;

	vxy(xx, yy);		/* verify x & y coordinates */

	/*
	 * cancel only works 5% of time for demon prince and god 
	 */
	if (c[CANCELLATION])
		if (mitem[xx][yy].mon >= DEMONPRINCE) {
			if (rnd(100) >= 95)
				return (0);
		} else
			return (0);

	/* staff of power cancels demonlords/wraiths/vampires 75% of time */
	/* lucifer is unaffected */
	if (mitem[xx][yy].mon != LUCIFER) {
		if ((mitem[xx][yy].mon >= DEMONLORD) ||
		    (mitem[xx][yy].mon == WRAITH) ||
		    (mitem[xx][yy].mon == VAMPIRE))
			for (i = 0; i < IVENSIZE; i++)
				if (iven[i] == OPSTAFF)
					if (rnd(100) < 75)
						return (0);
	}
	/* if have cube of undead control,  wraiths and vampires do nothing */
	if ((mitem[xx][yy].mon == WRAITH) || (mitem[xx][yy].mon == VAMPIRE))
		if ((c[CUBEofUNDEAD]) || (c[UNDEADPRO]))
			return (0);

	switch (x) {
	case 1:		/* rust your armor, j=1 when rusting has
				 * occurred */
		m = k = c[WEAR];
		if ((i = c[SHIELD]) != -1) {
			--ivenarg[i];
			if (ivenarg[i] < -1)
				ivenarg[i] = -1;
			else
				j = 1;
		}
		if ((j == 0) && (k != -1)) {
			m = iven[k];
			for (i = 0; i < ARMORTYPES; i++)
				/* find his armor in table */
				if (m == rustarm[i][0]) {
					--ivenarg[k];
					if (ivenarg[k] < rustarm[i][1])
						ivenarg[k] = rustarm[i][1];
					else
						j = 1;
					break;
				}
		}
		/* if rusting did not occur */
		if (j == 0)
			switch (m) {
			case OLEATHER:
				p = "\nThe %s hit you -- you are lucky you have leather on.";
				break;
			case OSSPLATE:
				p = "\nThe %s hit you -- you are fortunate to have stainless steel armor!";
				break;
			case OELVENCHAIN:
				p = "\nThe %s hit you -- you are very lucky to have such strong elven chain!";
				break;
			}
		else {
			beep();
			p = "\nThe %s hit you -- your armor feels weaker.";
		}
		break;

	case 2:
		i = rnd(15) + 8 - c[AC];
spout:
		p = "\nThe %s breathes fire at you!";
		if (c[FIRERESISTANCE])
			p = "\nThe %s's flame doesn't faze you!";
		else
	spout2:
		if (p) {
			lprintf(p, lastmonst);
			beep();
		}
		checkloss(i);
		return (0);

	case 3:
		i = rnd(20) + 25 - c[AC];
		goto spout;

	case 4:
		if (c[STRENGTH] > 3) {
			p = "\nThe %s stung you!  You feel weaker.";
			beep();
			if (--c[STRENGTH] < 3)
				c[STRENGTH] = 3;
		} else
			p = "\nThe %s stung you!";
		break;

	case 5:
		p = "\nThe %s blasts you with his cold breath.";
		i = rnd(15) + 18 - c[AC];
		goto spout2;

	case 6:
		lprintf("\nThe %s drains you of your life energy!", lastmonst);
		loselevel();
		if (mitem[xx][yy].mon == DEMONPRINCE)
			losemspells(1);
		if (mitem[xx][yy].mon == LUCIFER) {
			loselevel();
			losemspells(2);
			for (i = 0; i <= 5; i++)
				if (c[i]-- < 3)
					c[i] = 3;
		}
		beep();
		return (0);

	case 7:
		p = "\nThe %s got you with a gusher!";
		i = rnd(15) + 25 - c[AC];
		goto spout2;

	case 8:
		/* he has a device of no theft */
		if (c[NOTHEFT])
			return (0);
		if (c[GOLD]) {
			p = "\nThe %s hit you.  Your purse feels lighter.";
			if (c[GOLD] > 32767)
				c[GOLD] >>= 1;
			else
				c[GOLD] -= rnd((int) (1 + (c[GOLD] >> 1)));
			if (c[GOLD] < 0)
				c[GOLD] = 0;
		} else
			p = "\nThe %s couldn't find any gold to steal.";
		lprintf(p, lastmonst);
		disappear(xx, yy);
		beep();
		bottomgold();
		return (1);

	case 9:
		/* disenchant */
		for (j = 50;;) {
			i = rund(IVENSIZE);
			m = iven[i];	/* randomly select item */
			if (m>0 && ivenarg[i]>0 && m!=OSCROLL && m!=OPOTION){
				if ((ivenarg[i] -= 3) < 0)
					ivenarg[i] = 0;
		lprintf("\nThe %s hits you with a spell of disenchantment! ", 
				lastmonst);
				srcount = 0;
				beep();
				show3(i);
				bottomline();
				return (0);
			}
			if (--j <= 0) {
				p = "\nThe %s nearly misses.";
				break;
			}
			break;
		}
		break;

	case 10:
		p = "\nThe %s hit you with its barbed tail.";
		i = rnd(25) - c[AC];
		goto spout2;

	case 11:
		if (wizard)
			return (0);

		p = "\nThe %s has confused you.";
		beep();
		c[CONFUSE] += 10 + rnd(10);
		break;

	case 12:		/* performs any number of other special
				 * attacks	 */
		return (spattack(spsel[rund(10)], xx, yy));

	case 13:
		p = "\nThe %s flattens you with it's psionics!";
		i = rnd(15) + 30 - c[AC];
		goto spout2;

	case 14:
		if (c[NOTHEFT])
			return (0); /* he has device of no theft */

		if (emptyhanded() == 1) {
			p = "\nThe %s couldn't find anything to steal.";
			break;
		}
		lprintf("\nThe %s picks your pocket and takes:", lastmonst);
		beep();
		if (stealsomething(xx,yy) == 0)
			lprcat("  nothing");
		teleportmonst(xx, yy, mitem[xx][yy].mon);
		bottomline();
		return (1);

	case 15:
		i = rnd(10) + 5 - c[AC];
spout3:
		p = "\nThe %s bit you!";
		goto spout2;

	case 16:
		i = rnd(15) + 10 - c[AC];
		goto spout3;
	};

	if (p) {
		lprintf(p, lastmonst);
		bottomline();
	}
	return (0);
}

/*
 * Routine to subtract hp from user and flag bottomline display checkloss(x) 
 * int x; 
 *
 * Enter with the number of hit points to lose Note: if x > c[HP] this routine
 * could kill the player! 
 */
checkloss(x)
	int             x;
{
	if (x > 0) {
		losehp(x);
		bottomhp();
	}
}

/*
 * Routine to annihilate all monsters around player (playerx,playery)
 * annihilate() 	
 *
 * Gives player experience, but no dropped objects Returns the experience gained
 * from all monsters killed 
 */
annihilate()
{
	int             i, j;
	long            k;
	char          *p;

	for (k = 0, i = playerx - 1; i <= playerx + 1; i++)
	    for (j = playery - 1; j <= playery + 1; j++)
		if (!verifyxy(&i, &j))	/* if not out of bounds */
		    if (*(p = &mitem[i][j].mon))	/* if a monster there */
			if (*p < DEMONLORD) {
				k += monster[*p].experience;
				*p = know[i][j] = 0;
			} else {
			  lprintf("\nThe %s barely escapes being annihilated!",
					monster[*p].name);
				/* lose half hit points */
				hitp[i][j] = (hitp[i][j] >> 1) + 1;
			}
	if (k > 0) {
		lprcat("\nYou hear loud screams of agony!");
		raiseexperience((long) k);
	}
}

/*
 * Function to ask for monster and genocide from game genmonst()		
 *
 * This is done by setting a flag in the monster[] structure 
 */
genmonst()
{
	int  i, j;

	cursors();
	lprcat("\nGenocide which monster? ");
	for (i = 0; (!isalpha(i)) && (i != ' ');
	     i = getcharacter());
	lprc(i);
	for (j = 0; j < MAXMONST; j++)	/* search for the monster type */
	    if (monstnamelist[j] == i)	/* have we found it? */
		if ((monster[j].flags & FL_GENOCIDED) == 0 || wizard) {
			monster[j].flags |= FL_GENOCIDED;  /* genocided from game */
			lprintf("  There will be no more %ss.", monster[j].name);
				/* now wipe out monsters on this level */
			newcavelevel(level);
			draws(0, MAXX, 0, MAXY);
			bot_linex();
			return;
		}
	lprcat("  You sense failure!");
}

/*
 * makemonst(lev) int lev; 
 *
 * function to return monster number for a randomly selected monster for the
 * given cave level	 
 */
makemonst(lev)
int             lev;
{
	int             tmp, x;

	if (lev < 1)
		lev = 1;
	if (lev > 12)
		lev = 12;

	tmp = WATERLORD;

	if (lev < 5)
		while (tmp == WATERLORD)
			tmp = rnd(((x = monstlevel[lev - 1]) != 0) ? x : 1);

	else while (tmp == WATERLORD)
    		tmp = rnd(((x=monstlevel[lev-1] - monstlevel[lev-4]) != 0) ? x : 1)
			+ monstlevel[lev - 4];

	while ((monster[tmp].flags & FL_GENOCIDED) != 0 && tmp < MAXMONST)
		tmp++;

	if (level <= DBOTTOM)
		if (rnd(100) < 10)
			tmp = LEMMING;

	return (tmp);
}
/*
	subroutine to randomly create monsters if needed
 */
randmonst ()
{	/*	don't make monsters if time is stopped	*/
	if (c[TIMESTOP]) 
		return;
	if (--rmst <= 0) {
		rmst = 120 - (level<<2);  
		fillmonst(makemonst(level));
	}
}
