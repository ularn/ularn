/* bill.c		 */
#include "header.h"
#include "player.h"
#include "extern.h"

static char mail600[32];
static int pid;
long Taxes;

letter1()
{
	sprintf(mail600, "/tmp/#%dmail600",pid); /* prepare path */
	if (lcreat(mail600) < 0) { 
		write(1,"can't write 600 letter\n",23); 
		return(0);
	}

lprcat("From: lrs@ularn.com (Ularn Revenue Service)\n");
lprcat("Subject: undeclared income\n");
lprcat("\n   We heard you survived the caverns of Ularn.  Let me be the");
lprcat("\nfirst to congratulate you on your success.  It is quite a feat.");
lprcat("\nIt must also have been very profitable for you.");
lprcat("\n\n   The Dungeon Master has informed us that you brought");
lprintf("\n%d gold piece%s back with you from your journey.  As the",
	(long)c[GOLD], (c[GOLD]==1)?"":"s");
lprcat("\ncounty of Ularn is in dire need of funds, we have spared no time");
lprintf("\nin preparing your tax bill.  You owe %d gold piece%s as", Taxes,
	(Taxes==1)?"":"s");
lprcat("\nof this notice, and is due within 5 days.  Failure to pay will");
lprcat("\nmean penalties.  Once again, congratulations.  We look forward");
lprcat("\nto your future successful expeditions.\n");

	lwclose(); 
	return(1);
}

letter2 ()
{
	sprintf(mail600,"/tmp/#%dmail600",pid); /* prepare path */
	if (lcreat(mail600) < 0) { 
		write(1,"can't write 600 letter\n",23); 
		return(0);
	}

lprcat("From: wilfred@ularn.com (His Majesty King Wilfred)\n");
lprcat("Subject: a noble deed\n");
lprcat("\n   I have heard of your magnificent feat and I, King Wilfred,");
lprcat("\nhereby declare this day to be a national holiday.  Furthermore,");
lprcat("\nthree days hence, ye be invited to the castle to receive the");
lprintf("\nhonour of %s of the Realm.  Upon thy name shall it be written...",
       sex?"Knight":"Lady");
lprcat("\nBravery and courage be yours.");
lprcat("\nMay you live in happiness for evermore...\n");
	lwclose(); 
	return(1);
}

letter3 ()
{
	sprintf(mail600,"/tmp/#%dmail600",pid); /* prepare path */
	if (lcreat(mail600) < 0) { 
		write(1,"can't write 600 letter\n",23); 
		return(0);
	}

	lprcat("From: endelford@ularn.com (Count Endelford)\n");
	lprintf("Subject: You %s\n!", sex?"Bastard":"Bitch"); 
lprcat("\n   I heard (from sources) of your journey.  Congratulations!");
lprintf("You %s!  With several attempts I have yet to endure the", sex?"Bastard":"Bitch");
lprcat(" caves,\nand you, a nobody, make the journey!  From this time");
lprcat(" onward, be warned:\nupon our meeting you shall pay the price!\n");
	lwclose(); 
	return(1);
}

letter4 ()
{
	sprintf(mail600,"/tmp/#%dmail600",pid); /* prepare path */
	if (lcreat(mail600) < 0) { 
		write(1,"can't write 600 letter\n",23); 
		return(0);
	}
	lprcat("From: mainair@ularn.com (Duke of Ularnty)\n");
	lprcat("Subject: High Praise\n");
lprcat("\n   With a certainty, a hero I declare to be amongst us!  A nod of");
lprcat("\nfavour I send to thee.  Methinks Count Endelford this day of");
lprcat("\nright breaths fire as of dragon of whom ye are slayer.  I");
lprcat("\nyearn to behold his anger and jealously.  Should ye choose to");
lprcat("\nunleash some of thy wealth upon those who be unfortunate, I,");
	lprcat("\nDuke Mainair, Shall equal thy gift also.\n");
	lwclose(); 
	return(1);
}

letter5 ()
{
	sprintf(mail600,"/tmp/#%dmail600",pid); /* prepare path */
	if (lcreat(mail600) < 0) { 
		write(1,"can't write 600 letter\n",23); 
		return(0);
	}
	lprcat("From: stmarys@ularn.com (St. Mary's Children's Home)\n");
	lprcat("Subject: these poor children\n");
lprcat("\n   News of your great conquest has spread to all of Ularndom.");
lprintf("\nMight I have a moment of a great %sman's time?  We here at St.",
       sex?"":"wo");
lprcat("\nMary's Children's Home are very poor and many children are");
lprcat("\nstarving.  Disease is widespread and very often fatal without");
lprcat("\ngood food.  Could you possibly find it in your heart to help us");
	lprcat("\nin our plight?  Whatever you could give would help much.");
	lprcat("\n(your gift is tax-deductible)\n");
	lwclose(); 
	return(1);
}

letter6 ()
{
	sprintf(mail600,"/tmp/#%dmail600",pid); /* prepare path */
	if (lcreat(mail600) < 0) { 
		write(1,"can't write 600 letter\n",23); 
		return(0);
	}
	lprcat("From: nds@ularn.com (National Dianthroritis Society of Ularn)\n");
	lprcat("Subject: hope\n");
lprcat("\nCongratulations on your successful expedition.  We are sure much");
lprcat("\ncourage and determination was needed on your quest.  There are");
lprcat("\nmany though, that could never hope to undertake such a journey");
lprcat("\ndue to an enfeebling disease -- dianthroritis.  We at the National");
lprcat("\nDianthroritis Society of Ularn wish to appeal to your philanthropy in");
lprcat("\norder to save many good people -- possibly even yourself or a loved one a few");
lprcat("\nyears from now.  Much work needs to be done in researching this");
lprcat("\ndreaded disease and you can help today.  Could you please see it");
lprcat("\nin your heart to give generously?  Your continued good health");
lprcat("\ncan be your everlasting reward.\n");
	lwclose(); 
	return(1);
}

/*
 *	function to mail the letters to the player if a winner
 */
static int (*pfn[])()= { 
	letter1, letter2, letter3, letter4, letter5, letter6 
};

mailbill ()
{
	int i;
	char buf[128];

	wait((int *)0);
	pid=getpid();
	if (fork() == 0) {
		resetscroll();
		for (i=0; i<sizeof(pfn)/sizeof(int (*)()); i++)
			if (pfn[i]()) {
				sleep(20);  
				sprintf(buf,"/bin/mail %s < %s",
					loginname,mail600);
				system(buf);  
				unlink(mail600);
			}
		exit(0);
	}
}
