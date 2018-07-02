/* sphere.c */
#include "header.h"
#include "monst.h"
#include "player.h"
#include "itm.h"
#include "sphere.h"
#include "extern.h"

/*
 *		  		create a new sphere of annihilation
 *	newsphere(x,y,dir,lifetime)  
 *		int x,y,dir,lifetime;
 *
 *	rmsphere(x,y)		delete a sphere of annihilation from list
 *		int x,y;
 *
 *	sphboom(x,y)		perform the effects of a sphere detonation
 *		int x,y;
 */



/*
 *		Function to create a new sphere of annihilation
 *	newsphere(x,y,dir,lifetime)  
 *		int x,y,dir,lifetime;
 *
 *	Enter with the coordinates of the sphere in x,y
 *	  the direction (0-8 diroffx format) in dir, and the lifespan of the
 *	  sphere in lifetime (in turns)
 *	Returns the number of spheres currently in existence
 */
void newsphere (int x, int y, int dir, int life)
{
	int m,i;
	struct sphere *sp;

	if (((sp=(struct sphere *)malloc(sizeof(struct sphere)))) 
	     == (struct sphere *)NULL)
		died(282);
	if (dir>=9) 
		dir=0;	/* no movement if direction not found */
	if (level==0) {
		vxy(x,y);	/* don't go out of bounds */
	} else {
		if (x<1) x=1;  
		if (x>=MAXX-1) x=MAXX-2;
		if (y<1) y=1;  
		if (y>=MAXY-1) y=MAXY-2;
	}

	for (i=0;i<IVENSIZE;i++) 
		if (iven[i]==OSPHTALISMAN) 
			goto out;

	/* demons dispel spheres */
	if ((m=mitem[x][y].mon) >= DEMONLORD) {
		i = hitp[x][y];
		know[x][y]=1; 
		show1cell(x,y);	/* show the demon (ha ha) */
		cursors(); 
		lprintf("\nThe %s dispels the sphere!",monster[m].name);
		beep(); 
		rmsphere(x,y);	/* remove any spheres that are here */
		mitem[x][y].mon = m;
		hitp[x][y] = i;
		know[x][y]=0; 
	}

	/* disenchantress cancels spheres */
	if (m==DISENCHANTRESS) {
		cursors(); 
	lprintf("\nThe %s cancels the sphere!",monster[m].name); 
		beep();

boom:		
		sphboom(x,y);	/* blow up stuff around sphere */
		rmsphere(x,y);	/* remove any spheres that are here */
	}

	/* cancellation cancels spheres */
	if (c[CANCELLATION]) {
		cursors();
lprcat("\nAs the cancellation takes effect, you hear a great earth shaking blast!"); 
		beep();
		goto boom;
	}
	/* collision of sphere and player! */
	if (playerx==x && playery==y) {
		cursors();
	lprcat("\nYou have been enveloped by the zone of nothingness!\n");
		beep(); 
		rmsphere(x,y);	/* remove any spheres that are here */
		lflush();
		nap(4000);  
		died(258);
	}
out:
	/* collision of spheres detonates spheres */
	if (item[x][y]==OANNIHILATION) {
		cursors(); 
lprcat("\nTwo spheres of annihilation collide! You hear a great earth-shaking blast!");
		beep();
		rmsphere(x,y);
		goto boom;
	}

	item[x][y]=OANNIHILATION;  
	mitem[x][y].mon=0;  
	know[x][y]=1;
	show1cell(x,y);	/* show the new sphere */
	sp->x=x;  
	sp->y=y;  
	sp->lev=level;  
	sp->dir=dir;  
	sp->lifetime=life;  
	sp->p=(struct sphere *)NULL;
	if (spheres==(struct sphere *)NULL)
		spheres=sp;	/* if first node in the sphere list */
	/* add sphere to beginning of linked list */
	else {
		sp->p = spheres;	
		spheres = sp;
	}
	c[SPHCAST]++;
}

