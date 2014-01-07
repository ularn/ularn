/*	player.c
 *
 *	raiselevel()		subroutine to raise the player one level
 *	loselevel()		subroutine to lower the player by one level
 *	raiseexperience(x)	subroutine to increase experience points
 *	loseexperience(x)	subroutine to lose experience points
 *	losehp(x)		subroutine to remove hit points from the player
 *	losemhp(x)		subroutine to remove max # hit points from 
 *				the player
 *	raisehp(x)		subroutine to gain hit points
 *	raisemhp(x)		subroutine to gain maximum hit points
 *	losespells(x)		subroutine to lose spells
 *	losemspells(x)		subroutine to lose maximum spells
 *	raisespells(x)		subroutine to gain spells
 *	raisemspells(x)		subroutine to gain maximum spells
 *	recalc()		function to recalculate the armor class of 
 *				the player
 *	positionplayer()	function to be sure player is not in a wall
 *	quit()			subroutine to ask if the player really wants 
 *				to quit
 */

#include "header.h"
#include "player.h"
#include "monst.h"
#include "itm.h"
#include "extern.h"

/*
 *	***********
 *	RAISE LEVEL
 *	***********
 *	raiselevel()
 *
 *	subroutine to raise the player one level
 *	uses the skill[] array to find level boundarys
 *	uses c[EXPERIENCE]  c[LEVEL]
 */
raiselevel ()
{
	if (c[LEVEL] < MAXPLEVEL) 
		raiseexperience((long)(skill[c[LEVEL]]-c[EXPERIENCE]));
}

/*
 *	***********
 *	LOSE LEVEL
 *	***********
 *   loselevel()
 *
 *	subroutine to lower the players character level by one
 */
loselevel ()
{
	if (c[LEVEL] > 1) 
		loseexperience((long)(c[EXPERIENCE] - skill[c[LEVEL]-1] + 1));
}

/*
 *	****************
 *	RAISE EXPERIENCE
 *	****************
 *	raiseexperience(x)
 *
 *	subroutine to increase experience points
 */
raiseexperience (x)
long x;
{
	int i,tmp;

	i=c[LEVEL];	
	c[EXPERIENCE]+=x;
	while (c[EXPERIENCE] >= skill[c[LEVEL]] && (c[LEVEL] < MAXPLEVEL)) {
		tmp = (c[CONSTITUTION]-c[HARDGAME])>>1;
		c[LEVEL]++;	
		raisemhp((int)(rnd(3)+rnd((tmp>0)?tmp:1)));
		raisemspells((int)rund(3));
		if (c[LEVEL] < 7-c[HARDGAME]) 
			raisemhp((int)(c[CONSTITUTION]>>2));
	}
	if (c[LEVEL] != i) {
		cursors();
		beep(); 
		lprintf("\nWelcome to level %d!",(long)c[LEVEL]);	
		/* if we changed levels	*/
		switch ((int)c[LEVEL]) {
		case 94:	/* earth guardian */
			c[WTW] = 99999L;
			break;
		case 95: 	/* air guardian */
			c[INVISIBILITY] = 99999L;
			break;
		case 96:	/* fire guardian */
			c[FIRERESISTANCE] = 99999L;
			break;
		case 97: 	/* water guardian */
			c[CANCELLATION] = 99999L;
			break;
		case 98: 	/* time guardian */
			c[HASTESELF] = 99999L;
			break;
		case 99:	/* ethereal guardian */
			c[STEALTH] = 99999L;
			c[SPIRITPRO] = 99999L;
			break;
		case 100:
			lprcat("\nYou are now The Creator!");
			{
				int i,j;

				for (i=0; i<MAXY; i++)
					for (j=0; j<MAXX; j++)
						know[j][i]=1;
				for (i=0; i<SPNUM; i++)	
					spelknow[i]=1;
				for (i=0; i<MAXSCROLL; i++)
				        scrollknown[i]=1;
				for (i=0; i<MAXPOTION; i++)
				        potionknown[i]=1;
			}
			break;
		}
	}
}

