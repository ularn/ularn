/*	data.c */
#include "header.h"
#include "monst.h"
#include "itm.h"

char *libdir = LIBDIR;

/* the game save filename   */
char savefilename[MAXPATHLEN];

/* the score file	    	*/
char scorefile[MAXPATHLEN];

/* the help text file	*/
char helpfile[MAXPATHLEN];

/* the maze data file	*/
char larnlevels[MAXPATHLEN];

/* the fortune data file */
char fortfile[MAXPATHLEN];

/* the options file filename */
char optsfile[MAXPATHLEN];

/* the checkpoint file filename */
char ckpfile[MAXPATHLEN];

/* the diagnostic filename	*/
char diagfile[] ="Diagfile";		

/* the wizard's password */
char *password ="main(){}";		


/*
 *	class[c[LEVEL]-1] gives the correct name of the player's experience level
 */
static char aa1[] =	" mighty evil master";
static char aa2[] =	"apprentice demi-god";
static char aa3[] =	"  minor demi-god   ";
static char aa4[] =	"  major demi-god   ";
static char aa5[] =	"    minor deity    ";
static char aa6[] =	"    major deity    "; 
static char aa7[] =	"  novice guardian  ";
static char aa8[] =	"apprentice guardian";
static char aa9[] =	"    The Creator    ";
char *class[]=
{
"  novice explorer  ", "apprentice explorer", " practiced explorer",/*  -3*/
"  expert explorer  ", " novice adventurer ", "     adventurer    ",/*  -6*/
"apprentice conjurer", "     conjurer      ", "  master conjurer  ",/*  -9*/
"  apprentice mage  ", "       mage        ", " experienced mage  ",/* -12*/
"    master mage    ", " apprentice warlord", "  novice warlord   ",/* -15*/
"  expert warlord   ", "   master warlord  ", " apprentice gorgon ",/* -18*/
"      gorgon       ", "  practiced gorgon ", "   master gorgon   ",/* -21*/
"    demi-gorgon    ", "    evil master    ", " great evil master ",/* -24*/
	aa1       ,   aa1       ,   aa1       ,/* -27*/
	aa1       ,   aa1       ,   aa1       ,/* -30*/
	aa1       ,   aa1       ,   aa1       ,/* -33*/
	aa1       ,   aa1       ,   aa1       ,/* -36*/
	aa1       ,   aa1       ,   aa1       ,/* -39*/
	aa2       ,   aa2       ,   aa2       ,/* -42*/
	aa2       ,   aa2       ,   aa2       ,/* -45*/
	aa2       ,   aa2       ,   aa2       ,/* -48*/
	aa3       ,   aa3       ,   aa3       ,/* -51*/
	aa3       ,   aa3       ,   aa3       ,/* -54*/
	aa3       ,   aa3       ,   aa3       ,/* -57*/
	aa4       ,   aa4       ,   aa4       ,/* -60*/
	aa4       ,   aa4       ,   aa4       ,/* -63*/
	aa4       ,   aa4       ,   aa4       ,/* -66*/
	aa5       ,   aa5       ,   aa5       ,/* -69*/
	aa5       ,   aa5       ,   aa5       ,/* -72*/
	aa5       ,   aa5       ,   aa5       ,/* -75*/
	aa6       ,   aa6       ,   aa6       ,/* -78*/
	aa6       ,   aa6       ,   aa6       ,/* -81*/
	aa6       ,   aa6       ,   aa6       ,/* -84*/
	aa7       ,   aa7       ,   aa7       ,/* -87*/
	aa8       ,   aa8       ,   aa8       ,/* -90*/
	aa8       ,   aa8       ,   aa8       ,/* -93*/
"  earth guardian   ", "   air guardian    ", "   fire guardian   ",/* -96*/
"  water guardian   ", "   time guardian   ", " ethereal guardian ",/* -99*/
	aa9       ,   aa9       ,   aa9       ,/* -102*/
};

/*
	table of experience needed to be a certain level of player
	skill[c[LEVEL]] is the experience required to attain the next level
 */
