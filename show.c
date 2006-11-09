/* show.c */
#include "header.h"
#include "player.h"
#include "itm.h"
#include "extern.h"

/*
	showstr()

	show character's inventory
 */
showstr()
{
	int i,number;

	for (number=3, i=0; i<IVENSIZE; i++)
		if (iven[i]) number++;	/* count items in inventory */
	t_setup(number);	
	qshowstr();
	t_endup(number);
}

qshowstr()
{
	int k;

	srcount=0;  
	nosignal=1; /* don't allow ^c etc */

	if (c[GOLD]) { 	
		lprintf(".)   %d gold piece%s.",(long)c[GOLD], 
			c[GOLD]==1?"":"s"); 
		srcount++; 
	}

	for (k=0; k<IVENSIZE; k++)
		if (iven[k]) show3(k);

	lprintf("\nElapsed time is %d.  You have %d mobul%s left.",
		(long)((gtime+99)/100+1),(long)((TIMELIMIT-gtime)/100),
		(TIMELIMIT-gtime)/100==1?"":"s");
	more();		
	nosignal=0;
}

/*
 *	subroutine to clear screen depending on # lines to display
 */
t_setup (count)
int count;
{
	if (count<20)  {
		cl_up(79,count);  
		cursor(1,1);
	}
	else {
		resetscroll(); 
		clear();
	}
}

/*
 *	subroutine to restore normal display screen depending on t_setup()
 */
t_endup (count)
int count;
{
	if (count<18)  /* how did we clear the screen? */
		draws(0,MAXX,0,(count>MAXY) ? MAXY : count);
	else {
		drawscreen(); 
		setscroll();
	}
}

/*
	function to show the things player is wearing only
 */
showwear()
{
	int count, i,j;	

	nosignal=1; /* don't allow ^c etc */
	srcount=0;

	/* count number of items we will display */
	for (count=2,j=0; j<IVENSIZE; j++)	 
	   if ((i=iven[j])!=0)
		switch(i) {
			case OLEATHER:	
			case OPLATE:	
			case OCHAIN:
			case ORING:		
			case OSTUDLEATHER:	
			case OSPLINT:
			case OPLATEARMOR:	
			case OSSPLATE:	
			case OSHIELD:
			case OELVENCHAIN:
				count++;
		};

	t_setup(count);

	for (i=22; i<93; i++)
		 for (j=0; j<IVENSIZE; j++)
		   if (i==iven[j])
			switch(i) {
				case OLEATHER:	
				case OPLATE:	
				case OCHAIN:
				case ORING:		
				case OSTUDLEATHER:	
				case OSPLINT:
				case OPLATEARMOR:	
				case OSSPLATE:	
				case OSHIELD:
				case OELVENCHAIN:
					show3(j);
			};
	more();		
	nosignal=0;
	t_endup(count);
}

/*
	function to show the things player can wield only 
 */
showwield()
{
	int i,j,count;

	nosignal=1; /* don't allow ^c etc */
	srcount=0;

	 for (count=2,j=0; j<IVENSIZE; j++)	/* count how many items */
	   if ((i=iven[j])!=0)
		switch(i) {
			case ODIAMOND:  
			case ORUBY:  
			case OEMERALD:  
			case OSAPPHIRE:
			case OBOOK:     
			case OCHEST:  
			case OLARNEYE: 
			case ONOTHEFT:
			case OSPIRITSCARAB:  
			case OCUBEofUNDEAD:
			case OPOTION:   
			case OORB:
			case OHANDofFEAR:
			case OBRASSLAMP:
			case OURN:
			case OWWAND:
			case OSPHTALISMAN:
			case OSCROLL:  
				break;
			default:  count++;
		};

	t_setup(count);

	for (i=22; i<93; i++)
		 for (j=0; j<IVENSIZE; j++)
		   if (i==iven[j])
			switch(i) {
				case ODIAMOND:  	case ORUBY:  
				case OEMERALD:  	case OSAPPHIRE:
				case OBOOK:     	case OCHEST:  
				case OLARNEYE: 		case ONOTHEFT:
				case OSPIRITSCARAB:  	case OCUBEofUNDEAD:
				case OPOTION:   	case OHANDofFEAR:
				case OBRASSLAMP:  	case OURN:
				case OSPHTALISMAN: 	case OWWAND:
				case OORB: 		case OSCROLL:  
					break;
				default:  show3(j);
			};
		more();		
		nosignal=0;
		t_endup(count);
	}