/*
 *	****************
 *	LOSE EXPERIENCE
 *	****************
 *	loseexperience(x)
 *
 *	subroutine to lose experience points
 */
loseexperience (x)
long x;
{
	int i,tmp;

	i=c[LEVEL];		
	c[EXPERIENCE]-=x;
	if (c[EXPERIENCE] < 0) c[EXPERIENCE]=0;
	while (c[EXPERIENCE] < skill[c[LEVEL]-1]) {
		if (--c[LEVEL] <= 1) 
			c[LEVEL]=1;	/*	down one level		*/
		tmp = (c[CONSTITUTION]-c[HARDGAME])>>1;	/* lose hpoints */
		losemhp((int)rnd((tmp>0)?tmp:1));	/* lose hpoints */
		if (c[LEVEL] < 7-c[HARDGAME]) 
			losemhp((int)(c[CONSTITUTION]>>2));
		losemspells((int)rund(3));	/*	lose spells */
	}
	if (i!=c[LEVEL]) {
		cursors();
		beep(); 
		lprintf("\nYou went down to level %d!",(long)c[LEVEL]);
	}
	bottomline();
}

/*
 *	********
 *	LOSE HP
 *	********
 *	losehp(x)
 *	losemhp(x)
 *
 *	subroutine to remove hit points from the player
 *	warning -- will kill player if hp goes to zero
 */
losehp (x)
int x;
{
	if ((c[HP] -= x) <= 0) {
		beep(); 
		lprcat("\nAlas, you have died.\n");
		lprcat("\n");
		lflush();
		nap(3000);
		died(lastnum);
	}
}

losemhp (x)
int x;
{
	c[HP] -= x;		
	if (c[HP] < 1)		
		c[HP]=1;
	c[HPMAX] -= x;	
	if (c[HPMAX] < 1)	
		c[HPMAX]=1;
}

/*
 * 	********
 *	RAISE HP
 *	********
 *	raisehp(x)
 *	raisemhp(x)
 *
 *	subroutine to gain maximum hit points
 */
raisehp (x)
int x;
{
	if ((c[HP] += x) > c[HPMAX]) 
		c[HP] = c[HPMAX];
}

raisemhp (x)
int x;
{
	c[HPMAX] += x;	
	c[HP] += x;
}

/*
 *	************
 *	RAISE SPELLS
 *	************
 *	raisespells(x)
 *	raisemspells(x)
 *
 *	subroutine to gain maximum spells
 */
raisespells (x)
int x;
{
	if ((c[SPELLS] += x) > c[SPELLMAX])	
		c[SPELLS] = c[SPELLMAX];
}

raisemspells (x)
int x;
{
	c[SPELLMAX]+=x; 
	c[SPELLS]+=x;
}

/*
 *	************
 *	LOSE SPELLS
 *	************
 *	losespells(x)
 *	losemspells(x)
 *
 *	subroutine to lose maximum spells
 */
losespells (x)
int x;
{
	if ((c[SPELLS] -= x) < 0) 
		c[SPELLS]=0;
}

losemspells (x)
int x;
{
	if ((c[SPELLMAX] -= x) < 0) 
		c[SPELLMAX]=0;
	if ((c[SPELLS] -= x) < 0) 
		c[SPELLS]=0;
}


/*
 *	positionplayer()
 *
 *	function to be sure player is not in a wall
 */
positionplayer ()
{
	int try;
	try = 2;

	while ((item[playerx][playery] || mitem[playerx][playery].mon) && (try))
		if (++playerx >= MAXX-1) {
			playerx = 1;
			if (++playery >= MAXY-1) {	
				playery = 1;	
				--try;	
			}
		}
	if (try==0)	 
		lprcat("Failure in positionplayer\n");
}

/*
 *	recalc()	function to recalculate the armor class of the player
 */
