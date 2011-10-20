/* player.h */

	/*************** Defines for the character attibute array c[] ******/

#define STRENGTH 0	/* characters physical strength not due to objects */
#define INTELLIGENCE 1
#define WISDOM 2
#define CONSTITUTION 3
#define DEXTERITY 4
#define CHARISMA 5
#define HPMAX 6
#define HP 7
#define GOLD 8
#define EXPERIENCE 9
#define LEVEL 10
#define REGEN 11
#define WCLASS 12
#define AC 13
#define BANKACCOUNT 14
#define SPELLMAX 15
#define SPELLS 16
#define ENERGY 17
#define ECOUNTER 18
#define MOREDEFENSES 19
#define WEAR 20
#define PROTECTIONTIME 21
#define WIELD 22
#define AMULET 23		/* if have amulet of invisibility */
#define REGENCOUNTER 24
#define MOREDAM 25
#define DEXCOUNT 26
#define STRCOUNT 27
#define BLINDCOUNT 28		/* if blind */
#define CAVELEVEL 29
#define CONFUSE 30		/* if confused */
#define ALTPRO 31
#define HERO 32			/* if hero  */
#define CHARMCOUNT 33
#define INVISIBILITY 34		/* if invisible */
#define CANCELLATION 35		/* if cancel cast */
#define HASTESELF 36		/* if hasted */
#define EYEOFLARN 37		/* if have eye */
#define AGGRAVATE 38
#define GLOBE 39
#define TELEFLAG 40		/* if been teleported */
#define SLAYING 41		/* if have orb of dragon slaying */
#define NEGATESPIRIT 42		/* if negate spirit */
#define SCAREMONST 43		/* if scare cast */
#define AWARENESS 44		/* if awareness cast */
#define HOLDMONST 45
#define TIMESTOP 46
#define HASTEMONST 47
#define CUBEofUNDEAD 48		/* if have cube */
#define GIANTSTR 49		/* if giant strength */
#define FIRERESISTANCE 50
#define BESSMANN 51		/* flag for hammer */
#define NOTHEFT 52
#define HARDGAME 53
#define BYTESIN 54		/* used for checkpointing in tok.c */

		/* 55 to 58 are open */

#define VORPAL 59               /* non-zero if Vorpal Blade exists */
#define LANCEDEATH 60		/* if have LoD */
#define SPIRITPRO 61
#define UNDEADPRO 62
#define SHIELD 63
#define STEALTH 64
#define ITCHING 65
#define LAUGHING 66		/* not implemented */
#define DRAINSTRENGTH 67
#define CLUMSINESS 68
#define INFEEBLEMENT 69
#define HALFDAM 70
#define SEEINVISIBLE 71
#define FILLROOM 72
	/* 73 is open */
#define SPHCAST 74	/* nz if an active sphere of annihilation */
#define WTW 75		/* walk through walls */
#define STREXTRA 76	/* character strength due to objects or enchantments */
#define TMP 77		/* misc scratch space */
#define LIFEPROT 78 	/* life protection counter */

			/* FLAGS : non-zero if object has been made */
#define ORB 79		/* orb - 1 if created, 2 if held */
#define ELVUP 80	/* elevator up */
#define ELVDOWN 81	/* elevator down */
#define HAND 82		/* Hand of Fear */
#define CUBEUNDEAD 83	/* cube of undead control */
#define DRAGSLAY 84	/* orb of dragon slaying */
#define NEGATE 85	/* scarab of negate spirit */
#define URN 86		/* golden urn */
#define LAMP 87		/* brass lamp */
#define TALISMAN 88	/* Talisman of the Sphere */
#define WAND 89		/* wand of wonder */
#define STAFF 90	/* staff of power */
#define DEVICE 91	/* anti-theft */
#define SLASH 92 	/* sword of slashing */
#define ELVEN 93	/* elven chain */
#define VORP  94	/* Vorpal */
#define SLAY  95	/* Slayer */
#define PAD   96	/* Dealer McDopes */

#define COKED 97	/* timer for being coked out */
#define TMP2  98	/* another tmp location */
/* 99 is open */
/* used up to 98 of 0-99 */
