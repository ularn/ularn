/*	create.c		*/
#include "header.h"
#include "monst.h"
#include "player.h"
#include "itm.h"
#include "extern.h"

/*
 *	makeplayer()
 *
 *	subroutine to create the player and the players attributes
 *	this is called at the beginning of a game and at no other time
 */
makeplayer ()
{
	int i;

	scbr();
	clear();
	c[LEVEL]=1;		/*	player starts at level one	*/
	c[REGENCOUNTER]=16;
	c[ECOUNTER]=96;		/*start regeneration correctly*/

	c[SHIELD] = c[WEAR] = c[WIELD] = -1;

	if (char_picked >= 'a' && char_picked <= 'h')
		pick_char(char_picked);
	else {
		for (i=0; i<IVENSIZE; i++)
			iven[i]=0;
		pick_char(0);
	}
	playerx=rnd(MAXX-2);
	playery=rnd(MAXY-2);
	oldx=0;
	oldy=25;
	gtime=0;	/*	time clock starts at zero	*/
	cbak[SPELLS] = -50;
	recalc();
}

/*
 *	newcavelevel(level)
 *	int level;
 *
 *	function to enter a new level.  This routine must be called anytime the
 *	player changes levels.  If that level is unknown it will be created.
 *	A new set of monsters will be created for a new level, and existing
 *	levels will get a few more monsters.
 *	Note that it is here we remove genocided monsters from the present level
 */
void newcavelevel (int x)
{
	int i,j;

	if (beenhere[level])
		savelevel();	/* put the level back into storage	*/
	level = x;	/* get the new level and put in working storage*/
	if (beenhere[x]) {
		getlevel();
		sethp(0);
		checkgen();
		return;
	}
	for (i=0; i<MAXY; i++)
		for (j=0; j<MAXX; j++)
			know[j][i]=mitem[j][i].mon=0;
	makemaze(x);
	makeobject(x);
	beenhere[x]=1;   /* first time here */
	sethp(1);

	if (wizard || x==0)
		for (j=0; j<MAXY; j++)
			for (i=0; i<MAXX; i++)
				know[i][j]=1;

	checkgen();	/* wipe out any genocided monsters */
}

/*
 *	makemaze(level)
 *	int level;
 *
 *	subroutine to make the caverns for a given level.  only walls are made.
 */
static int mx,mxl,mxh,my,myl,myh,tmp2;

void makemaze (int lev)
{
	int i,j;
	int tmp, z;

	if (lev > 0) {
		/* read maze from data file */
		if (cannedlevel(lev) == 1)
			return;
	}

	if (lev==0)
		tmp=0;
	else
		tmp=OWALL;

	/* fill up maze */
	for (i=0; i<MAXY; i++)
		for (j=0; j<MAXX; j++)
			item[j][i]=tmp;

	/* don't need to do anymore for level 0 */
	if (lev==0)
		return;

	eat(1,1);

	/* if this is level 1 */
	if (lev==1)
		item[33][MAXY-1]=0;	/* exit from dungeon */

	/*	now for open spaces -- not on level 15 or V5 */
	if (lev != DBOTTOM && lev != VBOTTOM) {
		tmp2 = rnd(3)+3;
		for (tmp=0; tmp<tmp2; tmp++) {
			my = rnd(11)+2;
			myl = my - rnd(2);
			myh = my + rnd(2);
			if (lev <= DBOTTOM) { 	/* in dungeon */
				mx = rnd(44)+5;
				mxl = mx - rnd(4);
				mxh = mx + rnd(12)+3;
				z=0;
			}
			else { 	/* in volcano */
				mx = rnd(60)+3;
				mxl = mx - rnd(2);
				mxh = mx + rnd(2);
				z = makemonst(lev);
			}
			for (i=mxl; i<mxh; i++)
				for (j=myl; j<myh; j++) {
					item[i][j]=0;
					if ((mitem[i][j].mon=z))
						hitp[i][j]=monster[z].hitpoints;
				}
		}
	}
	if (lev!=DBOTTOM && lev!=VBOTTOM) {
		my=rnd(MAXY-2);
		for (i=1; i<MAXX-1; i++)
			item[i][my] = 0;
	}

	/* no treasure rooms above level 5 */
	if (lev>4)
		treasureroom(lev);
}

