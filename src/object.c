/*	object.c		*/
#include "header.h"
#include "monst.h"
#include "itm.h"
#include "player.h"
#include "extern.h"

/*
	***************
	LOOK_FOR_OBJECT
	***************

	subroutine to look for an object and give the player his options
	if an object was found.
 */
void lookforobject()
{
	int i,j;

	/* can't find objects is time is stopped*/
	if (c[TIMESTOP])  return;	

	i=item[playerx][playery];	
	if (i==0) return;

	showcell(playerx,playery);  
	cursors();  
	yrepcount=0;

	switch(i) {
	case OGOLDPILE:	
	case OMAXGOLD:
	case OKGOLD:	
	case ODGOLD:	
		ogold(i);	
		break;

	case OPOTION:	
		lprcat("\n\nYou find a magic potion");
		i = iarg[playerx][playery];
		if (potionknown[i]) lprintf(" of %s",&potionname[i][1]);
		lprcat(".");
		opotion(i);  
		break;

	case OSCROLL:	
		lprcat("\n\nYou find a magic scroll");
		i = iarg[playerx][playery];
		if (scrollknown[i]) lprintf(" of %s",&scrollname[i][1]);
		lprcat(".");
		oscroll(i);  
		break;

	case OALTAR:	
		if (nearbymonst()) return;
		lprcat("\n\nThere is a holy altar here.");
		oaltar(); 
		break;

	case OBOOK:	
		lprcat("\n\nYou find a book.");
		obook(); 
		break;

	case OCOOKIE:	
		lprcat("\n\nYou find a fortune cookie.");
		ocookie(); 
		break;

	case OTHRONE:	
		if (nearbymonst()) return;
		lprintf("\n\nThere is %s here.",objectname[i]);
		othrone(0); 
		break;

	case OTHRONE2:	
		if (nearbymonst()) return;
		lprintf("\n\nThere is %s here.",objectname[i]);
		othrone(1); 
		break;

	case ODEADTHRONE: 
		lprintf("\n\nThere is %s here.",objectname[i]);
		odeadthrone(); 
		break;

	case OORB:	
		if (nearbymonst()) return;
		finditem(i);
		break;

	case OBRASSLAMP: 
		lprcat("\nYou find a brass lamp.");
	lprcat("\nDo you want to (r) rub it, (t) take it, or (i) ignore it? ");
		i=0;
		while ((i!='r') && (i!='i') && (i!='t') && (i!=ESC)) 
			i=getcharacter();
		if (i=='r') {
			i=rnd(100);
			if (i>90) {
		lprcat("\nThe magic genie was very upset at being disturbed!");
				lastnum = 286;
				losehp((int)c[HP]/2+1);
				beep();
			}
			/* higher level, better chance of spell */
			else if ( (rnd(100)+c[LEVEL]/2) > 80) {
				int a,b,d;
				lprcat("\nA magic genie appears!");
				cursors();
				lprcat("\n  What spell would you like? : ");
				while ((a=getcharacter())=='D') { 	
					seemagic(99);
					cursors();  
					lprcat("\n  What spell would you like? : ");
				}
				/* to escape casting a spell	*/
				if (a==ESC) goto over; 	
				if ((b=getcharacter())==ESC) 
					goto over;
				if ((d=getcharacter())==ESC) { 
over: 
					lprcat("aborted"); 
					return;
				}
				lprc('\n');
				for (i=0; i<SPNUM; i++)
					if ((spelcode[i][0]==a) 
					    && (spelcode[i][1]==b) 
					    && (spelcode[i][2]==d)) {
						spelknow[i]=1;
						lprintf("\nSpell \"%s\":  %s\n%s",spelcode[i],
								spelname[i],speldescript[i]); 
						lprcat("\nThe genie prefers not to be disturbed "
							   "again.");
						forget();
						bottomline();
						return;
					}
				lprcat("\nThe genie has never heard of such a spell!");
				lprcat("\nThe genie prefers not to be disturbed again.");
				forget();
				bottomline();
				return;
			}
			else lprcat("\nnothing happened.");
			if (rnd(100) < 15) {
				lprcat("\nThe genie prefers not to be disturbed again!");
				forget();
				c[LAMP]=0;  /* chance of finding lamp again */
			}
			bottomline();
		}
		else if (i=='t') {
			lprcat("take.");
			if (take(OBRASSLAMP,0)==0) 
				forget();
		}
		else lprcat("ignore.");
		return;

	case OWWAND:
		if (nearbymonst()) return;
		finditem(i);
		break;

	case OHANDofFEAR:
		if (nearbymonst()) return;
		finditem(i);
		break;

	case OPIT:	
		lprcat("\n\nYou're standing at the top of a pit."); 
		opit(); 
		break;

	case OSTAIRSUP:	
		lprcat("\n\nThere is a circular staircase here."); 
		ostairs(1);  /* up */
		break;

	case OELEVATORUP:
		lprcat("\n\nYou have found an express elevator going up.");
		oelevator(1);  /*  up  */
		break;

	case OELEVATORDOWN:	
		lprcat("\n\nYou have found an express elevator going down.");
		oelevator(-1);	/*	down	*/
		break;

	case OFOUNTAIN:	
		if (nearbymonst()) return;
		lprcat("\n\nThere is a fountain here."); 
		ofountain(); 
		break;

	case OSTATUE:	
		if (nearbymonst()) return;
		lprcat("\n\nYou stand before a statue."); 
		ostatue(); 
		break;

	case OCHEST:	
		lprcat("\n\nThere is a chest here.");  
		ochest();  
		break;

	case OIVTELETRAP:	
		if (rnd(11)<6) return;
		item[playerx][playery] = OTELEPORTER;
		know[playerx][playery] = 1;

	case OTELEPORTER:
		lprcat("\nZaaaappp!  You've been teleported!\n");
		beep(); 
		nap(3000); 
		oteleport(0);
		break; 

	case OSCHOOL:	
		if (nearbymonst()) return;
		lprcat("\n\nYou have found the College of Ularn.");
		lprcat("\nDo you (g) go inside, or (i) stay here? ");
		i=0; 
		while ((i!='g') && (i!='i') && (i!=ESC)) i=getcharacter();
		if (i == 'g') { 
			oschool();  /*	the college of larn	*/
		}
		else	lprcat(" stay here.");
		break;

	case OMIRROR:	
		if (nearbymonst()) return;
		lprcat("\n\nThere is a mirror here.");
		omirror();	
		break;

	case OBANK2:
	case OBANK:	
		if (nearbymonst()) return;
		if (i==OBANK) 
			lprcat("\n\nYou have found the bank of Ularn.");
		else 
		    lprcat("\n\nYou have found a branch office of the bank of Ularn.");
		lprcat("\nDo you (g) go inside, or (i) stay here? ");
		j=0; 
		while ((j!='g') && (j!='i') && (j!=ESC)) 
			j=getcharacter();
		if (j == 'g') {  
			if (i==OBANK) 
				obank(); 
			else 
			    obank2(); /*  the bank of larn  */

		}
		else   
			lprcat(" stay here.");
		break;

	case ODEADFOUNTAIN:	
		if (nearbymonst()) return;
		lprcat("\n\nThere is a dead fountain here.");
		break;

	case ODNDSTORE:	
		if (nearbymonst()) 
			return;
		lprcat("\n\nThere is a DND store here.");
		lprcat("\nDo you (g) go inside, or (i) stay here? ");
		i=0; 
		while ((i!='g') && (i!='i') && (i!=ESC)) i=getcharacter();
		if (i == 'g')
			dndstore();  /*  the dnd adventurers store  */
		else  
			lprcat(" stay here.");
		break;

	case OSTAIRSDOWN:
		lprcat("\n\nThere is a circular staircase here.");
		ostairs(-1); /* down */
		break;

	case OOPENDOOR:		
		lprcat("\nThere is an open door here.");
		break;


	case OCLOSEDDOOR:
		if (dropflag)
			return;
		lprintf("\n\nYou find %s",objectname[i]);
		lprcat("\nDo you (o) try to open it"); 
		iopts();
		i=0; 
		while ((i!='o') && (i!='i') && (i!=ESC)) i=getcharacter();
		if ((i==ESC) || (i=='i')) { 
			ignore();  
			playerx = lastpx;
			playery = lastpy; 
			break; 
		}
		else {
			lprcat("open.");
			if (rnd(11)<7) {
				switch(iarg[playerx][playery]) {
				case 6: 
					c[AGGRAVATE] += rnd(400);	
					break;

				case 7:	
				case 8:
					lprcat("\nYou are jolted by an electric shock!");
					lastnum=274; 
					losehp(rnd(20));  
					bottomline();  
					break;

/* Losing a level is just too harsh... */
/*				case 8:	
					loselevel();  
					break;
*/
				case 9:	
					lprcat("\nYou suddenly feel weaker!");
					if (c[STRENGTH]>3) c[STRENGTH]--;
					bottomline();  
					break;

				default:	
					break;
				}
				playerx = lastpx;  
				playery = lastpy;
			}
			else {
				forget();  
				item[playerx][playery]=OOPENDOOR;
			}
		}
		break;

	case OENTRANCE:	
		lprcat("\nYou have found ");
		lprcat(objectname[OENTRANCE]);
		lprcat("\nDo you (g) go inside"); 
		iopts();
		i=0; 
		while ((i!='g') && (i!='i') && (i!=ESC)) i=getcharacter();
		if (i == 'g')
		{
			newcavelevel(1); 
			playerx=33; 
			playery=MAXY-2;
			item[33][MAXY-1]=know[33][MAXY-1]=mitem[33][MAXY-1].mon=0;
			draws(0,MAXX,0,MAXY); 
			bot_linex(); 
			return;
		}
		else   ignore();
		break;

	case OVOLDOWN:	
		lprcat("\nYou have found "); 
		lprcat(objectname[OVOLDOWN]);
		lprcat("\nDo you (c) climb down"); 
		iopts();
		i=0; 
		while ((i!='c') && (i!='i') && (i!=ESC)) 
			i=getcharacter();
		if ((i==ESC) || (i=='i')) { 
			ignore();  
			break; 
		}
		if (level!=0) { 
			lprcat("\nThe shaft only extends 5 feet downward!"); 
			return; 
		}
		if (packweight() > 45+3*(c[STRENGTH]+c[STREXTRA])) { 
			lprcat("\nYou slip and fall down the shaft.");
			beep();
			lastnum=275;  
			losehp(30+rnd(20)); 
			bottomhp(); 
		}
		else lprcat("climb down.");
		nap(3000); 
		newcavelevel(DBOTTOM+1); /* down to V1 */
		playerx = rnd(MAXX-2);
		playery = rnd(MAXY-2);
		positionplayer(); 
		draws(0,MAXX,0,MAXY); 
		bot_linex(); 
		return;

	case OVOLUP:	
		lprcat("\nYou have found "); 
		lprcat(objectname[OVOLUP]);
		lprcat("\nDo you (c) climb up"); 
		iopts();
		i=0; 
		while ((i!='c') && (i!='i') && (i!=ESC)) i=getcharacter();
		if ((i==ESC) || (i=='i')) { 
			ignore();  
			break; 
		}
	if (packweight() > 40+5*(c[DEXTERITY]+c[STRENGTH]+c[STREXTRA])) { 
			lprcat("\nYou slip and fall down the shaft.");
			beep();
			lastnum=275; 
			losehp(15+rnd(20)); 
			bottomhp(); 
			return; 
		}
		lprcat("climb up.");
		lflush(); 
		nap(3000); 
		newcavelevel(0);
		for (i=0; i<MAXY; i++)  for (j=0; j<MAXX; j++) /* put player near volcano shaft */
			if (item[j][i]==OVOLDOWN) { 
				playerx=j; 
				playery=i; 
				j=MAXX; 
				i=MAXY; 
				positionplayer(); 
			}
		draws(0,MAXX,0,MAXY); 
		bot_linex(); 
		return;

	case OTRAPARROWIV:	
		if (rnd(17)<13) return;	/* for an arrow trap */
		item[playerx][playery] = OTRAPARROW;
		know[playerx][playery] = 0;
	case OTRAPARROW:	
		lprcat("\nYou are hit by an arrow!");
		beep();	/* for an arrow trap */
		lastnum=259;	
		losehp(rnd(10)+level);
		bottomhp();	
		return;

	case OIVDARTRAP:	
		if (rnd(17)<13) 
			return;		/* for a dart trap */
		item[playerx][playery] = ODARTRAP;
		know[playerx][playery] = 0;
	case ODARTRAP:		
		lprcat("\nYou are hit by a dart!");
		beep();	/* for a dart trap */
		lastnum=260;	
		losehp(rnd(5));
		if ((--c[STRENGTH]) < 3) c[STRENGTH] = 3;
		bottomline();	
		return;

	case OIVTRAPDOOR:	
		if (rnd(17)<13) 
			return;		/* for a trap door */
		item[playerx][playery] = OTRAPDOOR;
		know[playerx][playery] = 1;
	case OTRAPDOOR:		
		lastnum = 272; /* a trap door */
		for (i=0;i<IVENSIZE;i++)
			if (iven[i]==OWWAND) {
				lprcat("\nYou escape a trap door.");
				return;
			}
		if ((level==DBOTTOM)||(level==VBOTTOM)) { 
			lprcat("\nYou fall through a trap door leading straight to HELL!");
			beep();
			lflush();
			nap(3000);
			died(271); 
		}
		lprcat("\nYou fall through a trap door!");
		beep();	
		lflush();
		losehp(rnd(5+level));
		nap(2000);
		newcavelevel(level+1);  
		draws(0,MAXX,0,MAXY); 
		bot_linex();
		return;

	case OTRADEPOST:
		if (nearbymonst()) return;
		lprcat("\nYou have found the Ularn trading Post.");
		lprcat("\nDo you (g) go inside, or (i) stay here? ");
		i=0; 
		while ((i!='g') && (i!='i') && (i!=ESC)) i=getcharacter();
		if (i == 'g')  otradepost();  
		else  lprcat("stay here.");
		return;

	case OHOME:	
		if (nearbymonst()) return;
		lprcat("\nYou have found your way home.");
		lprcat("\nDo you (g) go inside, or (i) stay here? ");
		i=0; 
		while ((i!='g') && (i!='i') && (i!=ESC)) 
			i=getcharacter();
		if (i == 'g')  
			ohome();  
		else  lprcat("stay here.");
		return;
	case OPAD:	
		if (nearbymonst()) return;
		lprcat("\nYou have found Dealer McDope's Hideout!");
		lprcat("\nDo you (c) check it out, or (i) ignore it? ");
		i=0;
		while ((i!='c') && (i!='i') && (i!=ESC)) 
			i=getcharacter();
		if (i == 'c')  
			opad();
		else  lprcat("forget it.");
		return;

	case OSPEED:   	
		lprcat("\nYou find some speed.");
		lprcat("\nDo you (s) snort it, (t) take it, or (i) ignore it? ");
		i=0; 
		while ((i!='s') && (i!='i') && (i!='t') && (i!=ESC)) 
			i=getcharacter();
		if (i=='s') {
			lprcat("snort!");
			lprcat("\nOhwowmanlikethingstotallyseemtoslowdown!");
			c[HASTESELF] += 200 + c[LEVEL];
			c[HALFDAM] += 300 + rnd(200);
			if ((c[INTELLIGENCE]-=2) < 3)
				c[INTELLIGENCE]=3;
			if ((c[WISDOM]-=2) < 3)
				c[WISDOM]=3;
			if ((c[CONSTITUTION]-=2) <3)
				c[CONSTITUTION]=3;
			if ((c[DEXTERITY]-=2) <3)
				c[DEXTERITY]=3;
			if ((c[STRENGTH]-=2) <3)
				c[STRENGTH]=3;
			forget();
			bottomline();
		} 
		else if (i=='t') {
			lprcat("take.");
			if (take(OSPEED,0)==0) forget();
		} 
		else 
		    lprcat("ignore.");
		break;

	case OSHROOMS:	
		lprcat("\nYou find some magic mushrooms.");
		lprcat("\nDo you (e) eat them, (t) take them, or (i) ignore them? ");
		i=0; 
		while ((i!='e') && (i!='i') && (i!='t') && (i!=ESC)) 
			i=getcharacter();
		if (i=='e') {
			lprcat("eat!");
			lprcat("\nThings start to get real spacey...");
			c[HASTEMONST] += rnd(75) + 25;
			c[CONFUSE] += 30+rnd(10);
			c[WISDOM]+=2;
			c[CHARISMA]+=2;
			forget();
			bottomline();
		} 
		else if (i=='t') {
			lprcat("take.");
			if (take(OSHROOMS,0)==0) forget();
		} 
		else
			lprcat("ignore.");
		break;

	case OACID:	
		lprcat("\nYou find some LSD.");
		lprcat("\nDo you (e) eat it, (t) take it, or (i) ignore it? ");
		i=0; 
		while ((i!='e') && (i!='i') && (i!='t') && (i!=ESC)) 
			i=getcharacter();
		if (i=='e') {
			lprcat("eat!");
			lprcat("\nYou are now frying your ass off!");
			c[CONFUSE]+=30+rnd(10);
			c[WISDOM]+=2;
			c[INTELLIGENCE]+=2;
			c[AWARENESS]+=1500;
			c[AGGRAVATE]+=1500;
			{ 	
				int j,k;	/* heal monsters */
				for(j=0;j<MAXY;j++)
					for(k=0;k<MAXX;k++)
						if (mitem[k][j].mon)
							hitp[k][j]=monster[mitem[k][j].mon].hitpoints;
			}
			forget();
			bottomline();
		}
		else if (i=='t') {
			lprcat("take.");
			if (take(OACID,0)==0) forget();
		}
		else lprcat("ignore.");
		break;

	case OHASH:	
		lprcat("\nYou find some hashish.");
		lprcat("\nDo you (s) smoke it, (t) take it, or (i) ignore it? ");
		i=0; 
		while ((i!='s') && (i!='i') && (i!='t') && (i!=ESC)) 
			i=getcharacter();
		if (i=='s') {
			lprcat("smoke!");
			lprcat("\nWOW! You feel stooooooned...");
			c[HASTEMONST]+=rnd(75)+25;
			c[INTELLIGENCE]+=2;
			c[WISDOM]+=2;
			if( (c[CONSTITUTION]-=2) < 3) 
				c[CONSTITUTION]=3;
			if( (c[DEXTERITY]-=2) < 3) 
				c[DEXTERITY]=3;
			c[HALFDAM]+=300+rnd(200);
			c[CLUMSINESS]+=rnd(1800)+200;
			forget();
			bottomline();
		}
		else if (i=='t') {
			lprcat("take.");
			if (take(OHASH,0)==0) forget();
		}
		else lprcat("ignore.");
		break;

	case OCOKE:	
		lprcat("\nYou find some cocaine.");
		lprcat("\nDo you want to (s) snort it, (t) take it, or (i) ignore it? ");
		i=0; 
		while ((i!='s') && (i!='i') && (i!='t') && (i!=ESC)) 
			i=getcharacter();
		if (i=='s') {
			lprcat("snort!");
			lprcat("\nYour nose begins to bleed!");
			if ((c[DEXTERITY]-=2) <3)
				c[DEXTERITY]=3;
			if ((c[CONSTITUTION]-=2) <3)
				c[CONSTITUTION]=3;
			c[CHARISMA]+=3;
			for(i=0;i<6;i++)
				c[i]+=33;
			c[COKED]+=10;
			forget();
			bottomline();
		}
		else if (i=='t') {
			lprcat("take.");
			if (take(OCOKE,0)==0) forget();
		}
		else lprcat("ignore.");
		break;	

	case OWALL:	
		break;

	case OANNIHILATION:
		for (i=0;i<IVENSIZE;i++)
			if (iven[i]==OSPHTALISMAN) {
				lprcat("\nThe Talisman of the Sphere protects you from "
					   "annihilation!");
				return;
			}
		/* annihilated by sphere of annihilation */	
		died(283);
		return;

	case OLRS:	
		if (nearbymonst()) return;
		lprcat("\n\nThere is an LRS office here.");
		lprcat("\nDo you (g) go inside, or (i) stay here? ");
		i=0; 
		while ((i!='g') && (i!='i') && (i!=ESC)) i=getcharacter();
		if (i == 'g')
			olrs();  /*  the larn revenue service */
		else  lprcat(" stay here.");
		break;
	default:	
		finditem(i); 
		break;
	};
}