#define MEG 1000000
long skill[] = {
0, 10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120,                 /*  1-11 */
10240, 20480, 40960, 100000, 200000, 400000, 700000, 1*MEG,         /* 12-19 */
2*MEG,3*MEG,4*MEG,5*MEG,6*MEG,8*MEG,10*MEG,                         /* 20-26 */
12*MEG,14*MEG,16*MEG,18*MEG,20*MEG,22*MEG,24*MEG,26*MEG,28*MEG,     /* 27-35 */
30*MEG,32*MEG,34*MEG,36*MEG,38*MEG,40*MEG,42*MEG,44*MEG,46*MEG,     /* 36-44 */
48*MEG,50*MEG,52*MEG,54*MEG,56*MEG,58*MEG,60*MEG,62*MEG,64*MEG,     /* 45-53 */
66*MEG,68*MEG,70*MEG,72*MEG,74*MEG,76*MEG,78*MEG,80*MEG,82*MEG,     /* 54-62 */
84*MEG,86*MEG,88*MEG,90*MEG,92*MEG,94*MEG,96*MEG,98*MEG,100*MEG,    /* 63-71 */
105*MEG,110*MEG,115*MEG,120*MEG, 125*MEG, 130*MEG, 135*MEG, 140*MEG,/* 72-79 */
145*MEG,150*MEG,155*MEG,160*MEG, 165*MEG, 170*MEG, 175*MEG, 180*MEG,/* 80-87 */
185*MEG,190*MEG,195*MEG,200*MEG, 210*MEG, 220*MEG, 230*MEG, 240*MEG,/* 88-95 */
	260*MEG,280*MEG,300*MEG,320*MEG, 340*MEG, 370*MEG	    /* 96-101*/
};
#undef MEG

char *tempfilename;

char 	*lpbuf,
	*lpnt,
     	*inbuffer,
	*lpend;  	/* input/output pointers to the buffers */

char lastmonst[40];		/* name of the current monster*/
char loginname[20];		/* players login name */
char logname[LOGNAMESIZE];	/* players name storage for scoring	*/
char char_class[20];		/* character class */

short hitp[MAXX][MAXY];		/*	monster hp on level	*/
short iarg[MAXX][MAXY];		/*	arg for the item array	*/
short lastnum=0;		/* the number of the monster last hitting player, or */
						/* reason why dead */

char item[MAXX][MAXY];		/*	objects in maze if any	*/
char know[MAXX][MAXY];		/*	1 or 0 if here before	*/
/*char mitem[MAXX][MAXY];		/*	monster item array 	*/
char moved[MAXX][MAXY];		/*	monster movement flags  */
char stealth[MAXX][MAXY];	/*  0=sleeping 1=awake monst    */
char iven[IVENSIZE];		/*	inventory for player	*/
short ivenarg[IVENSIZE];	/*	inventory args for player	*/
char beenhere[NLEVELS]={0};	/*  1 if have been on this level */
char nosignal=0;	/* set to 1 to disable the signals from doing anything*/
char predostuff=0;	/*  
			** 2 means that the trap handling routines must do a
			** showplayer() after a trap.  0 means don't showplayer()
			**		0 - we are in create player screen
			**		1 - we are in welcome screen
			**		2 - we are in the normal game	
			*/

char ramboflag=0;
char do_fork=0;
char sex=1;		/* default is man, 0=woman	*/
char boldon=1;		/* 1=bold objects,  0=inverse objects	*/
char mail=1;		/* 1=mail letters after win game */
char ckpflag=1;		/* 1 if want checkpointing of game, 0 otherwise	*/
char cheat=0;		/* 1 if the player has fudged save file	*/
char level=0;		/* cavelevel player is on = c[CAVELEVEL]*/
char wizard=0;		/* the wizard mode flag	*/
char hitflag=0;		/* flag for if player has been hit when running */
char hit2flag=0;	/* flag for if player has been hit when running */
char hit3flag=0;	/* flag for if player has been hit flush input*/
char playerx,playery;	/* the room on the present level of the player*/
char lastpx,lastpy;	/* 0 --- MAXX-1  or  0 --- MAXY-1		*/
char oldx,oldy;
char lasthx=0,lasthy=0;/* location of monster last hit by player*/
char nobeep=0;		/* true if program is not to beep*/
char char_picked;	/* the character chosen */

