/* action.c */
#include "header.h"
#include "itm.h"
#include "player.h"
#include "extern.h"

run (dir)
int dir;
{
	int i;
	i=1; 
	while (i) {
		i=moveplayer(dir);
		if (i>0) {  	
			if (c[HASTEMONST]) 
				movemonst();  
			movemonst(); 
			randmonst(); 
			regen(); 
		}
		if (hitflag) i=0;
		if (i!=0)  
			showcell(playerx,playery);
	}
}

/*
	function to wield a weapon
 */
wield ()	
{
	int i;

	while (1) {
		if ((i = whatitem("wield"))==ESC)  return;
		if (i != '.') {
			if (i=='*') showwield();
			else if (i=='-') {
				c[WIELD]=-1;
				lprcat("\nYou unwield your weapon.");
				bottomline();
				return;
			}
			else if (iven[i-'a']==0) { ydhi(i); return; }
			else if (iven[i-'a']==OPOTION) { ycwi(i); return; }
			else if (iven[i-'a']==OSCROLL) { ycwi(i); return; }
			else if ((c[SHIELD]!= -1) && (iven[i-'a']==O2SWORD)) {
				lprcat("\nBut one arm is busy with your shield!");
				return; 
			}
			else if (i-'a'==c[WEAR] || i-'a'==c[SHIELD]) {
				lprintf("\nYou can't wield your %s while you're wearing it!",
						(i-'a'==c[WEAR]) ? "armor" : "shield");
				return;
			}
			else  {
				c[WIELD]=i-'a';
				lprintf("\nYou wield %s", objectname[iven[i-'a']]);
				if (ivenarg[i-'a']>0) lprintf (" +%d", (long)ivenarg[i-'a']);
				if (ivenarg[i-'a']<0) lprintf (" %d", (long)ivenarg[i-'a']);
				lprc('.');
				if (iven[i-'a'] == OLANCE) c[LANCEDEATH]=1; 
				else c[LANCEDEATH]=0;  
				bottomline();
				return; 
			}
		}
	}
}

/*
	common routine to say you don't have an item
 */
ydhi (x)
int x;
{ 
	cursors();  
	lprintf("\nYou don't have item %c!",x); 
}

ycwi (x)
int x;
{ 
	cursors();  
	lprintf("\nYou can't wield item %c!",x); 
}

/*
	function to wear armor
 */
wear ()
{
	int i;

	while (1) {
		if ((i = whatitem("wear"))==ESC)  return;
		if (i != '.') {
			if (i=='*') showwear(); else
			switch(iven[i-'a']) {
				case 0:  
					ydhi(i); 
					return;
				case OLEATHER:  
				case OCHAIN:  
				case OPLATE:	
				case OSTUDLEATHER:
				case ORING:		
				case OSPLINT:	
				case OPLATEARMOR:	
				case OELVENCHAIN:
				case OSSPLATE:
					if (c[WEAR] != -1) { 
						lprcat("\nYou are already wearing some armor.");
				    	return; 
					}
					c[WEAR]=i-'a';
					if (c[WIELD] == i-'a') c[WIELD] = -1;
					lprintf("\nYou put on your %s", objectname[iven[i-'a']]);
					if (ivenarg[i-'a']>0)
						lprintf (" +%d", (long)ivenarg[i-'a']);
					if (ivenarg[i-'a']<0)
						lprintf (" %d", (long)ivenarg[i-'a']);
					lprc('.');
					bottomline(); 
					return;

				case OSHIELD:	
					if (c[SHIELD] != -1) { 
						lprcat("\nYou are already wearing a shield.");
						return; 
					}
					if (iven[c[WIELD]]==O2SWORD) {
						lprcat("\nYour hands are busy with the two handed "
							   "sword!");
						return; 
					}
					c[SHIELD] = i-'a';
					if (c[WIELD] == i-'a') c[WIELD] = -1;
					lprcat("\nYou put on your shield");
					if (ivenarg[i-'a']>0)
						lprintf (" +%d", (long)ivenarg[i-'a']);
					if (ivenarg[i-'a']<0) 
						lprintf (" %d", (long)ivenarg[i-'a']);
					lprc('.');
					bottomline(); 
					return;

			   default: lprcat("\nYou can't wear that!");
			};
		}
	}
}

/*
	function to drop an object
 */