recalc ()
{
	int i,j,k;

	c[AC] = c[MOREDEFENSES];
	if (c[WEAR] >= 0)
		switch(iven[c[WEAR]]) {
		case OSHIELD:		
			c[AC] += 2 + ivenarg[c[WEAR]]; 
			break;
		case OLEATHER:		
			c[AC] += 2 + ivenarg[c[WEAR]]; 
			break;
		case OSTUDLEATHER:	
			c[AC] += 3 + ivenarg[c[WEAR]]; 
			break;
		case ORING:			
			c[AC] += 5 + ivenarg[c[WEAR]]; 
			break;
		case OCHAIN:		
			c[AC] += 6 + ivenarg[c[WEAR]]; 
			break;
		case OSPLINT:		
			c[AC] += 7 + ivenarg[c[WEAR]]; 
			break;
		case OPLATE:		
			c[AC] += 9 + ivenarg[c[WEAR]]; 
			break;
		case OPLATEARMOR:	
			c[AC] += 10 + ivenarg[c[WEAR]]; 
			break;
		case OSSPLATE:		
			c[AC] += 12 + ivenarg[c[WEAR]]; 
			break;
		case OELVENCHAIN:
			c[AC] += 15 + ivenarg[c[WEAR]]; 
			break;
		}

	if (c[SHIELD] >= 0 && iven[c[SHIELD]] == OSHIELD) 
		c[AC] += 2 + ivenarg[c[SHIELD]];
	if (c[WIELD] < 0)
		c[WCLASS] = 0;
	else {
		i = ivenarg[c[WIELD]];
		switch(iven[c[WIELD]]) {
		case ODAGGER:  
			c[WCLASS] =  3 + i;
			break;
		case OBELT:
			c[WCLASS] =  7 + i;
			break;
		case OSHIELD:	 
			c[WCLASS] =  8 + i;
			break;
		case OPSTAFF:
		case OSPEAR:   
			c[WCLASS] = 10 + i;
			break;
		case OFLAIL:   
			c[WCLASS] = 14 + i;
			break;
		case OBATTLEAXE: 
			c[WCLASS] = 17 + i;
			break;
		case OLANCE:	 
			c[WCLASS] = 20 + i;
			break;
		case OLONGSWORD:
		case OVORPAL:
			c[WCLASS] = 22 + i;
			break;
		case O2SWORD:  
			c[WCLASS] = 26 + i;
			break;
		case OSWORDofSLASHING: 
			c[WCLASS] = 30 + i; 
			break;
		case OSLAYER:	 
			c[WCLASS] = 30 + i; 
			break;
		case OSWORD:   
			c[WCLASS] = 32 + i;
			break;
		case OHAMMER:  
			c[WCLASS] = 35 + i;
			break;
		default:	   
			c[WCLASS] = 0;
		}
	}
	c[WCLASS] += c[MOREDAM];

	/*	now for regeneration abilities based on rings	*/
	c[REGEN]=1;		
	c[ENERGY]=0;
	j=0;
	for (k=25; k>0; k--)
		if (iven[k]) {
			j=k; 
			k=0; 
		}
	for (i=0; i<=j; i++) {
		switch(iven[i]) {
		case OPROTRING: 
			c[AC]     += ivenarg[i] + 1;	
			break;
		case ODAMRING:
			c[WCLASS] += ivenarg[i] + 1;	
			break;
		case OBELT:   
			c[WCLASS] += ((ivenarg[i]<<1)) + 2;	
			break;
		case OREGENRING:	
			c[REGEN]  += ivenarg[i] + 1;	
			break;
		case ORINGOFEXTRA:	
			c[REGEN]  += 5 * (ivenarg[i]+1); 
			break;
		case OENERGYRING:	
			c[ENERGY] += ivenarg[i] + 1;	
			break;
		}
	}
}


/*
 *	quit()
 *
 *	subroutine to ask if the player really wants to quit
 */
