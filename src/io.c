/* io.c
 *
 *	Below are the functions in this file:
 *
 *	getcharacter() 	Routine to read in one character from the terminal
 *	newgame() 	Subroutine to save the initial time and seed rnd()
 *
 *	FILE OUTPUT ROUTINES
 *
 *	lprintf(format,args . . .)	printf to the output buffer
 *	lwrite(buf,len)			write a buffer to the output buffer
 *	lprcat(str)			sent string to output buffer
 *
 *	FILE OUTPUT MACROS (in header.h)
 *
 *	lprc(character)			put the character into the output buffer
 *
 *	FILE INPUT ROUTINES
 *
 *	long lgetc()			read one character from input buffer
 *	lrfill(address,number)		put input bytes into a buffer
 *	char *lgetw()			get a whitespace ended word from input
 *	char *lgetl()			get a \n or EOF ended line from input
 *
 *	FILE OPEN / CLOSE ROUTINES
 *
 *	lcreat(filename)		create a new file for write
 *	lopen(filename)			open a file for read
 *	lappend(filename)		open for append to an existing file
 *	lrclose()			close the input file
 *	lwclose()			close output file
 *	lflush()			flush the output buffer
 *
 *	cursors()		position cursor at [1,24] (saves memory)
 *  cl_line(x,y)         	Clear line at [1,y] and leave cursor at [x,y]
 *  cl_up(x,y)    		Clear screen from [x,1] to current line.
 *  cl_dn(x,y) 			Clear screen from [1,y] to end of display.
 *  standout(str)	 	Print the string in standout mode.
 *  set_score_output() 		Called when output should be literally printed.
 ** putcharacter(ch)		Print one character in decoded output buffer.
 ** flush_buf()			Flush buffer with decoded output.
 ** init_term()			Terminal initialization -- setup termcap info
 *  tmcapcnv(sd,ss)  	Routine to convert vt100 \33's to termcap format
 *  beep() 	  Routine to emit a beep if enabled (see no-beep in .Ularnopts)
 *
 * Note: ** entries are available only in termcap mode.
 */
#include "header.h"
#include "extern.h"

#define LINBUFSIZE 128		/* size of the lgetw() and lgetl() buffer */

extern short ospeed;
static int lfd;			/*  output file numbers	*/
static int fd;				/*  input file numbers	*/
int ipoint=BUFSIZ,iepoint=BUFSIZ;	/*  input buffering pointers  */
static char lgetwbuf[LINBUFSIZE];	/* get line (word) buffer*/

/*
 *	getcharacter() 	Routine to read in one character from the terminal
 */
getcharacter ()
{
	char byt;
	lflush();		/* be sure output buffer is flushed */
/*        byt=getchar();		/* get character from terminal */
	byt=getc(stdin);
	return(byt);
}


/*
 *	newgame() 
 *		Subroutine to save the initial time and seed rnd()
 */
newgame ()
{
	long *p,*pe;

	for (p=c,pe=c+100; p<pe; *p++ =0)
		;
	time(&initialtime);
	srand((unsigned)initialtime);
	lcreat((char*)0);	/* open buffering for output to terminal */
}

/*
 *	lprintf(format,args . . .)		printf to the output buffer
 *		char *format;
 *		??? args . . .
 *
 *	Enter with the format string in "format", as per printf() usage
 *		and any needed arguments following it
 *Note: lprintf() only supports %s, %c and %d, with width modifier and left
 *	or right justification.
 *	No correct checking for output buffer overflow is done, but flushes
 *		are done beforehand if needed.
 *	Returns nothing of value.
 */
/*VARARGS*/

