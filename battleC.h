#define _battleC_h_
#define TYPESOFC 6
#define BCLOGF ".\\battleClog.txt"
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
	cas({ 300, +6, 30, 20, 140, 75 }),
	cas({ 200, 12, 20, 20, 140, 25 }),
	cas({ +30, +9, 10, 20, 140, 25 }),
	cas({ +75, +9, 15, 20, 140, 25 }),
	cas({ +50, 12, 10, 20, 140, 25 }),
	cas({ 123, +6, 20, 20, 140, 25 })
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
	BattleC(const char* name_, SCENEid sID, ROOMid tRID, dot pos, dot fDi, dot uDi, int group_, int AItype_ = 1){
		strcpy(name, name_);
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
	virtual void gen_act(vector<BattleC*>&allBattleC){}
	virtual void gen_mov(vector<BattleC*>&allBattleC, OBJECTid tID){}
};