quit ()
{
	int i;

	cursors();
	strcpy(lastmonst,"");
	lprcat("\n\nDo you really want to quit? (y)es, (n)o, (s)ave");
	while (1) {
		i=getcharacter();
		if (i == 'y')	{ 
			clear();
			lflush();
			died(300); 
			return; 
		}
		if ((i == 'n') || (i == ESC))	{ 
			lprcat(" no."); 
			lflush(); 
			return; 
		}
		if (i == 's') {
			lprcat(" save.");
			lflush();
			clear();
			lprcat("Saving...");
			lflush();
			savegame(savefilename); 
			wizard=1; 
			died(-257);
		}
		lprcat("\n");
		setbold();
		lprcat("Yes");
		resetbold();
		lprcat(" ");
		setbold();
		lprcat("Save");
		resetbold();
		lprcat(" or ");
		setbold();
		lprcat("No");
		resetbold();
		lprcat(" please?   Do you want to quit? ");
	}
}

/*
 *	function to ask --more-- then the user must enter a space
 */
more()
{
	char c;

	lprcat("\n  --- press ");
	standout("space");
	lprcat(" to continue --- ");
	while ((c = getcharacter()) != ' ' && !isspace(c))
		;
}

/*
 *	function to put something in the players inventory
 *	returns 0 if success, 1 if a failure
 */
take (itm, arg)
int itm, arg;
{
	int i;
	int limit;
	char eyeflag=0;

	if ((limit = 15+(c[LEVEL]>>1)) > IVENSIZE)
		limit=IVENSIZE;
	for (i=0; i<limit; i++)
		if (iven[i]==0) {
			iven[i] = itm;
			ivenarg[i] = arg;
			limit=0;
			switch(itm) {
			case OPROTRING:	
			case ODAMRING: 
			case OBELT:
				limit=1;
				break;
			case ODEXRING:	
				c[DEXTERITY] += ivenarg[i]+1; 
				limit=1;	
				break;
			case OSTRRING:	
				c[STREXTRA]  += ivenarg[i]+1;
				limit=1; 
				break;
			case OCLEVERRING:
				c[INTELLIGENCE] += ivenarg[i]+1;
				limit=1; 
				break;
			case OHAMMER:	
				c[DEXTERITY] += 10;	
				c[STREXTRA]+=10;
				c[TMP2] = c[INTELLIGENCE];
				c[INTELLIGENCE]=(c[TMP2]-10 <= 0)?1:c[TMP2]-10;
				limit=1;	 
				break;
			case OORB:	
				c[ORB]++;
				c[AWARENESS]++;
				break;
			case OORBOFDRAGON:
				c[SLAYING]++;		
				break;
			case OSPIRITSCARAB: 
				c[NEGATESPIRIT]++;	
				break;
			case OCUBEofUNDEAD: 
				c[CUBEofUNDEAD]++;	
				break;
			case ONOTHEFT:	
				c[NOTHEFT]++;		
				break;
			case OSWORDofSLASHING:	
				c[DEXTERITY] +=5;	
				limit=1; 
				break;
			case OSLAYER:
				c[INTELLIGENCE]+=10;
				break;
			case OPSTAFF:
				c[WISDOM]+=10;
				break;
			case  OLARNEYE:
				c[EYEOFLARN] = 1;
				eyeflag=1;
				break;
			};

			lprcat("\nYou pick up:"); 
			srcount=0;
			show3(i);
			if (limit) 
				bottomline();
		        if (c[BLINDCOUNT] == 0 && eyeflag) {
				lprcat("\nYour sight fades for a moment...");
				lflush();
				nap(2000);
				drawscreen();
				cursors();
				lprcat("\nYour sight returns, and everything looks "
					   "crystal-clear!");
			}
			return(0);
		}
	lprcat("\nYou can't carry anything else.");
	return(1);
}

/*
 *	subroutine to drop an object  returns 1 if something there already else 
 *
 *	k is index into iven list of object to drop
 */