/*
	function to say what object we found and ask if player wants to take it
 */
void finditem(int itm)
{
	int tmp,i;
	lprintf("\n\nYou find %s",objectname[itm]);
	tmp=iarg[playerx][playery];
	switch(itm)
	{
	case ODIAMOND:		
	case ORUBY:			
	case OEMERALD:
	case OSAPPHIRE:		
	case OSPIRITSCARAB:	
	case OORBOFDRAGON:
	case OORB:
	case OHANDofFEAR:
	case OWWAND:
	case OCUBEofUNDEAD:	
	case ONOTHEFT:
		lprcat(".");
		break;

	default:
		if (tmp>0) 
			lprintf(" + %d",(long)tmp); 
		else if (tmp<0) lprintf(" %d",(long)tmp);
	}
	lprcat("\nDo you want to (t) take it"); 
	iopts();
	i=0; 
	while (i!='t' && i!='i' && i!=ESC) i=getcharacter();
	if (i == 't') {	
		lprcat("take.");
		if (take(itm,tmp)==0)  
			forget();	
		return;	
	}
	ignore();
}


/*
	*******
	OSTAIRS
	*******

	subroutine to process the stair cases
	if dir > 0 then up else down
 */
void ostairs(int dir)
{
	int x, y;

	lprcat("\nDo you (s) stay here  or  ");
	if (dir > 0)	
		lprcat("(u) go up?  ");	
	else	
	    lprcat("(d) go down?  ");

	while (1) 
		switch(getcharacter()) {
		case ESC:
		case 's':	
		case 'i':	
			lprcat("stay here.");	
			return;

		case 'u':	
			lprcat("go up.");
			if (dir < 0)
				lprcat("\nThe stairs don't go up!");
			else
					/* not on V1 */
				if (level>=2 && level!=DBOTTOM+1) {
					newcavelevel(level-1);
					for (x=0;x<MAXX;x++)
					  for (y=0;y<MAXY;y++)
						if (item[x][y] == OSTAIRSDOWN){
							playerx=x;
							playery=y;
							x=MAXX;
							y=MAXY;
						}
					draws(0,MAXX,0,MAXY); 
					bot_linex();
				}
				else lprcat("\nThe stairs lead to a dead end!");
			return;

		case 'd':	
			lprcat("go down.");
			if (dir > 0)	
				lprcat("\nThe stairs don't go down!");
			else
				/* not on dungeon botto or V5 */
				if ((level!=0) && (level!=DBOTTOM) && 
				    (level!=VBOTTOM)) {
					newcavelevel(level+1);
					for (x=0;x<MAXX;x++)
					  for (y=0;y<MAXY;y++)
						if (item[x][y] == OSTAIRSUP){
							playerx=x;
							playery=y;
							x=MAXX;
							y=MAXY;
						}
					draws(0,MAXX,0,MAXY); 
					bot_linex();
				}
				else lprcat("\nThe stairs lead to a dead end!");
			return;
		};
}


