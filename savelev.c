/* savelev.c	*/
#include "header.h"
#include "sphere.h"
#include "player.h"
#include "itm.h"
#include "monst.h"
#include "extern.h"

#include <errno.h>

extern int errno;

typedef char	Char_Ary[MAXX][MAXY];
typedef short	Short_Ary[MAXX][MAXY];
typedef long	Long_Ary[MAXX][MAXY];
typedef struct_mitem	Mitem_Ary[MAXX][MAXY];

static int w = 0;
static int r = 0;

/* this is the structure that holds the entire dungeon specifications */
typedef struct save_lev_str {
	Short_Ary hitp;
	Mitem_Ary mitem;
	Char_Ary  item;
	Short_Ary iarg;	/* must be long for goldpiles */
	Char_Ary  know;
} Saved_Level;

static Saved_Level *saved_levels[NLEVELS];
static unsigned int level_sums[NLEVELS];
static unsigned int FileSum;

/*
 *	routine to save the present level into storage
 */
savelevel()
{
	Saved_Level *storage = saved_levels[level];

	memcpy((char *)storage->hitp,  (char *)hitp,  sizeof(Short_Ary));
	memcpy((char *)storage->mitem, (char *)mitem, sizeof(Mitem_Ary));
	memcpy((char *)storage->item,  (char *)item,  sizeof(Char_Ary));
	memcpy((char *)storage->iarg,  (char *)iarg,  sizeof(Short_Ary));
	memcpy((char *)storage->know,  (char *)know,  sizeof(Char_Ary));
	level_sums[level] = sum((unsigned char *)storage, sizeof(Saved_Level));
}


/*
 *	routine to restore a level from storage
 */
getlevel()
{
	unsigned int i;

	Saved_Level *storage = saved_levels[level];

	memcpy((char *)hitp,  (char *)storage->hitp,  sizeof(Short_Ary));
	memcpy((char *)mitem, (char *)storage->mitem, sizeof(Mitem_Ary));
	memcpy((char *)item,  (char *)storage->item,  sizeof(Char_Ary));
	memcpy((char *)iarg,  (char *)storage->iarg,  sizeof(Short_Ary));
	memcpy((char *)know,  (char *)storage->know,  sizeof(Char_Ary));

	if (level_sums[level] > 0) {
	  if ((i = sum((unsigned char *)storage,sizeof(Saved_Level))) 
	     != level_sums[level]) {
		lprintf("\nOH NO!!!! INTERNAL MEMORY CORRUPTION!!!!\n");
		lprintf("(sum %u of level %d does not match saved sum %u)\n",
			i, level, level_sums[level]);
		beep();
		lflush();
		sleep(5);
		died(282);
	  }
	} else { 
		level_sums[level] = sum((unsigned char *)storage, 
			sizeof(Saved_Level));
	}
}


/*
 *	to save the game in a file
 */