/*
	function to eat away a filled in maze
 */
void eat (int xx,int yy)
{
	int dir,try;

	dir = rnd(4);
	try=2;
	while (try) {
		switch(dir) {
		case 1:
			if (xx <= 2) break;	/*	west	*/
			if ((item[xx-1][yy]!=OWALL) || (item[xx-2][yy]!=OWALL))
				break;
			item[xx-1][yy] = item[xx-2][yy] = 0;
			eat(xx-2,yy);
			break;
		case 2:
			if (xx >= MAXX-3) break;  /*	east	*/
			if ((item[xx+1][yy]!=OWALL) || (item[xx+2][yy]!=OWALL))
				break;
			item[xx+1][yy] = item[xx+2][yy] = 0;
			eat(xx+2,yy);
			break;
		case 3:
			if (yy <= 2) break;	/*	south	*/
			if ((item[xx][yy-1]!=OWALL) || (item[xx][yy-2]!=OWALL))
				break;
			item[xx][yy-1] = item[xx][yy-2] = 0;
			eat(xx,yy-2);
			break;
		case 4:
			if (yy >= MAXY-3 ) break;	/*north	*/
			if ((item[xx][yy+1]!=OWALL) || (item[xx][yy+2]!=OWALL))
				break;
			item[xx][yy+1] = item[xx][yy+2] = 0;
			eat(xx,yy+2);
			break;
		};
		if (++dir > 4)	{
			dir=1;
			--try;
		}
	}
}

/*
 *	function to read in a maze from a data file
 *
 *	Only read in a maze 50% of time.
 *
 *	Format of maze data file:
 *				1st character = # of mazes in file (ascii digit)
 *				For each maze:
 *					18 lines (1st 17 used)
 *					67 characters per line
 * 
 *	line seperating maps must be single newline character
 *
 *	Special characters in maze data file:
 *
 *		#	wall			D	door
 *		.	random monster		~	eye of larn
 *		!	cure dianthroritis	-	random object
 */

int cannedlevel (int lev)
{
	int i,j;
	int it,arg,mit,marg;
	FILE *fp;
	char *row, buf[128];

	if (lev != DBOTTOM && lev != VBOTTOM)
		/* only read a maze from file around half the time */
		if (rnd(100) < 50)
		    return -1;

    	if ((fp = fopen(larnlevels, "r")) == (FILE *)NULL)
		return -1;

		/*
		**		Umap format
		**	- lines must be MAXX characters long
		**	- must be MAXY characters per map 
		**	- each map must be seperated by 1 blank line
		**	  (a single newline character)
		*/
	i=rund(20);
	fseek(fp, (long)(i * ((MAXX * MAXY)+MAXY+1)), 0);

	for (i=0; i<MAXY; i++) {
		if ((row = fgets(buf, 128, fp)) == (char *)NULL) {
			perror("fgets");
			fclose(fp);
			return (-1);
		}
		for (j=0; j<MAXX; j++) {
			it = mit = arg = marg = 0;
			switch(*row++) {
			case '#':
				it = OWALL;
				break;
			case 'D':
				it = OCLOSEDDOOR;
				arg = rnd(30);
				break;
			case '~':
				if (lev!=DBOTTOM) 
					break;
				it = OLARNEYE;
				mit = DEMONPRINCE;
				marg = monster[mit].hitpoints;
				break;
			case '!':
				if (lev!=VBOTTOM)
					break;
				it = OPOTION;
				arg = 21;
				mit = LUCIFER;
				marg = monster[mit].hitpoints;
				break;
			case '.':
 				if (lev<=DBOTTOM-5)  break;
				mit = makemonst(lev+1);
				marg = monster[mit].hitpoints;
				break;
			case '-':
				it = newobject(lev+1,&arg);
				break;
			};
			item[j][i] = it;
			iarg[j][i] = arg;
			mitem[j][i].mon = mit;
			hitp[j][i] = marg;
			know[j][i] = (wizard) ? 1 : 0;
		}
	}
	fclose(fp);
	return(1);
}

