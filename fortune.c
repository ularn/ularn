/* fortune.c */
#include "header.h"
#include "extern.h"
/*
 *	function to return a random fortune from the fortune file
 */
static char *base=NULL;	/* pointer to the fortune text */
static char **flines=NULL;	/* array of pointers to each fortune */
static int fortune_fd=0;	/* true if we have loaded the fortune info */
static int nlines=0;	/* # lines in fortune database */

char *fortune(file)
char *file;
{
	char *p;
	int lines,tmp;
	struct stat stat;
	int retval;
	char *malloc();

	if (fortune_fd==0) {
		/* open the file */
		if ((fortune_fd=open(file,O_RDONLY)) < 0)	
			return(0); /* can't find file */

		/* find out how big fortune file is and get memory for it */
		retval = fstat(fortune_fd,&stat);
		if ((retval < 0) || ((base=malloc((unsigned)(1+stat.st_size))) 
		     == (char *)NULL)) {
			close(fortune_fd); 
			fortune_fd= -1; 
			if (base) free(base); 
			return(0); 	/* can't stat file */
		}

		/* read in the entire fortune file */
		if ((stat.st_size = read(fortune_fd,(char *)base,stat.st_size))
		     == -1) {
			close(fortune_fd); 
			fortune_fd= -1; 
			if (base) free(base); 
			return(0); 	/* can't read file */
		}
		close(fortune_fd);  
		base[stat.st_size] = 0 ; /* final NULL termination */

	/* count up all the lines (and NULL terminate) to know memory needs */
		for (p=base,lines=0; p<base+stat.st_size; p++) /* count lines */
			if (*p == '\n') { 
				*p= 0;
				lines++;
			}
		nlines = lines;

		/* get memory for array of pointers to each fortune */
		if ((flines=(char**)malloc((unsigned)(nlines*sizeof(char*)))) 
		     == (char **)NULL) {
			if (base) free(base); 
			fortune_fd= -1; 
			return(0); /* malloc() failure */
		}

		/* now assign each pointer to a line */
		for (p=base,tmp=0; tmp<nlines; tmp++) {
			flines[tmp]=p;  
			while (*p++)
				; /* advance to next line */
		}
	}

	if (fortune_fd > 2)	/* if we have a database to look at */
		return(flines[rund((nlines<=0)?1:nlines)]);
	else 
	    return(0);
}
