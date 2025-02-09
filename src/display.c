/*	display.c */
#include "header.h"
#include "player.h"
#include "itm.h"
#include "monst.h"
#include "extern.h"

static int minx,maxx,miny,maxy,k,m;
static char always=0;
char bot1f=0, bot2f=0, bot3f=0;
static char mimicmonst=MIMIC;

#define botsub(idx,x,y,str)  		\
	if (c[(idx)] != cbak[(idx)]) {	\
		cbak[(idx)]=c[(idx)]; 	\
		cursor((x),(y)); 	\
		lprintf(str,c[(idx)]);	\
	}

void bottomdo()
{
	if (bot1f) {
		bot3f=bot1f=bot2f=0;
		bot_linex();
		return;
	}
	if (bot2f) {
		bot2f=0;
		bot_hpx();
	}
	if (bot3f) {
		bot3f=0;
		bot_spellx();
	}
}

/* update only the gold number on the bottomline, called from ogold() */
void bottomgold() { botsub(GOLD,72,19,"%-8d"); }

/* update number of spells called from regen() */
void bot_spellx() { botsub(SPELLS,9,18,"%2d"); }

void bot_linex()
{
	int i;
	char buf[12];

	if (cbak[SPELLS] <= -50 || (always)) {
		cursor(1,18);
		if (c[SPELLMAX]>99)
			lprintf("Spells:%3d(%3d)", c[SPELLS],c[SPELLMAX]);
		else
			lprintf("Spells:%3d(%2d) ",c[SPELLS],c[SPELLMAX]);
		lprintf(" AC: %-3d  WC: %-3d  Level",c[AC],c[WCLASS]);
		if (c[LEVEL]>99)
			lprintf("%3d",c[LEVEL]);
		else
			lprintf(" %-2d",c[LEVEL]);

		lprintf(" Exp: %-9d %s",c[EXPERIENCE],class[c[LEVEL]-1]);

		cursor(1,19);

		/* lprintf("HP: %4d (%4d) STR=%-2d INT=%-2d ", */
			/* c[HP], c[HPMAX], c[STRENGTH]+c[STREXTRA], */
			/* c[INTELLIGENCE]); */

		sprintf(buf, "%ld (%ld)", c[HP], c[HPMAX]);
		/*       12345   111122   222222   3 */
		/*               678901   345678   0 */
		lprintf("HP: %11s STR=%-2d INT=%-2d ",
			buf, c[STRENGTH]+c[STREXTRA], c[INTELLIGENCE]);

		cursor(31,19);
		/*       33333   333444   444444   555555   5566 */
		/*       12345   789012   456789   123456   8901 */
		lprintf("WIS=%-2d CON=%-2d DEX=%-2d CHA=%-2d LV:",
			c[WISDOM],c[CONSTITUTION],c[DEXTERITY],c[CHARISMA]);

		if ((level==0) || (wizard))
			c[TELEFLAG]=0;
		cursor(62,19);
		if (c[TELEFLAG])
			lprcat(" ?");
		else
			lprintf("%2s", levelname[level]);
		cursor(64,19);
		/*       666666777 */
		/*       456789012 */
		lprintf("  Gold: %-8d",c[GOLD]);
		always=1;
		botside();
		c[TMP] = c[STRENGTH]+c[STREXTRA];
		for (i=0; i<100; i++)
			cbak[i]=c[i];
		return;
	}

	botsub(SPELLS,8,18,"%3d");
	if (c[SPELLMAX]>99) {
		botsub(SPELLMAX,12,18,"%3d)");
	}
	else  {
		botsub(SPELLMAX,12,18,"%2d) ");
	}
	botsub(AC,21,18,"%-3d");
	botsub(WCLASS,30,18,"%-3d");
	botsub(EXPERIENCE,49,18,"%-9d");

	if (c[LEVEL] != cbak[LEVEL]) {
		cursor(59,18);
		lprcat(class[c[LEVEL]-1]);
	}
	if (c[LEVEL]>99)  {
		botsub(LEVEL,40,18,"%3d");
	}
	else  {
		botsub(LEVEL,40,18," %-2d");
	}

	c[TMP] = c[STRENGTH]+c[STREXTRA];
	bothp();
	botsub(TMP,21,19,"%-2d");
	botsub(INTELLIGENCE,28,19,"%-2d");
	botsub(WISDOM,35,19,"%-2d");
	botsub(CONSTITUTION,42,19,"%-2d");
	botsub(DEXTERITY,49,19,"%-2d");
	botsub(CHARISMA,56,19,"%-2d");

	if ((level != cbak[CAVELEVEL]) || (c[TELEFLAG] != cbak[TELEFLAG])) {
		if ((level==0) || (wizard))
			c[TELEFLAG]=0;
		cbak[TELEFLAG] = c[TELEFLAG];
		cbak[CAVELEVEL] = level;
		cursor(62,19);
		if (c[TELEFLAG])
			lprcat(" ?");
		else
			lprintf("%2s", levelname[level]);
	}
	botsub(GOLD,72,19,"%-8d");
	botside();
}


