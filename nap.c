/* nap.c */
#include "header.h"

int nonap = 0;

/*
 *	routine to take a nap for n milliseconds
 */
nap(x)
int x;
{
	if (x<=0 || nonap)
		return; /* eliminate chance for infinite loop */
	lflush();
	if (x > 999) 
		sleep ((int)(x/1000));
	else napms(x);
}

#ifdef ITIMER
#define bit(_a) (1<<((_a)-1))

SIGTYPE nullf(sig)int sig; { }

/*	napms - sleep for time milliseconds - uses setitimer() */
napms(time)
int time;
{
	struct itimerval    timeout;
	SIGTYPE     (*oldhandler) ();
	int     oldsig=0;

	if (time <= 0) return;

	timerclear(&timeout.it_interval);
	timeout.it_value.tv_sec = time / 1000;
	timeout.it_value.tv_usec = (time % 1000) * 1000;

/* 	oldsig = sigblock(bit(SIGALRM)); */
	setitimer(ITIMER_REAL, &timeout, (struct itimerval *)0);
	oldhandler = signal(SIGALRM, nullf);
	sigpause(oldsig);
	signal(SIGALRM, oldhandler);
/*  	sigsetmask(oldsig); */
}

#else /* ITIMER */
#  ifdef FTIMER
/*	napms - sleep for time milliseconds - uses ftime() */

napms(time)
int time;
{
	struct timeb _gtime;
	time_t matchtime;
	unsigned short matchmilli;
	struct timeb *tp = & _gtime;

	if (time <= 0) return;
	ftime(tp);
	matchmilli = tp->millitm + time;
	matchtime  = tp->time;
	while (matchmilli >= 1000) {
		++matchtime;
		matchmilli -= 1000;
	}

	while(1) {
		ftime(tp);
		if ((tp->time > matchtime) ||
		    ((tp->time == matchtime) && (tp->millitm >= matchmilli)))
			break;
	}
}
#  else /* FTIMER */
#    ifdef HZ
/*	napms - sleep for time milliseconds - uses times() */
napms(time)
int time;
{
	long matchclock, times();
	struct tms stats;

	if (time<=0) 
		time=1; /* eliminate chance for infinite loop */
	if ((matchclock = times(&stats)) == -1 || matchclock == 0)
		return;
	matchclock += (time / (1000 / HZ)); 
	while(matchclock > times(&stats))
		;
}
#    else
napms(x) int x; {}
#    endif /* HZ */
#  endif /* FTIMER */
#endif /* ITIMER */