char enable_scroll=0;	/* constant for enabled/disabled scrolling regn */


long initialtime=0;		/* time playing began 	*/
long gtime=0;			/* the clock for the game	*/
long outstanding_taxes=0;	/* present tax bill from score file	*/
long c[100],cbak[100];		/* the character description arrays*/

char *levelname[] =  {
" H"," 1"," 2"," 3"," 4"," 5",
" 6"," 7"," 8"," 9","10","11","12","13","14","15",
"V1","V2","V3","V4","V5"};

char monstnamelist[]=
" lGHJKOScjtAELNQRZabhiCTYdegmvzFWflorXV pqsyUkMwDDPxnDDuD123456790..............................................................";

char objnamelist[]=
" AToP%^F&^+M=%^$$f*OD#~][[)))(((||||||||{?!BC.o...<<<<EVV))([[]]](^ [H*** ^^ S tsTLcu...//))]:::::@.............................";

char *objectname[]=
{
"",
"a holy altar","a handsome, jewel-encrusted throne",
"an orb of enlightenment",
"a pit",
"a staircase leading upwards",
"an elevator going up",
"a bubbling fountain",
"a great marble statue",
"a teleport trap",
"the College of Larn",
"a mirror",
"the DND store",
"a staircase going down",
"an elevator going down",
"the Bank of Larn",
"the 8th branch of the Bank of Larn",
"a dead fountain",
"gold",
"an open door",
"a closed door",
"a wall",
"The Eye of Larn",
"plate mail",
"chain mail",
"leather armor",
"a sword of slashing",
"Bessman's flailing hammer",
"a sunsword",
"a two-handed sword",
"a spear",
"a dagger",
"a ring of extra regeneration",
"a ring of regeneration",
"a ring of protection",
"an energy ring",
"a ring of dexterity",
"a ring of strength",
"a ring of cleverness",
"a ring of increase damage",
"a belt of striking",
"a magic scroll",
"a magic potion",
"a book",
"a chest",
"an amulet of invisibility",
"an orb of dragon slaying",
"a scarab of negate spirit",
"a cube of undead control",
"a device of theft-prevention",
"a brilliant diamond",
"a ruby",
"an enchanting emerald",
"a sparkling sapphire",
"the dungeon entrance",
"a volcanic shaft leaning downward",
"the base of a volcanic shaft",
"a battle axe",
"a longsword",
"a flail",
"ring mail",
"studded leather armor",
"splint mail",
"plate armor",
"stainless plate armor",
"a lance of death",
"an arrow trap",
"an arrow trap",
"a shield",
"your home",
"gold",
"gold",
"gold",
"a dart trap",
"a dart trap",
"a trapdoor",
"a trapdoor",
"the local trading post",
"a teleport trap", 
"a massive throne",
"a sphere of annihilation",
"a handsome, jewel-encrusted throne",
"the Larn Revenue Service",
"a fortune cookie",
"a golden urn",
"a brass lamp",
"The Hand of Fear",
"The Talisman of the Sphere",
"a wand of wonder",
"a staff of power",
"the Vorpal Blade",
"Slayer",
"elven chain",
"some speed",
"some LSD",
"some hashish",
"some magic mushrooms",
"some cocaine",
"Dealer McDope's Pad",
"","","","","","","","","","",""
};


/*
 *	for the monster data
 *
 *	array to do rnd() to create monsters <= a given level
 */
char monstlevel[] = {5, 11, 17, 22, 27, 33, 39, 42, 46, 50, 53, 56};