drop_object (k)
int k;
{
	int itm, pitflag=0;

	if ((k<0) || (k>25)) 
		return(0);
	itm = iven[k];
	cursors();
	if (itm==0) { 
		lprintf("\nYou don't have item %c! ",k+'a'); 
		return(1); 
	}
	if (item[playerx][playery] == OPIT)
		pitflag=1;
	else if (item[playerx][playery]) {
		beep(); 
		lprcat("\nThere's something here already.");
		return(1); 
	}
	if (playery==MAXY-1 && playerx==33) 
		return(1); /* not in entrance */

	if (!pitflag) {
		item[playerx][playery] = itm;
		iarg[playerx][playery] = ivenarg[k];
	}
	srcount=0; 
	if (c[WIELD]==k) 
		c[WIELD]= -1;
	if (c[WEAR]==k)
		c[WEAR] = -1;
	if (c[SHIELD]==k) 
		c[SHIELD]= -1;
	adjustcvalues(itm,ivenarg[k]);
	if (itm==OLANCE) 
		recalc();
	lprcat("\n  You drop:"); 
	show3(k); /* show what item you dropped*/
	if (pitflag)
		lprintf("\nIt disappears down the pit.");
	know[playerx][playery] = 0;
	iven[k]=0;
/* say dropped an item so wont ask to pick it up right away */
	dropflag=1;
	return(0);
}

/*
 *	function to enchant armor player is currently wearing
 */
enchantarmor (how)
	int how;
{
	int which;
	int tmp;

	/*
	 *  Bomb out if we're not wearing anything.
	 */
	if (c[WEAR] < 0 && c[SHIELD] < 0) {
		cursors(); 
		beep(); 
		lprcat("\nYou feel a sense of loss.");
		return; 
	}

	/*
	 *  Choose what to enchant.
	 */
	which = (rund(100) < 50) ? SHIELD : WEAR;
	if (c[which] < 0)
		which = (which == SHIELD) ? WEAR : SHIELD;

	/*
	 *  Enchant it and check for destruction at >= +10.
	 */
	ivenarg[c[which]]++;
	if (ivenarg[c[which]] >= 10) {
		if (how == ENCH_ALTAR) {
			ivenarg[c[which]]--;
			lprintf("\nYour %s glows briefly.", objectname[iven[c[which]]]);
			return;
		}
		else if (rnd(10) <= 9) {
			cursors();
			lprintf("\nYour %s vibrates violently and crumbles into dust!",
					objectname[iven[c[which]]]);
			iven[c[which]] = ivenarg[c[which]] = 0;
			c[which] = -1;
			adjustcvalues(iven[c[which]], ivenarg[c[which]]); /* Surely not? */
			return;
		}
	}

	lprintf("\nYour %s glows for a moment.", objectname[iven[c[which]]]);
	bottomline();
}

/*
 *	function to enchant a weapon presently being wielded
 */
enchweapon (how)
	int how;
{
	int tmp;

	if (c[WIELD]<0) { 
		cursors(); 
		beep(); 
		lprcat("\nYou feel depressed.");
		return; 
	}
	tmp = iven[c[WIELD]];
	if (tmp!=OSCROLL && tmp!=OPOTION) { 
		ivenarg[c[WIELD]]++;
		if (tmp==OCLEVERRING) 
			c[INTELLIGENCE]++;
		else if (tmp==OSTRRING)	
			c[STREXTRA]++;
		else if (tmp==ODEXRING)  
			c[DEXTERITY]++;		
		if (ivenarg[c[WIELD]] >= 10 && rnd(10) <= 9) {
			if (how==ENCH_ALTAR) {
				ivenarg[c[WIELD]]--;
				lprcat("\nYour weapon glows a little.");
			}
			else {
				cursors();
				lprcat("\nYour weapon vibrates violently and crumbles into "
					   "dust!");
				tmp=c[WIELD];
				iven[tmp] = 0;
				ivenarg[tmp]=0;
				if (tmp == OLANCE)
					c[LANCEDEATH]=0;
				adjustcvalues(iven[tmp],ivenarg[tmp]);
				c[WIELD] = -1;
			}
		}
		else
			lprcat("\nYour weapon glows for a moment.");
		bottomline(); 
	}
}