/*
	*********
	OTELEPORT
	*********

	subroutine to handle a teleport trap +/- 1 level maximum
 */
void oteleport(int err)
{
	int tmp;

	if (err) if (rnd(151)<3)  died(264);  /* stuck in a rock */

	/*show ?? on bottomline if been teleported	*/
	if (!wizard) c[TELEFLAG]=1;	

	if (level==0) 
		tmp=0;
	else if (level <= DBOTTOM) { 	/* in dungeon */
		tmp=rnd(5)+level-3; 
		if (tmp>DBOTTOM)
			tmp=DBOTTOM;
		if (tmp<0) 
			tmp=0; 
	}
	else { 				/* in volcano */
		tmp=rnd(4)+level-2; 
		if (tmp>=VBOTTOM)
			tmp=VBOTTOM;
		if (tmp<DBOTTOM+1)
			tmp=0;  /* back to surface */
	}
	playerx = rnd(MAXX-2);
	playery = rnd(MAXY-2);
	if (level != tmp)
		newcavelevel(tmp);  
	positionplayer();
	draws(0,MAXX,0,MAXY); 
	bot_linex();
}

/*
	*******
	OPOTION
	*******
	}
	playerx = rnd(MAXX-2);	
	playery = rnd(MAXY-2);
	if (level != tmp)
		newcavelevel(tmp);  
	positionplayer();
	draws(0,MAXX,0,MAXY); 
	bot_linex();
}

/*
	*******
	OPOTION
	*******

	function to process a potion
 */