struct monst monster[] = {

/*NAME                   LV   AC    DAM ATT  INT GOLD     HP     EXP   FLAGS
  ---------------------------------------------------------------------------------------------- */
{ "",                     0,    0,   0,  0,   1,    0,     0,       0, 0},
{ "lemming",              1,    0,   0,  0,   3,    0,     0,       1, FL_HEAD},
{ "gnome",                1,   10,   1,  0,   8,   30,     2,       2, FL_HEAD},
{ "hobgoblin",            1,   13,   2,  0,   5,   25,     3,       2, FL_HEAD},
{ "jackal",               1,    7,   1,  0,   4,    0,     1,       1, FL_HEAD},
{ "kobold",               1,   15,   1,  0,   7,   10,     1,       1, FL_HEAD},
{ "orc",                  2,   15,   3,  0,   9,   40,     5,       2, FL_HEAD},
{ "snake",                2,   10,   1,  0,   3,    0,     3,       1, FL_HEAD},
{ "giant centipede",      2,   13,   1,  4,   3,    0,     2,       2, FL_HEAD},
{ "jaculi",               2,    9,   1,  0,   3,    0,     2,       1, FL_HEAD},
{ "troglodyte",           2,   10,   2,  0,   5,   80,     5,       3, FL_HEAD},
{ "giant ant",            2,    8,   1,  4,   4,    0,     5,       4, FL_HEAD},
{ "floating eye",         3,    8,   2,  0,   3,    0,     7,       2, 0},
{ "leprechaun",           3,    3,   0,  8,   3, 1500,    15,      40, FL_HEAD},
{ "nymph",                3,    3,   0, 14,   9,    0,    20,      40, FL_HEAD},
{ "quasit",               3,    5,   3,  0,   3,    0,    14,      10, FL_HEAD},
{ "rust monster",         3,    5,   0,  1,   3,    0,    18,      20, FL_HEAD},
{ "zombie",               3,   12,   3,  0,   3,    0,     9,       7, FL_HEAD},
{ "assassin bug",         4,    4,   3,  0,   3,    0,    23,      13, FL_HEAD},
{ "bitbug",               4,    5,   4, 15,   5,   40,    24,      33, FL_HEAD},
{ "hell hound",           4,    5,   2,  2,   6,    0,    20,      33, FL_HEAD},
{ "ice lizard",           4,   11,   3, 10,   6,   50,    19,      23, FL_HEAD},
{ "centaur",              4,    6,   4,  0,  10,   40,    25,      43, FL_HEAD},
{ "troll",                5,    9,   5,  0,   9,   80,    55,     250, FL_HEAD},
{ "yeti",                 5,    8,   4,  0,   5,   50,    45,      90, FL_HEAD},
{ "white dragon",         5,    4,   5,  5,  16,  500,    65,    1000, FL_HEAD},
{ "elf",                  5,    3,   3,  0,  15,   50,    25,      33, FL_HEAD},
{ "gelatinous cube",      5,    9,   3,  0,   3,    0,    24,      43, 0},
{ "metamorph",            6,    9,   3,  0,   3,    0,    32,      40, 0},
{ "vortex",               6,    5,   4,  0,   3,    0,    33,      53, 0},
{ "ziller",               6,   15,   3,  0,   3,    0,    34,      33, FL_HEAD},
{ "violet fungus",        6,   12,   3,  0,   3,    0,    39,      90, 0},
{ "wraith",               6,    3,   1,  6,   3,    0,    36,     300, FL_HEAD},
{ "forvalaka",            6,    3,   5,  0,   7,    0,    55,     270, FL_HEAD},
{ "lama nobe",            7,   14,   7,  0,   6,    0,    36,      70, FL_HEAD},
{ "osequip",              7,    4,   7, 16,   4,    0,    36,      90, FL_HEAD},
{ "rothe",                7,   15,   5,  0,   3,  100,    53,     230, FL_HEAD},
{ "xorn",                 7,    6,   7,  0,  13,    0,    63,     290, 0},
{ "vampire",              7,    5,   4,  6,  17,    0,    55,     950, FL_HEAD},
{ "invisible stalker",    7,    5,   6,  0,   5,    0,    55,     330, FL_HEAD},
{ "poltergeist",          8,    1,   8,  0,   3,    0,    55,     430, 0},
{ "disenchantress",       8,    3,   1,  9,   3,    0,    57,     500, FL_HEAD},
{ "shambling mound",      8,   13,   5,  0,   6,    0,    47,     390, 0},
{ "yellow mold",          8,   12,   4,  0,   3,    0,    37,     240, 0},
{ "umber hulk",           8,    6,   7, 11,  14,    0,    67,     600, FL_HEAD},
{ "gnome king",           9,   -1,  10,  0,  18, 2000,   120,    3000, FL_HEAD},
{ "mimic",                9,    9,   7,  0,   8,    0,    57,     100, 0},
{ "water lord",           9,  -10,  15,  7,  20,    0,   155,   15000, FL_HEAD|FL_NOBEHEAD},
{ "bronze dragon",        9,    5,   9,  3,  16,  300,    90,    4000, FL_HEAD},
{ "green dragon",         9,    4,   4, 10,  15,  200,    80,    2500, FL_HEAD},
{ "purple worm",          9,   -1,  13,  0,   3,  100,   130,   15000, FL_HEAD},
{ "xvart",                9,   -2,  14,  0,  13,    0,   100,    1000, FL_HEAD},
{ "spirit naga",         10,  -20,  15, 12,  23,    0,   100,   20000, FL_HEAD|FL_NOBEHEAD},
{ "silver dragon",       10,   -4,  10,  3,  20,  700,   110,   10000, FL_HEAD},
{ "platinum dragon",     10,   -7,  15, 13,  22, 1000,   150,   25000, FL_HEAD},
{ "green urchin",        10,   -5,  12,  0,   3,    0,    95,    5000, 0},
{ "red dragon",          10,   -4,  13,  3,  19,  800,   120,   14000, FL_HEAD},
{ "type I demon lord",   12,  -40,  20,  3,  20,    0,   150,   50000, FL_HEAD|FL_NOBEHEAD},
{ "type II demon lord",  13,  -45,  25,  5,  22,    0,   200,   75000, FL_HEAD|FL_NOBEHEAD},
{ "type III demon lord", 14,  -50,  30,  9,  24,    0,   250,  100000, FL_HEAD|FL_NOBEHEAD},
{ "type IV demon lord",  15,  -55,  35, 11,  26,    0,   300,  125000, FL_HEAD|FL_NOBEHEAD},
{ "type V demon lord",   16,  -60,  40, 13,  28,    0,   350,  150000, FL_HEAD|FL_NOBEHEAD},
{ "type VI demon lord",  17,  -65,  45, 13,  30,    0,   400,  175000, FL_HEAD|FL_NOBEHEAD},
{ "type VII demon lord", 18,  -70,  50,  6,  32,    0,   450,  200000, FL_HEAD|FL_NOBEHEAD},
{ "demon prince",        19, -100,  80,  6,  40,    0,  1000,  500000, FL_HEAD|FL_NOBEHEAD},
{ "God of Hellfire",     20, -127, 127,  6, 100,    0, 32767, 1000000, FL_HEAD|FL_NOBEHEAD}
};