/*
 *	make a treasure room on a level
 *	- level 10's treasure room has the eye in it and demon lords
 *	- level V5 has potion of cure dianthroritis and demon prince
 */
void treasureroom(int lv)
{
	int tx,ty,xsize,ysize;

	for (tx=1+rnd(10);  tx<MAXX-10;  tx+=10)
	    if ( (lv==DBOTTOM) || (lv==VBOTTOM) || rnd(10)<=2) { /* 20% chance */
			xsize = rnd(6)+3;
			ysize = rnd(3)+3;
			ty = rnd(MAXY-9)+1;  /* upper left corner of room */
			if (lv==DBOTTOM || lv==VBOTTOM)
			   troom(lv,xsize,ysize,(tx=tx+rnd(MAXX-24)),ty,rnd(3)+6);
			else
			    troom(lv,xsize,ysize,tx,ty,rnd(9));
		}
}

/*
 *	subroutine to create a treasure room of any size at a given location
 *	room is filled with objects and monsters
 *	the coordinate given is that of the upper left corner of the room
 */
void troom(int lv,int xsize,int ysize,int tx,int ty,int glyph)
{
	int i,j;
	int bupx, bupy;

	for (j=ty-1; j<=ty+ysize; j++)
		for (i=tx-1; i<=tx+xsize; i++)	/* clear out space for room */
			item[i][j]=0;
	for (j=ty; j<ty+ysize; j++)
		/* now put in the walls */
		for (i=tx; i<tx+xsize; i++) {
			item[i][j]=OWALL;
			mitem[i][j].mon=0;
		}
	for (j=ty+1; j<ty+ysize-1; j++)
		for (i=tx+1; i<tx+xsize-1; i++)	/* now clear out interior */
			item[i][j]=0;
	/* locate the door on the treasure room */
	switch(rnd(2))	{
	case 1:
		item[i=tx+rund(xsize)][j=ty+(ysize-1)*rund(2)]=OCLOSEDDOOR;
		iarg[i][j] = glyph;  /* on horizontal walls */
		break;
	case 2:
		item[i=tx+(xsize-1)*rund(2)][j=ty+rund(ysize)]=OCLOSEDDOOR;
		iarg[i][j] = glyph;	/* on vertical walls */
		break;
	};

		/*
		** must save and use playerx,playery because that's what
		** createmonster() uses
		*/
	bupx = playerx;
	bupy = playery;
	playery=ty+(ysize>>1);
	if (c[HARDGAME]<3)
		for (playerx=tx+1; playerx<=tx+xsize-2; playerx+=2)
			for (i=0, j=rnd(6); i<=j; i++) {
				something(playerx, playery, lv+2);
				createmonster(makemonst(lv+2));
			}
	else
		for (playerx=tx+1; playerx<=tx+xsize-2; playerx+=2)
			for (i=0, j=rnd(4); i<=j; i++) {
				something(playerx,playery,lv+2);
				createmonster(makemonst(lv+4));
			}
	playerx = bupx;
	playery = bupy;
}

/*
 *	***********
 *	MAKE_OBJECT
 *	***********
 *	subroutine to create the objects in the maze for the given level
 */
