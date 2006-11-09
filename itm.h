/* itm.h */
struct _itm {
	long	price;
	int	*mem;
	char	obj;
	char	arg;
	char	qty;
};

extern struct _itm itm[];

	/************* Defines for the objects in the game ************/

#define OALTAR 1
#define OTHRONE 2
#define OORB 3		/* orb of enlightement - gives expanded awareness
			  	as long as held */
#define OPIT 4
#define OSTAIRSUP 5
#define OELEVATORUP 6
#define OFOUNTAIN 7
#define OSTATUE 8
#define OTELEPORTER 9
#define OSCHOOL 10
#define OMIRROR 11
#define ODNDSTORE 12
#define OSTAIRSDOWN 13
#define OELEVATORDOWN 14
#define OBANK2 15
#define OBANK 16
#define ODEADFOUNTAIN 17
#define OMAXGOLD 70
#define OGOLDPILE 18
#define OOPENDOOR 19
#define OCLOSEDDOOR 20
#define OWALL 21
#define OTRAPARROW 66
#define OTRAPARROWIV 67

#define OLARNEYE 22	/* The Eye of Larn - enables you to see demons */

/* armor */
#define OPLATE 23
#define OCHAIN 24
#define OLEATHER 25
#define ORING 60
#define OSTUDLEATHER 61
#define OSPLINT 62
#define OPLATEARMOR 63
#define OSSPLATE 64
#define OSHIELD 68
#define OELVENCHAIN 92		/* elven chain - strong and light, 
				   impervious to rust */


/* weapons */
#define OSWORDofSLASHING 26	/* impervious to rust, light, strong */
#define OHAMMER 27		/* Bessman's Flailing Hammer */
#define OSWORD 28
#define O2SWORD 29		/* 2 handed sword */
#define OSPEAR 30
#define ODAGGER 31
#define OBATTLEAXE 57
#define OLONGSWORD 58
#define OFLAIL 59
#define OLANCE 65
#define OVORPAL 90	/* 1/20 chance of beheading most monsters */
#define OSLAYER 91	/* magical sword - increases intelligence by 10,
			   halves damage caused by demons, demon prince 
			   and lucifer - strong as lance of death against them*/



/* rings */
#define ORINGOFEXTRA 32
#define OREGENRING 33
#define OPROTRING 34
#define OENERGYRING 35
#define ODEXRING 36
#define OSTRRING 37
#define OCLEVERRING 38
#define ODAMRING 39

#define OBELT 40

#define OSCROLL 41
#define OPOTION 42
#define OBOOK 43
#define OCHEST 44

#define OAMULET 45
#define OORBOFDRAGON 46
#define OSPIRITSCARAB 47
#define OCUBEofUNDEAD 48
#define ONOTHEFT 49

/* gems */
#define ODIAMOND 50
#define ORUBY 51
#define OEMERALD 52
#define OSAPPHIRE 53

#define OENTRANCE 54
#define OVOLDOWN 55
#define OVOLUP 56
#define OHOME 69

#define OKGOLD 71
#define ODGOLD 72
#define OIVDARTRAP 73
#define ODARTRAP 74
#define OTRAPDOOR 75
#define OIVTRAPDOOR 76
#define OTRADEPOST 77
#define OIVTELETRAP 78
#define ODEADTHRONE 79
#define OANNIHILATION 80		/* sphere of annihilation */
#define OTHRONE2 81			/* dead throne */
#define OLRS 82				/* Larn Revenue Service */
#define OCOOKIE 83
#define OURN 84			/* golden urn - not implemented */
#define OBRASSLAMP 85	/* brass lamp - genie pops up and offers spell */
#define OHANDofFEAR 86		/* hand of fear - scare monster spell lasts
				   twice as long if have this */
#define OSPHTALISMAN 87		/* talisman of the sphere */
#define OWWAND 88	/* wand of wonder - cant fall in trap doors/pits */
#define OPSTAFF 89		/* staff of power - cancels drain life attack*/

#define OSPEED 93		
#define OACID 94
#define OHASH 95
#define OSHROOMS 96
#define OCOKE 97
#define OPAD 98		/* Dealer McDope's Pad */
/* used up to 98 */


/*** Scrolls ***/
#define SENCHANTARM   0
#define SENCHANTWEAP  1
#define SENLIGHTEN    2
#define SBLANK        3
#define SCREATEMONST  4
#define SCREATEITEM   5
#define SAGGMONST     6
#define STIMEWARP     7
#define STELEPORT     8
#define SAWARENESS    9
#define SHASTEMONST  10
#define SMONSTHEAL   11
#define SSPIRITPROT  12
#define SUNDEADPROT  13
#define SSTEALTH     14
#define SMAGICMAP    15
#define SHOLDMONST   16
#define SGEMPERFECT  17
#define SSPELLEXT    18
#define SIDENTIFY    19
#define SREMCURSE    20
#define SANNIHILATE  21
#define SPULVERIZE   22
#define SLIFEPROT    23
#define S_MAX        23  /* Greatest defined scroll number */


/*** Potions ***/
#define PSLEEP        0
#define PHEALING      1
#define PRAISELEVEL   2
#define PINCABILITY   3
#define PWISDOM       4
#define PSTRENGTH     5
#define PCHARISMA     6
#define PDIZZINESS    7
#define PLEARNING     8
#define PGOLDDET      9
#define PMONSTDET    10
#define PFORGETFUL   11
#define PWATER       12
#define PBLINDNESS   13
#define PCONFUSION   14
#define PHEROISM     15
#define PSTURDINESS  16
#define PGIANTSTR    17
#define PFIRERESIST  18
#define PTREASURE    19
#define PINSTHEAL    20
#define PCUREDIANTH  21
#define PPOISON      22
#define PSEEINVIS    23
#define P_MAX        23  /* Greatest defined potion number */


/*** How enchantment happened ***/
#define ENCH_SCROLL  0  /* Enchantment from reading a scroll */
#define ENCH_ALTAR   1  /* Enchantment from an altar         */
