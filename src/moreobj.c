/* moreobj.c
 *
 *	Routines in this file:
 *
 *	oaltar()
 *	othrone()
 *	ochest()
 *	ofountain()
 */
#include "header.h"
#include "player.h"
#include "itm.h"
#include "monst.h"
#include "extern.h"

/*
 *	******
 *	OALTAR
 *	******
 *
 */
void oaltar()
{
	long k;
	int  p;

start:
	lprcat("\nDo you (p) pray  (d) desecrate"); 
	iopts();
	while (1) {
		while (1) switch(getcharacter()) {
		case 'p':	
		    lprcat(" pray.\nDo you (m) give money or (j) just pray? ");
			while (1) switch(getcharacter()) {
			case 'j':
				p = rund(100);
				if      (p < 12) createmonster(makemonst(level+2));
				else if (p < 17) enchweapon(ENCH_ALTAR);
				else if (p < 22) enchantarmor(ENCH_ALTAR);
				else if (p < 27) ohear();
				else             lprcat("\nNothing happens.");

				return;

			case 'm':
				cursor(1,24);
				cltoeoln();
				cursor(1,23);
				cltoeoln();
				lprcat("how much do you donate? ");
				if ((k = readnum(c[GOLD])) < 0)
					goto start;
				if (c[GOLD] < k) {
					lprcat("You don't have that much!");
					nap(1001);
					goto start;
				}
				if (k < (c[GOLD]/10) && rnd(60)<30 && !wizard) {
					lprcat("Cheapskate! The Gods are insulted by such a "
						   "tiny offering!");
					forget();
					createmonster(DEMONPRINCE);
					c[AGGRAVATE] += 1500;
					return;
				}

				c[GOLD] -= k;
				if (k < (c[GOLD]+k)/10 || k < rnd(50) && !wizard) {
					createmonster(makemonst(level+2));
					c[AGGRAVATE] += 500;
					bottomline();
					return;
				}

				p = rund(16);
				if (p <  4) 
					lprcat("Thank you.");
				else if (p <  6) { 
					enchantarmor(ENCH_ALTAR);
					enchantarmor(ENCH_ALTAR);
				}
				else if (p < 8) {
					enchweapon(ENCH_ALTAR);
					enchweapon(ENCH_ALTAR);
				}
				else
					ohear();

				bottomline();
				return;
			} /* end while switch : case j or m */

		case 'd': 
			lprcat(" desecrate");
			if (rnd(100)<60) { 
				createmonster(makemonst(level+3)+8); 
				c[AGGRAVATE] += 2500; 
			}
			else if(rnd(100)<5) raiselevel();
			else if (rnd(101)<30) {
	lprcat("\nThe altar crumbles into a pile of dust before your eyes.");
				forget();    /*remember to destroy the altar*/
			}
			else lprcat("\nNothing happens.");
			return;

		case 'i':
		case ESC: 
			ignore();
			if (rnd(100)<30) { 
				createmonster(makemonst(level+2)); 
				c[AGGRAVATE] += rnd(450); 
			}
			else	
				lprcat("\nNothing happens.");
			return;
		} /* end while switch: pray, des, ignore */
	} /* NOTREACHED */
} /* end oaltar */

/*
	function to cast a +3 protection on the player
 */
void ohear()
{
	lprcat("\nYou have been heard!");
	if (c[ALTPRO]==0) 
		c[MOREDEFENSES]+=5;
	c[ALTPRO] += 800;	/* protection field */
	bottomline();
}

/*
 *	*******
 *	OTHRONE
 *	*******
 *
 */
