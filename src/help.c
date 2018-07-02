/*	help.c		*/
#include "header.h"
#include "player.h"
#include "extern.h"

/*
 *	help function to display the help info	
 *
 *	format of the Ularn help file:
 *
 *	1st character of file:	# of pages of help available (ascii digit)
 *	page (23 lines) for the introductory message (not counted in above)
 *	pages of help text (23 lines per page)
 */
extern char helpfile[];

void help ()
{
	int i,j;
	char tmbuf[128];	

	if ((j=openhelp()) < 0)  
		return;	/* open the help file and get # pages */

	for (i=0; i<23; i++) 
		lgetl();	/* skip over intro message */
	for (;  j>0; j--) {
		clear();
		for (i=0; i<23; i++) { 
			tmcapcnv(tmbuf,lgetl());  
			lprcat(tmbuf); 
		} /* intercept ESC's */
		if (j>1) {
			lprcat("    ---- Press ");  
			ularn_standout("return");
			lprcat(" to exit, ");  
			ularn_standout("space");
			lprcat(" for more help ---- ");
			i=0; 
			while ((i!=' ') && (i!='\n') && (i!=ESC)) 
				i=getcharacter();
			if ((i=='\n') || (i==ESC)) {
				lrclose();  
				setscroll();  
				drawscreen();  
				return;
			}
		}
	}
	lrclose();  
	retcont();  
	drawscreen();
}

/*
 *	function to display the welcome message and background
 */
void welcome ()
{
	int i;
	char tmbuf[128];/* intermediate translation buffer when not a VT100 */

	if (openhelp() < 0)  
		return;   	/* open the help file */
	clear();
	for(i=0; i<23; i++) { 
		tmcapcnv(tmbuf,lgetl());  
		lprcat(tmbuf); 
	} /* intercept ESC's */
	lrclose();  
	retcont();	/* press return to continue */
}

/*
 *	function to say press return to continue and reset scroll when done
 */
void retcont ()
{
	cursor(1,24); 
	lprcat("Press "); 
	ularn_standout("return");
	lprcat(" to continue: ");   
	while (getcharacter() != '\n')
		;
	setscroll();
}

/*
 *	routine to open the help file and return the first character - '0'
 */
int openhelp ()
{
	if (lopen(helpfile)<0) {
		lprintf("Can't open help file \"%s\" ",helpfile);
		lflush(); 
		sleep(4);	
		drawscreen();	
		setscroll(); 
		return(-1);
	}
	resetscroll();  
	return(lgetc() - '0');
}