dropobj ()
{
	int i, pitflag=0;
	char *p;
	long amt;

	p = &item[playerx][playery];
	while (1) {
		if ((i = whatitem("drop"))==ESC)  
			return;
		if (i=='*') 
			showstr(); 
		else {
			/* drop some gold */
			if (i=='.')	{
				if (*p == OPIT) pitflag=1;
				if (*p && !pitflag) { 
				lprcat("\nThere's something here already!"); 
						return; 
				}
				lprcat("\n\n");
				cl_dn(1,23);
				lprcat("How much gold do you drop? ");
				if ((amt=readnum((long)c[GOLD])) <= 0) return;
				if (amt>c[GOLD]) { 
					lprcat("\nYou don't have that much!"); 
					return; 
				}
				if (amt<=32767) { 
					*p=OGOLDPILE; 
					i=(int)amt; 
				}
				else if (amt<=327670L) { 
					*p=ODGOLD; 
					i=(int)amt/10; 
					amt = 10L*i; 
				}
				else if (amt<=3276700L) { 
					*p=OMAXGOLD; 
					i=(int)amt/100; 
					amt = 100L*i; 
				}
				else if (amt<=32767000L) { 
					*p=OKGOLD; 
					i=(int)amt/1000; 
					amt = 1000L*i; 
				}
				else { 
					*p=OKGOLD; 
					i=(int)32767; 
					amt = 32767000L; 
				}
				c[GOLD] -= amt; 
				lprintf("You drop %d gold piece%s.",(long)amt,(amt==1)?"":"s");
				if (pitflag) {
					*p = OPIT;
					lprcat("\nThe gold disappears down the pit.");
				} else
					iarg[playerx][playery]=i; 
				bottomgold();
				know[playerx][playery]=0; 
				dropflag=1;  
				return;
			}
			drop_object(i-'a');
			return;
		}
	}
}

/*
 *	readscr()		Subroutine to read a scroll one is carrying
 */
readscr ()
{
	int i;

	while (1) {
		if ((i = whatitem("read"))==ESC)  return;
		if (i != '.') {
			if (i=='*') 
				showread(); 
			else {
				if (iven[i-'a']==OSCROLL) { 
					read_scroll(ivenarg[i-'a']); 
					iven[i-'a']=0; 
					return; 
				}
				if (iven[i-'a']==OBOOK)   { 
					readbook(ivenarg[i-'a']);  
					iven[i-'a']=0; 
					return; 
				}
				if (iven[i-'a']==0) { 
					ydhi(i); 
					return; 
				}
			lprcat("\nThere's nothing on it to read.");  return;
			} /* end else */
		}
	}
}

/*
 *	subroutine to eat a cookie one is carrying
 */
eatcookie ()
{
	int i;
	char *fortune(), *p;

	while (1) {
	if ((i = whatitem("eat"))==ESC)  
		return;
	if (i != '.')
	    if (i=='*') 
		showeat(); 
	    else {
		if (iven[i-'a']==OCOOKIE) {
		    lprcat("\nThe cookie was delicious.");
		    iven[i-'a']=0;
		    if (!c[BLINDCOUNT]) {
			if ((p=fortune(fortfile))!=0) {
				lprcat("  Inside you find a scrap of paper that says:\n");
				lprcat(p);
			}
		    }
		    return;
		}
		if (iven[i-'a']==0) { ydhi(i); return; }
		lprcat("\nYou can't eat that!");  return;
	    }
	}
}

/*
 *	subroutine to quaff a potion one is carrying
 */
quaff ()
{
	int i;

	while (1) {
		if ((i = qwhatitem()) == ESC)  
			return;
		if (i != '.') {
			if (i=='*') 
				showquaff(); 
			else {
				if (iven[i-'a']==OPOTION) { 
					quaffpotion(ivenarg[i-'a']); 
					iven[i-'a']=0; 
					return; 
				}
				if (iven[i-'a']==0) { 
					ydhi(i); 
					return; 
				}
				lprcat("\nYou wouldn't want to quaff that, would you? ");
				return;
			}
		}
	}
}

qwhatitem ()
{
	int j, i=0;
	char tmp[IVENSIZE];

	cursors();
	for (j=0; j<IVENSIZE; j++)
		switch(iven[j]) {
			case OPOTION:	tmp[i++] = j;
		};
	lprintf("\nWhat do you want to quaff [");
	if (i)
		for (j=0;j<i;j++) 
			lprintf("%c",tmp[j] + 'a');
	lprintf(" * for all] ?");
	i=0; 
	while (i>'z' || (i<'a' && i!='*' && !isspace(i) && i!=ESC )) 
		i=getcharacter();
	if (i==ESC || isspace(i))  
		lprcat(" aborted.");
	if (isspace(i)) i= ESC;

	return(i);
}

/*
	function to ask what player wants to do
 */
