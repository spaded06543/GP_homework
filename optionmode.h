#include "../Include/FlyWin32.h"
struct{
	VIEWPORTid vID;
	SCENEid sID;
	OBJECTid cID, tID, spID;
	TEXTid textID;
	AUDIOid BGMid;
	int option;
	int key_select;
	float volume;
	
	void load(){
		textID = FAILED_ID;
		spID = FAILED_ID;
		vID = FyCreateViewport(0, 0, 800, 600);
		FnViewport vp;
		vp.ID(vID);
		textID = FyCreateText("Trebuchet MS3", 20, FALSE, FALSE);
		option = 0;
		key_select = -1;
		
		FnAudio sd;
		BGMid = FyCreateAudio();
		sd.ID(BGMid);
		sd.Load("Bgm/bgm001");
		volume = 0.5;
		sd.SetVolume(volume);
		
		FnScene scene;
		sID = FyCreateScene(1);
		scene.ID(sID);
		scene.SetSpriteWorldSize(800, 600);
		
		FnSprite sp;
		// BGM Volume (?
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/BGM", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(150, 500, 0);
		
		// SE Volume (?
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/SE", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(150, 450, 0);
		
		// Key Setting
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Key_setting", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(150, 400, 0);
		
		// Key Up
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Up", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(180, 360, 0);
		
		// Key Down
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Down", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(180, 320, 0);
		
		// Key Right
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Right", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(180, 280, 0);
		
		// Key Left
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Left", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(180, 240, 0);
		
		// Key Attact
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Atk", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(180, 200, 0);
		
		// To Main Menu
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Opt_back", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(150, 150, 0);
		
		// Selection mark
		spID = scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(200, 40);
		sp.SetImage("Image/Opt_select", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
		sp.SetPosition(150, 500, 0);
	}
	void GameAI(int skip){}
	
	int Movement(BYTE code, BOOL4 value){
		int ret = 2;
		
		if (code == FY_DOWN && option != 3 && value){
			option += 1;
			if (option == 3) option += 1;
		}else if (code == FY_UP && option != 3 && value){
			option -= 1;
			if (option == 3) option -= 1;
		}else if (code == FY_LEFT && option == 0 && value){
			FnAudio sd;
			sd.ID(BGMid);
			volume -= 0.1f;
			if (volume < 0) volume = 0;
			sd.SetVolume(volume);
		}else if (code == FY_RIGHT && option == 0 && value){
			FnAudio sd;
			sd.ID(BGMid);
			volume += 0.1f;
			if (volume > 1) volume = 1;
			sd.SetVolume(volume);
		}else if (code == FY_Z && option == 2 && value){
			option = 3;
			key_select = 0;
		}else if (option == 3 && value){
			key_select += 1;
			if (key_select == 5){
				option = 2;
				key_select = -1;
			}
		}else if (code == FY_Z && option == 4 && value){
			ret = 1;
			option = 0;
		}
		
		if (option < 0) option = 0;
		if (option > 4) option = 4;
		
		return ret;
	}
	void RenderIt(int skip){
		FnSprite sp;
		sp.ID(spID);
		if (option < 3){
			sp.SetPosition(150, 500-50*option, 0);
		}else if (option == 3){
			sp.SetPosition(180, 360-40*key_select, 0);
		}else{
			sp.SetPosition(150, 150, 0);
		}
		
		FnViewport vp;
		vp.ID(vID);
		vp.Render3D(cID, TRUE, TRUE);
        vp.RenderSprites(sID, FALSE, TRUE);
		
	}
}OM;