/*
 *	rmsphere(x,y)	Function to delete a sphere of annihilation from list
 *		int x,y;
 *
 *	Enter with the coordinates of the sphere (on current level)
 *	Returns the number of spheres currently in existence
 */
void rmsphere (int x, int y)
{
	struct sphere *sp,*sp2=(struct sphere *)NULL;

	for (sp=spheres; sp; sp2=sp,sp=sp->p)
	    if (level==sp->lev)	/* is sphere on this level? */
		/* locate sphere at this location */
		if ((x==sp->x) && (y==sp->y))	{
			item[x][y]= mitem[x][y].mon= 0;  
			know[x][y]=1;
			show1cell(x,y);	/* show the now missing sphere */
			--c[SPHCAST];	
			if (sp==spheres) { 
				sp2=sp; 
				spheres=sp->p; 
				if (sp2) 
					free((char*)sp2); 
			}
			else { 
				sp2->p = sp->p;  
				if (sp) 
					free((char*)sp); 
			}
			break;
		}
}

/*
 *	sphboom(x,y)	Function to perform the effects of a sphere detonation
 *		int x,y;
 *
 *	Enter with the coordinates of the blast, Returns no value
 */
void sphboom (int x, int y)
{
	int i,j,k;

	if (c[HOLDMONST]) c[HOLDMONST]=1;
	if (c[CANCELLATION]) c[CANCELLATION]=1;
	for (j=max(1,x-2); j<min(x+3,MAXX-1); j++)
		for (i=max(1,y-2); i<min(y+3,MAXY-1); i++) {
			item[j][i]=mitem[j][i].mon=0;
			show1cell(j,i);
			for (k=0;k<IVENSIZE;k++) 
				if (iven[k]==OSPHTALISMAN) 
					return;
			if (playerx==j && playery==i) {
				cursors(); 
				beep();
				lprcat("\nYou were too close to the sphere!");
				lflush();
				nap(3000);
				died(283); /* player killed in explosion */
			}
		}
}
/*
 *	movsphere() 	Function to look for and move spheres of annihilation
 *
 * This function works on the sphere linked list, first duplicating the list
 * (the act of moving changes the list), then processing each sphere in order
 * to move it.  They eat anything in their way, including stairs, volcanic
 * shafts, potions, etc, except for upper level demons, who can dispel
 * spheres.
 * No value is returned.
 */
#define SPHMAX 20	/* maximum number of spheres movsphere can handle */
void movsphere()
{
	int x,y,dir,len;
	struct sphere *sp,*sp2;
	struct sphere sph[SPHMAX];

	/* first duplicate sphere list */
	/* look through sphere list */
	for (x=0,sp2=spheres; sp2; sp2=sp2->p)
		/* only if this level */
		if (sp2->lev == level) {
			sph[x] = *sp2;	
			sph[x++].p = (struct sphere *)NULL;  /* copy the struct */
			if (x>1)
				sph[x-2].p = &sph[x-1]; /* link pointers */
		}

	if (x) sp= sph;	/* if any spheres, point to them */
	else return;	/* no spheres */

	/* look through sphere list */
	for (sp=sph; sp; sp=sp->p) {
		x = sp->x;	  
		y = sp->y;
		if (item[x][y]!=OANNIHILATION) 
			continue;/* not really there */

		/* has sphere run out of gas? */
		if (--(sp->lifetime) < 0) {
			rmsphere(x,y); /* delete sphere */
			continue;
		}
		/* time to move the sphere */
		switch(rnd((int)max(7,c[INTELLIGENCE]>>1))) {
		case 1:
		case 2:		/* change direction to a random one */
			sp->dir = rnd(8);
		default:	/* move in normal direction */
			dir = sp->dir;		
			len = sp->lifetime;
			rmsphere(x,y);
			newsphere(x+diroffx[dir],y+diroffy[dir],dir,len);
		};
	}
}