/*
	special routine to update hp and level fields on bottom lines
	called in monster.c hitplayer() and spattack()
 */
void bot_hpx()
{
	if (c[EXPERIENCE] != cbak[EXPERIENCE]) {
		recalc();
		bot_linex();
	}
	else
		bothp();
}

/*
	common subroutine for a more economical bottomline()
 */
static struct bot_side_def {
	int typ;
	char *string;
} bot_data[] = {
	STEALTH,	"Stealth",
	UNDEADPRO,	"Undead Pro",
	SPIRITPRO,	"Spirit Pro",
	CHARMCOUNT,	"Charm",
	TIMESTOP,	"Time Stop",
	HOLDMONST,	"Hold Monst",
	GIANTSTR,	"Giant Str",
	FIRERESISTANCE,"Fire Resit",
	DEXCOUNT,	"Dexterity",
	STRCOUNT,	"Strength",
	SCAREMONST,	"Scare",
	HASTESELF,	"Haste Self",
	CANCELLATION,	"Cancel",
	INVISIBILITY,	"Invisible",
	ALTPRO,		"Protect 3",
	PROTECTIONTIME,	"Protect 2",
	WTW,		"Wall-Walk"
};

void botside()
{
	int i,idx;

	for (i=0; i<17; i++) {
		idx = bot_data[i].typ;
		if ((always) || (c[idx] != cbak[idx])) {
			if ((always) || (cbak[idx] == 0)) {
				if (c[idx]) {
					cursor(70,i+1);
					lprcat(bot_data[i].string);
				}
			}
			else {
				if (c[idx]==0)     {
					cursor(70,i+1);
					lprcat("          ");
				}
			}
			cbak[idx]=c[idx];
		}
	}
	always=0;
}

void bothp()
{
	char buf[12];

	if (c[HP] == cbak[HP] && c[HPMAX] == cbak[HPMAX])
		return;
	cbak[HP]=c[HP];
	cbak[HPMAX]=c[HPMAX];
	sprintf(buf, "%ld (%ld)", c[HP], c[HPMAX]);
	cursor(5, 19);
	lprintf("%11s", buf);
}

/*
 *	subroutine to draw only a section of the screen
 *	only the top section of the screen is updated.
 *	If entire lines are being drawn, then they will be cleared first.
 */
int 	d_xmin=0,
	d_xmax=MAXX,
	d_ymin=0,
	d_ymax=MAXY;	/* for limited screen drawing */
void draws(int xmin,int xmax,int ymin,int ymax)
{
	int i,idx;

		/* clear section of screen as needed */
	if (xmin==0 && xmax==MAXX) {
		if (ymin==0)
			cl_up(79,ymax);
		else
			for (i=ymin; i<ymin; i++)  {
				cl_line(1,i+1);
			}
		xmin = -1;
	}
	d_xmin=xmin;
	d_xmax=xmax;
	d_ymin=ymin;
	d_ymax=ymax;	/* for limited screen drawing */
	drawscreen();

		/* draw stuff on right side of screen as needed*/
	if (xmin<=0 && xmax==MAXX)  {
		for (i=ymin; i<ymax; i++) {
			idx = bot_data[i].typ;
			if (c[idx]) {
				cursor(70,i+1);
				lprcat(bot_data[i].string);
			}
			cbak[idx]=c[idx];
		}
	}
}