int lprintf(char *fmt, ...)
{
    va_list ap;
	char *outb,*tmpb;
	long wide,left,cont,n;		/* data for lprintf	*/
	char db[12];			/* %d buffer in lprintf	*/

	va_start(ap, fmt);	/* initialize the var args pointer */

	if (lpnt >= lpend) lflush();
	outb = lpnt;
	for ( ; ; ) {
		while (*fmt != '%')
			if (*fmt) *outb++ = *fmt++;
			else {
				lpnt=outb;
				return;
			}
		wide = 0;
		left = 1;
		cont=1;
		while (cont)
			switch(*(++fmt)) {
			case 'd':
			case 'u':
				n = (long) va_arg(ap, long);
				if (n<0 && *fmt == 'd') {
					n = -n;
					*outb++ = '-';
					if (wide) --wide;
				}
				tmpb = db+11;
				*tmpb = (char)(n % 10 + '0');
				while (n>9)
					*(--tmpb) = (char)((n /= 10) % 10+'0');
				if (wide==0)
					while (tmpb < db+12)
						*outb++ = *tmpb++;
				else {
					wide -= db-tmpb+12;
					if (left)
						while (wide-- > 0)
							*outb++ = ' ';
					while (tmpb < db+12)
						*outb++ = *tmpb++;
					if (left==0)
						while (wide-- > 0)
							*outb++ = ' ';
				}
				cont=0;
				break;

			case 's':
				if ((tmpb = (char *)va_arg(ap, char *))
				     == (char *)NULL) {
					cont=0;
					break;
				}
				if (wide==0)  {
					while (*outb++ = *tmpb++);
					--outb;
				}
				else {
					n = wide - strlen(tmpb);
					if (left)
						while (n-- > 0)
							*outb++ = ' ';
					while (*outb++ = *tmpb++)
						;
					--outb;
					if (left==0)
						while (n-- > 0)
							*outb++ = ' ';
				}
				cont=0;
				break;

			case 'c':
				*outb++ = (int)va_arg(ap, int);
				cont=0;
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				wide = 10*wide + *fmt - '0';
				break;
			case '-':
				left = 0;
				break;
			default:
				*outb++ = *fmt;
				cont=0;
				break;
			};
		fmt++;
	}
	va_end(ap);
}

/*
 *	lwrite(buf,len)
 *		char *buf;
 *		int len;
 *
 *		write a buffer to the output buffer
 *
 *	Enter with the address and number of bytes to write out
 *	Returns nothing of value
 */
lwrite (buf, len)
char *buf;
int len;
{
	char *str;
	int num2, write_len;

	/* don't copy data if can just write it */
	if (len > 399)  {
		for (str=buf;  len>0; --len)
			lprc(*str++);
		lflush();
		write_len = write(lfd,buf,len);
	}
	else while (len) {
		if (lpnt >= lpend)
			lflush();	/* if buffer is full flush it	*/

		/*	# bytes left in output buffer	*/
		num2 = lpbuf+BUFSIZ-lpnt;

		if (num2 > len) num2=len;
		str = lpnt;
		len -= num2;
		while (num2--)
			*str++ = *buf++;	/* copy in the bytes */
		lpnt = str;
	}
}

/*
 *	long lgetc()			Read one character from input buffer
 *
 *  Returns 0 if EOF, otherwise the character
 */
long lgetc1()
{
	int i;

	/* if (ipoint != iepoint)   */
		/* return(inbuffer[ipoint++]); */
	/* if (iepoint!=BUFSIZ)    */
		/* return(0); */

	if ((i=read(fd,inbuffer,BUFSIZ))<=0) {
		if (i!=0)   {
			fprintf(stderr,"lgetc1: error reading from input file\n");
		}
		iepoint = ipoint = 0;
		return(0);
	}
	ipoint=1;
	iepoint=i;
	return(*inbuffer);
}

/*
 *	lrfill(address,number)			put input bytes into a buffer
 *		char *address;
 *		int number;
 *
 *	Reads "number" bytes into the buffer pointed to by "address".
 *	Returns nothing of value
 */
lrfill (adr, num)
char *adr;
int num;
{
	char *pnt;
	int num2;

	while (num) {
		if (iepoint == ipoint) {
			if (num>5) {
				if (read(fd,adr,num) != num) {
					fprintf(stderr,"lrfill: error reading from input file\n");
				}
				num=0;
			}
			else {
				*adr++ = lgetc();
				--num;
			}
		}
		else {
		/*	# of bytes left in the buffer	*/
			num2 = iepoint-ipoint;

			if (num2 > num)
				num2=num;
			pnt = inbuffer+ipoint;
			num -= num2;
			ipoint += num2;
			while (num2--)
				*adr++ = *pnt++;
		}
	}
}

/*
 *	char *lgetw()		Get a whitespace ended word from input
 *
 *	Returns pointer to a buffer that contains word.  If EOF, returns a 0
 */
char *lgetw()
{
	char *lgp;
	int cc, n=LINBUFSIZE, quote=0;

	lgp = lgetwbuf;

	do
		cc = lgetc();
	while (isspace(cc));

	for ( ; ; --n, cc = lgetc()) {
		if ((cc==0) && (lgp==lgetwbuf))
			return((char *)0);	/* EOF */

		if ((n<=1) || ((cc<=32) && (quote==0))) {
			*lgp = 0;
			break;
		}
		if (cc != '"')
			*lgp++ = cc;
		else
			quote ^= 1;
	}
	return(lgetwbuf);
}

