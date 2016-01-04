#include<unordered_map>
#define SZ(x) (int)(x).size()
#define ALL(x) (x).begin(),(x).end()
#define TYPESOFC 6
#define LOGF ".\\fightmodelog.txt"
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
struct cas{
	int life;
	dou speed;
	int attnD, attnPT;
	dou attnR, attnT;
};
cas CASL[TYPESOFC] = {
	cas({300, +6, 30, 20, 140, 75 }),
	cas({200, 12, 20, 20, 140, 25 }),
	cas({+30, +9, 10, 20, 140, 25 }),
	cas({+75, +9, 15, 20, 140, 25 }),
	cas({+50, 12, 10, 20, 140, 25 }),
	cas({123, +6, 20, 20, 140, 25 })
};
unordered_map<string, int>my_cid;
void ini_actions_name(){
	static BOOL4 is_done = 0;
	if (is_done)return;
	is_done = 1;
	for (int i = 0; i < TYPESOFC; i++){
		my_cid.insert(pair<string, int>(cname[i], i));
	}
}
struct BattleC : public FnCharacter {
	int life;
	dou speed;
	int attnD, attnPT;
	dou attnR, attnT;
	int dead_time, wudi_time, play_time;
	char name[20];
	CHARACTERid aID;
	ACTIONid attnID, idleID, runnID, damnID, dieeID, curpID;  // those actions
	int AItype;//0 human, 1 random
	int group;
	BattleC(){}
	BattleC(const char* name_, SCENEid sID, ROOMid tRID, dot pos, dot fDi, dot uDi,int group_, int AItype_ = 1){
		strcpy(name,name_);
		BOOL4 beOK = my_cid.count(name);
		if (beOK == 0){
			fprintf(stderr, "no this character %s!", name);
		}
		assert(beOK);
		int cid = my_cid[name];
		FySetModelPath(sPath[cid]);
		FySetTexturePath(sPath[cid]);
		FySetCharacterPath(sPath[cid]);
		FnScene scene; scene.ID(sID);
		FnObject terrain; terrain.ID(tRID);
		ID(aID = scene.LoadCharacter(name));
		SetDirection(&fDi, &uDi);
		SetTerrainRoom(tRID, 10.0f);
		beOK = PutOnTerrain(&pos);
		if (beOK == 0){
			fprintf(stderr, "load %s failed!", name);
		}
		assert(beOK);
		attnID = GetBodyAction(NULL, sAttN[cid]);
		idleID = GetBodyAction(NULL, sIdle[cid]);
		runnID = GetBodyAction(NULL, sRunn[cid]);
		damnID = GetBodyAction(NULL, sDamN[cid]);
		dieeID = GetBodyAction(NULL, sDiee[cid]);
		curpID = idleID;
		SetCurrentAction(NULL, 0, idleID);
		Play(START, 0.0f, FALSE, TRUE);
		life = CASL[cid].life;
		speed = CASL[cid].speed;
		attnD = CASL[cid].attnD;
		attnPT = CASL[cid].attnPT;
		attnR = CASL[cid].attnR;
		attnT = CASL[cid].attnT;
		dead_time = wudi_time = play_time = 0;
		group = group_;
		AItype = AItype_;
	}
	dou Cdis(BattleC&C2){
		dot a_pos, b_pos;
		GetPosition(&a_pos, NULL);
		C2.GetPosition(&b_pos, NULL);
		return my_dis(b_pos - a_pos);
	}
	void Nattack(BattleC&beatenC){
		if (beatenC.life == 0 || beatenC.wudi_time != 0 || beatenC.group == group)return;
		dot a_pos, b_pos, fDi;
		GetPosition(&a_pos, NULL);
		GetDirection(&fDi, NULL);
		fDi = dan(fDi);
		beatenC.GetPosition(&b_pos, NULL);
		dot dd = b_pos - a_pos;
		dou disab = my_dis(dd);
		if (sgn(disab) == 0)return;//too close
		dot dandd = dan(dd);
		dou sint = my_dis(fDi*dandd);
		BOOL4 isbeaten = fDi%dandd >= 0 && disab < attnR && (asin(fabs(sint)) * 180 / pi) < attnT;
		if (isbeaten == 0)return;
		//decrease life
		beatenC.life = max(beatenC.life - attnD, 0);
		//turn and move back
		beatenC.SetDirection(&((-1)*dandd), NULL);
		beatenC.SetPosition(&(b_pos + 5 * dandd));
		//set Act
		beatenC.SetCurrentAction(NULL, 0, beatenC.curpID = (beatenC.life <= 0 ? beatenC.dieeID : beatenC.damnID));
		beatenC.Play(ONCE, 0.0f, FALSE, TRUE);
		//increase timer
		if (beatenC.life>0)beatenC.wudi_time = 34;
		beatenC.play_time = 24;
	}
	template<class T>
	BattleC* find_target(T st, T ed){
		BattleC*ans = NULL;
		for (; st != ed; st++)if ((*st)->group != group && (*st)->life){
			if (ans == NULL || Cdis(*ans) > Cdis(*(*st))){
				ans = *st;
			}
		}
		return ans;
	}
	virtual void gen_act(BattleC*targetC){}
};