savegame(fname)
char	*fname;
{
	int	i;
	char genocided; /* To keep save files compatible with old struct monst */
	struct sphere *sp;
	Saved_Level *storage;
	char buf[BUFSIZ];
    	int fd;
	FILE *fp, *popen();

	if (do_fork) 
		if (fork() > 0)
			return(0);

	nosignal = 1;
	lflush();
	savelevel();
	ointerest();

	if (compress) {
		sprintf(buf, "%s > %s 2>/dev/null", COMPRESS, fname);
		if ((fp = popen(buf, "w")) == (FILE *)NULL) {
			fprintf(stderr,"Can't open pipe to <%s> to save game\n", 
				COMPRESS);
			perror("popen");
			nosignal = 0;
			if (do_fork) exit(1); else return(-1);
		}
		fd = fileno(fp);
	} else {
		if ((fd = creat(fname, 0600)) < 0) {
			fprintf(stderr,"Can't open file <%s> to save game\n", 
				fname);
			perror("open of savefile");
			nosignal = 0;
			if (do_fork) exit(1); else return(-1);
		}
	}

	set_score_output();

	FileSum = 0;
/* I didn't comment this out. It was like this when I got here.  -jb */
/* 	bwrite(fd,(char *)level_sums, sizeof(unsigned int) * NLEVELS); */

	bwrite(fd,(char *)beenhere, sizeof(char) * NLEVELS);

	for (i = 0; i < NLEVELS; i++) {
		if (beenhere[i]) {
			storage = saved_levels[i];
			bwrite(fd,(char * )storage, sizeof(Saved_Level));
		}
	}

	bwrite(fd,(char * )c, 100 * sizeof(long));
	bwrite(fd,(char *)&gtime, sizeof(gtime));
	bwrite(fd,(char *)&level, sizeof(level));
	bwrite(fd,(char *)&playerx, sizeof(playerx));
	bwrite(fd,(char *)&playery, sizeof(playerx));
	bwrite(fd,(char * )iven, sizeof(char) * IVENSIZE);
	bwrite(fd,(char * )ivenarg, IVENSIZE * sizeof(short));
	bwrite(fd,(char * )char_class, 20 * sizeof(char));

/* we no longer need to save and reload the names... */
/*	for (i=0; i<MAXSCROLL; i++)
		bwrite(fd,(char *)scrollname[i], sizeof(scrollname[i]));*/
/* This doesn't work. */
/* but I don't know if it's because saving and loading doesn't work
   in general or what... */
/*	for (i=0; i<MAXPOTION; i++)
		bwrite(fd,(char *)potionname[i], sizeof(potionname[i]));*/

	bwrite(fd,(char *)scrollknown, sizeof(int)*MAXSCROLL); 
	bwrite(fd,(char *)potionknown, sizeof(int)*MAXPOTION); 

	bwrite(fd,(char *)spelknow, sizeof(char) * SPNUM);
	bwrite(fd,(char *)&wizard, sizeof(wizard));
	bwrite(fd,(char *)&rmst, sizeof(rmst));
	bwrite(fd,(char *)itm, sizeof(struct _itm) * DNDSIZE);
	bwrite(fd,(char *)course, sizeof(char) * 26);
	bwrite(fd,(char *)&cheat, sizeof(cheat));

		/* Write genocide status */
	for (i = 0; i < MAXMONST; i++)  {
		genocided = ((monster[i].flags & FL_GENOCIDED) != 0);
  		bwrite(fd, &genocided, sizeof(genocided));
	}

		/* save spheres of annihilation */
	sp=spheres;
	for (i = 0; i < c[SPHCAST]; i++) {
		bwrite(fd,(char * )sp, sizeof(struct sphere));
		sp = sp->p;
	}

		/* file sum */
	bwrite(fd,(char *)&FileSum, sizeof(FileSum));
	close(fd);

	if (compress) {
		if (pclose(fp) != 0) {	/* if compress failed */
			unlink(fname);
			nosignal = 0;
			if (do_fork)
				exit(1);
			else
				return (-1);
		}
	}
	nosignal = 0;
	if (do_fork)
		exit(0);
	else
		return(0);
}