/*	name array for scrolls		*/
char *scrollname[MAXSCROLL] = {
	" enchant armor",
	" enchant weapon",
	" enlightenment",
	" blank paper",
	" create monster",
	" create artifact",
	" aggravate monsters",
	" time warp",
	" teleportation",
	" expanded awareness",
	" haste monsters",
	" monster healing",
	" spirit protection",
	" undead protection",
	" stealth",
	" magic mapping",
	" hold monsters",
	" gem perfection",
	" spell extension",
	" identify",
	" remove curse",
	" annihilation",
	" pulverization",
	" life protection",
	"  ",
	"  ",
	"  ",
	"  "
};

int scrollknown[MAXSCROLL] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/*	name array for magic potions	*/
char *potionname[MAXPOTION] = {
	" sleep",
	" healing",
	" raise level",
	" increase ability",
	" wisdom",
	" strength",
	" raise charisma",
	" dizziness",
	" learning",
	" gold detection",
	" monster detection",
	" forgetfulness",
	" water",
	" blindness",
	" confusion",
	" heroism",
	" sturdiness",
	" giant strength",
	" fire resistance",
	" treasure finding",
	" instant healing",
	" cure dianthroritis",
	" poison",
	" see invisible",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  "
};

int potionknown[MAXPOTION] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0
};


/*
**	i = rund((tmp=splev[lev])?tmp:1); 
**	i = rnd((tmp=splev[lev]-9) ?tmp:1 ) + 9;
 */
char spelknow[SPNUM]={0};
char splev[] = {
	1,4,7,11,15,
	20,24,28,30,32,
	33,34,35,36,37,
	38,38,38,38,38,
	38};

