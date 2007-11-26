/* savelev.c	*/
#include "header.h"
#include "sphere.h"
#include "player.h"
#include "itm.h"
#include "monst.h"
#include "extern.h"

#ifdef ZLIB
#include <zlib.h>
#endif

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

/* This is the structure that holds the save game information before we
 * compress it
 */
typedef struct save_buffer {
    char *data;
    int data_len;
    int data_ptr;
} Save_Buffer;

/* private module functions */
void bwrite(Save_Buffer *save_buffer, char *buf, long num);
void bread(Save_Buffer *save_buffer, char *buf, long num);
void fill_buffer(char *fname, Save_Buffer *save_buffer);
void flush_buffer(char *fname, Save_Buffer *save_buffer);

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
	Save_Buffer *save_data;
	char buf[BUFSIZ];
    	int fd;
	FILE *fp, *popen();

	if (do_fork) 
		if (fork() > 0)
			return(0);

	if (!(save_data = malloc(sizeof(Save_Buffer)))) died(284);
	/* This value is completely arbitrary */
	if (!(save_data->data = malloc(256))) died(284);
	save_data->data_len = 256;
	save_data->data_ptr = 0;

	nosignal = 1;
	lflush();
	savelevel();
	ointerest();
	set_score_output();

	FileSum = 0;
/* I didn't comment this out. It was like this when I got here.  -jb */
/* 	bwrite(fd,(char *)level_sums, sizeof(unsigned int) * NLEVELS); */

	bwrite(save_data,(char *)beenhere, sizeof(char) * NLEVELS);

	for (i = 0; i < NLEVELS; i++) {
		if (beenhere[i]) {
			storage = saved_levels[i];
			bwrite(save_data,(char * )storage, sizeof(Saved_Level));
		}
	}

	bwrite(save_data,(char * )c, 100 * sizeof(long));
	bwrite(save_data,(char *)&gtime, sizeof(gtime));
	bwrite(save_data,(char *)&level, sizeof(level));
	bwrite(save_data,(char *)&playerx, sizeof(playerx));
	bwrite(save_data,(char *)&playery, sizeof(playerx));
	bwrite(save_data,(char * )iven, sizeof(char) * IVENSIZE);
	bwrite(save_data,(char * )ivenarg, IVENSIZE * sizeof(short));
	bwrite(save_data,(char * )char_class, 20 * sizeof(char));

/* we no longer need to save and reload the names... */
/*	for (i=0; i<MAXSCROLL; i++)
		bwrite(fd,(char *)scrollname[i], sizeof(scrollname[i]));*/
/* This doesn't work. */
/* but I don't know if it's because saving and loading doesn't work
   in general or what... */
/*	for (i=0; i<MAXPOTION; i++)
		bwrite(fd,(char *)potionname[i], sizeof(potionname[i]));*/

	bwrite(save_data,(char *)scrollknown, sizeof(int)*MAXSCROLL); 
	bwrite(save_data,(char *)potionknown, sizeof(int)*MAXPOTION); 

	bwrite(save_data,(char *)spelknow, sizeof(char) * SPNUM);
	bwrite(save_data,(char *)&wizard, sizeof(wizard));
	bwrite(save_data,(char *)&rmst, sizeof(rmst));
	bwrite(save_data,(char *)itm, sizeof(struct _itm) * DNDSIZE);
	bwrite(save_data,(char *)course, sizeof(char) * 26);
	bwrite(save_data,(char *)&cheat, sizeof(cheat));

		/* Write genocide status */
	for (i = 0; i < MAXMONST; i++)  {
		genocided = ((monster[i].flags & FL_GENOCIDED) != 0);
  		bwrite(save_data, &genocided, sizeof(genocided));
	}

		/* save spheres of annihilation */
	sp=spheres;
	for (i = 0; i < c[SPHCAST]; i++) {
		bwrite(save_data,(char * )sp, sizeof(struct sphere));
		sp = sp->p;
	}

		/* file sum */
	bwrite(save_data,(char *)&FileSum, sizeof(FileSum));
	flush_buffer(fname, save_data);

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
	Save_Buffer *save_data;
	char buf[1024], *tmp="/tmp/UtmpXXXXXX";

	printf(" Reading data...");fflush(stdout);
	if (!(save_data = malloc(sizeof(Save_Buffer)))) died(284);
	if (!(save_data->data = malloc(1024))) died(284);
	save_data->data_len = 1024;
	save_data->data_ptr = 0;
	fill_buffer(fname, save_data);
	init_cells();

	FileSum = 0;
/* 	bread(fd,(char * )level_sums, sizeof(unsigned int) * NLEVELS); */
	bread(save_data,(char * )beenhere,sizeof(char) * NLEVELS);

	for (i = 0; i < NLEVELS; i++) {
		if (beenhere[i]) {
			storage = saved_levels[i];
 			bread(save_data,(char * )storage, sizeof(Saved_Level));
		}
	}

	bread(save_data,(char * )c, 100 * sizeof(long));
	if (c[HP] <= 0) {
		died(284);
		return;
	}
	bread(save_data,(char *)&gtime, sizeof(gtime));
	bread(save_data,(char *)&level, sizeof(level));
	c[CAVELEVEL] = level;

	bread(save_data,(char *)&playerx, sizeof(playerx));
	bread(save_data,(char *)&playery, sizeof(playerx));

	bread(save_data,(char * )iven, sizeof(char) * IVENSIZE);
	bread(save_data,(char * )ivenarg, sizeof(short) * IVENSIZE);
	bread(save_data,(char * )char_class, sizeof(char) * 20);