restoregame(fname)
char	*fname;
{
	int	i;
	char genocided; /* To keep save files compatible with old struct monst */
	unsigned int thesum, asum;
	struct sphere *sp,*splast;
	Saved_Level * storage;
	char buf[1024], *tmp="/tmp/UtmpXXXXXX";
    	int fd;

	if (compress) {
		if (mkstemp(tmp) == -1) {
			fprintf(stderr,"Can't create temp file to restore game\n");
			perror("mkstemp");
			nosignal=0;
			sleep(4);
			c[GOLD] = c[BANKACCOUNT] = 0;
			died(-265);
			return;
		}
		tempfilename = tmp;
		sprintf(buf, "%s < %s > %s 2>/dev/null", 
			UNCOMPRESS, fname, tempfilename);
		printf(" Uncompressing...");fflush(stdout);
		if (system(buf) != 0) {
			fprintf(stderr,"\"%s\" failed\n",buf);
			perror("system");
			sleep(4);
			c[GOLD] = c[BANKACCOUNT] = 0;
			died(-265);
			unlink(tempfilename);
			return;
		}
		if ((fd = open(tempfilename, O_RDONLY)) < 0) {
			fprintf(stderr,"Can't open temp file to restore game\n");
			perror("open");
			sleep(4);
			c[GOLD] = c[BANKACCOUNT] = 0;
			died(-265);
			return;
		}
	} else {
		if ((fd = open(fname, O_RDONLY)) <= 0) {
			fprintf(stderr,"Can't open file <%s> to restore game\n", 
				fname);
			sleep(4);
			c[GOLD] = c[BANKACCOUNT] = 0;
			died(-265);
			return;
		}
	}
	printf(" Reading data...");fflush(stdout);
	init_cells();

	FileSum = 0;
/* 	bread(fd,(char * )level_sums, sizeof(unsigned int) * NLEVELS); */
	bread(fd,(char * )beenhere,sizeof(char) * NLEVELS);

	for (i = 0; i < NLEVELS; i++) {
		if (beenhere[i]) {
			storage = saved_levels[i];
 			bread(fd,(char * )storage, sizeof(Saved_Level));
		}
	}

	bread(fd,(char * )c, 100 * sizeof(long));
	if (c[HP] <= 0) {
		died(284);
		return;
	}
	bread(fd,(char *)&gtime, sizeof(gtime));
	bread(fd,(char *)&level, sizeof(level));
	c[CAVELEVEL] = level;

	bread(fd,(char *)&playerx, sizeof(playerx));
	bread(fd,(char *)&playery, sizeof(playerx));

	bread(fd,(char * )iven, sizeof(char) * IVENSIZE);
	bread(fd,(char * )ivenarg, sizeof(short) * IVENSIZE);
	bread(fd,(char * )char_class, sizeof(char) * 20);

/*	for (i=0; i<MAXSCROLL; i++)
		bread(fd,(char *)scrollname[i], sizeof(scrollname[i]));*/
/* maybe doesn't work. */
/*	for (i=0; i<MAXPOTION; i++)
		bread(fd,(char *)potionname[i], sizeof(potionname[i]));*/

	bread(fd,(char *)scrollknown, sizeof(int) * MAXSCROLL);
	bread(fd,(char *)potionknown, sizeof(int) * MAXPOTION);

	bread(fd,(char * )spelknow, sizeof(char) * SPNUM);

	bread(fd,(char *)&wizard, sizeof(wizard));
	bread(fd,(char *)&rmst, sizeof(rmst));

	bread(fd,(char *)itm, sizeof(struct _itm) * DNDSIZE);

	bread(fd,(char * )course, sizeof(char) * 26);
	bread(fd,(char * )&cheat, sizeof(cheat));

	/* Read genocide info into monster flags */
	for (i = 0; i < MAXMONST; i++)  {
		bread(fd, &genocided, sizeof(genocided));
		if (genocided) monster[i].flags |= FL_GENOCIDED;
	}
		/* get spheres of annihilation */
	for (i = 0; i < c[SPHCAST]; i++) {
		if ((sp = (struct sphere *)malloc(sizeof(struct sphere ))) 
		     == (struct sphere *)NULL)
			died(282);
		bread(fd, (char * )sp, sizeof(struct sphere ));
		if (i==0) {
			spheres = sp;
			splast = sp;
			sp = sp->p;
		} else {
			splast->p = sp;
			splast = sp;
			sp = sp->p;
		}
	}
	thesum = FileSum;	/* sum of everything so far */
	bread(fd, (char *)&asum, sizeof(asum));
	if (asum != thesum)
		fsorry();

	close(fd);
	if (compress)
		if (unlink(tempfilename) == -1)
			fcheat();

	oldx = oldy = 0;
	if (strcmp(fname, ckpfile) == 0) {
		if (lappend(fname) < 0)
			fcheat();
		else {
			lprc(' ');
			lwclose();
		}
		lcreat((char * )0);
	} else if (unlink(fname) == -1)
		fcheat(); /* can't unlink save file */

	/*	for the greedy cheater checker	*/
	for (i=0; i<6; i++) if (c[i]>99) 
		greedy();
	if (c[HPMAX]>999 || c[SPELLMAX]>125) 
		greedy();

	if (c[LEVEL] == 25 && c[EXPERIENCE] > skill[24]) {
		long	tmp = c[EXPERIENCE]-skill[24]; /* amount to go up */
		c[EXPERIENCE] = skill[24];
		raiseexperience((long)tmp);
	}
	getlevel();
	lasttime = gtime;

	/*
	 *  closedoor() in action.c sets dropflag to stop the player being
	 *  asked to re-open a door they just closed.  However, if they
	 *  save the game before moving off that square, dropflag is lost.
	 *  We restore it here.
	 */
	if (item[playerx][playery] == OCLOSEDDOOR)
		dropflag = 1;
}