short 	screen[MAXX][MAXY],
		d_flag;	/* template for the screen */

/*
**	drawscreen()
**
**	redraw the whole screen as the player knows it
*/
void drawscreen()
{
	int i,j,k;
	int lastx,lasty;  /* used to optimize the object printing */

	if (d_xmin==0 && d_xmax==MAXX && d_ymin==0 && d_ymax==MAXY) {
		d_flag=1;
		clear(); /* clear the screen */
	}
	else {
		d_flag=0;
		cursor(1,1);
	}
	if (d_xmin<0)
		d_xmin=0; /* d_xmin=-1 means display all without bottomline */

	for (i=d_ymin; i<d_ymax; i++)
	    for (j=d_xmin; j<d_xmax; j++)
		if (know[j][i]==0)
		    screen[j][i] = objnamelist[0];
		else {
		    if (k=mitem[j][i].mon)  {
			if (k==MIMIC) {
				if (gtime % 10 == 0)
					while((mimicmonst = rnd(MAXMONST))==INVISIBLESTALKER);
				screen[j][i]=monstnamelist[mimicmonst];
			} else if (k==INVISIBLESTALKER && c[SEEINVISIBLE]==0)
				screen[j][i]=objnamelist[item[j][i]];
			else /* demons are invisible if not have the eye */
			    if (k>=DEMONLORD && k<=LUCIFER && c[EYEOFLARN]==0)
			 	screen[j][i] = objnamelist[item[j][i]];
			else
			    screen[j][i] = monstnamelist[k];
		    } else {
		         if ((k=item[j][i])==OWALL)
		       	    screen[j][i] = objnamelist[OWALL];
		         else
			    screen[j][i] = objnamelist[0]; /* blank */
		    }
		 }

	for (i=d_ymin; i<d_ymax; i++) {
		j=d_xmin;
		while ((screen[j][i]==' ') && (j<d_xmax))
			j++;
		if (j >= d_xmax)
			m=d_xmin; /* don't search backwards if blank line */
		else {	/* search backwards for end of line */
			m=d_xmax-1;
			while ((screen[m][i]==' ') && (m>d_xmin))
				--m;
			if (j<=m) {
				cursor(j+1, i+1);
			} else
				continue;
		}
		while (j <= m) {
			if (j <= m-3) {
				for (k=j; k<=j+3; k++)
					if (screen[k][i] != ' ')
						k=1000;
				if (k < 1000) {
					while(screen[j][i]==' ' && j<=m)
						j++;
					cursor(j+1,i+1);
				}
			}
			lprc(screen[j++][i]);
		}
	}

	setbold();		/* print out only bold objects now */

	for (lastx=lasty=127, i=d_ymin; i<d_ymax; i++)
	  for (j=d_xmin; j<d_xmax; j++) {
		if ( (k=item[j][i])!=0 )
		    if (k != OWALL)
			if ( know[j][i] && ( (mitem[j][i].mon==0) ||
		     (mitem[j][i].mon==INVISIBLESTALKER && c[SEEINVISIBLE]==0)))
			    if (objnamelist[k]!=objnamelist[0]) {
					if (lasty!=i+1 || lastx!=j) {
					   	cursor(lastx=j+1,lasty=i+1);
					} else
						lastx++;
					lprc(objnamelist[k]);
			     }
	  }

	resetbold();

	if (d_flag)  {
		always=1;
		botside();
		always=1;
		bot_linex();
	}
	oldx=99;

		/* for limited screen drawing */
	d_xmin = 0;
	d_xmax = MAXX;
	d_ymin = 0;
	d_ymax = MAXY;
}