void opotion(int pot)
{
	lprcat("\nDo you (d) drink it, (t) take it"); 
	iopts();
	while (1) switch(getcharacter())
	{
	case ESC:
	case 'i':	
		ignore();  
		return;

	case 'd':	
		lprcat("drink.\n");
		forget();	/*	destroy potion	*/
		quaffpotion(pot);		
		return;

	case 't':	
		lprcat("take.\n");
		if (take(OPOTION,pot)==0)  forget();
		return;
	};
}

/*
	function to drink a potion
 */
void quaffpotion(int pot)
{
	int i,j;
	int k;

	if (pot<0 || pot>=MAXPOTION) 
		return; /* check for within bounds */

/* first character of potion name starts off as \0. */
/* drinking a certain type of potion changes that to a space */
/* ...and from then on it is printable.  */
/*	if (potionname[pot][0] == '\0')   */
/*		potionname[pot][0] = ' '; */
	if (potionknown[pot] == 0) 
	  potionknown[pot] = 1;
	lprintf("\nYou drink a potion of %s.", &potionname[pot][1]);

	switch(pot) {
	case PSLEEP:
		lprcat("  You fall asleep...");
		lflush();
		i=rnd(11)-(c[CONSTITUTION]>>2)+2;
		while(--i>0) {
			parse2();
			nap(1000);
		}
		cursors();
		lprcat("\n.. you wake up.");
		return;

	case PHEALING:
		lprcat("  You feel better.");
		if (c[HP] == c[HPMAX])
			raisemhp(1);
		else if ((c[HP] += rnd(20)+20+c[LEVEL]) > c[HPMAX])
			c[HP]=c[HPMAX];
		break;

	case PRAISELEVEL:
		lprcat("  You feel much more skillful!");
		raiselevel();
		raisemhp(1);
		return;

	case PINCABILITY:
		lprcat("  You feel strange for a moment.");
		c[rund(6)]++;
		break;

	case PWISDOM:
		lprcat("  You feel more self-confident!");
		c[WISDOM] += rnd(2);
		break;

	case PSTRENGTH:
		lprcat("  Wow!  You feel great!");
		if (c[STRENGTH]<12) c[STRENGTH]=12;
		else c[STRENGTH]++;
		break;

	case PCHARISMA:
		lprcat("  You feel charismatic!");
		c[CHARISMA]++;
		break;

	case PDIZZINESS:
		lprcat("  You become dizzy!");
		if (--c[STRENGTH] < 3) c[STRENGTH]=3;
		break;

	case PLEARNING:
		lprcat("  You feel clever!");
		c[INTELLIGENCE]++;
		break;

	case PGOLDDET:
		lprcat("  You feel greedy...");
		lflush();
		nap(2000);
		for (i=0; i<MAXY; i++)  for (j=0; j<MAXX; j++)
			if ((item[j][i]==OGOLDPILE) || (item[j][i]==OMAXGOLD)) {
				know[j][i]=1; 
				show1cell(j,i);
			}
		showplayer();
		return;

	case PMONSTDET:
		for (i=0; i<MAXY; i++)  for (j=0; j<MAXX; j++)
			if (mitem[j][i].mon) {
				know[j][i]=1; 
				show1cell(j,i);
			}
		return;

	case PFORGETFUL:
		lprcat("  You stagger for a moment...");
		lflush();
		for (i=0; i<MAXY; i++)  for (j=0; j<MAXX; j++)
			know[j][i]=0;
		nap(2000);	
		draws(0,MAXX,0,MAXY);
		return;

	case PWATER:
		return;

	case PBLINDNESS:
		lprcat("  You can't see anything!"); 
		c[BLINDCOUNT]+=500;  /* dang, that's a long time. */
		/* erase the character, too! */
		cursor(playerx+1,playery+1);
		lprc(' ');
		cursor(playerx+1,playery+1);
		return;

	case PCONFUSION:
		lprcat("  You feel confused.");
		c[CONFUSE]+= 20+rnd(9); 
		return;

	case PHEROISM:
		lprcat("  WOW!  You feel fantastic!");
		if (c[HERO]==0) for (i=0; i<6; i++) c[i] += 11;
		c[HERO] += 250;  
		break;

	case PSTURDINESS:
		lprcat("  You feel healthier!");
		c[CONSTITUTION]++;  
		break;

	case PGIANTSTR:
		lprcat("  You now have incredible bulging muscles!");
		if (c[GIANTSTR]==0) c[STREXTRA] += 21;
		c[GIANTSTR] += 700;  
		break;

	case PFIRERESIST:
		lprcat("  You feel a chill run up your spine!");
		c[FIRERESISTANCE] += 1000;  
		break;

	case PTREASURE:
		lprcat("  You feel greedy...");
		lflush();
		nap(2000);
		for (i=0; i<MAXY; i++)  for (j=0; j<MAXX; j++) {
			k=item[j][i];
			if ((k==ODIAMOND) || (k==ORUBY) || (k==OEMERALD) 
		 	    || (k==OMAXGOLD) || (k==OSAPPHIRE) 
			    || (k==OLARNEYE) || (k==OGOLDPILE)) {
				know[j][i]=1; 
				show1cell(j,i);
			}
		}
		showplayer();  
		return;

	case PINSTHEAL:
		c[HP] = c[HPMAX]; 
		removecurse();
		break;

	case PCUREDIANTH:
		lprcat("  You don't seem to be affected.");
		return;

	case PPOISON:
		lprcat("  You feel a sickness engulf you!");
		c[HALFDAM] += 200 + rnd(200);  
		return;

	case PSEEINVIS:
		lprcat("  You feel your vision sharpen.");
		c[SEEINVISIBLE] += rnd(1000)+400;
		monstnamelist[INVISIBLESTALKER] = 'I';  
		return;
	};
	/*	show new stats		*/
	bottomline();		
	return;
}

