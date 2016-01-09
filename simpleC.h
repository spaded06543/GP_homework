#ifndef _battleC_h_
#include"battleC.h"
#endif
typedef pair<ACTIONid, int> PA;
struct SimpleC : public BattleC {
	SimpleC() {}
	SimpleC(const char* name_, SCENEid sID, ROOMid tRID, dot pos, dot fDi, dot uDi, int group_) : BattleC(name_, sID, tRID, pos, fDi, uDi, group_, 1){}
	void gen_act(vector<BattleC*>&allBattleC){
		vector<PA>posA(0);//possilbe act, weight possibility
		int total_weight = 0;
		BattleC*targetC = find_target(ALL(allBattleC));
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
				posA.push_back(PA(curpID, 10 + 40 * (curpID == idleID)));
			}
			if (disab < 2 * attnR){
				posA.push_back(PA(attnID, 10));
			}
			if (disab >= 64){
				posA.push_back(PA(runnID, 10));
			}
			if (disab < 2 * attnR && ctype != 2){
				posA.push_back(PA(defeID, 10));
			}
		}
		else if (curpID == attnID){
			if (TRUE){
				posA.push_back(PA(idleID, 1 + 10 * (disab >= 2 * attnR)));
			}
			if (TRUE){
				posA.push_back(PA(attnID, 100));
			}
			if (disab >= 64){
				posA.push_back(PA(runnID, 10));
			}
			if (disab < 2 * attnR && ctype != 2){
				posA.push_back(PA(defeID, 10));
			}
		}
		else if (curpID == runnID){
			if (TRUE){
				posA.push_back(PA(idleID, 1 + 1 * (disab >= 2 * attnR)));
			}
			if (disab < 2 * attnR){
				posA.push_back(PA(attnID, 10));
			}
			if (disab >= 64){
				posA.push_back(PA(runnID, 100));
			}
			if (disab < 2 * attnR && ctype != 2){
				posA.push_back(PA(defeID, 10));
			}
		}
		else if (curpID == defeID){
			if (TRUE){
				posA.push_back(PA(idleID, 1));
			}
			if (disab < 2 * attnR){
				posA.push_back(PA(attnID, 10));
			}
			if (disab >= 64){
				posA.push_back(PA(runnID, 10));
			}
			if (TRUE && defe_time < 60){
				posA.push_back(PA(defeID, 100));
			}
		}
		else{
			return;
		}
		for (auto act : posA)total_weight += act.second;
		if (!total_weight){
			FILE*fp = fopen(BCLOGF, "a");
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
	void gen_mov(vector<BattleC*>&allBattleC, OBJECTid tID){
		if (curpID != runnID && curpID != attnID)return;
		dot a_pos, b_pos;
		GetPosition(&a_pos);
		BattleC*targetC = find_target(ALL(allBattleC));
		if (targetC)(*targetC).GetPosition(&b_pos);
		else {
			GetDirection(&b_pos,NULL);
			dou theta = dou(atan2D(b_pos) + ((rand() % 8) - 4) / 180.0 * pi);
			b_pos = a_pos + 2 * speed * dot(cos(theta), sin(theta), 0);
		}
		dot dd = b_pos - a_pos;
		dou movedis = (curpID == runnID) * min(speed, my_dis(dd) / 2 - 15.0f);
		SetDirection(&dd, NULL);
		MoveForward(movedis, TRUE, FALSE, 0.0f, TRUE);
	}
};