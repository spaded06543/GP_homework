#include "../Include/FlyWin32.h"
struct{
	VIEWPORTid vID;
	SCENEid sID;
	OBJECTid cID, tID;
	TEXTid textID;
	void load(){
		textID = FAILED_ID;
		vID = FyCreateViewport(0, 0, 800, 600);
		FnViewport vp;
		vp.ID(vID);
		textID = FyCreateText("Trebuchet MS3", 20, FALSE, FALSE);
	}
	void GameAI(int skip){

	}
	int Movement(BYTE code, BOOL4 value){
		if (code == FY_UP)return 3;
		return 2;
	}
	void RenderIt(int skip){
		FnViewport vp;
		vp.ID(vID);
		vp.Render3D(cID, TRUE, TRUE);
		FnText text;
		text.ID(textID);
		text.Begin(vID);
		char S[256];
		sprintf(S, "Lyubu x Donzo is irreversible!!!\n");
		text.Write(S, 300, 200, 255, 0, 255);
		sprintf(S, "press up to select Lyubu.\n");
		text.Write(S, 300, 300, 255, 0, 255);
		text.End();
	}
}SM;