void makeobject (int j)
{
	int i;

	if (j==0) {
		fillroom(OENTRANCE,0);	/*	entrance to dungeon*/
		fillroom(ODNDSTORE,0);	/*	the DND STORE	*/
		fillroom(OSCHOOL,0);	/*	college of Larn	*/
		fillroom(OBANK,0);	/*	1st national bank of larn*/
		fillroom(OVOLDOWN,0);	/*	volcano shaft to temple*/
		fillroom(OHOME,0);	/*	the players home & family*/
		fillroom(OTRADEPOST,0);	/*  	the trading post	*/
		fillroom(OLRS,0);	/*  	the larn revenue service */
		return;
	}
	if (j==DBOTTOM+1) 	/* V1 */
		fillroom(OVOLUP,0); /* volcano shaft up from the temple */

	/*	make the fixed object in the maze STAIRS and
		random object ELEVATORS */

		/* stairs down only on V1 and V2 */
	if ((j>0) && (j != DBOTTOM) && (j < VBOTTOM-2)) 
		fillroom(OSTAIRSDOWN,0);

	if ((j > 1) && (j != DBOTTOM))
		fillroom(OSTAIRSUP,0);

		/* > 3, not on V1 or V5 or 15 */
	if ((j>3) && (j != DBOTTOM+1)
	   && (j != VBOTTOM) && (j != DBOTTOM))
		if (c[ELVUP]==0)
			if (rnd(100) > 85) {
				fillroom(OELEVATORUP,0);
				c[ELVUP]++;
			}

	/* < lev 10, or 15 or V5 */
	if ((j>0) && (j<=DBOTTOM-5 || j==DBOTTOM || j==VBOTTOM))
		if (c[ELVDOWN]==0)
			if (rnd(100) > 85) {
				fillroom(OELEVATORDOWN,0);
				c[ELVDOWN]++;
			}

	/*	make the random objects in the maze */
	fillmroom(rund(3),OBOOK,j);
	fillmroom(rund(3),OCOOKIE,0);
	fillmroom(rund(3),OALTAR,0);
	fillmroom(rund(3),OSTATUE,0);
	fillmroom(rund(3),OFOUNTAIN,0);
	fillmroom(rund(2),OTHRONE,0);
	fillmroom(rund(2),OMIRROR,0);

	/* be sure to have pits on V3, V4, and V5 */
	/* because there are no stairs on those levels */
	if (j >= VBOTTOM-2)
		fillroom(OPIT,0);
	fillmroom(rund(3),OPIT,0);

	/* be sure to have trapdoors on V3, V4, and V5 */
	if (j >= VBOTTOM-2)
		fillroom(OIVTRAPDOOR,0);
	fillmroom(rund(2),OIVTRAPDOOR,0);
	fillmroom(rund(2),OTRAPARROWIV,0);
	fillmroom(rnd(3)-2,OIVTELETRAP,0);
	fillmroom(rnd(3)-2,OIVDARTRAP,0);

	if (j==1)
		fillmroom(1,OCHEST,j);
	else 	
		fillmroom(rund(2),OCHEST,j);

	if (j<=DBOTTOM) {
		fillmroom(rund(2),ODIAMOND,rnd(10*j+1)+10);
		fillmroom(rund(2),ORUBY,rnd(6*j+1)+6);
		fillmroom(rund(2),OEMERALD,rnd(4*j+1)+4);
		fillmroom(rund(2),OSAPPHIRE,rnd(3*j+1)+2);
	}

	for (i=0; i<rnd(4)+3; i++)
		fillroom(OPOTION,newpotion());	/*	make a POTION	*/

	for (i=0; i<rnd(5)+3; i++)
		fillroom(OSCROLL,newscroll());	/*	make a SCROLL	*/

	for (i=0; i<rnd(12)+11; i++)
		fillroom(OGOLDPILE,12*rnd(j+1)+(j<<3)+10); /* make GOLD	*/

	if (j==8)
		fillroom(OBANK2,0);	/*	branch office of the bank */

	if ( (c[PAD]==0) &&  (j>=4) )
		if (rnd(100) > 75) {
			fillroom(OPAD,0);	/* Dealer McDope's Pad */
			c[PAD]++;
		}

	froom(2,ORING,0);		/* a ring mail 	*/
	froom(1,OSTUDLEATHER,0);	/* a studded leather	*/
	froom(3,OSPLINT,0);		/* a splint mail*/
	froom(5,OSHIELD,rund(3));	/* a shield	*/
	froom(2,OBATTLEAXE,rund(3));	/* a battle axe	*/
	froom(5,OLONGSWORD,rund(3));	/* a long sword	*/
	froom(5,OFLAIL,rund(3));	/* a flail	*/
	froom(7,OSPEAR,rnd(5));		/* a spear	*/
	froom(4,OREGENRING,rund(3));	/* ring of regeneration */
	froom(1,OPROTRING,rund(3));	/* ring of protection	*/
	froom(2,OSTRRING,rund(5)); 	/* ring of strength  */
	froom(2,ORINGOFEXTRA,0);	/* ring of extra regen	*/

	if (c[LAMP]==0) {
		if (rnd(120) < 8) {
			fillroom (OBRASSLAMP,0);
			c[LAMP]++;
			goto zug;
		}
	}

	if (c[WAND]==0) {	/* wand of wonder */
		if (rnd(120) < 8) {
			fillroom(OWWAND,0);
			c[WAND]++;
			goto zug;
		}
	}

	if (c[DRAGSLAY]==0) /* orb of dragon slaying */
		if(rnd(120) < 8) {
			fillroom(OORBOFDRAGON,0);
			c[DRAGSLAY]++;
			goto zug;
		}

	if (c[NEGATE]==0)	/* scarab of negate spirit */
		if(rnd(120) < 8) {
			fillroom(OSPIRITSCARAB,0);
			c[NEGATE]++;
			goto zug;
		}

	if (c[CUBEUNDEAD]==0)		/* cube of undead control */
		if (rnd(120) < 8) {
			fillroom(OCUBEofUNDEAD,0);
			c[CUBEUNDEAD]++;
			goto zug;
		}

	if (c[DEVICE]==0)	/* device of antitheft */
		if (rnd(120) < 8) {
			fillroom(ONOTHEFT,0);
			c[DEVICE]++;
			goto zug;
		}

	if(c[TALISMAN]==0) 		/* talisman of the sphere */
		if(rnd(120) < 8) {
			fillroom(OSPHTALISMAN,0);
			c[TALISMAN]++;
			goto zug;
		}

	if (c[HAND]==0)		/* hand of fear */
		if (rnd(120) < 8) {
			fillroom(OHANDofFEAR,0);
			c[HAND]++;
			goto zug;
		}

	if (c[ORB] == 0) 	/* orb of enlightenment */
		if (rnd(120) < 8) {
			fillroom(OORB,0);
			c[ORB]++;
			goto zug;
		}

	if (c[ELVEN]==0)	/* elven chain */
		if (rnd(120) < 8) {
			fillroom(OELVENCHAIN,0);
			c[ELVEN]++;
		}
zug:
	if (c[SLASH]==0)	/* sword of slashing */
		if (rnd(120) < 8) {
			fillroom(OSWORDofSLASHING,0);
			c[SLASH]++;
		}

	if (c[BESSMANN]==0)	/* Bessman's flailing hammer */
	if(rnd(120) < 8) {
		fillroom(OHAMMER,0);
		c[BESSMANN]++;
	}

	if ((j>=10)&&(j<=VBOTTOM)&&(c[SLAY]==0))	/* Slayer */
		if (rnd(100) > 85-(j-10)) {
			fillroom (OSLAYER,0);
			c[SLAY]++;
		}

	if (c[VORPAL] == 0 && rnd(120) < 8) /* The Vorpal Blade */
	{
		fillroom (OVORPAL, 0);
		c[VORPAL]++;
	}

	if ((c[STAFF]==0) && (j>=8) && (j<=20))	/* staff of power */
		if (rnd(100) > 85-(j-10)) {
			fillroom(OPSTAFF,0);
			c[STAFF]++;
		}

		/* we don't get these if the difficulty level
		** is >= 3
		*/
	if (c[HARDGAME]<3 || (rnd(4)==3)) {
		if (j>3) { 	 /* only on levels 3 or below */
			froom(3,OSWORD,rund(6));  /* sunsword */
			froom(5,O2SWORD,rnd(6));  /* a two handed sword	*/
			froom(3,OBELT,rund(7));	  /* belt of striking	*/
			froom(3,OENERGYRING,rund(6));	/* energy ring	*/
			froom(4,OPLATE,rund(8));  /* platemail */
		}
	}
}