/*
**	showcell(x,y)
**
**	subroutine to display a cell location on the screen
*/
void showcell(int x,int y)
{
	int i,j,k,m;

/*	if (c[BLINDCOUNT])
		return;	/* see nothing if blind		*/

	if (c[AWARENESS]) {
		minx = x-3;
		maxx = x+3;
		miny = y-3;
		maxy = y+3;
	}
	else	  {
		minx = x-1;
		maxx = x+1;
		miny = y-1;
		maxy = y+1;
	}

/* Let's try this-- only draw what the player is standing on if they're
   blind... */	
	if (c[BLINDCOUNT]) {
		minx = x;
		maxx = x;
		miny = y;
		maxy = y;
	}

	if (minx < 0) minx=0;
	if (maxx > MAXX-1) maxx = MAXX-1;
	if (miny < 0) miny=0;
	if (maxy > MAXY-1) maxy = MAXY-1;

	for (j=miny; j<=maxy; j++)
		for (m=minx; m<=maxx; m++)
			if ((know[m][j] & 1)==0) {
				cursor(m+1,j+1);
				x=maxx;
				while (know[x][j] & 1)
					--x;

				for (i=m; i<=x; i++) {
				  if ((k=mitem[i][j].mon) != 0) {
				    if (k==MIMIC) {
					if (gtime % 10 == 0)
						while((mimicmonst = rnd(MAXMONST))
							==INVISIBLESTALKER);
				    	lprc(monstnamelist[mimicmonst]);
				    } else if (k==INVISIBLESTALKER  && c[SEEINVISIBLE]==0) {
					lprc(objnamelist[item[i][j]]);
				    } else if (k >= DEMONLORD && k <= LUCIFER && c[EYEOFLARN] == 0)
						goto doswitch2;
				/* do not show demons if dont have eye */
				     else {
					lprc(monstnamelist[k]);
				     }
				  } else  {
doswitch2:
					switch(k=item[i][j]) {
					case 0:
					case OWALL:
					case OIVTELETRAP:
					case OTRAPARROWIV:
					case OIVDARTRAP:
					case OIVTRAPDOOR:
						lprc(objnamelist[k]);
						break;
					default:
						setbold();
						lprc(objnamelist[k]);
						resetbold();
				        }; /* end switch */
				  } /* end else */
			        know[i][j]=1;
			      } /* end for */
			m = maxx;
			} /* end if */
}

/*
**	this routine shows only the spot that is given it.  the spaces around
**	these coordinated are not shown
**	used in godirect() in monster.c for missile weapons display
*/
void show1cell(int x,int y)
{
	if (c[BLINDCOUNT])
		return;	/* see nothing if blind		*/
	cursor(x+1,y+1);
	if ((k=mitem[x][y].mon) != 0)  {
		if (k==MIMIC) {
			if (gtime % 10 == 0)
				while((mimicmonst = rnd(MAXMONST))
					==INVISIBLESTALKER);
			lprc(monstnamelist[mimicmonst]);
		} else if (k==INVISIBLESTALKER  && c[SEEINVISIBLE]==0) {
			lprc(objnamelist[item[x][y]]);
		} else if (k >= DEMONLORD && k<= LUCIFER && c[EYEOFLARN]==0)
			goto doswitch;
		else {
			lprc(monstnamelist[k]);
		}
	} else
doswitch:
		switch(k=item[x][y]) {
		case 0:
		case OWALL:
		case OIVTELETRAP:
		case OTRAPARROWIV:
		case OIVDARTRAP:
		case OIVTRAPDOOR:
			lprc(objnamelist[k]);
			break;
		default:
			setbold();
			lprc(objnamelist[k]);
			resetbold();
		};
	know[x][y] |= 1;	/* we end up knowing about it */
}

/*
**	showplayer()
**
**	subroutine to show where the player is on the screen
**	cursor values start from 1 up
*/
void showplayer()
{

	cursor(playerx+1,playery+1);
	if (!c[BLINDCOUNT]) {
		lprc('@');
		cursor(playerx+1,playery+1);
	}
	oldx=playerx;
	oldy=playery;
}

/*
 * 	moveplayer(dir)
 *
 * subroutine to move the player from one room to another
 * returns 0 if can't move in that direction or hit a monster or on an object
 * else returns 1
 * nomove is set to 1 to stop the next move (inadvertent monsters hitting
 * players when walking into walls) if player walks off screen or into wall
 *
 */
int diroffx[] = { 0,  0, 1,  0, -1,  1, -1, 1, -1 };
int diroffy[] = { 0,  1, 0, -1,  0, -1, -1, 1,  1 };