char *spelcode[]={
	"pro",	"mle",	"dex",	"sle",	"chm",	"ssp", /* 0 - 5 */
	"web",	"str",	"enl",	"hel",	"cbl",	"cre",	"pha",	"inv", /*6-13 */
	"bal",	"cld",	"ply",	"can",	"has",	"ckl",	"vpr",  /* 14-20 */
	"dry",	"lit",	"drl",	"glo",	"flo",	"fgr", /* 21 - 26 */
	"sca",	"hld",	"stp",	"tel",	"mfi",  "mkw", /* 27 - 34 */
	"sph",	"gen",	"sum",	"wtw",	"alt",	"per"  /* 35 - 38 */
};

char *spelname[]={
	"protection",	/* 0 */
	"magic missile",
	"dexterity",
	"sleep",
	"charm monster",
	"sonic spear",	/* 5 */
	"web",
	"strength",
	"enlightenment",
	"healing",
	"cure blindness",	/* 10 */
	"create monster",
	"phantasmal forces",
	"invisibility",
	"fireball",
	"cold",			/* 15 */
	"polymorph",
	"cancellation",	
	"haste self",
	"cloud kill",
	"vaporize rock",	/* 20 */
	"dehydration",
	"lightning",
	"drain life",
	"invulnerability",
	"flood",		/* 25 */
	"finger of death",
	"scare monster",
	"hold monster",	
	"time stop",
	"teleport away",	/* 30 */
	"magic fire",
	"make a wall",
	"sphere of annihilation",
	"genocide",		/* 34 */
	"summon demon",
	"walk through walls",
	"alter reality",
	"permanence",		/* 38 */
	""			/* 39 */
};

char *speldescript[]={
	/* 1 */
	"Generates a +2 protection field",
	"Creates and hurls a magic missile equivalent to a +1 magic arrow",
	"Adds +2 to the caster's dexterity",
	"Causes some monsters to go to sleep",
	"Some monsters may be awed at your magnificence",
	"Causes the caster's hands to emit a screeching sound",
	/* 7 */
	"Causes strands of sticky thread to entangle an enemy",
	"Adds +2 to the caster's strength for a time",
	"The caster becomes more aware of things around them",
	"Restores some of the caster's health",
	"Restores sight to one so unfortunate as to be blinded",
	"Creates a monster near to the caster",
	"Creates illusions which, if believed, cause monsters to die",
	"The caster becomes invisible",
	/* 15 */
	"Creates a ball of fire that burns whatever it hits",
	"Sends forth a cone of cold which freezes whatever it touches",
	"You can find out what this does for yourself",
	"Stops a monster from using its special abilities",
	"Speeds up the caster's movements",
	"Creates a fog of poisonous gas which kills all that is within it",
	"Changes rock to air",
	/* 22 */
	"Dries up water in the immediate vicinity",
	"Causes the caster's finger to emit lightning bolts",
	"Subtracts hit points from both you and a monster",
	"This globe helps to protect the player from physical attack",
	"Creates a deluge of water, flooding the immediate chamber",
	"A holy spell calling on your god to back you up",
	/* 28 */
	"Terrifies the monster so that it may not hit the caster",
	"Freezes monsters in their tracks",
	"All movement in the caverns ceases for a limited duration",
	"Teleports a monster",
	"Creates a curtain of fire around the caster",
	/* 33 */
	"Makes a wall in the specified place",
	"Anything caught in this sphere is instantly killed.",
	"Eliminates a species of monster from the game",
	"Summons a demon who may help you out",
	"Allows the player to walk through walls for a short period of time",
	"God only knows what this will do",
	"Makes a character spell permanent, e.g. protection, strength, etc.",
	""
};