/*
 *	routine to tell if player can carry one more thing
 *	returns 1 if pockets are full, else 0
 */
pocketfull ()
{
	int i,limit; 
	if ((limit = 15+(c[LEVEL]>>1)) > IVENSIZE)
		limit=IVENSIZE;
	for (i=0; i<limit; i++) 
		if (iven[i]==0) 
			return(0);
	return(1);
}

/*
 *	function to return 1 if a monster is next to the player else returns 0
 */
nearbymonst ()
{
	int tmp,tmp2;

	for (tmp=playerx-1; tmp<playerx+2; tmp++)
		for (tmp2=playery-1; tmp2<playery+2; tmp2++)
			if (mitem[tmp][tmp2].mon) 
				return(1); /* if monster nearby */
	return(0);
}

/*
 *	function to steal an item from the players pockets
 *	returns 1 if steals something else returns 0
 */
stealsomething (x,y)
int x,y;
{
	int i,n=100;

	/* max of 6 stolen items per monster */
	if (mitem[x][y].n >= 6) 
		return 0;
	while (n--) {
	    i=rund(IVENSIZE);
	    if (iven[i]) 
	    	if (c[WEAR]!=i && c[WIELD]!=i && c[SHIELD]!=i) {
	    		srcount=0; 
	    		show3(i);
	    		adjustcvalues(iven[i],ivenarg[i]);
	    		n = mitem[x][y].n++;
	    		mitem[x][y].it[n].item = iven[i];
	    		mitem[x][y].it[n].itemarg = ivenarg[i];
    			iven[i]=0; 
    			ivenarg[i]=0; 
			beenhere[level]++;
    			return(1);
    		}
	}
	return 0;
}

/*
 *	function to return 1 is player carrys nothing else return 0
 */
emptyhanded ()
{
	int i;

	for (i=0; i<IVENSIZE; i++)
		if (iven[i]) 
			if (i!=c[WIELD] && i!=c[WEAR] && i!=c[SHIELD]) 
				return(0);
	return(1);
}

/*
 *	function to create a gem on a square near the player
 */
creategem ()
{
	int i,j;

	switch(rnd(4)) {
	case 1:	 
		i=ODIAMOND;	
		j=50;	
		break;
	case 2:	 
		i=ORUBY;		
		j=40;	
		break;
	case 3:	 
		i=OEMERALD;	
		j=30;	
		break;
	default: 
		i=OSAPPHIRE;	
		j=20;	
		break;
	};
	createitem(playerx,playery,i,(long)(rndl(j)+(j/10)));
}

/*
 *	function to change character levels as needed when dropping an object
 *	that affects these characteristics
 */
adjustcvalues (itm, arg)
int itm, arg;
{
	int flag,i;

	flag=0;
	switch(itm) {
	case ODEXRING:	
		c[DEXTERITY] -= arg+1;
		flag=1; 
		break;
	case OSTRRING:	
		c[STREXTRA]  -= arg+1;
		flag=1; 
		break;
	case OCLEVERRING: 
		c[INTELLIGENCE] -= arg+1;
		flag=1; 
		break;
	case OHAMMER:	
		c[DEXTERITY] -= 10;	
		c[STREXTRA] -= 10;
		c[INTELLIGENCE] = c[TMP2]; 
		flag=1; 
		break;
	case OORB:	
		c[ORB]--;
		c[AWARENESS]--;
		break;
	case OSWORDofSLASHING:		
		c[DEXTERITY] -= 5;
		flag=1; 
		break;
	case OSLAYER:		
		c[INTELLIGENCE]-=10; 
		flag=1; 
		break;
	case OPSTAFF:		
		c[WISDOM]-=10; 
		flag=1;
		break;
	case OORBOFDRAGON:		
		--c[SLAYING];		
		return;
	case OSPIRITSCARAB:		
		--c[NEGATESPIRIT];	
		return;
	case OCUBEofUNDEAD:		
		--c[CUBEofUNDEAD];	
		return;
	case ONOTHEFT:			
		--c[NOTHEFT]; 		
		return;
	case OLANCE:			
		c[LANCEDEATH]=0;	
		return;
	case OLARNEYE:
		c[EYEOFLARN] = 0;
		cursors();
	    if (c[BLINDCOUNT] == 0) {
		lprcat("\nYour sight fades for a moment...");
		nap(2000);
		drawscreen();
		cursors();
		lprcat("\nYour sight returns but everything looks dull and faded.");
	    }
		return;
	case OPOTION:	
	case OSCROLL:	
		return;

	default:	
		flag=1;
	};
	if (flag) bottomline();

	for (i=0;i<6;i++)
		if (c[i] < 3)
			c[i] = 3;
}

