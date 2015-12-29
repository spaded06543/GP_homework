#define SZ(x) (int)(x).size()
#define TYPESOFC 6
string cname[TYPESOFC] = {
	"Donzo2",
	"Lyubu2",
	"Robber02",
	"Footman01",
	"Sorcerer01",
	"WarLord01"
};
char sPath[TYPESOFC][64] = {
	"Data\\NTU6\\Characters",
	"Data\\NTU6\\Characters",
	"Data\\NTU6\\Characters",
	"Data\\NTU6\\Characters_More\\Footman01",
	"Data\\NTU6\\Characters_More\\Sorcerer01",
	"Data\\NTU6\\Characters_More\\WarLord01"
};
char sAttN[TYPESOFC][64] = {
	"AttackL1",
	"NormalAttack1",
	"NormalAttack1",
	"1H_swing_left",
	"swing_right",
	"2H_swing_mid_left"
};
char sIdle[TYPESOFC][64] = {
	"Idle",
	"Idle",
	"CombatIdle",
	"Idle",
	"Idle",
	"Idle"
};
char sRunn[TYPESOFC][64] = {
	"Run",
	"Run",
	"Run",
	"Run",
	"Run",
	"Run"
};
char sDamN[TYPESOFC][64] = {
	"DamageL",
	"LeftDamaged",
	"Damage1",
	"Hit_from_front",
	"Hit_from_back",
	"Hit_from_front"
};
char sDiee[TYPESOFC][64] = {
	"Die",
	"Die",
	"Dead",
	"Dying_A",
	"Dying_A",
	"Dying_A"
};
map<string, char*>aPath, aAttN, aIdle, aRunn, aDamN, aDiee;
void ini_actions_name(){
	static BOOL4 is_done = 0;
	if (is_done)return;
	is_done = 1;
	for (int i = 0; i < TYPESOFC; i++){
		aPath.insert(pair<string, char*>(cname[i], sPath[i]));
		aAttN.insert(pair<string, char*>(cname[i], sAttN[i]));
		aIdle.insert(pair<string, char*>(cname[i], sIdle[i]));
		aRunn.insert(pair<string, char*>(cname[i], sRunn[i]));
		aDamN.insert(pair<string, char*>(cname[i], sDamN[i]));
		aDiee.insert(pair<string, char*>(cname[i], sDiee[i]));
	}
}
typedef struct BattleC : public FnCharacter {
	int life;
	int dead_time, wudi_time;
	char name[20];
	CHARACTERid aID;
	BattleC(){}
	BattleC(const char* name_, SCENEid sID, ROOMid terrainRoomID, dot pos, dot fDi, dot uDi, int life_){
		strcpy(name,name_);
		FySetModelPath(aPath[name]);
		FySetTexturePath(aPath[name]);
		FySetCharacterPath(aPath[name]);
		FnScene scene; scene.ID(sID);
		FnObject terrain; terrain.ID(terrainRoomID);
		ID(aID = scene.LoadCharacter(name));
		SetDirection(&fDi, &uDi);
		SetTerrainRoom(terrainRoomID, 10.0f);
		BOOL4 beOK = PutOnTerrain(&pos);
		if (beOK == 0){
			fprintf(stderr, "load %s failed!", name);
		}
		assert(beOK);
		SetCurrentAction(NULL, 0, GetBodyAction(NULL, aIdle[name]));
		Play(START, 0.0f, FALSE, TRUE);
		life = life_;
		dead_time = wudi_time = 0;
	}
}BattleC;

vector<BattleC*>allBattleC;
BattleC donzos[20], robbrs[20], mainC;
BattleC preloadmainC[TYPESOFC];
#define FIGHT1P 1
#define FIGHT2P 2 //todo
#define FIGHT32 3 //todo
struct{
	VIEWPORTid vID;                     // the major viewport
	SCENEid sID;                        // the 3D scene
	OBJECTid cID, tID;                  // the main camera and the terrain for terrain following
	ROOMid terrainRoomID;
	TEXTid textID;

	ACTIONid idleID, runnID, natkID;  // three actions
	ACTIONid curPoseID;

