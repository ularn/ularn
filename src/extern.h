#ifdef __STDC__

/* module: action.c */

void run(int);
void wield();
void ydhi(int);
void ycwi(int);
void wear();
void dropobj();
void readscr();
void eatcookie();
void quaff();
int qwhatitem();
int whatitem(char *);
long readnum(long);
void do_create();

/* module: bill.c */

int letter1();
int letter2();
int letter3();
int letter4();
int letter5();
int letter6();
int mailbill();

/* module: config.c */


/* module: create.c */

int makeplayer();
void newcavelevel(int);
void makemaze(int);
void eat(int, int);
int cannedlevel(int);
void treasureroom(int);
void troom(int, int, int, int, int, int);
void makeobject(int);
void fillmroom(int, int, int);
void froom(int, int, int);
void fillroom(int, int);
int fillmonst(int);
void sethp(int);
void checkgen();

/* module: data.c */


/* module: diag.c */

void diag();
void diagdrawscreen();

/* module: display.c */

void bottomdo();
void bottomgold();
void bot_spellx();
void bot_linex();
void bot_hpx();
void botside();
void bothp();
void draws(int, int, int, int);
void drawscreen();
void showcell(int, int);
void show1cell(int, int);
void showplayer();
int moveplayer(int);
void seemagic(int);
void seepage();

/* module: fortune.c */

char *fortune(char *);

/* module: help.c */

void help();
void welcome();
void retcont();
int openhelp();

/* module: io.c */

int getcharacter();
int newgame();
/*VARARGS*/
void lprintf(char *, ...);
int lwrite(char *, int);
long lgetc1();
int lrfill(char *, int);
char *lgetw();
char *lgetl();
int lcreat(char *);
int lopen(char *);
int lappend(char *);
int lrclose();
int lwclose();
int lprcat(char *);
int init_term();
int cl_up(int, int);
int cl_dn(int, int);
void ularn_standout(char *);
int set_score_output();
void lflush();
int putcharacter(int);
void flush_buf();
void tmcapcnv(char *, char *);
char *getword(char *);

/* module: main.c */

int main(int, char **);
void parse();
int parse2();

/* module: monster.c */

void createmonster(int);
int cgood(int, int, int, int);
void createitem(int, int, int, int);
void cast();
void speldamage(int);
void loseint();
int isconfuse();
int nospell(int, int);
int fullhit(int);
void direct(int, int, char *, int);
void godirect(int, int, char *, int, int);
int ifblind(int, int);
void tdirect(int);
void makewall(int);
void omnidirect(int, int, char *);
int dirsub(int *, int *);
int verifyxy(int *, int *);
void dirpoly(int);
void hitmonster(int, int);
int hitm(int, int, int);
void hitplayer(int, int);
void dropsomething(int, int, int);
int dropgold(int);
void something(int, int, int);
int newobject(int, int *);
int spattack(int, int, int);
int checkloss(int);
int annihilate();
void genmonst();
int makemonst(int);
void randmonst();

/* module: moreobj.c */

void oaltar();
void ohear();
void othrone(int);
void odeadthrone();
void ochest();
void ofountain();
void fntchange(int);
void fch(int, long *);

/* module: movem.c */

void movemonst();
void movemt(int, int);
void mmove(int, int, int, int);
void teleportmonst(int, int, int);

/* module: nap.c */

void nap(int);
void ularn_napms(int);

/* module: object.c */

void lookforobject();
void finditem(int);
void ostairs(int);
void oteleport(int);
void opotion(int);
void quaffpotion(int);
void oscroll(int);
void adjusttime(long);
void read_scroll(int);
void removecurse();
void opit();
void obottomless();
void oelevator(int);
void ostatue();
void omirror();
void obook();
void readbook(int);
void ocookie();
void ogold(int);
void ohome();
void iopts();
void ignore();
void closedoor();

/* module: player.c */

int raiselevel();
int loselevel();
int raiseexperience(long);
int loseexperience(long);
int losehp(int);
int losemhp(int);
int raisehp(int);
int raisemhp(int);
int raisespells(int);
int raisemspells(int);
int losespells(int);
int losemspells(int);
int positionplayer();
int recalc();
void quit();
void more();
int take(int, int);
int drop_object(int);
void enchantarmor(int);
void enchweapon(int);
int pocketfull();
int nearbymonst();
int stealsomething(int, int);
int emptyhanded();
void creategem();
void adjustcvalues(int, int);
void gettokstr(char *);
int getpassword();
int getyn();
int packweight();

/* module: regen.c */

void regen();

/* module: savelev.c */