/*
 *	function to read a string from token input "string"
 *	returns a pointer to the string
 */
gettokstr (str)
char *str;
{
	int i,j;

	i=50;
	while ((getcharacter() != '"') && (--i > 0))
		;
	i=36;
	while (--i > 0) {
		if ((j=getcharacter()) != '"') 
			*str++ = j;
		else 
			i=0;
	}
	*str = 0;
	i=50;
	if (j != '"') 
	    while ((getcharacter() != '"') && (--i > 0))
		; /* if end due to too long, then find closing quote */
}

/*
 *	function to ask user for a password (no echo)
 *	returns 1 if entered correctly, 0 if not
 */

getpassword ()
{
	char gpwbuf[BUFSIZ];
	char *fgets_ptr;

	scbr();
	lprcat("\nEnter Password: "); 
	lflush();
/* this should be fgets, with a limit of BUFSIZ or so, yes yes. */
/* 	gets(gpwbuf); */
	fgets_ptr = fgets(gpwbuf,BUFSIZ-1,stdin);
	gpwbuf[strcspn(gpwbuf,"\n")]='\0'; /* take off last \n */
/*	printf("Oh, look, we got %s.\n",gpwbuf);
	printf("we want %s.\n",password); */
	if (strcmp(gpwbuf,password) != 0) {	
		lprcat("\nSorry.\n");
		lflush();
		return(0);
	}
	else  return(1);
}

/*
 *	subroutine to get a yes or no response from the user
 *	returns y or n
 */
getyn ()
{
	int i;

	i=getcharacter();
	while (!isspace(i) && i!='y' && i!='n' && i!='Y' && i!='N' && i!=ESC)
		i=getcharacter();
	if (isspace(i))
		i = ESC;
	return(i);
}

/*
 *	function to calculate the pack weight of the player
 *	returns the number of pounds the player is carrying
 */
packweight ()
{
	int i,j,k;

	k=c[GOLD]/1000; 
	j=25;
	while ((iven[j]==0) && (j>0)) 
		--j;
	for (i=0; i<=j; i++)
		switch(iven[i]) {
		case 0: break;
		case OSSPLATE: 
		case OPLATEARMOR: 
			k += 40;
			break;
		case OPLATE:
			k += 35;	
			break;
		case OHAMMER:	
			k += 30;	
			break;
		case OSPLINT:	
			k += 26;	
			break;
		case OCHAIN:
		case OBATTLEAXE:   		
		case O2SWORD:		
			k += 23;	
			break;
		case OLONGSWORD:
		case OPSTAFF:
		case OSWORD:
		case ORING:
		case OFLAIL:		
			k += 20;	
			break;
		case OELVENCHAIN:
		case OSWORDofSLASHING:	
		case OLANCE:
		case OSLAYER:
		case OSTUDLEATHER:	
			k += 15;	
			break;
		case OLEATHER:
		case OSPEAR:		
			k += 8;		
			break;
		case OORBOFDRAGON:
		case OORB:
		case OBELT:		
			k += 4;		
			break;
		case OSHIELD:		
			k += 7;		
			break;
		case OCHEST:		
			k += 30 + ivenarg[i];	
			break;
		default:		
			k++;
		};
	return(k);
}