/*
 *	subroutine to fill in a number of objects of the same kind
 */
void fillmroom(int n,int what,int arg)
{
	int i;

	for (i=0; i<n; i++)
		fillroom(what,arg);
}

void froom(int n,int itm,int arg)
{
	if (rnd(151) < n)
		fillroom(itm,arg);
}

/*
 *	subroutine to put an object into an empty room
 *	uses a random walk
 */
void fillroom (int what,int arg)
{
	int x,y;

	x=rnd(MAXX-2);
	y=rnd(MAXY-2);
	while (item[x][y]) {
		x += rnd(3)-2;
		y += rnd(3)-2;
		if (x > MAXX-2)
			x=1;
		if (x < 1)
			x=MAXX-2;
		if (y > MAXY-2)
			y=1;
		if (y < 1)
			y=MAXY-2;
	}
	item[x][y]=what;
	iarg[x][y]=arg;
}

/*
 *	subroutine to put monsters into an empty room without walls or other
 *	monsters
 */
int fillmonst (int what)
{
	int x,y,trys;

	/* max # of creation attempts */
	for (trys=10; trys>0; --trys) {
		x=rnd(MAXX-2);
		y=rnd(MAXY-2);
		if ((item[x][y]==0) && (mitem[x][y].mon==0) &&
                    ((playerx!=x) || (playery!=y))) {
			mitem[x][y].mon = what;
			know[x][y]=0;
			hitp[x][y] = monster[what].hitpoints;
			return(0);
		}
	}
	return(-1); /* creation failure */
}

