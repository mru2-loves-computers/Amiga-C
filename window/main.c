#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <exec/types.h>
#include <intuition/intuition.h>


struct Window *win = NULL;
struct NewWindow winLayout = {
	20,20,300,200,
	0,1,
	IDCMP_CLOSEWINDOW,
	WFLG_SIZEGADGET | WFLG_DRAGBAR | WFLG_DEPTHGADGET |
	WFLG_CLOSEGADGET | WFLG_ACTIVATE,
	NULL,NULL,
	"Window",
	NULL,NULL,
	0,0,-1,-1,
	WBENCHSCREEN,
};

struct IntuiMessage *msg;
BOOL DONE = FALSE;


void processIDCMP(void) {
	while(!DONE) {
		Wait(1L << win->UserPort->mp_SigBit);
		msg = GT_GetIMsg(win->UserPort);
		GT_ReplyIMsg(msg);
		if(msg->Class == IDCMP_CLOSEWINDOW) DONE = TRUE;
	};
};

void closeAll() {
	if(win) CloseWindow(win);
};

int main() {
	win = OpenWindow(&winLayout);

	while(!DONE) processIDCMP();

	closeAll();



	return 0;
};