whatitem (str)
char *str;
{
	int j=0, flag=0, i=0;
	int wld=0, q=0, r=0, w=0, e=0, d=0;
	char tmp[IVENSIZE];

	cursors();
	if (!strcmp(str, "wield")) 
		wld = 1;
	else if (!strcmp(str, "quaff")) 
		q = 1;
 	else if (!strcmp(str, "read")) 
		r = 1;
	else if (!strcmp(str, "wear")) 
		w = 1;
	else if (!strcmp(str, "eat")) 
		e = 1;
	else if (!strcmp(str, "drop")) 
		d = 1;

	for (j=0; j<IVENSIZE; j++) {
		switch(iven[j]) {
		case OSWORDofSLASHING:
		case OHAMMER:
		case OSWORD:
		case O2SWORD:
		case OSPEAR:
		case ODAGGER:
		case OBATTLEAXE:
		case OLONGSWORD:
		case OFLAIL:
		case OSLAYER:
		case OLANCE:
		case OVORPAL:
				flag = 1; break; /* wield */
		case OPOTION:	
				flag = 2; break; /* quaff */
		case OSCROLL:
		case OBOOK:
				flag = 3; break; /* read */
		case OPLATE :
		case OCHAIN:
		case OLEATHER :
		case ORING :
		case OSTUDLEATHER :
		case OSPLINT :
		case OPLATEARMOR :
		case OSSPLATE :
		case OSHIELD :
		case OELVENCHAIN :
				flag = 4; break; /* wear */
		case OCOOKIE:
				flag = 5; break; /* eat */
		default :	flag = 0; break;
		}
		if (!d) 
			switch (flag) {
			case 1 : 	if (wld) 
						tmp[i++] = j;
					break;
			case 2 : 	if (q) 
						tmp[i++] = j;
					break;
			case 3 : 	if (r) 
						tmp[i++] = j;
					break;
			case 4 : 	if (w) 
						tmp[i++] = j;
					break;
			case 5 : 	if (e) 
						tmp[i++] = j;
			default : 	break;
			}
		else if (iven[j]) 
			tmp[i++] = j;
	}
	lprintf("\nWhat do you want to %s [", str);
	if (i) {
		for (j=0;j<i;j++) 
			lprintf("%c",tmp[j] + 'a');
		lprintf(" ");
	}
	lprintf("* for all%s] ?", (wld) ? " - for none":"");
	i=0;
	while (!((i>='a' && i<='z') || isspace(i) || i=='*' || i==ESC || i=='.'
			 || (i=='-' && wld)))
		i=getcharacter();
	if (i==ESC || isspace(i))  
		lprcat(" aborted.");
	if (isspace(i))
		 i = ESC;
	return(i);
}



/*
 *	subroutine to get a number from the player
 *	and allow * to mean return amt, else return the number entered
 */
long readnum (mx)
long mx;
{
	int i;
	long amt=0;

		/* allow him to say * for all gold */
	if ((i=getcharacter()) == '*')   {
		lprcat("*\n");
		return (mx);
	} else 
	while (i != '\n') {
		lprc(i);
		if (i==ESC || isspace(i)) { 
			lprcat(" aborted.");
			return(-1L); /* Return -1 to indicate abort. */
		}
		if ((i <= '9') && (i >= '0') && (amt<999999999L))
			amt = (long) (amt * 10L + i - '0');
		i = getcharacter();
	}
	lprc(i);
	return (amt);
}

do_create()
{
	int t, a;

gett:
	cursors();
	lprintf("\nType of item (Scroll/Potion/Monster/Other) : ");
	do {
		t=getcharacter();
	} while (isspace(t));
	switch(t) {
	case ESC:	return;
	case 's':
	case 'S':
		cursors();
		cl_dn(1, 23);
		lprcat("Scroll Arg: ");
		a=readnum((long)MAXSCROLL);
		lprintf("createitem(OSCROLL, %d)", a);
		createitem(playerx,playery,OSCROLL, a);
		dropflag=1;
		return;
	case 'p':
	case 'P':
		cursors();
		cl_dn(1, 23);
		lprcat("Potion Arg: ");
		a=readnum((long)MAXPOTION);
		lprintf("createitem(OPOTION, %d)", a);
		createitem(playerx,playery,OPOTION, a);
		dropflag=1;
		return;
	case 'o':
	case 'O':
		cursors();
		lprcat("\n\n");
		cl_dn(1, 23);
		lprcat("Item : ");
		t=readnum(0);
		lprcat("Arg : ");
		a=readnum(0);
		lprintf("\ncreateitem(%d, %d)", t, a);
		createitem(playerx,playery,t, a);
		dropflag=1;
		return;
	case 'm':
	case 'M':
		cursors();
		lprcat("\n\n");
		cl_dn(1, 23);
		lprcat("Monst : ");
		t=readnum(0);
		lprintf("\ncreatemonster(%d)", t);
		createmonster(t);
		dropflag=1;
		return;
	default:
		goto gett;
	}
}