void othrone(int arg)
{
	int i,k;

	lprcat("\nDo you (p) pry off jewels, (s) sit down"); 
	iopts();
	while (1) {
		while (1) switch(getcharacter()) {
		case 'p':	
			lprcat(" pry off");  
			k=rnd(101);
			if (k<25) {
				for (i=0; i<rnd(4); i++) 
					creategem(); /*gems pop off the throne*/
				item[playerx][playery]=ODEADTHRONE;
				know[playerx][playery]=0;
			}
			else if (k<40 && arg==0) {
				createmonster(GNOMEKING);
				item[playerx][playery]=OTHRONE2;
				know[playerx][playery]=0;
			}
			else lprcat("\nNothing happens.");
			return;

		case 's': 	
			lprcat(" sit down");  
			k=rnd(101);
			if (k<30 && arg==0) {
				createmonster(GNOMEKING);
				item[playerx][playery]=OTHRONE2;
				know[playerx][playery]=0;
			}
			else if (k<35) {
			    lprcat("\nZaaaappp!  You've been teleported!\n"); 
				beep();
				oteleport(0); 
			}
			else lprcat("\nNothing happens.");
			return;

		case 'i':
		case ESC: 
			ignore(); 
			return;
		};
	} /* NOTREACHED */
}

void odeadthrone()
{
	int k;

	lprcat("\nDo you (s) sit down"); 
	iopts();
	while (1) {
		while (1) switch(getcharacter()) {
		case 's': 	
			lprcat(" sit down");  
			k=rnd(101);
			if (k<5) raiselevel();
			else if (k<25) {
			      lprcat("\nZaaaappp!  You've been teleported!\n"); 
				beep(); 
				oteleport(0); 
			}
			else lprcat("\nNothing happens.");
			return;

		case 'i':
		case ESC: 
			ignore(); 
			return;
		};
	} /* NOTREACHED */
}

/*
 *	******
 *	OCHEST
 *	******
 *
 */
void ochest()
{
	int i,k;

	lprcat("\nDo you (t) take it, (o) try to open it"); 
	iopts();
	while (1) {
		while (1) switch(getcharacter()) {
		case 'o':	
			lprcat(" open it.");  
			k=rnd(101);
			if (k<40) {
				lprcat("\nThe chest explodes as you open it."); 
				beep();
				i = rnd(10);
				if (i > c[HP]) i = c[HP];
				lastnum=281;  /* in case he dies */
				lprintf("\nYou suffer %d hit point%s damage!", (long)i,
						i==1?"":"s");
				checkloss(i);
				switch(rnd(10))	{
				case 1:	
					c[ITCHING]+= rnd(1000)+100;
					lprcat("\nYou feel an irritation spread over your skin!");
					beep();
					break;

				case 2:	
					c[CLUMSINESS]+= rnd(1600)+200;
					lprcat("\nYou begin to lose hand-eye co-ordination!");
					beep();
					break;

				case 3:	
					c[HALFDAM]+= rnd(1600)+200;
					lprcat("\nYou suddenly feel sick and BARF all over your "
						   "shoes!");
					beep();
					break;
				};
				item[playerx][playery]=know[playerx][playery]=0;
				if (rnd(100)<69) 
					creategem(); /* gems from the chest */
				dropgold(rnd(110*iarg[playerx][playery]+200));
				for (i=0; i<rnd(4); i++) 
					something(playerx, playery, iarg[playerx][playery]+2);
			}
			else lprcat("\nNothing happens.");
			return;

		case 't':	
			lprcat(" take");
			if (take(OCHEST,iarg[playerx][playery])==0)
				item[playerx][playery]=know[playerx][playery]=0;
			return;

		case 'i':
		case ESC: 
			ignore(); 
			return;
		};
	} /* NOTREACHED */
}

/*
 *	*********
 *	OFOUNTAIN
 *	*********
 */