/*
	subroutine to not allow greedy cheaters */
greedy()
{
	if (wizard) 
		return;

lprcat("\n\nI am so sorry but your character is a little TOO good!  Since this\n");
lprcat("cannot normally happen from an honest game, I must assume that you cheated.\n");
lprcat("Since you are GREEDY as well as a CHEATER, I cannot allow this game\n");
	lprcat("to continue.\n");
	sleep(5);
	c[GOLD] = c[BANKACCOUNT] = 0;
	died(-267);
	return;
}


/*
	subroutine to not allow altered save files and terminate the attempted
	restart
 */
fsorry()
{
	if (cheat) 
		return;
	lprcat("\nSorry but your savefile has been altered.\n");
	lprcat("However, since I am a good sport, I will let you play.\n");
	lprcat("Be advised, though, that you won't be placed on the scoreboard.");
	cheat = 1;
	sleep(4);
}


/*
	subroutine to not allow game if save file can't be deleted
 */
fcheat()
{
	if (wizard) 
		return;
	if (cheat) 
		return;

lprcat("\nSorry but your savefile can't be deleted.  This can only mean\n");
lprcat("that you tried to CHEAT by protecting the directory the savefile\n");
lprcat("is in.  Since this is unfair to the rest of the Ularn community, I\n");
	lprcat("cannot let you play this game.\n");
	sleep(5);
	c[GOLD] = c[BANKACCOUNT] = 0;
	died(-268);
	return;
}


init_cells()
{
	int	i;

	for (i = 0; i < NLEVELS; i++) {
	    if ((saved_levels[i] = (Saved_Level * )
	        malloc(sizeof(Saved_Level))) == (Saved_Level *)NULL)
		    died(-285);
	}
}


bwrite(fd, buf, num)
int fd;
char *buf;
long num;
{
	int nwrote;
	static int ncalls = 0;

	ncalls++;
	if ((nwrote = write(fd, buf, num)) == -1) {
		fprintf(stderr, "Error writing to save file\n");
		fprintf(stderr, "errno = %d\t[%s]\n", errno, strerror(errno));
		fprintf(stderr, "    Wrote %d bytes so far\n", w);
		fprintf(stderr, "        Call: %d\n", ncalls);
		abort();
		perror("write");
		sleep(4);
		died(284);
	}
	w += nwrote;
	if (nwrote != num) {
		fprintf(stderr, "Error writing to save file\n");
		fprintf(stderr, "wrote %d, wanted %d\n", nwrote, num);
		fprintf(stderr, "    Wrote %d bytes so far\n", w);
		fprintf(stderr, "        Call: %d\n", ncalls);
		sleep(4);
		died(284);
	}
	FileSum += sum((unsigned char *)buf, num);
}

bread(fd, buf, num)
int fd;
char *buf;
long num;
{
	int nread;
	static int ncalls = 0;

	ncalls++;
	if ((nread = read(fd, buf, num)) == -1) {
		fprintf(stderr, "Error reading from save file\n");
		fprintf(stderr, "errno = %d\t[%s]\n", errno, strerror(errno));
		fprintf(stderr, "    Read %d bytes so far\n", w);
		fprintf(stderr, "        Call: %d\n", ncalls);
		perror("read");
		sleep(4);
		died(284);
	}
	r += nread;
	if (nread != num) {
		fprintf(stderr, "Error reading save file\n");
		fprintf(stderr, "  Got %d, wanted %d bytes\n", nread, num);
		fprintf(stderr, "    Read %d bytes so far\n", r);
		fprintf(stderr, "        Call: %d\n", ncalls);
		sleep(4);
		died(284);
	}
	FileSum += sum((unsigned char *)buf, num);
}

unsigned int
sum(data, n)
unsigned char *data;
int n;
{
	unsigned int sum;
	int c, nb;

	sum = nb = 0;
	while (nb++ < n) {
		c = *data++;
		if (sum & 01)
			sum = (sum >> 1) + 0x8000;
		else
			sum >>= 1;
		sum += c;
		sum &= 0xFFFF;
	}
	return(sum);
}
