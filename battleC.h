#include<unordered_map>
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
	"Idle",
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
char sAimm[TYPESOFC][64] = {
	"CombatIdle",
	"CombatIdle",
	"CombatIdle",
	"CombatIdle",
	"CombatIdle",
	"CombatIdle"
};
char sDefe[TYPESOFC][64] = {
	"Defence",
	"guard",
	"Defence",
	"Defence",
	"Defence",
	"Defence"
};
struct cas{
	int life;
	dou speed;
	int attnD, attnPT;
	dou attnR, attnT;
};
cas CASL[TYPESOFC] = {
	cas({ 300, +6, 30, 20, 120, 50 }),
	cas({ 200, 12, 20, 20, 140, 25 }),
	cas({ +30, +9, 10, 20, 120, 25 }),
	cas({ +75, +9, 15, 20, +70, 50 }),
	cas({ +50, 12, 10, 20, +70, 25 }),
	cas({ 123, +6, 20, 20, 120, 25 })
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
	int defe_time, wudi_time, play_time;
	char name[20];
	int ctype;
	CHARACTERid aID;
	ACTIONid attnID, idleID, runnID, damnID, dieeID, curpID, aimmID, defeID;  // those actions
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
		int ctype = my_cid[name];
		FySetModelPath(sPath[ctype]);
		FySetTexturePath(sPath[ctype]);
		FySetCharacterPath(sPath[ctype]);
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
		attnID = GetBodyAction(NULL, sAttN[ctype]);
		idleID = GetBodyAction(NULL, sIdle[ctype]);
		runnID = GetBodyAction(NULL, sRunn[ctype]);
		damnID = GetBodyAction(NULL, sDamN[ctype]);
		dieeID = GetBodyAction(NULL, sDiee[ctype]);
		aimmID = GetBodyAction(NULL, sAimm[ctype]);
		defeID = GetBodyAction(NULL, sDefe[ctype]);
		curpID = idleID;
		SetCurrentAction(NULL, 0, idleID);
		Play(START, 0.0f, FALSE, TRUE);
		life = CASL[ctype].life;
		speed = CASL[ctype].speed;
		attnD = CASL[ctype].attnD;
		attnPT = CASL[ctype].attnPT;
		attnR = CASL[ctype].attnR;
		attnT = CASL[ctype].attnT;
		defe_time = wudi_time = play_time = 0;
		group = group_;
		AItype = AItype_;
	}
	dot Cdvc(BattleC&C2){
		dot a_pos, b_pos;
		GetPosition(&a_pos, NULL);
		C2.GetPosition(&b_pos, NULL);
		return b_pos - a_pos;
	}
	dou Cdis(BattleC&C2){
		return my_dis(Cdvc(C2));
	}
	void Nattack(BattleC&beatenC){
		if (beatenC.life == 0 || beatenC.wudi_time != 0 || beatenC.group == group)return;
		dot a_pos, b_pos, a_fDi;
		GetPosition(&a_pos, NULL);
		GetDirection(&a_fDi, NULL);
		a_fDi = dan(a_fDi);
		beatenC.GetPosition(&b_pos, NULL);
		dot dd = b_pos - a_pos;
		dou disab = my_dis(dd);
		if (sgn(disab) == 0)return;//too close
		dot dandd = dan(dd);
		dou sint = my_dis(a_fDi*dandd);
		BOOL4 is_beaten = a_fDi%dandd >= 0 && disab < attnR && (asin(fabs(sint)) * 180 / pi) < attnT;
		if (is_beaten == 0)return;
		BOOL4 is_defenc = 0;
		if (beatenC.curpID == beatenC.defeID && ctype != 2){
			dot b_fDi;
			beatenC.GetDirection(&b_fDi, NULL);
			is_defenc = (b_fDi%dandd < 0);
		}
		if (is_defenc){
			beatenC.life = max(beatenC.life - min(attnD / 10, beatenC.life - 1), 0);
			beatenC.SetPosition(&(b_pos + 5 * dandd));
		}
		else{
			beatenC.life = max(beatenC.life - attnD, 0);
			beatenC.SetDirection(&((-1)*dandd), NULL);
			beatenC.SetPosition(&(b_pos + 5 * dandd));
			beatenC.SetCurrentAction(NULL, 0, beatenC.curpID = (beatenC.life <= 0 ? beatenC.dieeID : beatenC.damnID));
			beatenC.Play(ONCE, 0.0f, FALSE, TRUE);
			beatenC.play_time = 15;
		}
		if (beatenC.life > 0)beatenC.wudi_time = 34;
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
	template<class T>
	BattleC* find_face_target(T st, T ed){
		BattleC*ans = NULL; dou warc;
		dot fDi;
		GetDirection(&fDi,NULL);
		for (; st != ed; st++)if ((*st)->life){
			dot dvc = Cdvc(*(*st));
			if (my_dis(dvc) < eps || my_dis(dvc) >= attnR || fDi % dvc < 0)continue;
			dou sint = my_dis(fDi*dan(dvc));
			dou narc = fabs(asin(sint));
			if (narc >= attnT)continue;
			if (ans == NULL|| warc>narc){
				ans = *st;
				warc = narc;
			}
		}
		return ans;
	}
	virtual void gen_act(vector<BattleC*>&allBattleC){}
	virtual void gen_mov(vector<BattleC*>&allBattleC, OBJECTid tID){}
};