struct HumanCC : public BattleC {
	HumanCC() {}
	HumanCC(const char* name_, SCENEid sID, ROOMid tRID, dot pos, dot fDi, dot uDi, int group_) : BattleC(name_, sID, tRID, pos, fDi, uDi, group_, 0){}
	BOOL4 want_to_move(){
		return
			FyCheckHotKeyStatus(FY_UP) || FyCheckHotKeyStatus(FY_W) ||
			FyCheckHotKeyStatus(FY_RIGHT) || FyCheckHotKeyStatus(FY_D) ||
			FyCheckHotKeyStatus(FY_LEFT) || FyCheckHotKeyStatus(FY_A) ||
			FyCheckHotKeyStatus(FY_DOWN) || FyCheckHotKeyStatus(FY_S);
	}
	void gen_act(BattleC*targetC){
		if (play_time != 0 || curpID == dieeID){
			return;
		}
		else if (curpID == damnID){
			curpID = want_to_move() ? runnID : idleID;
		}
	}
};
struct SimpleC : public BattleC {
	SimpleC() {}
	SimpleC(const char* name_, SCENEid sID, ROOMid tRID, dot pos, dot fDi, dot uDi, int group_) : BattleC(name_, sID, tRID, pos, fDi, uDi, group_, 1){}
	void gen_act(BattleC*targetC){
		vector<pair<ACTIONid, int>>posA(0);//possilbe act, weight possibility
		int total_weight = 0;
		dou disab = targetC ? Cdis(*targetC) : 0;
		if (play_time != 0 || curpID == dieeID){
			return;
		}
		else if (targetC == 0){
			curpID = idleID;
			return;
		}
		else if (curpID == idleID || curpID == damnID){
			if (TRUE){
				posA.push_back(pair<ACTIONid, int>(curpID, 10 + 40 * (curpID == idleID)));
			}
			if (disab < 2 * attnR){
				posA.push_back(pair<ACTIONid, int>(attnID, 10));
			}
			if (disab >= 64){
				posA.push_back(pair<ACTIONid, int>(runnID, 10));
			}
		}
		else if (curpID == attnID){
			if (TRUE){
				posA.push_back(pair<ACTIONid, int>(idleID, 1 + 10 * (disab >= 2 * attnR)));
			}
			if (TRUE){
				posA.push_back(pair<ACTIONid, int>(attnID, 100));
			}
			if (disab >= 64){
				posA.push_back(pair<ACTIONid, int>(runnID, 10));
			}
		}
		else if (curpID == runnID){
			if (TRUE){
				posA.push_back(pair<ACTIONid, int>(idleID, 1 + 1 * (disab >= 2 * attnR)));
			}
			if (disab < 2 * attnR){
				posA.push_back(pair<ACTIONid, int>(attnID, 10));
			}
			if (disab >= 64){
				posA.push_back(pair<ACTIONid, int>(runnID, 100));
			}
		}
		else{
			return;
		}
		for (auto act : posA)total_weight += act.second;
		if (!total_weight){
			FILE*fp = fopen(LOGF, "a");
			fprintf(fp, "%s has no possilbe move! what do you want him to do?", name);
			fflush(fp);
			fclose(fp);
		}
		assert(total_weight);
		int rrr = rand() % total_weight;
		for (auto act : posA){
			if (rrr < act.second){
				curpID = act.first;
				break;
			}
			rrr -= act.second;
		}
	}
};