void ofountain()
{
	int x;

	cursors();
	lprcat("\nDo you (d) drink, (w) wash yourself"); 
	iopts();
	while (1) switch(getcharacter()) {
	case 'd':	
		lprcat("drink");
		if (rnd(1501)<4) {
		lprcat("\nOH MY GOD!! You have caught the *dreadful sleep*!");
			beep(); 
			lflush();  
			sleep(3);  
			died(280); 
			return;
		}
		x = rnd(100);
		if (x==1) raiselevel();
		else if (x < 11) { 	
			x=rnd((level<<2)+2);
			lprintf("\nBleah! The water tasted like stale gatorade!  "
					"You lose %d hit point%s!", (long)x, x==1?"":"s");
			lastnum=273; 
			losehp(x); 
			bottomline();  
			cursors();
		}
		else if (x<14) { 	
			c[HALFDAM] += 200+rnd(200);
			lprcat("\nThe water makes you vomit.");
		}
		else if (x<17) 
			quaffpotion(17); /* giant strength */
		else if (x < 45)
			lprcat("\nNothing seems to have happened.");
		else if (rnd(3) != 2)
			fntchange(1);	/*change char levels upward*/
		else
			fntchange(-1);	/*change char levels downward*/
		if (rnd(12)<3) {      
			lprcat("\nThe fountains bubbling slowly quietens.");
			/* dead fountain */
			item[playerx][playery]=ODEADFOUNTAIN; 
			know[playerx][playery]=0;
		}
		return;

	case 'i':	
	case ESC:
		ignore();  
		return;
	case 'w':	
		lprcat("wash yourself.");
		if (rnd(100) < 11) { 	
			x=rnd((level<<2)+2);
			lprintf("\nThe water burns like acid!  You lose %d hit point%s!",
					(long)x, x==1?"":"s");
			lastnum=273; 
			losehp(x); 
			bottomline();  
			cursors();
		}
		else if (rnd(100) < 29)
			lprcat("\nYou are now clean.");
		else if (rnd(100) < 31)
		  lprcat("\nThis water needs soap -- the dirt didn't come off.");
		else if (rnd(100) < 34)
			createmonster(WATERLORD); 
		else lprcat("\nNothing seems to have happened.");
		return;
	}
}

/*
	a subroutine to raise or lower character levels
	if how > 0 they are raised   if how < 0 they are lowered
 */
void fntchange(int how)
{
	long j;

	lprc('\n');
	switch(rnd(9)) {
	case 1:	
		lprcat("Your strength");	
		fch(how,&c[0]);	
		break;
	case 2:	
		lprcat("Your intelligence");	
		fch(how,&c[1]);	
		break;
	case 3:	
		lprcat("Your wisdom");
		fch(how,&c[2]);	
		break;
	case 4:	
		lprcat("Your constitution");
		fch(how,&c[3]);	
		break;
	case 5:	
		lprcat("Your dexterity");
		fch(how,&c[4]);	
		break;
	case 6:	
		lprcat("Your charm");
		fch(how,&c[5]);	
		break;
	case 7:	
		j=rnd(level+1);
		if (how < 0) { 	
			lprintf("You lose %d hit point%s!", (long)j, j==1?"":"s");
			losemhp((int)j);
		}
		else { 	
			lprintf("You gain %d hit point%s!",(long)j, j==1?"":"s");
			raisemhp((int)j);
		}
		bottomline();		
		break;
	case 8:	
		j=rnd(level+1);
		if (how > 0) { 	
			lprintf("You just gained %d spell%s!",(long)j, j==1?"":"s");
			raisemspells((int)j);
		}
		else { 	
			lprintf("You just lost %d spell%s!",(long)j, j==1?"":"s");
			losemspells((int)j);
		}
		bottomline();	
		break;
	case 9:	
		j = 5*rnd((level+1)*(level+1));
		if (how < 0) { 	
			lprintf("You just lost %d experience point%s!",(long)j,
					j==1?"":"s");
			loseexperience((long)j);
		}
		else { 	
			lprintf("You just gained %d experience point%s!",(long)j,
					j==1?"":"s");
			raiseexperience((long)j);
		}
		break;
	}
	cursors();
}

/*
 *	***
 *	FCH
 *	***
 *
 *	process an up/down of a character attribute for ofountain
 */
void fch(int how,long *x)
{
	if (how < 0 )	 { 	
		if (*x > 3) {
			lprcat(" went down by one!");	
			--(*x); 
		} else
			lprcat(" remained unchanged!");
	} 
	else {
		lprcat(" went up by one!");	
		(*x)++; 
	}
	bottomline();
}