/*
 *	char *lgetl()
 * 		Function to read in a line ended by newline or EOF
 *
 *Returns pointer to a buffer that contains the line.  If EOF, returns 0
 */
char *lgetl()
{
	int i=LINBUFSIZE,ch;
	char *str=lgetwbuf;

	for ( ; ; --i) {
		if ((*str++ = ch = lgetc()) == 0) {
			if (str == lgetwbuf+1)
				return((char *)0); /* EOF */
			break;
		}
		if ((ch=='\n') || (i<=1))
			break; /* line ended by \n */
	}
	*str = 0;
	return(lgetwbuf);	/* line ended by EOF */
}

/*
 *	lcreat(filename)			Create a new file for write
 *		char *filename;
 *
 *	lcreat((char*)0); means to the terminal
 *	Returns -1 if error, otherwise the file descriptor opened.
 */
lcreat(str)
char *str;
{
	lpnt = lpbuf;
	lpend = lpbuf+BUFSIZ;
	if (str==(char *)0)
		return(lfd=1);

	if ((lfd=open(str, (O_WRONLY | O_CREAT | O_TRUNC), 0644)) < 0) {
		lfd=1;
		lprintf("error creating file <%s>\n",str);
		lflush();
		return(-1);
	}
	return(lfd);
}

/*
 *	lopen(filename)			Open a file for read
 *		char *filename;
 *
 *	lopen(0) means from the terminal
 *	Returns -1 if error, otherwise the file descriptor opened.
 */
lopen (str)
char *str;
{
	ipoint = iepoint = BUFSIZ;
	if (str==(char *)0)
		return(fd=0);

	if ((fd=open(str, O_RDONLY)) < 0) {
		lwclose();
		lfd=1;
		lpnt=lpbuf;
		return(-1);
	}
	return(fd);
}

/*
 *	lappend(filename)		Open for append to an existing file
 *		char *filename;
 *
 *	lappend(0) means to the terminal
 *	Returns -1 if error, otherwise the file descriptor opened.
 */
lappend (str)
char *str;
{
	lpnt = lpbuf;
	lpend = lpbuf+BUFSIZ;
	if (str==(char *)0)
		return(lfd=1);

	if ((lfd=open(str, (O_WRONLY | O_APPEND))) < 0) {

		lfd=1;
		return(-1);
	}
	lseek(lfd, 0L, 2);	/* seek to end of file */
	return(lfd);
}

/*
 *	lrclose()
 *			close the input file
 *
 *	Returns nothing of value.
 */
lrclose()
{
	if (fd > 0)
		close(fd);
}

/*
 *	lwclose()			close output file flushing if needed
 *
 *	Returns nothing of value.
 */
lwclose ()
{
	lflush();
	if (lfd > 2)
		close(lfd);
}

/*
 *	lprcat(string)			append a string to the output buffer
 *					avoids calls to lprintf (time consuming)
 */
lprcat (str)
char *str;
{
	char *str2;

	if (lpnt >= lpend)
		lflush();
	str2 = lpnt;
	while (*str2++ = *str++)
		;
	lpnt = str2 - 1;
}

/*
 * Warning: ringing the bell is control code 7. Don't use in defines.
 * Don't change the order of these defines.
 * Also used in helpfiles. Codes used in helpfiles should be \E[1 to \E[7 with
 * obvious meanings.
 */

static char cap[256];
static char *outbuf=0;	/* translated output buffer */
char *CM, *CE, *CD, *CL, *SO, *SE, *AL, *DL;/* Termcap capabilities */
char *HO, *BC, *UP;
/* extern char *PC; */

/*
 * init_term()		Terminal initialization -- setup termcap info
 */