/*
 *	creates an entire set of monsters for a level
 *	must be done when entering a new level
 *	if sethp(1) then wipe out old monsters else leave them there
 */
void sethp (int flg)
{
	int i,j;

	if (flg)
		for (i=0; i<MAXY; i++)
			for (j=0; j<MAXX; j++)
				stealth[j][i]=0;
	if (level==0) {
		c[TELEFLAG]=0;
		return;
	} /*	if teleported and found level 1 then know level we are on */

	if (flg)
		j = rnd(12) + 2 + (level>>1);
	else
	    j = (level>>1) + 1;

	for (i=0; i<j; i++)
		fillmonst(makemonst(level));

	/*
	** level 11 gets 1 demon lord
	** level 12 gets 2 demon lords
	** level 13 gets 3 demon lords
	** level 14 gets 4 demon lords
	** level 15 gets 5 demon lords
	*/
	if ((level >= (DBOTTOM-4)) && (level<=DBOTTOM)) {
		i=level-10;
		for (j=1;j<=i;j++)
			if (fillmonst(DEMONLORD+rund(7))==-1)
				j--;
	}
	/*
	** level V1 gets 1 demon prince
	** level V2 gets 2 demon princes
	** level V3 gets 3 demon princes
	** level V4 gets 4 demon princes
	** level V5 gets 5 demon princes
	 */
	if (level > DBOTTOM ) {
		i=level-DBOTTOM;
		for (j=1;j<=i;j++)
			if (fillmonst(DEMONPRINCE)==-1)
				j--;
	}
	positionplayer();
}

/*
 *	Function to destroy all genocided monsters on the present level
 */
void checkgen ()
{
	int x,y;

	for (y=0; y<MAXY; y++)
		for (x=0; x<MAXX; x++)
			if ((monster[mitem[x][y].mon].flags&FL_GENOCIDED) != 0)
				mitem[x][y].mon=0; /* no more monster */
}
