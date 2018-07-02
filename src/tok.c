/* tok.c */
#include "header.h"
#include "player.h"
#include "monst.h"
#include "extern.h"
extern int nonap;

#define MAXUM 52	/* maximum number of user re-named monsters */
#define MAXMNAME 40	/* max length of a monster re-name */

static char lastok=0;
int yrepcount=0;

static char usermonster[MAXUM][MAXMNAME];
static char usermpoint=0;			/* the user monster pointer */

/*
 *	lexical analyzer for Ularn
 */
char yylex ()
{
	int cc, ic;
	char *sh, *getenv();

	if (hit2flag) {
		hit2flag=0;
		yrepcount=0;
		return(' ');
	}
	if (yrepcount>0)	{
		--yrepcount;
		return(lastok);	
	}
	else yrepcount=0;

	/*show where the player is*/
	if (yrepcount==0) {
		bottomdo();
		showplayer();
	}	
	lflush();
	while (1) {
		c[BYTESIN]++;
		if (ckpflag)
			/* check for periodic checkpointing */
			if (c[BYTESIN] == 1
			    || (c[BYTESIN] % CKCOUNT) == 0
		 	    || restorflag) {
				if (restorflag)
					restorflag=0;
			    	wait((int *)0);
				if (fork() == 0) {
					savegame(ckpfile);
					exit(0);
				}
			}
		flushall();
		if ((cc = getchar()) == EOF)
			return(lastok = -1);

		/* shell escape */
		if (cc == '!')	{
			int pgrp;
#ifdef USG
			pgrp = getpgrp();
#else
			pgrp = getpgrp();
#endif /* USG */
			
			resetscroll();
			clear();
			cl_dn(0,0);
			lflush();
			if ((ic=fork())==0) {
				settty();
				setuid(geteuid());
				setgid(getegid());
				setpgrp();
				if ((sh = getenv("SHELL")) != (char *)NULL) {
					execl(sh, sh, (char *)0);	
					if (!strcmp(sh, "/bin/sh")) {
						fprintf(stderr,
						    "Can't exec %s!\n", sh);
						sleep(2);
						exit(1);
					}
					/* fall through and try /bin/sh */
				}
				execl("/bin/sh", "sh", (char *)0);
				fprintf(stderr,"Can't exec /bin/sh!\n");
				sleep(2);
				exit(1);
			}
			if (ic == -1 ) {
				fprintf(stderr,"Can't fork %s!\n", sh);
				sleep(2);
			} else {
				sigsave();
				wait((int *)0);
				sigreset();
				setscroll();
				gettty();
			}
			return(lastok = 'L'-64);	/* redisplay screen */
		}
		if ((cc <= '9') && (cc >= '0'))
			yrepcount = yrepcount*10 + cc - '0';
		else {	
			if (yrepcount>0)
				--yrepcount;
			return(lastok = cc);
		}
	}
}

/*
 *	flushall()	Function to flush all type-ahead in the input buffer
 */
void flushall()
{
#ifdef TCFLSH
        ioctl(0, TCFLSH, 0);		/* standard ioctl to flush buffer */
#else
#  ifdef FIONREAD
	/* alternate way to flush buffer */
	int ic=0;
		/* flush keyboard input buffer */
	for (;;) {
		ioctl(0,FIONREAD,&ic);
		if (ic<=0) return;
		while (ic>0)   {
			getchar();
			--ic;
		} /* gobble up the byte */
	}
#  else
	/* forget it */
#  endif /* FIONREAD */
#endif /* TCFLSH */
}

/*
	function to set the desired hardness
	enter with hard= -1 for default hardness, else any desired hardness
*/
void sethard (int hard)
{
	int j,k,i;

	if (restorflag==0)  {    /* don't set c[HARDGAME] if restoring game */
		if (hashewon() == 0) {
			if (hard >= 0)
				c[HARDGAME] = hard;
		}
		else  if (hard > c[HARDGAME] || wizard)
			c[HARDGAME] = hard;
	}

	if (k=c[HARDGAME])
		for (j=0; j<=MAXMONST+8; j++) {
			i = ((6+k)*monster[j].hitpoints+1)/6;
			monster[j].hitpoints = (i > 32767) ? 32767 : i;
			i = ((6+k)*monster[j].damage+1)/5;
			monster[j].damage = (i>127) ? 127 : i;
			i = (10*monster[j].gold)/(10+k);
			monster[j].gold = (i>32767) ? 32767 : i;
			i = monster[j].armorclass - k;
			monster[j].armorclass = (i< -127) ? -127 : i;
			i = (int) ((7*monster[j].experience)/(7+k) + 1);
			monster[j].experience = (i<=0) ? 1 : i;
		}				
}

