/* signal.c */
#include "header.h"			
#include "extern.h"			

s2choose()	/* text to be displayed if ^C during intro screen */
{
	cursor(1,24); 
	lprcat("Press "); 
	if (boldon) setbold(); 
	lprcat("return"); 
	if (boldon) resetbold();
	lprcat(" to continue: ");
	lflush(); 
}

SIGTYPE
cntlc(int x)	/* what to do for an interrupt */
{
	if (nosignal)  {
		signal(SIGINT, cntlc);
		signal(SIGQUIT, cntlc);
		return;	/* don't do anything if inhibited */
	}
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	quit(); 
	if (predostuff==1) 
		s2choose(); 
	else 
		showplayer();
	lflush();
	signal(SIGQUIT,cntlc);	
	signal(SIGINT,cntlc);
}

/*
 *	subroutine to save the game if a hangup signal
 */
SIGTYPE
sgam(int x)
{
 	if (ckpflag) {
		if (fork() == 0) {
			signal(SIGHUP,  SIG_IGN);
 			savegame(ckpfile);
			exit(1);
		}
	}
	exit(1);
}

#ifdef SIGTSTP
SIGTYPE
tstop(sig)
int sig;
{
    if (sig == SIGTSTP) {
	if (nosignal)   {
		signal(SIGTSTP,tstop);
		return;  /* nothing if inhibited */
	}
	lcreat((char *)0);
	clearvt100();	
	lflush();	  

	sigsave();
	signal(SIGCONT,tstop);
	kill(getpid(),SIGTSTP);
    } else {
	sigreset();
	signal(SIGCONT,SIG_IGN);
	setupvt100();  
	if (predostuff==1) 
		s2choose(); 
	else 
		drawscreen();
	showplayer();	
	lflush();
    }
}
#endif /* SIGTSTP */


/*
**	sigsetup called at beginning of game
**	sigsave called before a shell escape
**	sigreset called after a shell escape
*/
static SIGTYPE 	(*intsave)(),
		(*quitsave)();
void
sigsetup()
{
	SIGTYPE tstop();
	signal(SIGHUP,  sgam);
	signal(SIGINT,  cntlc); 
	signal(SIGQUIT, cntlc); 		
 	signal(SIGILL,  sigpanic);
	signal(SIGTRAP, sigpanic);
#ifndef CYGWIN
	signal(SIGIOT,  sigpanic);		
#endif
#ifdef SIGEMT
	signal(SIGEMT,  sigpanic);
#endif
	signal(SIGFPE,  sigpanic);		
	signal(SIGBUS,  sigpanic);
	signal(SIGSEGV, sigpanic);
	signal(SIGSYS,  sigpanic);
	signal(SIGTERM, sigpanic);
#ifdef SIGTSTP
	signal(SIGTSTP,tstop);
	signal(SIGSTOP,tstop);
#endif /* SIGTSTP */
}

void
sigsave()
{
	signal(SIGHUP,  SIG_DFL);
	intsave  = signal(SIGINT,  SIG_DFL); 
	quitsave = signal(SIGQUIT, SIG_DFL); 		
 	signal(SIGILL,  SIG_DFL);
	signal(SIGTRAP, SIG_DFL);
#ifndef CYGWIN
	signal(SIGIOT,  SIG_DFL);		
#endif
#ifdef SIGEMT
	signal(SIGEMT,  SIG_DFL);
#endif
	signal(SIGFPE,  SIG_DFL);		
	signal(SIGBUS,  SIG_DFL);
	signal(SIGSEGV, SIG_DFL);
	signal(SIGSYS,  SIG_DFL);
	signal(SIGTERM, SIG_DFL);
#ifdef SIGTSTP
	signal(SIGTSTP, SIG_DFL);
	signal(SIGSTOP, SIG_DFL);
#endif /* SIGTSTP */
}

void
sigreset() 
{
	signal(SIGHUP,  sgam);
	signal(SIGINT,  intsave);
	signal(SIGQUIT, quitsave); 		
	signal(SIGILL,  sigpanic);
	signal(SIGTRAP, sigpanic);
#ifndef CYGWIN
	signal(SIGIOT,  sigpanic);		
#endif
#ifdef SIGEMT
	signal(SIGEMT,  sigpanic);
#endif
	signal(SIGFPE,  sigpanic);		
	signal(SIGBUS,  sigpanic);
	signal(SIGSEGV, sigpanic);
	signal(SIGSYS,  sigpanic);
	signal(SIGTERM, sigpanic);
#ifdef SIGTSTP
	signal(SIGTSTP,tstop);		
	signal(SIGSTOP, tstop);
#endif /* SIGTSTP */
}

static char *signame[NSIG] = { 
	"",
	"SIGHUP",  /*	1	 hangup */
	"SIGINT",  /*	2	 interrupt */
	"SIGQUIT", /*	3	 quit */
	"SIGILL",  /*	4	 illegal instruction (not reset when caught) */
	"SIGTRAP", /*	5	 trace trap (not reset when caught) */
	"SIGIOT",  /*	6	 IOT instruction */
	"SIGEMT",  /*	7	 EMT instruction */
	"SIGFPE",  /*	8	 floating point exception */
	"SIGKILL", /*	9	 kill (cannot be caught or ignored) */
	"SIGBUS",  /*	10	 bus error */
	"SIGSEGV", /*	11	 segmentation violation */
	"SIGSYS",  /*	12	 bad argument to system call */
	"SIGPIPE", /*	13	 write on a pipe with no one to read it */
	"SIGALRM", /*	14	 alarm clock */
	"SIGTERM", /*	15	 software termination signal from kill */
	"SIGUSR1", /*	16	 user defines signal 1 */
	"SIGUSR2", /*	17	 user defines signal 2 */
	"SIGCLD",  /*	18	 child death */
	"SIGPWR",  /*	19	 power fail */
	"SIGWINCH" /*	20	 window change */
};

/*
 *	routine to process a fatal error signal
 */
SIGTYPE
sigpanic(sig)
int sig;
{
	clearvt100();
	if (tempfilename)
		unlink(tempfilename);
	fprintf(stderr,"\nUlarn - SHIT! Signal %d !!!",sig);
	if (signame[sig])
		fprintf(stderr, " [%s]\n",signame[sig]);
	sleep(2);
#ifndef CYGWIN
	if (sig == SIGBUS || sig == SIGSEGV || sig == SIGIOT
	) {
		signal(SIGIOT, SIG_DFL);
		abort();
	}
#endif
	exit(sig);
}