	int is_end;
	void load(int mode = FIGHT1P, const char*p1 = "Lyubu2", const char*p2 = "Donzo2"){
		ini_actions_name();
		is_end = 0;
		terrainRoomID = textID = FAILED_ID;
		// setup the data searching paths
		FySetShaderPath("Data\\NTU6\\Shaders");
		FySetModelPath("Data\\NTU6\\Scenes");
		FySetTexturePath("Data\\NTU6\\Scenes\\Textures");
		FySetScenePath("Data\\NTU6\\Scenes");

		// create a viewport
		vID = FyCreateViewport(0, 0, 800, 600);
		FnViewport vp;
		vp.ID(vID);

		// create a 3D scene
		sID = FyCreateScene(10);
		FnScene scene;
		scene.ID(sID);

		// load the scene
		scene.Load("gameScene02");
		scene.SetAmbientLights(1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.6f);

		// load the terrain
		tID = scene.CreateObject(OBJECT);
		FnObject terrain;
		terrain.ID(tID);
		BOOL beOK1 = terrain.Load("terrain");
		terrain.Show(FALSE);

		// set terrain environment
		terrainRoomID = scene.CreateRoom(SIMPLE_ROOM, 10);
		FnRoom room;
		room.ID(terrainRoomID);
		room.AddObject(tID);

		// load the character

		allBattleC.clear();
		const int DN = TYPESOFC, RN = DN - 2 + (string(p1) == p2);
		//load all
		for (int i = 0; i < DN; i++)if (cname[i] != p1 && cname[i] != p2){
			dot fDi = dot(cos(2 * pi*i / RN), sin(2 * pi*i / RN), 0);
			dot uDi = dot(0, 0, 1);
			dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 150 * fDi;
			donzos[i] = BattleC(cname[i].c_str(), sID, terrainRoomID, pos, fDi, uDi, 30);
			allBattleC.push_back(&donzos[i]);
		}
		//load 2P
		{
			dot fDi = dot(cos(2 * pi / 4), sin(2 * pi / 4), 0);
			dot uDi = dot(0, 0, 1);
			dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 1000 * fDi;
			donzos[DN] = BattleC(p2, sID, terrainRoomID, pos, fDi, uDi, 100);
			allBattleC.push_back(&donzos[DN]);
		}
		//load 1P
		dot fDi = dot(0, -1, 0);
		dot uDi = dot(0, 0, 1);
		dot pos = dot(3569, -3208, 1000);
		{
			mainC = BattleC(p1, sID, terrainRoomID, pos, fDi, uDi, 200);
			allBattleC.push_back(&mainC);
			idleID = mainC.GetBodyAction(NULL, aIdle[mainC.name]);
			runnID = mainC.GetBodyAction(NULL, aRunn[mainC.name]);
			natkID = mainC.GetBodyAction(NULL, aAttN[mainC.name]);
			curPoseID = idleID;
		}
		// translate the camera
		cID = scene.CreateObject(CAMERA);

		FnCamera camera;
		camera.ID(cID);
		camera.SetNearPlane(5.0f);
		camera.SetFarPlane(100000.0f);

		// set camera initial position and orientation
		set_cam_dir(&fDi, &uDi);
		set_cam_pos(&pos, &pos, &uDi);
		camera.SetPosition(&pos);
		camera.SetDirection(&fDi, &uDi);
		// lighting
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

		// create a text object for displaying messages on screen
		textID = FyCreateText("Trebuchet MS", 18, FALSE, FALSE);
	}
	void GameAI(int skip){
		// play character pose
		for (BattleC* pointer : allBattleC){
			BattleC& beatenC = *pointer;

			beatenC.Play(beatenC.life <= 0 || beatenC.wudi_time != 0 ? ONCE : LOOP, (float)skip, FALSE, TRUE);

			beatenC.dead_time += (beatenC.life == 0) * skip;
			int old_wudi_time = beatenC.wudi_time;
			beatenC.wudi_time = max(beatenC.wudi_time - skip, 0);
			if (old_wudi_time > 1 && beatenC.wudi_time <= 1){
				beatenC.SetCurrentAction(NULL, 0, beatenC.GetBodyAction(NULL, aIdle[beatenC.name]));
				beatenC.Play(START, 0.0f, FALSE, TRUE);
			}
		}

		FnCharacter&actor=mainC;
		int is_atking = curPoseID == natkID;

		if (is_atking){
			dot a_pos, fDi;
			actor.GetPosition(&a_pos, NULL);
			actor.GetDirection(&fDi, NULL);
			fDi = dan(fDi);
			for (BattleC* pointer : allBattleC)if (pointer->life != 0 && pointer->wudi_time == 0){
				BattleC& beatenC = *pointer;
				dot b_pos;
				beatenC.GetPosition(&b_pos, NULL);
				dot dd = b_pos - a_pos;
				dou disab = my_dis(dd);
				if (sgn(disab) == 0)continue;//too close
				dot dandd = dan(dd);
				dou sint = my_dis(fDi*dandd);
				BOOL4 isbeaten = disab < 140 && fDi%dandd >= 0 && asin(fabs(sint)) <= pi*25.0 / 180;
				if (isbeaten == 0)continue;

				beatenC.life = max(beatenC.life - 10, 0);
				if (sgn(dd%dd)){
					beatenC.SetDirection(&((-1)*dandd), NULL);
					beatenC.SetPosition(&(b_pos + 5 * dandd));
				}
				if (beatenC.life <= 0){
					beatenC.SetCurrentAction(NULL, 0, beatenC.GetBodyAction(NULL, aDiee[beatenC.name]));
					beatenC.Play(ONCE, 0.0f, FALSE, TRUE);
				}
				else{
					beatenC.SetCurrentAction(NULL, 0, beatenC.GetBodyAction(NULL, aDamN[beatenC.name]));
					beatenC.Play(ONCE, 0.0f, FALSE, TRUE);
					beatenC.wudi_time += 24;
				}
			}
		}
		//judge end
		{
			int dead_cnt = 0;
			for (BattleC* pointer : allBattleC)dead_cnt += (pointer->life == 0);
			is_end = (mainC.life <= 0 || dead_cnt >= SZ(allBattleC) - 1);
		}
		// get camera object
		FnCamera camera; camera.ID(cID);
		// get terrain object
		FnObject terrain; terrain.ID(tID);
		// camera & actor position
		float cam_pos[3], act_pos[3];
		// get camera & actor direction
		float cam_fDir[3], cam_uDir[3], act_fDir[3];
		camera.GetDirection(cam_fDir, cam_uDir);
		actor.GetDirection(act_fDir, NULL);
		// dis-accuracy prevent
		const float e = EPSILON;

		// z value by cam.dir x act.dir
		float z = cam_fDir[0] * act_fDir[1] - cam_fDir[1] * act_fDir[0],
			dot = cam_fDir[0] * act_fDir[0] + cam_fDir[1] * act_fDir[1];

		// ***** Up *****
		if (FyCheckHotKeyStatus(FY_W) || FyCheckHotKeyStatus(FY_UP)) {

			// if not facing up, face to up
			if (dot < 0 || z < -e || z > e) {
				set_act_dir(cam_fDir, act_fDir);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			if (is_atking == 0)actor.MoveForward(12.0f, TRUE, FALSE, 0.0f, TRUE);
			camera.GetPosition(cam_pos);
			actor.GetPosition(act_pos);

			// if too far, camera foward
			if (distance(cam_pos, act_pos) > dis) {
				set_cam_pos(cam_pos, act_pos, cam_fDir);
				camera.SetPosition(cam_pos);
			}
		}

		// ***** Down *****
		if (FyCheckHotKeyStatus(FY_S) || FyCheckHotKeyStatus(FY_DOWN)) {

			// if not facing down, face to down
			if (dot > 0 || z < -e || z > e) {
				set_act_dir(cam_fDir, act_fDir, DOWN);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			if (is_atking == 0)actor.MoveForward(12.0f, TRUE, FALSE, 0.0f, TRUE);
			camera.GetPosition(cam_pos);
			actor.GetPosition(act_pos);

			// if too close, camera backward
			if (distance(cam_pos, act_pos) < dis) {
				set_cam_pos(cam_pos, act_pos, cam_fDir);
				camera.SetPosition(cam_pos);
			}
		}

		// ***** Right *****
		if (FyCheckHotKeyStatus(FY_D) || FyCheckHotKeyStatus(FY_RIGHT)) {
			// if not facing right, face to right
			if (z > 0 || dot < -e || dot > e) {
				set_act_dir(cam_fDir, act_fDir, RIGHT);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			actor.GetPosition(act_pos);
			camera.GetPosition(cam_pos);
			actor.TurnRight(THETA);
			if (is_atking == 0){
				if (actor.MoveForward(2 * DISTANCE_P * sin(degree2rad(THETA)), TRUE, FALSE, 0.0f, TRUE) == BOUNDARY) {
					float Dir[3] = { 0.0f, 0.0f, 1.0f };
					camera.SetDirection(NULL, Dir);
					camera.TurnRight(-90.0f + THETA);
					camera.MoveForward(12.0f, TRUE, FALSE, 0.0f, TRUE);
				}
			}

			// camera facing actor
			actor.GetPosition(act_pos);
			camera.GetPosition(cam_pos);
			cam_fDir[0] = act_pos[0] - cam_pos[0];
			cam_fDir[1] = act_pos[1] - cam_pos[1];
			set_cam_dir(cam_fDir, cam_uDir);
			camera.SetDirection(cam_fDir, cam_uDir);
		}

		// ***** Left *****
		if (FyCheckHotKeyStatus(FY_A) || FyCheckHotKeyStatus(FY_LEFT)) {
			// if not facing left, face to right
			if (z < 0 || dot < -e || dot > e) {
				set_act_dir(cam_fDir, act_fDir, LEFT);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			actor.GetPosition(act_pos);
			camera.GetPosition(cam_pos);
			actor.TurnRight(-THETA);
			if (is_atking == 0){
				if (actor.MoveForward(2 * DISTANCE_P * sin(degree2rad(THETA)), TRUE, FALSE, 0.0f, TRUE) == BOUNDARY) {
					float Dir[3] = { 0.0f, 0.0f, 1.0f };
					camera.SetDirection(NULL, Dir);
					camera.TurnRight(90.0f - THETA);
					camera.MoveForward(12.0f, TRUE, FALSE, 0.0f, TRUE);
				}
			}

			// camera facing actor
			actor.GetPosition(act_pos);
			camera.GetPosition(cam_pos);
			cam_fDir[0] = act_pos[0] - cam_pos[0];
			cam_fDir[1] = act_pos[1] - cam_pos[1];
			set_cam_dir(cam_fDir, cam_uDir);
			camera.SetDirection(cam_fDir, cam_uDir);
		}


		// ***** Double hot key pressed *****
		// assume no more key pressed at the same time
		// up and right
		if ((FyCheckHotKeyStatus(FY_W) || FyCheckHotKeyStatus(FY_UP)) &&
			(FyCheckHotKeyStatus(FY_D) || FyCheckHotKeyStatus(FY_RIGHT))) {
			mix_dir(cam_fDir, act_fDir, UP_RIGHT);
			actor.SetDirection(act_fDir, NULL);
		}
		// up and left
		else if ((FyCheckHotKeyStatus(FY_W) || FyCheckHotKeyStatus(FY_UP)) &&
			(FyCheckHotKeyStatus(FY_A) || FyCheckHotKeyStatus(FY_LEFT))) {
			mix_dir(cam_fDir, act_fDir, UP_LEFT);
			actor.SetDirection(act_fDir, NULL);
		}
		// down and right
		else if ((FyCheckHotKeyStatus(FY_S) || FyCheckHotKeyStatus(FY_DOWN)) &&
			(FyCheckHotKeyStatus(FY_D) || FyCheckHotKeyStatus(FY_RIGHT))) {
			mix_dir(cam_fDir, act_fDir, DOWN_RIGHT);
			actor.SetDirection(act_fDir, NULL);
		}
		// down and left
		else if ((FyCheckHotKeyStatus(FY_S) || FyCheckHotKeyStatus(FY_DOWN)) &&
			(FyCheckHotKeyStatus(FY_A) || FyCheckHotKeyStatus(FY_LEFT))) {
			mix_dir(cam_fDir, act_fDir, DOWN_LEFT);
			actor.SetDirection(act_fDir, NULL);
		}


		// if cam run into terrain
		/*camera.GetPosition ( cam_pos );
		float dir[3] = { 0.0f, 0.0f, -1.0f };

		if ( terrain.HitTest ( cam_pos, dir ) <= 0 ) {
		}*/
		/*if ( terrain.HitTest ( cam_pos, dir ) <= 0 ) {

		do {

		cam_angle += PHI;
		float fixed_dis = DISTANCE * cos ( degree2rad ( cam_angle ) );
		float k = sqrt ( ( dis * dis ) / ( cam_fDir[0] * cam_fDir[0] + cam_fDir[1] * cam_fDir[1] ) );
		cam_pos[0] = k * cam_fDir[0] * fixed_dis;
		cam_pos[1] = k * cam_fDir[1] * fixed_dis;
		cam_pos[2] = height = DISTANCE * sin ( degree2rad ( cam_angle ) );
		camera.SetPosition ( cam_pos );
		set_cam_dir ( cam_fDir, cam_uDir );
		camera.SetDirection ( cam_fDir, cam_uDir );
		} while ( terrain.HitTest ( cam_pos, cam_fDir ) > 0 );

		}*/
	}
	int Movement(BYTE code, BOOL4 value){
		FnCharacter&actor=mainC;
		int is_moving_code =
			code == FY_UP || code == FY_W ||
			code == FY_RIGHT || code == FY_D ||
			code == FY_LEFT || code == FY_A ||
			code == FY_DOWN || code == FY_S;
		int has_moving_key =
			FyCheckHotKeyStatus(FY_UP) || FyCheckHotKeyStatus(FY_W) ||
			FyCheckHotKeyStatus(FY_RIGHT) || FyCheckHotKeyStatus(FY_D) ||
			FyCheckHotKeyStatus(FY_LEFT) || FyCheckHotKeyStatus(FY_A) ||
			FyCheckHotKeyStatus(FY_DOWN) || FyCheckHotKeyStatus(FY_S);
		if (value) {
			if (is_moving_code) {
				if (curPoseID == idleID) {
					curPoseID = runnID;
					actor.SetCurrentAction(0, NULL, curPoseID, 5.0f);
					actor.Play(START, 0.0f, FALSE, TRUE);
				}
			}
			else if (code == FY_Z){
				if (curPoseID != natkID){
					curPoseID = natkID;
					actor.SetCurrentAction(0, NULL, curPoseID, 5.0f);
					actor.Play(START, 0.0f, FALSE, TRUE);
				}
			}
		}
		else {
			if (is_moving_code) {
				if (curPoseID == runnID) {
					if (has_moving_key == 0) {
						curPoseID = idleID;
						actor.SetCurrentAction(0, NULL, curPoseID, 5.0f);
						actor.Play(START, 0.0f, FALSE, TRUE);
					}
				}
			}
			else if (code == FY_Z){
				curPoseID = has_moving_key ? runnID : idleID;
				actor.SetCurrentAction(0, NULL, curPoseID, 5.0f);
				actor.Play(START, 0.0f, FALSE, TRUE);
			}
		}
		if (is_end && code == FY_L && value)return 1;
		return 4;
	}
	void RenderIt(int skip){
		FnViewport vp;

		// render the whole scene
		vp.ID(vID);
		vp.Render3D(cID, TRUE, TRUE);

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
			float curTime;

			curTime = FyTimerCheckTime(0);
			sprintf(string, "Fps: %6.2f", frame / curTime);
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
		if (is_end){
			sprintf(tts, "MISSION COMPLETE");
			text.Write(tts, 300, 400, 255, 255, 0);
			sprintf(tts, "PRESS L TO CONTINUE");
			text.Write(tts, 300, 415, 255, 255, 0);
		}
		else{
			for (int i = 0; i < SZ(allBattleC); i++){
				BattleC&battleC = *allBattleC[i];
				sprintf(tts, "%s life = %d", battleC.name, battleC.life);
				text.Write(tts, 20, 95 + 15 * i, 255, 255, 0);
			}
		}

		text.End();
	}
}FM;