/*
	*******
	OSCROLL
	*******

	function to process a magic scroll
 */
void oscroll(int typ)
{
	lprcat("\nDo you ");
	if (c[BLINDCOUNT]==0) 
		lprcat("(r) read it, "); 
	lprcat("(t) take it"); 
	iopts();
	while (1) switch(getcharacter()) {
	case ESC:
	case 'i':	
		ignore();  
		return;

	case 'r':	
		if (c[BLINDCOUNT]) break;
		lprcat("read.");
		forget();
		if (typ==2 || typ==15)  { 
			show1cell(playerx,playery); 
			cursors(); 
		}
		/*	destroy it	*/	
		read_scroll(typ);  
		return;

	case 't':	
		lprcat("take.");
		if (take(OSCROLL,typ)==0)
			forget();	/*	destroy it	*/
		return;
	};
}

/*
	data for the function to read a scroll
 */
static int xh,yh,yl,xl;
static char curse[] = { 
	BLINDCOUNT, CONFUSE, AGGRAVATE, HASTEMONST, ITCHING,
	LAUGHING, DRAINSTRENGTH, CLUMSINESS, INFEEBLEMENT, HALFDAM };
static char exten[] = { 
	PROTECTIONTIME, DEXCOUNT, STRCOUNT, CHARMCOUNT,
	INVISIBILITY, CANCELLATION, HASTESELF, GLOBE, SCAREMONST, HOLDMONST, TIMESTOP };