/*
 *	function to show the things player can read only
 */
showread ()
{
	int i,j,count;

	nosignal=1; /* don't allow ^c etc */
	srcount=0;

	for (count=2,j=0; j<IVENSIZE; j++)
		switch(iven[j]) {
			case OBOOK:	
			case OSCROLL:	
				count++;
			};
	t_setup(count);

	for (i=22; i<84; i++)
		 for (j=0; j<IVENSIZE; j++)
		   if (i==iven[j])
			switch(i) {
				case OBOOK:	
				case OSCROLL:	
					show3(j);
			};
		more();	
		nosignal=0;
		t_endup(count);
	}

/*
 *	function to show the things player can eat only
 */
showeat ()
{
	int i,j,count;

	nosignal=1; /* don't allow ^c etc */
	srcount=0;

	for (count=2,j=0; j<IVENSIZE; j++)
		switch(iven[j]) {
			case OCOOKIE:	
				count++;
		};
	t_setup(count);

	for (i=22; i<84; i++)
		 for (j=0; j<IVENSIZE; j++)
		   if (i==iven[j])
			switch(i) {
				case OCOOKIE:	
					show3(j);
			};
		more();		
		nosignal=0;
		t_endup(count);
	}

/*
	function to show the things player can quaff only
 */
showquaff ()
{
	int i,j,count;

	nosignal=1; /* don't allow ^c etc */
	srcount=0;

	for (count=2,j=0; j<IVENSIZE; j++)
		switch(iven[j]) {
			case OPOTION:	
				count++;
		};
	t_setup(count);

	for (i=22; i<84; i++)
	  for (j=0; j<IVENSIZE; j++)
	     if (i==iven[j])
		switch(i) {
			case OPOTION:	
				show3(j);
	        };
	more();			
	nosignal=0;
	t_endup(count);
}

show1 (idx, str2, known)
int idx;
char *str2[];
int known[];
{
 /* standard */
   if (known == 0) {
	if (str2==0)
		lprintf("\n%c)   %s",idx+'a',objectname[iven[idx]]);
	else if (*str2[ivenarg[idx]]==0)  
		lprintf("\n%c)   %s",idx+'a',objectname[iven[idx]]);
	else 
		lprintf("\n%c)   %s of%s",
			idx+'a',objectname[iven[idx]],str2[ivenarg[idx]]);
    } else {
 /* scroll or potion or something with a known array */	
	if (str2==0)
		lprintf("\n%c)   %s",idx+'a',objectname[iven[idx]]);
	else if (*str2[ivenarg[idx]]==0)  
		lprintf("\n%c)   %s",idx+'a',objectname[iven[idx]]);
	else if (known[ivenarg[idx]]==0) 
		lprintf("\n%c)   %s",idx+'a',objectname[iven[idx]]);	
	else 
		lprintf("\n%c)   %s of%s",
			idx+'a',objectname[iven[idx]],str2[ivenarg[idx]]);
    }
    if (wizard)
	lprintf(" [ %d ]", ivenarg[idx]);
}

show3 (index)
int index;
{
	switch(iven[index]) {
		case OPOTION:	show1(index,potionname,potionknown);  break;
		case OSCROLL:	show1(index,scrollname,scrollknown);  break;
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
		case ONOTHEFT:		show1(index,(char **)0, (int *)0);  break;

		default:	
			lprintf("\n%c)   %s",index+'a',objectname[iven[index]]);
			if (ivenarg[index]>0 || wizard)
				lprintf(" +%d",(long)ivenarg[index]);
			else if (ivenarg[index]<0) 
				lprintf(" %d",(long)ivenarg[index]);
			break;
	}
	if (c[WIELD]==index) 
		lprcat(" (weapon in hand)");
	if ((c[WEAR]==index) || (c[SHIELD]==index))  
		lprcat(" (being worn)");
	if (++srcount>=22) { 
		srcount=0; 
		more(); 
		clear(); 
	}
}
