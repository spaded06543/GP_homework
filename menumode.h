#include "../Include/FlyWin32.h"
struct{
	VIEWPORTid vID;
	SCENEid sID;
	OBJECTid cID, tID, spID;
	TEXTid textID;
	int mode_select;
	
	void load(){
		textID = FAILED_ID;
		spID = FAILED_ID;
		vID = FyCreateViewport(0, 0, 800, 600);
		FnViewport vp;
		vp.ID(vID);
		textID = FyCreateText("Trebuchet MS2", 18, FALSE, FALSE);
		mode_select = 0;
		FnScene scene;
		
		sID = FyCreateScene(1);
		scene.ID(sID);
		scene.SetSpriteWorldSize(800, 600);
		
		FnSprite sp;
		// START Button
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Start", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(300, 250, 0);
		
		// OPTION Button
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Option", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(300, 200, 0);
		
		// QUIT Button
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Quit", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(300, 150, 0);
		
		// Selection mark
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Select", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(300, 250-50*mode_select, 0);
		
	}
	void GameAI(int skip){

	}
	int Movement(BYTE code, BOOL4 value){
		int ret = 1;
		if (code == FY_DOWN && value) mode_select += 1;
		else if (code == FY_UP && value) mode_select -= 1;
		else if (code == FY_Z && mode_select == 0 && value) ret = 3;
		else if (code == FY_Z && mode_select == 1 && value) ret = 2;
		else if (code == FY_Z && mode_select == 2 && value) FyQuitFlyWin32();
		
		if (mode_select < 0) mode_select = 0;
		if (mode_select > 2) mode_select = 2;
		
		return ret;
	}
	void RenderIt(int skip){
		FnSprite sp;
		sp.ID(spID);
		sp.SetPosition(300, 250-50*mode_select, 0);
		
		FnViewport vp;
		vp.ID(vID);
		vp.Render3D(cID, TRUE, TRUE);
        vp.RenderSprites(sID, FALSE, TRUE);
		
		FnText text;
		text.ID(textID);
		text.Begin(vID);
		char S[256];
		sprintf(S, "Lyubu xxxxx Donzo is irreversible!!!\n");
		text.Write(S, 300, 200, 255, 255, 0);
		text.End();
	}
}MM;