char time_change[] = { 
	HASTESELF,HERO,ALTPRO,PROTECTIONTIME,DEXCOUNT,
	STRCOUNT,GIANTSTR,CHARMCOUNT,INVISIBILITY,CANCELLATION,
	HASTESELF,AGGRAVATE,SCAREMONST,STEALTH,AWARENESS,HOLDMONST,HASTEMONST,
	FIRERESISTANCE,GLOBE,SPIRITPRO,UNDEADPRO,HALFDAM,SEEINVISIBLE,
	ITCHING,CLUMSINESS, WTW };
/*
 *	function to adjust time when time warping and taking courses in school
 */
void adjusttime(long tim)
{
	int j;

	for (j=0; j<IVENSIZE; j++)	/* adjust time related parameters */
  		if (c[time_change[j]]) {
                         if (c[time_change[j]] < tim+1)
                           c[time_change[j]] = 1;
                         else
                           c[time_change[j]] -= tim;
                 }
	regen();
}

/*
	function to read a scroll
 */
void read_scroll(int typ)
{
	int i,j;
	if (typ<0 || typ>=MAXSCROLL) 
		return;  /* be sure we are within bounds */
	if (scrollknown[typ] == 0) 
		scrollknown[typ] = 1;
	lprintf("\nYou read a scroll of %s.", &(scrollname[typ][1]));

	switch(typ) {
	case SENCHANTARM:
		enchantarmor(ENCH_SCROLL);
		return;

	case SENCHANTWEAP:	
		enchweapon(ENCH_SCROLL);
		return;

	case SENLIGHTEN:
		lflush();
		yh = min(playery+7,MAXY);	
		xh = min(playerx+25,MAXX);
		yl = max(playery-7,0);		
		xl = max(playerx-25,0);
		for (i=yl; i<yh; i++)
			for (j=xl; j<xh; j++)
				know[j][i]=1;
		nap(2000);	
		draws(xl,xh,yl,yh);	
		return;

	case SBLANK:
		return;

	case SCREATEMONST:
		createmonster(makemonst(level+1));  
		return;

	case SCREATEITEM:
		something(playerx, playery, level);	
		return;

	case SAGGMONST:
		c[AGGRAVATE]+=800;
		return;

	case STIMEWARP:
		/*
		 *  This code is slightly wrong in that, if gtime is small and
		 *  we can't go back by the required number of mobuls, it's
		 *  still reported that we did.  I don't think this is
		 *  critical -- dmr
		 */
		 i = (rnd(1000) - 850)/100;
		 if (i==0) i=1;

		gtime += 100*i;
		if (gtime < 0) gtime = 0;

		lprintf("\nYou go %sward in time by %d mobul%s", (i<0)?"back":"for",
				(i<0)?-i:i, i==1?"":"s");

		adjusttime((long)(i*100));/* adjust time for time warping */
		return;

	case STELEPORT:
		oteleport(0);	  
		return;

	case SAWARENESS:
		c[AWARENESS] += 1800;  
		return;

	case SHASTEMONST:
		c[HASTEMONST] += rnd(55)+12; 
		lprintf("  You feel nervous.");
		return;

	case SMONSTHEAL:
		for (i=0; i<MAXY; i++)  
			for (j=0; j<MAXX; j++)
				if (mitem[j][i].mon)
					hitp[j][i] = monster[mitem[j][i].mon].hitpoints;
		lprintf("  You feel uneasy.");
		return;

	case SSPIRITPROT:
		c[SPIRITPRO] += 300 + rnd(200); 
		bottomline(); 
		return;

	case SUNDEADPROT:
		c[UNDEADPRO] += 300 + rnd(200); 
		bottomline(); 
		return;

	case SSTEALTH:
		c[STEALTH] += 250 + rnd(250);  
		bottomline(); 
		return;

	case SMAGICMAP:
		lflush();
		for (i=0; i<MAXY; i++) 
			for (j=0; j<MAXX; j++)  
				know[j][i]=1;
		nap(2000);	
		draws(0,MAXX,0,MAXY);	
		return;

	case SHOLDMONST:
		c[HOLDMONST] += 30; 
		bottomline(); 
		return;

	case SGEMPERFECT:
		for (i=0; i<IVENSIZE; i++)
			switch(iven[i]) {
			case ODIAMOND:	
			case ORUBY:
			case OEMERALD:	
			case OSAPPHIRE:
				j = ivenarg[i];  
				j *= 2;  
				if (j <= 0 && ivenarg[i]) 
					j=2550;
				ivenarg[i] = j;	
				break;
			}
		break;

	case SSPELLEXT:
		for (i=0; i<11; i++)
			c[exten[i]] <<= 1;
		break;

	case SIDENTIFY:
		for (i=0; i<IVENSIZE; i++)
		{
			if (iven[i]==OPOTION)  
				potionknown[ivenarg[i]] = 1;
			if (iven[i]==OSCROLL)  
				scrollknown[ivenarg[i]] = 1;
		}
		break;

	case SREMCURSE:
		removecurse();
		break;

	case SANNIHILATE:
		annihilate();
		break;

	case SPULVERIZE:
		godirect(22,150,"The ray hits the %s.",0,' ');
		break;

	case SLIFEPROT:
		c[LIFEPROT]++;
		break;
	};
}