/*	for (i=0; i<MAXSCROLL; i++)
		bread(save_data,(char *)scrollname[i], sizeof(scrollname[i]));*/
/* maybe doesn't work. */
/*	for (i=0; i<MAXPOTION; i++)
		bread(save_data,(char *)potionname[i], sizeof(potionname[i]));*/

	bread(save_data,(char *)scrollknown, sizeof(int) * MAXSCROLL);
	bread(save_data,(char *)potionknown, sizeof(int) * MAXPOTION);

	bread(save_data,(char * )spelknow, sizeof(char) * SPNUM);

	bread(save_data,(char *)&wizard, sizeof(wizard));
	bread(save_data,(char *)&rmst, sizeof(rmst));

	bread(save_data,(char *)itm, sizeof(struct _itm) * DNDSIZE);

	bread(save_data,(char * )course, sizeof(char) * 26);
	bread(save_data,(char * )&cheat, sizeof(cheat));

	/* Read genocide info into monster flags */
	for (i = 0; i < MAXMONST; i++)  {
		bread(save_data, &genocided, sizeof(genocided));
		if (genocided) monster[i].flags |= FL_GENOCIDED;
	}
		/* get spheres of annihilation */
	for (i = 0; i < c[SPHCAST]; i++) {
		if ((sp = (struct sphere *)malloc(sizeof(struct sphere ))) 
		     == (struct sphere *)NULL)
			died(282);
		bread(save_data, (char * )sp, sizeof(struct sphere ));
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
	bread(save_data, (char *)&asum, sizeof(asum));
	if (asum != thesum)
		fsorry();

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

	free(save_data->data);
	free(save_data);
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


void bwrite(Save_Buffer *save_buffer, char *buf, long num)
{
	int nwrote;
	static int ncalls = 0;

	if (save_buffer->data_len < save_buffer->data_ptr + num) {
		save_buffer->data = realloc(save_buffer->data,
			save_buffer->data_len + num);
		save_buffer->data_len += num;
	}

	memcpy(save_buffer->data + save_buffer->data_ptr, buf, num);
	save_buffer->data_ptr += num;

	FileSum += sum((unsigned char *)buf, num);
}

void bread(Save_Buffer *save_buffer, char *buf, long num)
{
	int nread;
	static int ncalls = 0;

	ncalls++;
	if (save_buffer->data_len < save_buffer->data_ptr + num) {
		fprintf(stderr, "Error reading from save file\n");
		fprintf(stderr, "        Call: %d\n", ncalls);
		perror("read");
		sleep(4);
		died(284);
	}
	memcpy(buf, save_buffer->data + save_buffer->data_ptr, num);
	save_buffer->data_ptr += num;
	FileSum += sum((unsigned char *)buf, num);
}

void fill_buffer(char *fname, Save_Buffer *save_buffer)
{
	int fd, err;
	size_t bytes_read;
	char buffer[1024];

#ifdef ZLIB
	Byte *compr;
	uLong uncomprLen;
#endif

	if ((fd = open(fname, O_RDONLY)) <= 0) {
		fprintf(stderr,"Can't open file <%s> to restore game\n", 
			fname);
		sleep(4);
		c[GOLD] = c[BANKACCOUNT] = 0;
		died(-265);
		return;
	}

	while(bytes_read = read(fd, buffer, 1024)) {
		if (save_buffer->data_len < save_buffer->data_ptr + bytes_read) {
			save_buffer->data = realloc(save_buffer->data,
				save_buffer->data_len + bytes_read);
			save_buffer->data_len += bytes_read;
		}
		memcpy(save_buffer->data + save_buffer->data_ptr, buffer, bytes_read);
		save_buffer->data_ptr += bytes_read;
	}
        save_buffer->data_ptr = 0;

#ifdef ZLIB
	uncomprLen = save_buffer->data_len * 1000;
	if (!(compr = malloc(uncomprLen))) died(284);
	err = uncompress(compr, &uncomprLen, save_buffer->data, save_buffer->data_len);
	if (err != Z_OK) died(284);
	free(save_buffer->data);
	save_buffer->data = realloc(compr, uncomprLen);
	save_buffer->data_len = uncomprLen;
#endif

	close(fd);
}

void flush_buffer(char *fname, Save_Buffer *save_buffer)
{
	int fd, err;

#ifdef ZLIB
	uLong comprLen = save_buffer->data_len * 2;
	Byte *compr;
#endif

	if ((fd = creat(fname, 0600)) < 0) {
		fprintf(stderr,"Can't open file <%s> to save game\n", 
			fname);
		perror("open of savefile");
		nosignal = 0;
		exit(1);
	}

#ifdef ZLIB
	if (!(compr = malloc(comprLen))) died(284);
	err = compress(compr, &comprLen, (const Bytef*)save_buffer->data, save_buffer->data_len);
	if (err != Z_OK) died(284);
	write(fd, compr, comprLen);
	free(compr);
#else
	write(fd, save_buffer->data, save_buffer->data_len);
#endif

	close(fd);
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
