#include <stdio.h>
#include <stdlib.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <intuition/intuition.h>
#include <proto/gadtools.h>
#include <libraries/gadtools.h>
#include <proto/exec.h>
#include <exec/types.h>
#include <string.h>

#define STRSIZE 100

struct Window *win=NULL;
struct RastPort *rp;

void convert(int sel,float enterData);
void updateStringGadget(struct Window *win, struct Gadget *gad, UBYTE *newstr);

// GADGETS


// STRING GADGETS
UBYTE DataBuffer[STRSIZE];
UBYTE StringBuffer[STRSIZE];
UBYTE UndoBuffer[STRSIZE];

SHORT StringGadgetPairs[] = {1,0,176,0,176,20,1,20,1,0};

struct Border StringGadgetBorder = {-2,-2,2,0,JAM1,5,StringGadgetPairs,NULL,};

struct StringInfo DataInfo = {
	&DataBuffer[0],
	&UndoBuffer[0],
	0,
	STRSIZE,
	0,0,0,0,0,0,NULL,0,NULL,
};


struct Gadget DataGadget = {
	NULL,
	20,40,175,20,
	GADGHCOMP | GADGHBOX,
	RELVERIFY | STRINGLEFT,
	STRGADGET,
	(APTR)&StringGadgetBorder,
	NULL,
	NULL,
	NULL,
	(APTR)&DataInfo,
	5, NULL,
};

struct StringInfo StringInfo = {
	&StringBuffer[0],
	&UndoBuffer[0],
	0,
	STRSIZE,
	0,0,0,0,0,0,NULL,0,NULL,
};

struct Gadget StringGadget = {
	&DataGadget,
	20,130,175,20,
	GADGHCOMP | GADGHBOX,
	RELVERIFY | STRINGLEFT | LONGINT,
	STRGADGET,
	(APTR)&StringGadgetBorder,
	NULL,
	NULL,
	NULL,
	(APTR)&StringInfo,
	4, NULL,
};
// STRING GADGET END


// BUTON GADGET

SHORT GadgetPairs[] = {0,0,55,0,55,21,0,21,0,0};

struct Border GadgetBorder = {-1,-1,2,0,JAM1,5,GadgetPairs,NULL,};

struct IntuiText Gadget3Text = { 2,0,JAM2,5,5,NULL,(UBYTE *) "L/100km",NULL };

struct Gadget Gadget3 = {
	&StringGadget,
	20,80,55,21,
	GADGHBOX,RELVERIFY,BOOLGADGET,(APTR)&GadgetBorder,
	NULL,&Gadget3Text,NULL,NULL,
	3, NULL,
};

struct IntuiText Gadget2Text = { 2,0,JAM2,5,5,NULL,(UBYTE *) "MPG",NULL };

struct Gadget Gadget2 = {
	&Gadget3,
	80,80,55,21,
	GADGHBOX,RELVERIFY,BOOLGADGET,(APTR)&GadgetBorder,
	NULL,&Gadget2Text,NULL,NULL,
	2, NULL,
};

struct IntuiText Gadget1Text = { 2,0,JAM2,5,5,NULL,(UBYTE *) "Km/L",NULL };

struct Gadget Gadget1 = {
	&Gadget2,
	140,80,55,21,
	GADGHBOX,RELVERIFY,BOOLGADGET,(APTR)&GadgetBorder,
	NULL,&Gadget1Text,NULL,NULL,
	1, NULL,
};
// BUTON GADGET END

struct Gadget *gadAddr;


struct NewWindow winLayout = {
    20,20,300,200,0,1,
    IDCMP_CLOSEWINDOW |IDCMP_GADGETUP,
    WFLG_SIZEGADGET | WFLG_DRAGBAR | WFLG_SMART_REFRESH | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE,
    &Gadget1,NULL,
    "Window",
    NULL,NULL,
    0,0,-1,-1,
    WBENCHSCREEN
};

BOOL DONE = FALSE;

void handleIDCMP() {
	struct IntuiMessage *msg;
	UWORD gadgetid;
	float enterData;

	while(!DONE) {
		Wait(1L << win->UserPort->mp_SigBit);
		msg = GT_GetIMsg(win->UserPort);
		GT_ReplyIMsg(msg);
		switch(msg->Class) {
			case IDCMP_CLOSEWINDOW :
				DONE = TRUE;
				break;
			case IDCMP_GADGETUP:
				gadAddr = (struct Gadget *)msg->IAddress;
				gadgetid = gadAddr->GadgetID;

				enterData = atof(&DataBuffer[0]);
				if(gadgetid == 1) convert(1,enterData);
				if(gadgetid == 2) convert(2,enterData);
				if(gadgetid == 3) convert(3,enterData);

				break;
			default :
				break;

		};

	};
};

void closeAll() {
	if(win) CloseWindow(win);
};

int main() {

	win = OpenWindow(&winLayout);

	while(DONE == FALSE) {
		handleIDCMP();
	}

	closeAll();

	return 0;
};

void convert(int sel,float enterData) {
	UBYTE line1[25];
	UBYTE line2[25];
	float kml;
	float mpg;
	float lkm;
	if(enterData != NULL || enterData > 0) {
		switch(sel) {
			case 1:
				kml = enterData;
				lkm = 100/kml;
				mpg = 282.481/lkm;
				sprintf(line1,"%.2fl/100km; ",lkm);
				sprintf(line2," %.2fmpg",mpg);
				break;
			case 2:
				mpg = enterData;
				kml = mpg/2.824;
				lkm = 282.481/mpg;
				sprintf(line1,"%.2fl/100km; ",lkm);
				sprintf(line2," %.2fkm/l",kml);
				break;
			case 3:
				lkm = enterData;
				kml = 100/lkm;
				mpg = 282.481/lkm;
				sprintf(line1,"%.2fkm/l; ",kml);
				sprintf(line2," %.2fmpg",mpg);
				break;
			default:
				break;
		};
		strcat(line1,line2);
		updateStringGadget(win,&StringGadget,line1);
	};

};

void updateStringGadget(struct Window * win,struct Gadget * gad, UBYTE *newstr) {

	RemoveGList(win,gad,1);
	strcpy(((struct StringInfo *)(gad->SpecialInfo))->Buffer,newstr);
	AddGList(win,gad,~0,1,NULL);
	RefreshGList(gad,win,NULL,1);
//	ActivateGadget(gad,win,NULL);

};