/*
 *  Cure the player of curses.
 */
void removecurse ()
{
	int i;

	for (i=0; i<10; i++)
		if (c[curse[i]]) c[curse[i]] = 1;
}

void opit()
{
	int i;
	if (rnd(101)>81)
		return;
	if (rnd(70) > 9*c[DEXTERITY]-packweight() || rnd(101)<5) {
		for (i=0;i<IVENSIZE;i++)
		  if (iven[i]==OWWAND) {
			lprcat("\nYou float right over the pit.");
			return;
		  }
		if (level==DBOTTOM || level == VBOTTOM)
			obottomless(); 
		else {
			if (rnd(101)<20) {
				i=0;
				lprcat("\nYou fell ino a pit!  "
					   "A poor monster cushions your fall!\n");
			} else {
				i = rnd(level*3+3);
				if (i > c[HP]) i = c[HP];
				lprintf("\nYou fell into a pit!  "
						"You suffer %d hit point%s damage.",(long)i,
						(i==1)?"":"s");
				lastnum=261; 
				/* if he dies scoreboard will say so */
			}
			losehp(i); 
			lflush();
			nap(2000);  
			newcavelevel(level+1);  
			draws(0,MAXX,0,MAXY);
		}
	}
}

void obottomless()
{
	lprcat("\nYou fell into a pit leading straight to HELL!");  
	beep(); 
	lflush();
	nap(3000);  
	died(262);
}

void oelevator(int dir)
{
	if (dir==1) { 
		if (level == 0) {
			lprcat(",\nunfortunately, it is out of order.");
			return;
		}
		playerx = rnd(MAXX-2);	
		playery = rnd(MAXY-2);
		nap(2000);
		newcavelevel(rund(level));
	}
	else {
		if (level==DBOTTOM || level==VBOTTOM) {
			nap(2000);
			lprcat("\nand it leads straight to HELL!");
			beep();
			lflush();
			nap(3000);
			died(287);
		}
		playerx = rnd(MAXX-2);	
		playery = rnd(MAXY-2);
		nap(2000);
	    	newcavelevel(level + rnd(DBOTTOM - level));
	}
	positionplayer();
	draws(0,MAXX,0,MAXY); 
	bot_linex();
}

void ostatue()
{ }	/* nothing happens when you move on a statue */


void omirror()
{ }	/* nothing happens when you move on a mirror */

void obook()
{
	lprcat("\nDo you ");
	if (c[BLINDCOUNT]==0) lprcat("(r) read it, "); 
	lprcat("(t) take it"); 
	iopts();
	while (1) 
		switch(getcharacter()) {
		case ESC:
		case 'i':	
			ignore();	
			return;

		case 'r':	
			if (c[BLINDCOUNT]) break;
			lprcat("read.");
			readbook(iarg[playerx][playery]);   
			/* no more book	*/
			forget();
			return;

		case 't':	
			lprcat("take.");
			if (take(OBOOK,iarg[playerx][playery])==0)  
				forget();	/* no more book	*/
			return;
		};
}

/*
	function to read a book
 */