void savelevel();
void getlevel();
int savegame(char *);
void restoregame(char *);
void greedy();
void fsorry();
void fcheat();
void init_cells();
unsigned int sum(unsigned char *, int);

/* module: scores.c */

int readboard();
int writeboard();
int makeboard();
int hashewon();
long paytaxes(long);
int winshou();
int shou(int);
void showscores();
void showallscores();
int sortboard();
void newscore(long, char *, int, int);
void new1sub(long, int, char *, long);
void new2sub(long, int, char *, int);
void died(int);
void diedsub(int);
void showscore3(int);
void showscore1(int, char **);

/* module: show.c */

int showstr();
int qshowstr();
int t_setup(int);
int t_endup(int);
int showwear();
int showwield();
int showread();
int showeat();
int showquaff();
int show1(int, char **, int *);
int show3(int);

/* module: signal.c */

int s2choose();
RETSIGTYPE cntlc(int);
RETSIGTYPE sgam(int);
RETSIGTYPE tstop(int);
void sigsetup();
void sigsave();
void sigreset();
RETSIGTYPE sigpanic(int);

/* module: sphere.c */

void newsphere(int, int, int, int);
void rmsphere(int, int);
void sphboom(int, int);
void movsphere();

/* module: store.c */

void dnd_2hed();
void dnd_hed();
void dndstore();
void handsfull();
void outofstock();
void nogold();
void dnditem(int);
void sch_hed();
void oschool();
void obank();
void obank2();
void banktitle(char *);
void ointerest();
void obanksub();
void appraise(int, int);
void otradhead();
void otradepost();
void cnsitm();
void olrs();
void nomore();
void nocash();
void pad_hd();
void opad();
int snag(int);
void pick_char(int);

/* module: tok.c */

char yylex();
void flushall();
void sethard(int);
void readopts();

/* module: tty.c */

int setctty();
int gettty();
int settty();
int setuptty();
int scbr();
int sncbr();
int setupvt100();
int clearvt100();

#else

/* module: action.c */

int run();
int wield();
int ydhi();
int ycwi();
int wear();
int dropobj();
int readscr();
int eatcookie();
int quaff();
int qwhatitem();
int whatitem();
long readnum();
int do_create();

/* module: bill.c */

int letter1();
int letter2();
int letter3();
int letter4();
int letter5();
int letter6();
int mailbill();

/* module: config.c */


/* module: create.c */

int makeplayer();
int newcavelevel();
int makemaze();
int eat();
int cannedlevel();
int treasureroom();
int troom();
int makeobject();
int fillmroom();
int froom();
int fillroom();
int fillmonst();
int sethp();
int checkgen();

/* module: data.c */


/* module: diag.c */

int diag();
int diagdrawscreen();

/* module: display.c */

int bottomdo();
int bottomgold();
int bot_spellx();
int bot_linex();
int bot_hpx();
int botside();
int bothp();
int draws();
int drawscreen();
int showcell();
int show1cell();
int showplayer();
int moveplayer();
int seemagic();
int seepage();

/* module: fortune.c */

char *fortune();

/* module: help.c */

int help();
int welcome();
int retcont();
int openhelp();

/* module: io.c */

int getcharacter();
int newgame();
/*VARARGS*/
int lprintf();
int lwrite();
long lgetc1();
int lrfill();
char *lgetw();
char *lgetl();
int lcreat();
int lopen();
int lappend();
int lrclose();
int lwclose();
int lprcat();
int init_term();
int cl_up();
int cl_dn();
int standout();
int set_score_output();
int lflush();
int putcharacter();
int flush_buf();
int tmcapcnv();
char *getword();

/* module: main.c */

int main();
int parse();
int parse2();

/* module: monster.c */

int createmonster();
int cgood();
int createitem();
int cast();
int speldamage();
int loseint();
int isconfuse();
int nospell();
int fullhit();
int direct();
int godirect();
int ifblind();
int tdirect();
int makewall();
int omnidirect();
int dirsub();
int verifyxy();
int dirpoly();
int hitmonster();
int hitm();
int hitplayer();
int dropsomething();
int dropgold();
int something();
int newobject();
int spattack();
int checkloss();
int annihilate();
int genmonst();
int makemonst();
int randmonst();

/* module: moreobj.c */

int oaltar();
int ohear();
int othrone();
int odeadthrone();
int ochest();
int ofountain();
int fntchange();
int fch();

/* module: movem.c */

int movemonst();
int movemt();
int mmove();
int teleportmonst();

/* module: nap.c */

int nap();
int napms();

/* module: object.c */