/*
 *	function to read and process the larn options file
 */
void readopts ()
{
	char s1buf[80], *str, s2buf[80];
	char *getword();
	int i, flag=1, j,k;
	FILE *fp;

	if ((fp = fopen(optsfile, "r")) == (FILE *)NULL) {
			/* user name if no character name */
		strcpy(logname,loginname);
		return;
	}

	while (1) {
		str = s1buf;
		for (i=0;i<80;i++)
			str[i] = '\0';

		if (fgets(s2buf, 80, fp) == (char *)NULL)
			break; /* check for EOF */

			/* remove trailing '\n' */
		strncpy (str, s2buf, strlen(s2buf)-1);

		while (isspace(*str))
			str++; /* eat leading whitespace*/

		switch(*str) {
		case 'b':	
			if (strcmp(str,"bold-off") == 0)
				boldon=0;
			break;
		case 'e':	
			if (strcmp(str,"enable-checkpointing") == 0)
				ckpflag=1;
			break;

		case 'f':
			if (strcmp(str,"female") == 0)	
				sex=0;
			else if (strcmp(str, "fork") == 0)
				do_fork=1;
			break;

			/* name favorite monster */
		case 'm':	
			if (strncmp(str,"monster:", strlen("monster:"))== 0) {
				if ((str=getword(str))==(char *)NULL)
					break;
				if (strlen(str)>=MAXMNAME)
					str[MAXMNAME-1]=0;
				strcpy(usermonster[usermpoint],str);

				/* defined all of em */
				if (usermpoint >= MAXUM)
					break;
				if (isalpha(j=usermonster[usermpoint][0])) {
				    for (k=1; k<MAXMONST+8; k++)
					if (monstnamelist[k] == j) {
				monster[k].name= &usermonster[usermpoint++][0];
						break;
					}
				}
			}
			else if (strcmp(str,"male") == 0)
				sex=1;
			break;

		case 'n':	
			if (strncmp(str,"name:", strlen("name:")) == 0) {
				if ((str=getword(str))==(char *)NULL)
					break;
				if (strlen(str)>=LOGNAMESIZE)
					str[LOGNAMESIZE-1]=0;
				strcpy(logname,str);
				flag=0;
			}
			else if (strcmp(str,"no-introduction") == 0)
				nowelcome=1;
			else if (strcmp(str,"no-beep") == 0)
				nobeep=1;
			else if (strcmp(str,"no-bold") == 0)
				boldon=0;
			else if (strcmp(str,"no-mail") == 0)
				mail=0;
			else if (strcmp(str,"no-nap") == 0)
				nonap=1;
			break;

		case 'c':
			if (strncmp(str,"character:",strlen("character:"))==0){
				int i;
        			if ((str=getword(str)) == (char *)NULL) {
					break;
				}
				strcpy(char_class, str);
				for (i=0; i<strlen(char_class); i++)
				    if (isupper(char_class[i]))
					char_class[i] = tolower(char_class[i]);
        			flag = 0;
				if (!strcmp(char_class, "ogre"))
					char_picked = 'a';
				else if (!strcmp(char_class, "wizard"))
					char_picked = 'b';
				else if (!strcmp(char_class, "klingon"))
					char_picked = 'c';
				else if (!strcmp(char_class, "elf"))
					char_picked = 'd';
				else if (!strcmp(char_class, "rogue"))
					char_picked = 'e';
				else if (!strcmp(char_class, "geek"))
					char_picked = 'f';
				else if (!strcmp(char_class, "dwarf"))
					char_picked = 'g';
				else if (!strcmp(char_class, "rambo"))
					char_picked = 'h';
			}
			break;

		case 's':
			if (strncmp(str,"savefile:",strlen("savefile:"))==0) {
				if ((str=getword(str))==(char *)NULL)
					break;
				if (strlen(str)>=MAXPATHLEN)
					str[MAXPATHLEN-1]=0;
				strcpy(savefilename,str);
				flag=0;
			}
			break;
		};
	}
	/* don't copy loginname to logname if we have already specified
	 * 'name:' in the opts file.
	 */
	if (flag)
		strcpy(logname,loginname);
}