void readbook(int arg)
{
	int i,tmp;

	if (arg<=3) 
		i = rund(((tmp=splev[arg])!=0)?tmp:1); 
	else {
		if (arg >= 21) 
			arg = 20;	/* # entries in splev = 21 */
		i = rnd( ((tmp=splev[arg]-9)!=0) ?tmp:1 ) + 9;
	}
	spelknow[i]=1;
	lprintf("\nSpell \"%s\":  %s\n%s.",
		spelcode[i],spelname[i],speldescript[i]);
	if (rnd(10)==4) {
		lprcat("\nYou feel clever!"); 
		c[INTELLIGENCE]++; 
		bottomline(); 
	}
}

void ocookie()
{
	char *fortune(), *p;

	lprcat("\nDo you (e) eat it, (t) take it"); 
	iopts();
	while (1) 
		switch(getcharacter()) {
		case ESC:
		case 'i':	
			ignore();	
			return;

		case 'e':	
			lprcat("eat.\nThe cookie tasted good.");
			forget(); /* no more cookie	*/
			if (c[BLINDCOUNT]) return;
			if ((p=fortune(fortfile))==(char *)NULL) return;
			lprcat("  A message inside the cookie reads:\n"); 
			lprcat(p);
			return;

		case 't':	
			lprcat("take.");  
			if (take(OCOOKIE,0)==0) forget();
			return;
		};
}


/* routine to pick up some gold -- if arg==OMAXGOLD then the pile is worth 100* the argument */
void ogold(int arg)
{
	long i;

	i = iarg[playerx][playery];

	if (arg==ODGOLD) i *= 10;
	else if (arg==OMAXGOLD) i *= 100;
	else if (arg==OKGOLD) i *= 1000;

	lprintf("\nYou find %d gold piece%s.",i, i==1 ? "": "s");
	c[GOLD] += i;  
	bottomgold();
	item[playerx][playery] = know[playerx][playery] = 0;/*destroy gold*/
}

void ohome()
{
	int i;

	nosignal = 1;	/* disable signals */
	for (i=0; i<IVENSIZE; i++) 
		/* remove the potion of cure dianthroritis from inventory */
		if (iven[i]==OPOTION) 
			if (ivenarg[i]==21) {   
				iven[i]=0;	
				clear(); 
				lprcat("Congratulations.  You found the potion of cure "
					   "dianthroritis!\n");
				lprcat("\nFrankly, No one thought you could do it.");
				lprcat("  Boy!  Did you surprise them!\n");
				nap(1000);
				if (gtime>TIMELIMIT) {
					lprcat("\nHowever... the doctor has the sad duty to "
						   "inform you that your daughter has\n");
					lprcat("died! You didn't make it in time.  In your agony, "
						   "you kill the doctor,\nyour ");
					if (sex == 1)
						lprcat("wife");
					else
						lprcat("husband");
					lprcat(" and yourself!  Too bad...\n");
					nap(5000); 
					died(269);
				}
				else {
					lprcat("\nThe doctor is now administering the potion and, "
						   "in a few moments,\n");
					lprcat("your daughter should be well on her way to "
						   "recovery.\n");
					nap(6000);
					lprcat("\nThe potion is."); 
					nap(1000); 
					lprcat(".");
					nap(1000); 
					lprcat(".");
					nap(1000); 
					lprcat(" working!  The doctor thinks that\n");
					lprcat("your daughter will recover in a few days.  "
						   "Congratulations!");
					beep(); 
					nap(5000); 
					died(263);
				}
			}

	while (1) {
		clear();
		lprintf("Welcome home %s.",logname);
		lprcat("  Latest word from the doctor is not good.\n");

		if (gtime>TIMELIMIT) {
			lprcat("\nThe doctor has the sad duty to inform you that your "
				   "daughter has died!\n");
			lprcat("You didn't make it in time.");
			lprcat("In your agony, you kill the doctor,\nyour ");
			if (sex == 1)
				lprcat("wife");
			else
				lprcat("husband");
			lprcat(" and yourself!  Too bad...");
			nap(5000); 
			died(269);
		}

		lprcat("\nThe diagnosis is confirmed as dianthroritis.  "
			   "He guesses that\n");
		lprintf("your daughter has only %d mobuls left in this world.  "
				"It's up to you,\n",(long)((TIMELIMIT-gtime+99)/100));
		lprintf("%s, to find the only hope for your daughter, the very rare\n",
				logname);
		lprcat("potion of cure dianthroritis.  It is rumored that only deep "
			   "in the\n");
		lprcat("depths of the caves can this potion be found.\n\n\n");
		lprcat("\n     ----- press "); 
		ularn_standout("return");
		lprcat(" to continue, "); 
		ularn_standout("escape");
		lprcat(" to leave ----- ");
		i=getcharacter();  
		while (i!=ESC && i!='\n') 
			i=getcharacter();
		if (i==ESC) { 
			drawscreen(); 
			nosignal = 0; /* enable signals */

			return; 
		}
	}
}

/*	routine to save program space	*/
void iopts()
{	
	lprcat(", or (i) ignore it? ");	
}
void ignore()
{	
	lprcat("ignore.\n");	
}

void closedoor()
{
	int i;

	/* can't find objects is time is stopped*/
	if (c[TIMESTOP])  return;	

	showcell(playerx,playery);  
	cursors();  
	yrepcount=0;

	i=item[playerx][playery];	
	if (i != OOPENDOOR) {
		lprcat("\nThere is no open door here.");
		return;
	}
	lprcat("\nThe door closes.");
	forget();
	item[playerx][playery]=OCLOSEDDOOR;
	iarg[playerx][playery]=0;
	dropflag=1; /* So we won't be asked to open it */
/*	playerx = lastpx;  
	playery = lastpy;*/
		/* if there is now a monster on the space we 
		** need to move back on to, get rid of it */
/*	if (mitem[lastpx][lastpy].mon) {
		lprintf("\nThe %s disappears mysteriously.",
			monster[mitem[lastpx][lastpy].mon].name);
		disappear(lastpx, lastpy);
		hitp[lastpx][lastpy]=0;
		}*/
}