int lookforobject();
int finditem();
int ostairs();
int oteleport();
int opotion();
int quaffpotion();
int oscroll();
int adjusttime();
int read_scroll();
void removecurse();
int opit();
int obottomless();
int oelevator();
int ostatue();
int omirror();
int obook();
int readbook();
int ocookie();
int ogold();
int ohome();
int iopts();
int ignore();
int closedoor();

/* module: player.c */

int raiselevel();
int loselevel();
int raiseexperience();
int loseexperience();
int losehp();
int losemhp();
int raisehp();
int raisemhp();
int raisespells();
int raisemspells();
int losespells();
int losemspells();
int positionplayer();
int recalc();
int quit();
int more();
int take();
int drop_object();
int enchantarmor(int);
int enchweapon(int);
int pocketfull();
int nearbymonst();
int stealsomething();
int emptyhanded();
int creategem();
int adjustcvalues();
int gettokstr();
int getpassword();
int getyn();
int packweight();

/* module: regen.c */

int regen();

/* module: savelev.c */

int savelevel();
int getlevel();
int savegame();
int restoregame();
int greedy();
int fsorry();
int fcheat();
int init_cells();
int bwrite();
int bread();
unsigned int sum();

/* module: scores.c */

int readboard();
int writeboard();
int makeboard();
int hashewon();
long paytaxes();
int winshou();
int shou();
int showscores();
int showallscores();
int sortboard();
int newscore();
int new1sub();
int new2sub();
int died();
int diedsub();
int showscore3();
int showscore1();

/* module: show.c */

int showstr();
int qshowstr();
int t_setup();
int t_endup();
int showwear();
int showwield();
int showread();
int showeat();
int showquaff();
int show1();
int show3();

/* module: signal.c */

int s2choose();
RETSIGTYPE cntlc();
RETSIGTYPE sgam();
RETSIGTYPE tstop();
void sigsetup();
void sigsave();
void sigreset();
RETSIGTYPE sigpanic();

/* module: sphere.c */

int newsphere();
int rmsphere();
int sphboom();
int movsphere();

/* module: store.c */

int dnd_2hed();
int dnd_hed();
int dndstore();
int handsfull();
int outofstock();
int nogold();
int dnditem();
int sch_hed();
int oschool();
int obank();
int obank2();
int banktitle();
int ointerest();
int obanksub();
int appraise();
int otradhead();
int otradepost();
int cnsitm();
int olrs();
int nomore();
int nocash();
int pad_hd();
int opad();
int snag();
int pick_char();

/* module: tok.c */

int yylex();
int flushall();
int sethard();
int readopts();

/* module: tty.c */

int setctty();
int gettty();
int settty();
int setuptty();
int intron();
int introff();
int scbr();
int sncbr();
int setupvt100();
int clearvt100();

#endif /* __STD__ */



	/************** extern decls for all data items **********/

extern char *tempfilename, *libdir;
extern char bot1f,bot2f,bot3f;	/* in display.c */
extern char ckpfile[], monstnamelist[];
extern char larnlevels[],lastmonst[];
extern char savefilename[],scorefile[], beenhere[];
extern char objnamelist[],optsfile[],*potionname[],stealth[MAXX][MAXY];
extern char *scrollname[],*spelcode[],*speldescript[];
extern int scrollknown[],potionknown[];
extern char *class[],course[],diagfile[],fortfile[],helpfile[];
extern char *inbuffer,drug[], know[MAXX][MAXY], item[MAXX][MAXY];
extern char *levelname[],loginname[],logname[],*lpbuf,*lpend;
/* extern char *lpnt, *password, mitem[MAXX][MAXY], moved[MAXX][MAXY];  */
extern char *lpnt, *password, moved[MAXX][MAXY]; 
extern char *objectname[], *spelname[],*spelmes[],char_class[], iven[];
extern char nosignal, nobeep,oldx,oldy,playerx,playery, dropflag;
extern char restorflag,nomove, srcount, char_picked;
extern char nowelcome, level, cheat, enable_scroll;
extern char nch[], ndgg[], ckpflag, monstlevel[];
extern char nlpts[], nplt[],nsw[], mail,boldon, splev[];
extern char potprob[], predostuff, scprob[], spelknow[], do_fork, sex;
extern char spelweird[MAXMONST+8][SPNUM], wizard;
extern int diroffx[],diroffy[],hitflag,hit2flag,hit3flag;
extern char rmst, lasthx,lasthy,lastpx,lastpy;
extern char ramboflag;

extern short hitp[MAXX][MAXY], ivenarg[], screen[MAXX][MAXY];
extern short iarg[MAXX][MAXY], lastnum;	/* must be short for gold piles */

extern int yrepcount,userid;
extern int ipoint, iepoint;

extern long initialtime,outstanding_taxes,skill[],gtime,c[],cbak[];
extern long lasttime;