int moveplayer(int dir)
			/*	from = present room #  direction = [1-north]
				[2-east] [3-south] [4-west] [5-northeast]
				[6-northwest] [7-southeast] [8-southwest]
			if direction=0, don't move--just show where he is */
{
	int k,m,i,j;

/* mute */        
	cursor(playerx+1,playery+1);
	show1cell(playerx,playery);

	if (c[CONFUSE])
		if (c[LEVEL]<rnd(30))
			dir=rund(9); /*if confused any dir*/

	k = playerx + diroffx[dir];
	m = playery + diroffy[dir];

	if (k<0 || k>=MAXX || m<0 || m>=MAXY) {
		nomove=1;
		return(yrepcount = 0);
	}

	i = item[k][m];
	j = mitem[k][m].mon;

	/*	hit a wall	*/
	if (i==OWALL && c[WTW]==0) {
		nomove=1;
		return(yrepcount = 0);
	}

	if (k==33 && m==MAXY-1 && level==1) {
		newcavelevel(0);
		for (k=0; k<MAXX; k++) for (m=0; m<MAXY; m++)
			if (item[k][m]==OENTRANCE) {
				playerx=k;
				playery=m;
				positionplayer();
				drawscreen();
				return(0);
			}
	}

	if (j>0) {
		hitmonster(k,m);
		return(yrepcount = 0);
	} /* hit a monster*/

	lastpx = playerx;
	lastpy = playery;
	playerx = k;
	playery = m;

	if (i && i!=OTRAPARROWIV && i!=OIVTELETRAP && i!=OIVDARTRAP
	      && i!=OIVTRAPDOOR)
		return(yrepcount = 0);
	else
		return(1);
}

/*
 *	function to show what magic items have been discovered thus far
 *	enter with -1 for just spells, 99 gives all spells in game (for when
 *	genie asks you what you want). anything else will give scrolls & potions
 */
static int lincount,count;

void seemagic(int arg)
{
	int i,number;
	count = lincount = 0;
	nosignal=1;

	if (arg == 99) {
		number = (SPNUM+2)/3 + 4;	/* # lines needed to display */
		cl_up(79,number);
		cursor(1,1);
		lprcat("Availible spells are:\n\n");
		for (i=0; i<SPNUM; i++) {
			lprintf("%s %-20s ",spelcode[i],spelname[i]);
			seepage();
		}
		seepage();
		more();
		nosignal=0;
		draws(0,MAXX,0,number);
		return;
	}
	if (arg== -1) /* if display spells while casting one */
	{
		for (number=i=0; i<SPNUM; i++) if (spelknow[i]) number++;
		number = (number+2)/3 + 4;	/* # lines needed to display */
		cl_up(79,number);
		cursor(1,1);
	}
	else {
		resetscroll();
		clear();
	}

	lprcat("The magic spells you have discovered to date are:\n\n");
	for (i=0; i<SPNUM; i++)
		if (spelknow[i]) {
			lprintf("%s %-20s ",spelcode[i],spelname[i]);
			seepage();
		}

	if (arg== -1) {
		seepage();
		more();
		nosignal=0;
		draws(0,MAXX,0,number);
		return;
	}

	lincount += 3;
	if (count!=0) {
		count=2;
		seepage();
	}

	lprcat("\nThe magic scrolls you have found to date are:\n\n");
	count=0;
	for (i=0; i<MAXSCROLL; i++)
		if (scrollknown[i]) {
		/*	if (scrollname[i][1]!=' ') { */
				lprintf("%-26s",&scrollname[i][1]);
				seepage();
		/*	} */
		}
	
	lincount += 3;
	if (count!=0) {
		count=2;
		seepage();
	}

	lprcat("\nThe magic potions you have found to date are:\n\n");
	count=0;
	for (i=0; i<MAXPOTION; i++)
		if (potionknown[i]) {
		     /* if (potionname[i][1]!=' ') { */
				lprintf("%-26s",&potionname[i][1]);
				seepage();
			 /* }  */
		}
	
	if (lincount!=0) more();
	nosignal=0;
	setscroll();
	drawscreen();
}

/*
 *	subroutine to paginate the seemagic function
 */
void seepage()
{
	if (++count==3) {
		lincount++;
		count=0;
		lprc('\n');
		if (lincount>17) {
			lincount=0;
			more();
			clear();
		}
	}
}
