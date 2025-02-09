/* sphere.h */
/* for maintaining & moving the spheres of annihilation */
struct sphere
{
	struct sphere *p;	/* pointer to next structure */
	short x,y,lev;		/* location of the sphere */
	short dir;		/* direction sphere is going in */
	short lifetime;		/* duration of the sphere */
};

extern struct sphere *spheres;
