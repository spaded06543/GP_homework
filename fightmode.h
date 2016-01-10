#include"humanCC.h"
#include"simpleC.h"
#define FMLOGF ".\\fightmodelog.txt"

vector<BattleC*>allBattleC;
HumanCC mainC;
SimpleC donzos[20];
#define FIGHT1P 1
#define FIGHT2P 2 //todo
#define FIGHTMP 3
struct{
	VIEWPORTid vID;                     // the major viewport
	SCENEid sID, sID2;                  // the 3D scene and scene for sprites
	OBJECTid cID, tID, spID;            // the main camera and the terrain for terrain following
	OBJECTid spID_P1HP[3], spID_P2HP[3];
	ROOMid tRID;
	TEXTid textID, textID2;
	SimpleC * P2_pointer;
	int P1HP_max, P1HP_loseL;
	int P2HP_max, P2HP_loseL;
	int is_end;
	void load(){
		tRID = FAILED_ID;
		// create a text object for displaying messages on screen
		textID = FyCreateText("Trebuchet MS", 18, FALSE, FALSE);
		textID2 = FyCreateText("Trebuchet MS", 36, FALSE, FALSE);
		// create a viewport
		vID = FyCreateViewport(0, 0, 800, 600);
	}
	void Renderload(char*s){
		FnViewport vp;
		vp.ID(vID);
		vp.Render3D(cID, TRUE, TRUE);
		FnText text;
		text.ID(textID2);
		text.Begin(vID);
		text.Write(s, 200, 400, 255, 0, 0);
		text.End();
	}
	void true_load(int mode = FIGHT1P, const char*p1 = "Lyubu2", const char*p2 = "Donzo2"){
		is_end = 0;
		// setup the data searching paths
		FySetShaderPath("Data\\NTU6\\Shaders");
		FySetModelPath("Data\\NTU6\\Scenes");
		FySetTexturePath("Data\\NTU6\\Scenes\\Textures");
		FySetScenePath("Data\\NTU6\\Scenes");
		FySetGameFXPath("Data\\NTU6\\FX0");

		sID = FyCreateScene(10);
		FnScene scene(sID);
		cID = scene.CreateObject(CAMERA);
		// load the scene
		FM.Renderload("NOW loading...  10% Scene"); FySwapBuffers();
		scene.Load("gameScene02");
		scene.SetAmbientLights(1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.6f);
		
		sID2 = FyCreateScene(1);
		FnScene s_scene(sID2);
		s_scene.SetSpriteWorldSize(800, 600);

		// load the terrain
		FM.Renderload("NOW loading...  20% terrain"); FySwapBuffers();
		tID = scene.CreateObject(OBJECT);
		FnObject terrain(tID);
		BOOL beOK1 = terrain.Load("terrain");
		terrain.Show(FALSE);

		// set terrain environment
		tRID = scene.CreateRoom(SIMPLE_ROOM, 10);
		FnRoom room(tRID);
		room.AddObject(tID);

		// load the character
		allBattleC.clear();
		int cc = 0;
		if (mode == FIGHTMP){
			FM.Renderload("NOW loading...  30% xiaobing"); FySwapBuffers();
			const int DN = TYPESOFC, RN = DN - 2 + (string(p1) == p2);
			//load all
			for (int i = 0; i < DN; i++)if (cname[i] != p1 && cname[i] != p2){
				dot fDi = dot(cos(pi*i / RN), sin(pi*i / RN), 0);
				dot uDi = dot(0, 0, 1);
				dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 500 * dot(0, 1, 0) - 150 * fDi;
				donzos[cc] = SimpleC(cname[i].c_str(), sID, tRID, pos, fDi, uDi, 1);
				allBattleC.push_back(&donzos[cc]);
				cc++;
			}
			//load all
			for (int i = DN; i < 2 * DN; i++)if (cname[i - DN] != p1 && cname[i - DN] != p2){
				dot fDi = dot(cos(pi*i / RN), sin(pi*i / RN), 0);
				dot uDi = dot(0, 0, 1);
				dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 150 * fDi;
				donzos[cc] = SimpleC(cname[i - DN].c_str(), sID, tRID, pos, fDi, uDi, 0);
				allBattleC.push_back(&donzos[cc]);
				cc++;
			}
		}
		FM.Renderload("NOW loading...  40% main characters"); FySwapBuffers();
		//load 2P
		{
			dot fDi = dot(0, 1, 0);
			dot uDi = dot(0, 0, 1);
			dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 1000 * fDi;
			donzos[cc] = SimpleC(p2, sID, tRID, pos, fDi, uDi, 1);
			P2_pointer = &donzos[cc];
			P2HP_max = donzos[cc].life;
			allBattleC.push_back(&donzos[cc]);
			cc++;
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
			BattleC&p = *pointer;
			p.attn_time = (p.curpID == p.attnID) * (p.attn_time + skip);
			if (p.attn_time >= p.attnPT)p.attn_time -= p.attnPT;
			p.defe_time = (p.curpID == p.defeID) * (p.defe_time + skip);
			p.wudi_time = max(p.wudi_time - skip, 0);
			p.play_time = max(p.play_time - skip, 0);
		}
		// character generate action
		for (BattleC* pointer : allBattleC){
			BattleC&battleC = *pointer;
			ACTIONid oldpID = battleC.curpID;
			battleC.gen_act(allBattleC);
			if (oldpID == battleC.curpID)continue;
			battleC.SetCurrentAction(0, NULL, battleC.curpID, 5.0f);
			battleC.Play(START, 0.0f, FALSE, TRUE);
			if (battleC.curpID == battleC.attnID && battleC.AItype != 0){
				battleC.play_time = battleC.attnPT;
			}
			else if (oldpID == battleC.defeID){
				battleC.play_time = 2;
			}
			else if (oldpID == battleC.damnID){
				battleC.play_time = 0;
			}
			else if (battleC.curpID == battleC.defeID){
				battleC.play_time = 20;
			}
			else{
				battleC.play_time = 10;
			}
		}
		// move position
		for (BattleC* pointer : allBattleC){
			(*pointer).gen_mov(allBattleC, tID);
		}
		// play character pose
		for (BattleC* pointer : allBattleC){
			BattleC& p = *pointer;
			BOOL4 is_once = p.curpID == p.dieeID || p.curpID == p.damnID || p.curpID == p.defeID;
			p.Play(is_once ? ONCE : LOOP, (float)skip, FALSE, TRUE);
		}
		// attack judge
		for (BattleC* pointer : allBattleC){
			BattleC&p = *pointer;
			BOOL4 is_atk_point = p.curpID == p.attnID && p.attnST <= p.attn_time && p.attn_time < p.attnED;
			if (is_atk_point == 0)continue;
			for (BattleC* btC : allBattleC){
				p.Nattack(*btC);
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
		// play FX
		for (BattleC* pointer : allBattleC){
			BattleC&p = *pointer;
			if (p.gFXID != FAILED_ID) {
				FnGameFXSystem gxS(p.gFXID);
				BOOL4 beOK = gxS.Play((float)skip, ONCE);
				if (!beOK) {
					FnScene scene(sID);
					scene.DeleteGameFXSystem(p.gFXID);
					p.gFXID = FAILED_ID;
				}
			}
		}
	}
	int Movement(BYTE code, BOOL4 value){
		if (mainC.play_time != 0)return 5;
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
			else if (code == FY_C){
				BOOL4 can_defe = mainC.curpID != mainC.attnID && mainC.curpID != mainC.dieeID;
				if (can_defe){
					mainC.SetCurrentAction(0, NULL, mainC.curpID = mainC.defeID, 5.0f);
					mainC.Play(START, 0.0f, FALSE, TRUE);
				}
			}
			else if (code == FY_X){
				BOOL4 can_aimm = mainC.curpID != mainC.attnID && mainC.curpID != mainC.dieeID && mainC.curpID != mainC.defeID;
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
			else if (code == FY_C){
				if (mainC.curpID == mainC.defeID){
					mainC.SetCurrentAction(0, NULL, mainC.curpID = (mainC.want_to_move() ? mainC.runnID : mainC.idleID), 5.0f);
					mainC.Play(START, 0.0f, FALSE, TRUE);
					mainC.play_time = 2;
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
		FnViewport vp(vID);
		vp.Render3D(cID, TRUE, TRUE);
		vp.RenderSprites(sID2, FALSE, TRUE);
		if (tRID == FAILED_ID)return;
		// get camera's data
		FnCamera camera(cID);

		float pos[3], fDir[3], uDir[3];
		camera.GetPosition(pos);
		camera.GetDirection(fDir, uDir);

		// show frame rate
		static char S[256];
		if (frame == 0) {
			FyTimerReset(0);
		}

		if (frame / 10 * 10 == frame) {
			sprintf(S, "Fps: %6.2f", frame / FyTimerCheckTime(0));
		}
		frame += skip;
		if (frame >= 1000) {
			frame = 0;
		}
		FnText text(textID);

		text.Begin(vID);
		text.Write(S, 20, 20, 255, 0, 0);

		static char posS[256], fDiS[256], uDiS[256];
		sprintf(posS, "pos: %8.3f %8.3f %8.3f", pos[0], pos[1], pos[2]);
		sprintf(fDiS, "facing: %8.3f %8.3f %8.3f", fDir[0], fDir[1], fDir[2]);
		sprintf(uDiS, "up: %8.3f %8.3f %8.3f", uDir[0], uDir[1], uDir[2]);

		text.Write(posS, 20, 35, 255, 255, 0);
		text.Write(fDiS, 20, 50, 255, 255, 0);
		text.Write(uDiS, 20, 65, 255, 255, 0);

		char ts[256];
		if (is_end == 0){
			for (int i = 0; i < SZ(allBattleC); i++){
				BattleC&p = *allBattleC[i];
				sprintf(ts, "%s life = %d, aid = %d, dis2main = %.3f, ctype = %d", p.name, p.life, p.aID, p.Cdis(mainC),p.ctype);
				text.Write(ts, 20, 95 + 15 * (2 * i + 0), 255, 255, 0);
				float xyz[3],ppp[3];
				vp.HitPosition(tID, cID, 100, 200, xyz);
				p.GetPosition(ppp, NULL);
				sprintf(ts, "GFXid = %d, (%f %f %f), (%f %f %f)\n", p.gFXID, ppp[0], ppp[1], ppp[2], xyz[0], xyz[1], xyz[2]);
				text.Write(ts, 20, 95 + 15 * (2 * i + 1), 255, 255, 0);
			}
		}
		text.End();

		text.ID(textID2);
		text.Begin(vID);
		if (is_end == 2){
			sprintf(ts, "MISSION COMPLETE");
			text.Write(ts, 200, 380, 255, 255, 0);
			sprintf(ts, "PRESS L TO CONTINUE");
			text.Write(ts, 200, 420, 255, 255, 0);
		}
		else if (is_end == 1){
			sprintf(ts, "MISSION FAILED");
			text.Write(ts, 200, 380, 255, 255, 0);
			sprintf(ts, "PRESS L TO CONTINUE");
			text.Write(ts, 200, 420, 255, 255, 0);
		}
		text.End();
	}
}FM;