vector<BattleC*>allBattleC;
HumanCC mainC;
SimpleC donzos[20];
#define FIGHT1P 1
#define FIGHT2P 2 //todo
#define FIGHT32 3 //todo
struct{
	VIEWPORTid vID;                     // the major viewport
	SCENEid sID;                        // the 3D scene
	OBJECTid cID, tID;                  // the main camera and the terrain for terrain following
	ROOMid tRID;
	TEXTid textID;

	int is_end;
	void load(int mode = FIGHT1P, const char*p1 = "Lyubu2", const char*p2 = "Donzo2"){
		ini_actions_name();
		is_end = 0;
		tRID = textID = FAILED_ID;
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
		tRID = scene.CreateRoom(SIMPLE_ROOM, 10);
		FnRoom room;
		room.ID(tRID);
		room.AddObject(tID);

		// load the character

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
		//load 2P
		{
			dot fDi = dot(0, 1, 0);
			dot uDi = dot(0, 0, 1);
			dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 1000 * fDi;
			donzos[2*DN] = SimpleC(p2, sID, tRID, pos, fDi, uDi, 1);
			allBattleC.push_back(&donzos[2*DN]);
		}
		//load 1P
		dot fDi = dot(0, -1, 0);
		dot uDi = dot(0, 0, 1);
		dot pos = dot(3569, -3208, 1000);
		{
			mainC = HumanCC(p1, sID, tRID, pos, fDi, uDi, 0);
			allBattleC.push_back(&mainC);
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
		//update timer
		for (BattleC* pointer : allBattleC){
			BattleC&battleC = *pointer;
			battleC.dead_time += (battleC.life == 0) * skip;
			battleC.wudi_time = max(battleC.wudi_time - skip, 0);
			battleC.play_time = max(battleC.play_time - skip, 0);
		}
		//character generate action
		for (BattleC* pointer : allBattleC){
			BattleC&battleC = *pointer;
			BattleC*targetC = battleC.find_target(ALL(allBattleC));
			ACTIONid oldpID = battleC.curpID;
			battleC.gen_act(targetC);
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
		//AI move
		for (BattleC* pointer : allBattleC)if (pointer->AItype != 0 && (pointer->curpID == pointer->runnID || pointer->curpID == pointer->attnID)){
			BattleC&battleC = *pointer;
			BattleC*targetC = battleC.find_target(ALL(allBattleC));
			dot a_pos, b_pos;
			battleC.GetPosition(&a_pos);
			if (targetC)targetC->GetPosition(&b_pos);
			else {
				dou theta = dou((rand() % 360) / 180.0 * pi);
				b_pos = a_pos + 32 * dot(cos(theta), sin(theta), 0);
			}
			dot dd = b_pos - a_pos;
			dou movedis = (pointer->curpID == pointer->runnID) * min(battleC.speed, my_dis(dd)/2 - 15.0f);
			battleC.SetDirection(&dd,NULL);
			battleC.MoveForward(movedis, TRUE, FALSE, 0.0f, TRUE);
		}
		// play character pose
		for (BattleC* pointer : allBattleC){
			BattleC& battleC = *pointer;
			battleC.Play(battleC.curpID == battleC.dieeID || battleC.curpID == battleC.damnID ? ONCE : LOOP, (float)skip, FALSE, TRUE);
		}
		//attack judge
		for (BattleC* pointer : allBattleC){
			BattleC&battleC = *pointer;
			BOOL4 is_atking = battleC.curpID == battleC.attnID;
			if (is_atking == 0)continue;
			for (BattleC* btC : allBattleC){
				battleC.Nattack(*btC);
			}
		}
		//judge end
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
		BOOL4 mainC_can_move = mainC.curpID != mainC.dieeID && mainC.curpID != mainC.damnID && mainC.curpID != mainC.attnID;
		BOOL4 mainC_can_turn = mainC.curpID != mainC.dieeID && mainC.curpID != mainC.damnID;
		FnCharacter&actor = mainC;
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
			if (mainC_can_turn && (dot < 0 || z < -e || z > e)){
				set_act_dir(cam_fDir, act_fDir);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			if (mainC_can_move)actor.MoveForward(6.0f, TRUE, FALSE, 0.0f, TRUE);
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
			if (mainC_can_turn && (dot > 0 || z < -e || z > e)){
				set_act_dir(cam_fDir, act_fDir, DOWN);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			if (mainC_can_move)actor.MoveForward(6.0f, TRUE, FALSE, 0.0f, TRUE);
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
			if (mainC_can_turn && (z > 0 || dot < -e || dot > e)) {
				set_act_dir(cam_fDir, act_fDir, RIGHT);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			actor.GetPosition(act_pos);
			camera.GetPosition(cam_pos);
			if (mainC_can_turn)actor.TurnRight(THETA);
			if (mainC_can_move){
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
			if (mainC_can_turn && (z < 0 || dot < -e || dot > e)) {
				set_act_dir(cam_fDir, act_fDir, LEFT);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			actor.GetPosition(act_pos);
			camera.GetPosition(cam_pos);
			if (mainC_can_turn)actor.TurnRight(-THETA);
			if (mainC_can_move){
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

		if ( !(FyCheckHotKeyStatus(FY_D) || FyCheckHotKeyStatus(FY_RIGHT) || FyCheckHotKeyStatus(FY_A) || FyCheckHotKeyStatus(FY_LEFT) ) ) {
			if (FyCheckHotKeyStatus(FY_W) || FyCheckHotKeyStatus(FY_UP) || FyCheckHotKeyStatus(FY_S) || FyCheckHotKeyStatus(FY_DOWN) ) {
				if (mainC_can_move)actor.MoveForward(6.0f, TRUE, FALSE, 0.0f, TRUE);
				camera.GetPosition(cam_pos);
				actor.GetPosition(act_pos);
				
				// if too far, camera foward
				if (distance(cam_pos, act_pos) > dis) {
					set_cam_pos(cam_pos, act_pos, cam_fDir);
					camera.SetPosition(cam_pos);
				}
	
				// if too close, camera backward
				else if (distance(cam_pos, act_pos) < dis) {
					set_cam_pos(cam_pos, act_pos, cam_fDir);
					camera.SetPosition(cam_pos);
				}
			}
		}


		// ***** Double hot key pressed *****
		// assume no more key pressed at the same time
		// up and right
		if ((FyCheckHotKeyStatus(FY_W) || FyCheckHotKeyStatus(FY_UP)) &&
			(FyCheckHotKeyStatus(FY_D) || FyCheckHotKeyStatus(FY_RIGHT))) {
			mix_dir(cam_fDir, act_fDir, UP_RIGHT);
			if (mainC_can_turn)actor.SetDirection(act_fDir, NULL);
		}
		// up and left
		else if ((FyCheckHotKeyStatus(FY_W) || FyCheckHotKeyStatus(FY_UP)) &&
			(FyCheckHotKeyStatus(FY_A) || FyCheckHotKeyStatus(FY_LEFT))) {
			mix_dir(cam_fDir, act_fDir, UP_LEFT);
			if (mainC_can_turn)actor.SetDirection(act_fDir, NULL);
		}
		// down and right
		else if ((FyCheckHotKeyStatus(FY_S) || FyCheckHotKeyStatus(FY_DOWN)) &&
			(FyCheckHotKeyStatus(FY_D) || FyCheckHotKeyStatus(FY_RIGHT))) {
			mix_dir(cam_fDir, act_fDir, DOWN_RIGHT);
			if (mainC_can_turn)actor.SetDirection(act_fDir, NULL);
		}
		// down and left
		else if ((FyCheckHotKeyStatus(FY_S) || FyCheckHotKeyStatus(FY_DOWN)) &&
			(FyCheckHotKeyStatus(FY_A) || FyCheckHotKeyStatus(FY_LEFT))) {
			mix_dir(cam_fDir, act_fDir, DOWN_LEFT);
			if (mainC_can_turn)actor.SetDirection(act_fDir, NULL);
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
		int is_moving_code =
			code == FY_UP || code == FY_W ||
			code == FY_RIGHT || code == FY_D ||
			code == FY_LEFT || code == FY_A ||
			code == FY_DOWN || code == FY_S;
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
					mainC.SetCurrentAction(0, NULL, mainC.curpID = (mainC.want_to_move() ? mainC.runnID : mainC.idleID), 5.0f);
					mainC.Play(START, 0.0f, FALSE, TRUE);
				}
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