init_term ()
{
	char termbuf[1024];
	char *pc, *capptr = cap+10;
	char *tgetstr(), *term, *getenv();

	switch (tgetent(termbuf, term = getenv("TERM"))) {
	case -1:
		fprintf(stderr, "Cannot open termcap file.\n");
		exit(1);
	case 0:
		fprintf(stderr, "Cannot find termcap entry for %s\n", term);
		exit(1);
	};

	SO = tgetstr("so", &capptr);  /* Begin standout mode */
	SE = tgetstr("se", &capptr);  /* End standout mode */
	CM = tgetstr("cm", &capptr);  /* Cursor motion */
	CE = tgetstr("ce", &capptr);  /* Clear to eoln */
	CL = tgetstr("cl", &capptr);  /* Clear screen */
	CD = tgetstr("cd", &capptr);  /* Clear to end of display */
	AL = tgetstr("al", &capptr);  /* Insert line */
	DL = tgetstr("dl", &capptr);  /* Delete line */
	UP = tgetstr("up", &capptr);	/* cursor up */
	HO = tgetstr("ho", &capptr);	/* home cursor */

/*	if (pc = tgetstr("pc", &capptr)) /* padding character */
/*		PC = pc;
/*
	else
		*PC = '\0';
*/

	if (!(BC = tgetstr("bc", &capptr))) {	/* backspace */
		if (!(BC=tgetstr("le", &capptr))) {
			if (!tgetflag("bs")) {
				fprintf(stderr,"Sorry, terminal must backspace\n");
				exit(1);
			}
			BC = capptr;
			capptr += 2;
			*BC = '\b';
		}
	}

	/* can't find cursor motion entry */
	if (!CM){
	    fprintf(stderr, "Sorry, for a %s,",term);
	    fprintf(stderr, "I can't find the cursor motion entry\n");
	    exit(1);
	}
	/* can't find clear to end of line entry */
	if (!CE)	{
		fprintf(stderr, "Sorry, for a %s",term);
		fprintf(stderr,", I don't know how to clear to end of line\n");
		exit(1);
	}
	/* can't find clear entire screen entry */
	if (!CL)	{
		fprintf(stderr, "Sorry, for a %s",term);
		fprintf(stderr, ", I don't know how to clear the screen\n");
		exit(1);
	}

	/* get memory for decoded output buffer*/
	if ((outbuf=(char *)malloc(BUFSIZ+16))==(char *)NULL) {
	  fprintf(stderr,"Error malloc'ing memory for decoded output buffer\n");
		died(-285);
	}
}

/*
 * cl_up(x,y) Clear screen from [x,1] to current position. Leave cursor at [x,y]
 */
cl_up (x,y)
int x, y;
{
	int i;

	cursor(1,1);
	for (i=1; i<=y; i++)   {
		*lpnt++ = CL_LINE;
		*lpnt++ = '\n';
	}
	cursor(x,y);
}

/*
 * cl_dn(x,y) 	Clear screen from [1,y] to end of display. Leave cursor at [x,y]
 */
cl_dn (x,y)
int x, y;
{
	int i;

	cursor(1,y);
	if (!CD) {
		*lpnt++ = CL_LINE;
		for (i=y; i<=24; i++) {
			*lpnt++ = CL_LINE;
			if (i!=24)
				*lpnt++ = '\n';
		}
		cursor(x,y);
	}
	else
		*lpnt++ = CL_DOWN;
	cursor(x,y);
}

/*
 * standout(str)  Print the argument string in inverse video (standout mode).
 */
standout (str)
char *str;
{
	if (boldon == 0) {
		lprcat(str);
		return;
	}
	*lpnt++ = ST_START;
	while (*str)
		*lpnt++ = *str++;
	*lpnt++ = ST_END;
}

/*
 * set_score_output() 	Called when output should be literally printed.
 */
set_score_output() {
	enable_scroll = -1;
}

/*
 *	lflush()					Flush the output buffer
 *
 *	Returns nothing of value.
 *	for termcap version: Flush output in output buffer according to output
 *	status as indicated by `enable_scroll'
 */
static int scrline=18; /* line # for wraparound instead of scrolling if no DL */

