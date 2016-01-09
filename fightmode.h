#include<unordered_map>
#include"humanCC.h"
#include"simpleC.h"
#define FMLOGF ".\\fightmodelog.txt"

vector<BattleC*>allBattleC;
HumanCC mainC;
SimpleC donzos[20];
#define FIGHT1P 1
#define FIGHT2P 2 //todo
#define FIGHT32 3 //todo
struct{
	VIEWPORTid vID;                     // the major viewport
	SCENEid sID, sID2;                  // the 3D scene and scene for sprites
	OBJECTid cID, tID, spID;            // the main camera and the terrain for terrain following
	OBJECTid spID_P1HP[3], spID_P2HP[3];
	ROOMid tRID;
	TEXTid textID;
	SimpleC * P2_pointer;
	int P1HP_max, P1HP_loseL;
	int P2HP_max, P2HP_loseL;
	int is_end;
	void load(){
		tRID = FAILED_ID;
		// create a text object for displaying messages on screen
		textID = FyCreateText("Trebuchet MS", 18, FALSE, FALSE);
		// create a viewport
		vID = FyCreateViewport(0, 0, 800, 600);
	}
	void Renderload(char*s){
		FnViewport vp;
		vp.ID(vID);
		vp.Render3D(cID, TRUE, TRUE);
		FnText text;
		text.ID(textID);
		text.Begin(vID);
		text.Write(s, 20, 20, 255, 0, 0);
		text.End();
	}
	void true_load(int mode = FIGHT1P, const char*p1 = "Lyubu2", const char*p2 = "Donzo2"){
		is_end = 0;
		// setup the data searching paths
		FySetShaderPath("Data\\NTU6\\Shaders");
		FySetModelPath("Data\\NTU6\\Scenes");
		FySetTexturePath("Data\\NTU6\\Scenes\\Textures");
		FySetScenePath("Data\\NTU6\\Scenes");

		sID = FyCreateScene(10);
		FnScene scene; scene.ID(sID);
		cID = scene.CreateObject(CAMERA);
		// load the scene
		FM.Renderload("NOW loading...  10% Scene"); FySwapBuffers();
		scene.Load("gameScene02");
		scene.SetAmbientLights(1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.6f);
		
		sID2 = FyCreateScene(1);
		FnScene s_scene;
		s_scene.ID(sID2);
		s_scene.SetSpriteWorldSize(800, 600);

		// load the terrain
		FM.Renderload("NOW loading...  20% terrain"); FySwapBuffers();
		tID = scene.CreateObject(OBJECT);
		FnObject terrain;
		terrain.ID(tID);
		BOOL beOK1 = terrain.Load("terrain");
		terrain.Show(FALSE);

		// set terrain environment
		tRID = scene.CreateRoom(SIMPLE_ROOM, 10);
		FnRoom room;
		room.ID(tRID);
		room.AddObject(tID);

		// load the character
		FM.Renderload("NOW loading...  30% characters"); FySwapBuffers();
		allBattleC.clear();
		const int DN = TYPESOFC, RN = DN - 2 + (string(p1) == p2);
		//load all
		for (int i = 0; i < DN; i++)if (cname[i] != p1 && cname[i] != p2){
			dot fDi = dot(cos(pi*i / RN), sin(pi*i / RN), 0);
			dot uDi = dot(0, 0, 1);
			dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 500 * dot(0, 1, 0) - 150 * fDi;
			donzos[i] = SimpleC(cname[i].c_str(), sID, tRID, pos, fDi, uDi, 1);
			allBattleC.push_back(&donzos[i]);
		}
		//load all
		for (int i = DN; i < 2*DN; i++)if (cname[i-DN] != p1 && cname[i-DN] != p2){
			dot fDi = dot(cos(pi*i / RN), sin(pi*i / RN), 0);
			dot uDi = dot(0, 0, 1);
			dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 150 * fDi;
			donzos[i] = SimpleC(cname[i-DN].c_str(), sID, tRID, pos, fDi, uDi, 0);
			allBattleC.push_back(&donzos[i]);
		}
		FM.Renderload("NOW loading...  40% main characters"); FySwapBuffers();
		//load 2P
		{
			dot fDi = dot(0, 1, 0);
			dot uDi = dot(0, 0, 1);
			dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 1000 * fDi;
			donzos[2*DN] = SimpleC(p2, sID, tRID, pos, fDi, uDi, 1);
			P2_pointer = &donzos[2*DN];
			P2HP_max = donzos[2*DN].life;
			allBattleC.push_back(&donzos[2*DN]);
		}
		//load 1P
		{
			dot fDi = dot(0, -1, 0);
			dot uDi = dot(0, 0, 1);
			dot pos = dot(3569, -3208, 1000);
			mainC = HumanCC(p1, sID, tRID, pos, fDi, uDi, 0);
			allBattleC.push_back(&mainC);
			cID = mainC.cID;
			P1HP_max = mainC.life;
		}
		// lighting
		FM.Renderload("NOW loading...  50% light"); FySwapBuffers();
		dot mainLightPos = dot( -4579,   -714,   15530);
		dot mainLightFDi = dot(0.276f,   0.0f, -0.961f);
		dot mainLightUDi = dot(0.961f, 0.026f,  0.276f);

		FnLight lgt;
		lgt.ID(scene.CreateObject(LIGHT));
		lgt.Translate(mainLightPos[0], mainLightPos[1], mainLightPos[2], REPLACE);
		lgt.SetDirection(&mainLightFDi, &mainLightUDi);
		lgt.SetLightType(PARALLEL_LIGHT);
		lgt.SetColor(1.0f, 1.0f, 1.0f);
		lgt.SetName("MainLight");
		lgt.SetIntensity(0.4f);
		// Sprites (HP, image, etc.)
		FM.Renderload("NOW loading...  60% sprites"); FySwapBuffers();
		P1HP_loseL = 300;
		P2HP_loseL = 300;
		FnSprite sp;
		
		spID = s_scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(310, 40);
		sp.SetImage("Image/HP_frame", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.SetPosition(25, 545, 0);
		
		spID = s_scene.CreateObject(SPRITE);
		sp.ID(spID);
		sp.SetSize(310, 40);
		sp.SetImage("Image/HP_frame", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.SetPosition(465, 545, 0);
		
		spID_P1HP[0] = s_scene.CreateObject(SPRITE);
		sp.ID(spID_P1HP[0]);
		sp.SetSize(300, 30);
		sp.SetImage("Image/HP_G", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.SetPosition(30, 550, 0);
		
		spID_P1HP[1] = s_scene.CreateObject(SPRITE);
		sp.ID(spID_P1HP[1]);
		sp.SetSize(300, 30);
		sp.SetImage("Image/HP_R", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.Twiddle(FALSE); // set invisible
		sp.SetPosition(30, 550, 0);
		
		spID_P1HP[2] = s_scene.CreateObject(SPRITE);
		sp.ID(spID_P1HP[2]);
		sp.SetSize(300, 30);
		sp.SetImage("Image/HP_lose", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		//sp.Twiddle(FALSE); // set invisible
		sp.SetPosition(30, 550, 0);
		
		spID_P2HP[0] = s_scene.CreateObject(SPRITE);
		sp.ID(spID_P2HP[0]);
		sp.SetSize(300, 30);
		sp.SetImage("Image/HP_G", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.SetPosition(470, 550, 0);
		
		spID_P2HP[1] = s_scene.CreateObject(SPRITE);
		sp.ID(spID_P2HP[1]);
		sp.SetSize(300, 30);
		sp.SetImage("Image/HP_R", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.Twiddle(FALSE); // set invisible
		sp.SetPosition(470, 550, 0);
		
		spID_P2HP[2] = s_scene.CreateObject(SPRITE);
		sp.ID(spID_P2HP[2]);
		sp.SetSize(300, 30);
		sp.SetImage("Image/HP_lose", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		//sp.Twiddle(FALSE); // set invisible
		sp.SetPosition(470, 550, 0);
		
		FM.Renderload("NOW loading... 100%"); FySwapBuffers();
	}
	void GameAI(int skip){
		// update timer
		for (BattleC* pointer : allBattleC){
			BattleC&battleC = *pointer;
			battleC.dead_time += (battleC.life == 0) * skip;
			battleC.wudi_time = max(battleC.wudi_time - skip, 0);
			battleC.play_time = max(battleC.play_time - skip, 0);
		}
		// character generate action
		for (BattleC* pointer : allBattleC){
			BattleC&battleC = *pointer;
			ACTIONid oldpID = battleC.curpID;
			battleC.gen_act(allBattleC);
			if (oldpID == battleC.curpID)continue;
			battleC.SetCurrentAction(0, NULL, battleC.curpID, 5.0f);
			battleC.Play(START, 0.0f, FALSE, TRUE);
			if (battleC.curpID == battleC.attnID){
				battleC.play_time = battleC.attnPT;
			}
			else{
				battleC.play_time = 10;
			}
		}
		// move position
		for (BattleC* pointer : allBattleC){
			BattleC& battleC = *pointer;
			battleC.gen_mov(allBattleC, tID);
		}
		// play character pose
		for (BattleC* pointer : allBattleC){
			BattleC& battleC = *pointer;
			battleC.Play(battleC.curpID == battleC.dieeID || battleC.curpID == battleC.damnID ? ONCE : LOOP, (float)skip, FALSE, TRUE);
		}
		// attack judge
		for (BattleC* pointer : allBattleC){
			BattleC&battleC = *pointer;
			BOOL4 is_atking = battleC.curpID == battleC.attnID;
			if (is_atking == 0)continue;
			for (BattleC* btC : allBattleC){
				battleC.Nattack(*btC);
			}
		}
		// judge end
		{
			int dead_cnt[2] = { 0, 0 };
			int grop_cnt[2] = { 0, 0 };
			for (BattleC* pointer : allBattleC){
				dead_cnt[pointer->group] += (pointer->life == 0);
				grop_cnt[pointer->group] ++;
			}
			if (mainC.life <= 0){
				is_end = 1;
			}
			else if (dead_cnt[1] == grop_cnt[1]){
				is_end = 2;
			}
		}
	}
	int Movement(BYTE code, BOOL4 value){
		int is_moving_code = code == FY_UP || code == FY_RIGHT || code == FY_LEFT || code == FY_DOWN;
		if (value) {
			if (is_moving_code) {
				if (mainC.curpID == mainC.idleID) {
					mainC.SetCurrentAction(0, NULL, mainC.curpID = mainC.runnID, 5.0f);
					mainC.Play(START, 0.0f, FALSE, TRUE);
				}
			}
			else if (code == FY_Z){
				BOOL4 can_attk = mainC.curpID != mainC.attnID && mainC.curpID != mainC.dieeID;
				if (can_attk){
					mainC.SetCurrentAction(0, NULL, mainC.curpID = mainC.attnID, 5.0f);
					mainC.Play(START, 0.0f, FALSE, TRUE);
				}
			}
			else if (code == FY_X){
				BOOL4 can_aimm = mainC.curpID != mainC.attnID && mainC.curpID != mainC.dieeID;
				if (can_aimm){
					mainC.SetCurrentAction(0, NULL, mainC.curpID = mainC.aimmID, 5.0f);
					mainC.Play(START, 0.0f, FALSE, TRUE);
				}
			}
		}
		else {
			if (is_moving_code) {
				if (mainC.curpID == mainC.runnID) {
					if (mainC.want_to_move() == 0) {
						mainC.SetCurrentAction(0, NULL, mainC.curpID = mainC.idleID, 5.0f);
						mainC.Play(START, 0.0f, FALSE, TRUE);
					}
				}
			}
			else if (code == FY_Z){
				if (mainC.curpID == mainC.attnID){
					if (FyCheckHotKeyStatus(FY_X)){
						mainC.curpID = mainC.aimmID;
					}
					else{
						mainC.curpID = (mainC.want_to_move() ? mainC.runnID : mainC.idleID);
					}
					mainC.SetCurrentAction(0, NULL, mainC.curpID, 5.0f);
					mainC.Play(START, 0.0f, FALSE, TRUE);
				}
			}
			else if (code == FY_X){
				if (mainC.curpID == mainC.aimmID){
					mainC.SetCurrentAction(0, NULL, mainC.curpID = (mainC.want_to_move() ? mainC.runnID : mainC.idleID), 5.0f);
					mainC.Play(START, 0.0f, FALSE, TRUE);
				}
			}
		}
		if (is_end && code == FY_L && value)return 1;
		return 5;
	}
	void RenderIt(int skip){
		FnViewport vp;
		
		int P1HP_length = int(300.0 * float(mainC.life) / float(P1HP_max));
		int P2HP_length = int(300.0 * float((*P2_pointer).life) / float(P2HP_max));
		FnSprite sp;
		sp.ID(spID_P1HP[0]);
		sp.SetSize(P1HP_length, 30);
		if (P1HP_loseL > P1HP_length) P1HP_loseL -= 2;
		sp.ID(spID_P1HP[2]);
		sp.SetSize(P1HP_loseL, 30);
		
		sp.ID(spID_P2HP[0]);
		sp.SetSize(P2HP_length, 30);
		if (P2HP_loseL > P2HP_length) P2HP_loseL -= 2;
		sp.ID(spID_P2HP[2]);
		sp.SetSize(P2HP_loseL, 30);
		
		// render the whole scene
		vp.ID(vID);
		vp.Render3D(cID, TRUE, TRUE);
		vp.RenderSprites(sID2, FALSE, TRUE);
		if (tRID == FAILED_ID)return;
		// get camera's data
		FnCamera camera;
		camera.ID(cID);

		float pos[3], fDir[3], uDir[3];
		camera.GetPosition(pos);
		camera.GetDirection(fDir, uDir);

		// show frame rate
		static char string[128];
		if (frame == 0) {
			FyTimerReset(0);
		}

		if (frame / 10 * 10 == frame) {
			sprintf(string, "Fps: %6.2f", frame / FyTimerCheckTime(0));
		}
		frame += skip;
		if (frame >= 1000) {
			frame = 0;
		}
		FnText text;
		text.ID(textID);

		text.Begin(vID);
		text.Write(string, 20, 20, 255, 0, 0);

		char posS[256], fDiS[256], uDiS[256];
		sprintf(posS, "pos: %8.3f %8.3f %8.3f", pos[0], pos[1], pos[2]);
		sprintf(fDiS, "facing: %8.3f %8.3f %8.3f", fDir[0], fDir[1], fDir[2]);
		sprintf(uDiS, "up: %8.3f %8.3f %8.3f", uDir[0], uDir[1], uDir[2]);

		text.Write(posS, 20, 35, 255, 255, 0);
		text.Write(fDiS, 20, 50, 255, 255, 0);
		text.Write(uDiS, 20, 65, 255, 255, 0);

		char tts[256];
		if (is_end == 2){
			sprintf(tts, "MISSION COMPLETE");
			text.Write(tts, 300, 400, 255, 255, 0);
			sprintf(tts, "PRESS L TO CONTINUE");
			text.Write(tts, 300, 415, 255, 255, 0);
		}
		else if (is_end == 1){
			sprintf(tts, "MISSION FAILED");
			text.Write(tts, 300, 400, 255, 255, 0);
			sprintf(tts, "PRESS L TO CONTINUE");
			text.Write(tts, 300, 415, 255, 255, 0);
		}
		else{
			sprintf(tts, "mainC.aID = %d", mainC.aID);
			text.Write(tts, 20, 95 + 15 * (-1), 255, 255, 0);
			for (int i = 0; i < SZ(allBattleC); i++){
				BattleC&battleC = *allBattleC[i];
				sprintf(tts, "%s life = %d, aid = %d, dis2main = %.3f", battleC.name, battleC.life, battleC.aID, battleC.Cdis(mainC));
				text.Write(tts, 20, 95 + 15 * i, 255, 255, 0);
			}
		}

		text.End();
	}
}FM;