/*
**	spelweird[ monster ] [ spell ] = [ reaction ]
**
**	spell =  index into spelldescript[] and spellname[]
**	reaction = index into spelmes[]
*/
char spelweird[MAXMONST+8][SPNUM] = {
/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/* lemming */
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,  0,0,0,0,0,0 	} ,
/*gnome */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*      hobgoblin */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         jackal */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         kobold */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
 /*            orc */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*          snake */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*giant centipede */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         jaculi */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     troglodyte */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*      giant ant */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*   floating eye */ 
{  0,0,0,8,0,10,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     leprechaun */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*          nymph */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         quasit */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*   rust monster */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         zombie */ 
{  0,0,0,8,0,4,   0,0,0,0,0,0,4,0,   0,0,0,0,0,4,0,   4,0,4,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*   assassin bug */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*        bugbear */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     hell hound */ 
{  0,6,0,0,0,0,   12,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*     ice lizard */ 
{  0,0,0,0,0,0,   11,0,0,0,0,0,0,0,  0,15,0,0,0,0,0,  0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*        centaur */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*          troll */ 
{  0,7,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   4,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*           yeti */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,15,0,0,0,0,0,  0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*   white dragon */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,14,0,  0,15,0,0,0,0,0,  4,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*       elf */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,14,5,  0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*gelatinous cube */ 
{  0,13,0,8,0,10, 2,0,0,0,0,0,0,0,   0,0,0,0,0,4,0,   0,0,0,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*      metamorph */ 
{  0,13,0,0,0,0,  2,0,0,0,0,0,0,0,   0,0,0,0,0,4,0,   4,0,0,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         vortex */ 
{  0,13,0,0,0,10, 1,0,0,0,0,0,0,0,   0,0,0,0,0,4,0,   4,0,0,0,4,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         ziller */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i     f c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*   violet fungi */ 
{  0,0,0,8,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         wraith */
{  0,13,0,8,0,4,   0,0,0,0,0,0,14,0,   0,0,0,0,0,4,0,   4,0,4,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*      forvalaka */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*      lama nobe */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*        osequip */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*          rothe */ 
{  0,7,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*           xorn */
{  0,7,0,8,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   4,0,0,0,4,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*        vampire */
{  0,0,0,0,0,4,   2,0,0,0,0,0,14,0,   0,0,0,0,0,4,0,   0,0,4,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*invisible staker*/
{  0,0,0,0,0,0,   1,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*    poltergeist */ 
{  0,13,0,8,0,4,  1,0,0,0,0,0,0,0,   0,4,0,0,0,4,0,   4,0,4,0,4,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/* disenchantress */
{  0,0,0,8,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*shambling mound */ 
{  0,0,0,0,0,10,  0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*    yellow mold */ 
{  0,0,0,8,0,10,  1,0,0,0,0,0,0,0,   0,0,0,0,0,4,0,   0,0,0,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     umber hulk */ 
{  0,7,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     gnome king */ 
{  0,7,0,0,3,0,   0,0,0,0,0,0,0,5,   0,0,9,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*          mimic */
{  0,0,0,0,0,0,   2,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     water lord */ 
{  0,13,0,8,3,4,  1,0,0,0,0,0,0,0,   0,0,9,0,0,4,0,   0,0,0,0,16,4,  0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*  bronze dragon */ 
{  0,7,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*   green dragon */ 
{  0,7,0,0,0,0,   11,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*    purple worm */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*          xvart */ 
{  0,13,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,4,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*    spirit naga */
{  0,13,0,8,3,4,  1,0,0,0,0,0,14,5,   0,4,9,0,0,4,0,   4,0,4,0,4,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*  silver dragon */
{  0,6,0,9,0,0,   12,0,0,0,0,0,0,0,  0,0,9,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*platinum dragon */ 
{  0,7,0,9,0,0,   11,0,0,0,0,0,14,0, 0,0,9,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*   green urchin */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     red dragon */ 
{  0,6,0,0,0,0,   12,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,9,0,0,4,0,   4,0,9,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,9,0,0,4,0,   4,0,9,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,9,0,0,4,0,   4,0,9,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,9,0,0,4,0,   4,0,9,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,9,0,0,4,0,   4,0,9,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,4,0,0,4,0,   4,0,0,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,4,0,0,4,0,   4,0,0,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*demon prince */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,4,0,0,4,0,   4,0,4,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/* God of Hellfire */ 
{  0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  18,0,9,0,0,4,0,   4,18,4,0,0,4,   4,4,0,9,4,   9,0,17,0,0,0 	}
};

char *spelmes[] = { 
	/*  0 */	"",		/* spell has no effect on the monster */
	/*  1 */	"the web had no effect on the %s",
	/*  2 */	"the %s changed shape to avoid the web",
	/*  3 */	"the %s isn't afraid of you",
	/*  4 */	"the %s isn't affected",
	/*  5 */	"the %s can see you with his infravision",
	/*  6 */	"the %s vaporizes your missile",
	/*  7 */	"your missile bounces off the %s",
	/*  8 */	"the %s doesn't sleep",
	/*  9 */	"the %s resists",
	/* 10 */	"the %s can't hear the noise",
	/* 11 */	"the %s's tail cuts it free of the web",
	/* 12 */	"the %s burns through the web",
	/* 13 */	"your missiles pass right through the %s",
	/* 14 */	"the %s sees through your illusions",
	/* 15 */	"the %s loves the cold!",
	/* 16 */	"the %s loves the water!",
	/* 17 */	"the demon is terrified of the %s!",
	/* 18 */	"the %s loves fire and lightning!"
};

/*
 *	function to create scroll numbers with appropriate probability of 
 *	occurrence
 */
char scprob[]= {
    SENCHANTARM, SENCHANTARM, SENCHANTARM, SENCHANTARM,
    SENCHANTWEAP, SENCHANTWEAP, SENCHANTWEAP, SENCHANTWEAP, SENCHANTWEAP,
    SENLIGHTEN, SENLIGHTEN, SENLIGHTEN, SENLIGHTEN, SENLIGHTEN, SENLIGHTEN,
    SBLANK, SBLANK, SBLANK, SBLANK, SBLANK,
    SCREATEMONST, SCREATEMONST, SCREATEMONST,
    SCREATEITEM, SCREATEITEM, SCREATEITEM, SCREATEITEM, SCREATEITEM,
    SAGGMONST, SAGGMONST, SAGGMONST, SAGGMONST, SAGGMONST,
    STIMEWARP, STIMEWARP, STIMEWARP, STIMEWARP,
    STELEPORT, STELEPORT, STELEPORT,
    SAWARENESS, SAWARENESS, SAWARENESS, SAWARENESS,
    SHASTEMONST, SHASTEMONST, SHASTEMONST, SHASTEMONST,
    SMONSTHEAL, SMONSTHEAL, SMONSTHEAL,
    SSPIRITPROT, SSPIRITPROT, SSPIRITPROT,
    SUNDEADPROT, SUNDEADPROT, SUNDEADPROT, SUNDEADPROT,
    SSTEALTH, SSTEALTH,
    SMAGICMAP, SMAGICMAP,
    SHOLDMONST, SHOLDMONST, SHOLDMONST,
    SGEMPERFECT, SGEMPERFECT,
    SSPELLEXT, SSPELLEXT,
    SIDENTIFY, SIDENTIFY, SIDENTIFY,
    SREMCURSE, SREMCURSE, SREMCURSE, SREMCURSE,
    SANNIHILATE,
    SPULVERIZE, SPULVERIZE, SPULVERIZE,
    SLIFEPROT /* 81 total */
};


/*
 *	function to return a potion number created with appropriate probability
 *	of occurrence
 */
char potprob[] = { 
	PSLEEP, PSLEEP,
	PHEALING, PHEALING, PHEALING,
	PRAISELEVEL,
	PINCABILITY, PINCABILITY, 
	PWISDOM, PWISDOM,
	PSTRENGTH, PSTRENGTH, 
	PCHARISMA, PCHARISMA,
	PDIZZINESS, PDIZZINESS,
	PLEARNING,
	PGOLDDET, PGOLDDET, PGOLDDET,
	PMONSTDET, PMONSTDET, PMONSTDET,
	PFORGETFUL, PFORGETFUL,
	PWATER, PWATER,
	PBLINDNESS,
	PCONFUSION,
	PHEROISM,
	PSTURDINESS,
	PGIANTSTR,
	PFIRERESIST,
	PTREASURE, PTREASURE,
	PINSTHEAL, PINSTHEAL,
	/* No Cure Dianthroritis */
	PPOISON, PPOISON,
	PSEEINVIS, PSEEINVIS
};	/* 41 total */

char nlpts[] = { 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7 };
char nch[] = { 0, 0, 0, 1, 1, 1, 2, 2, 3, 4 };
char nplt[] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 4 };
char ndgg[] = { 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 4, 5 };
char nsw[] = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 3 };