lflush ()
{
	int lpoint;
	char *str;
	static int curx = 0;
	static int cury = 0;

	if ((lpoint = lpnt - lpbuf) > 0) {
		if (enable_scroll <= -1) {
			flush_buf();
			if (write(lfd,lpbuf,lpoint) != lpoint) {
			       fprintf(stderr,"lflush: error writing to output file\n");
			}
			lpnt = lpbuf;	/* point back to beginning of buffer */
			return;
		}
		for (str = lpbuf; str < lpnt; str++) {
			if (*str>=32)	{
				putcharacter (*str);
				curx++;
			}
			else switch (*str) {
			case CLEAR:
				tputs (CL, 0, putcharacter);
				curx = cury = 0;
				break;

			case CL_LINE:
				tputs (CE, 0, putcharacter);
				break;

			case CL_DOWN:
				tputs (CD, 0, putcharacter);
				break;

			case ST_START:
				tputs (SO, 0, putcharacter);
				break;

			case ST_END:
				tputs (SE, 0, putcharacter);
				break;

			case CURSOR:
				curx = *++str - 1;
				cury = *++str - 1;
				tputs (tgoto (CM, curx, cury), 0, putcharacter);
				break;

			case '\n':
				if ((cury == 23) && enable_scroll) {
				    if (!DL || !AL) {
					if (++scrline > 23)
						scrline=19;
					if (++scrline > 23)
						scrline=19;
					tputs(tgoto (CM,0,scrline),
					      0, putcharacter);
					tputs(CE, 0, putcharacter);

					if (--scrline < 19)
						scrline=23;
					tputs(tgoto (CM, 0, scrline),
						0, putcharacter);
					tputs(CE, 0, putcharacter);
				    }
				    else {
					tputs(tgoto (CM, 0, 19),
						0, putcharacter);
					tputs(DL, 0, putcharacter);
					tputs(tgoto (CM, 0, 23),
						0, putcharacter);
				    }
				}
				else {
					putcharacter ('\n');
					cury++;
				}
				curx = 0;
				break;

			default:
				putcharacter (*str);
				curx++;
			};
		}
	}
	lpnt = lpbuf;
	flush_buf();	/* flush real output buffer now */
}

static int ind=0;

/*
 * putcharacter(c)		Print one character in decoded output buffer.
 */
putcharacter(c)
int c;
{
	outbuf[ind++]=c;
	if(ind>=BUFSIZ)
		flush_buf();
}

/*
 * flush_buf()			Flush buffer with decoded output.
 */
flush_buf()
{
	int write_len;
	
	if (ind)
		write_len = write(lfd, outbuf, ind);
	ind = 0;
}


/*
 *	tmcapcnv(sd,ss)
 *
 *		Converts strings of form:	^ [ [ <digit> m
 *		to ST_START
 *
 *		and ^ [ [ m to ST_END
 *
 *
 */
tmcapcnv(sd,ss)
char *sd, *ss;
{
	int tmstate=0;	/* 0=normal, 1=ESC 2=[ 3=# */
	char tmdigit=0;	/* the # in \33[#m */
	int c, c2;

	while (ss && *ss) {
		switch(tmstate) {
		case 0:
		if ((c = *ss++) != EOF) {
			if (c == '^') {
				if ((c2 = *ss++) != EOF) {
					if (c2 == '[') {
						tmstate++;
						ss--;
						goto br;
					}
					else {
						*sd++ = c;
						ss--;
					}
				} else
					ss--;
			} else
				ss--;
		}
ign:
		*sd++ = *ss;
ign2:
		tmstate = 0;
br:
		break;

		case 1:
			if (*ss!='[')
				goto ign;
			tmstate++;
			break;
		case 2:
			if (isdigit(*ss)) {
				tmdigit= *ss-'0';
				tmstate++;
				break;
			}
			if (*ss == 'm') {
				*sd++ = ST_END;
				goto ign2;
			}
			goto ign;
		case 3:
			if (*ss == 'm') {
				if (tmdigit) *sd++ = ST_START;
				else *sd++ = ST_END;
				goto ign2;
			}
		default:
			goto ign;
		};
		ss++;
	}
	*sd='\0';
}


char *
getword(buf)
char *buf;
{
	char c;
	static char s[80];
	int quote=0, n, i=0;

	n = strlen(buf);

	/* advance to end of first word */
	while (n-- && !isspace(*buf))
		buf++;
	if (n<=0)
		return((char *)NULL);

	/* advance to beginning of next word */
	while (n-- && isspace(*buf))
		buf++;
	if (n<=0)
		return((char *)NULL);

	n = strlen(buf);

	/* copy next word */
	while (n-- && (c = *buf++)) {

		/* if it's a space and we haven't seen a quote, this is
	  	 * the end of the word
	 	 */
		if (isspace(c) && quote==0)
			break;

		/* if it's the first quote, set a flag and continue,
		 * else if second quote, it's the end of the word, break.
		 */
		if (c == '"') {
			if (quote == 0) {
				quote = 1;
				continue;
			} else
				break;
		}
		/* else copy the character */
		s[i++] = c;
	}
	/* null terminate */
	s[i] = '\0';

	/* if we have a word, return it - else return NULL */
	if (i)
		return (s);
	else
		return ((char *)NULL);
}
