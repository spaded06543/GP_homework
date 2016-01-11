#ifndef _battleC_h_
#include"battleC.h"
#endif
struct HumanCC : public BattleC {
	OBJECTid cID;
	HumanCC() {}
	HumanCC(const char* name_, SCENEid sID, ROOMid tRID, dot pos, dot fDi, dot uDi, int group_) : BattleC(name_, sID, tRID, pos, fDi, uDi, group_, 0){
		FnScene scene(sID);
		// translate the camera
		cID = scene.CreateObject(CAMERA);

		FnCamera camera(cID);
		camera.SetNearPlane(5.0f);
		camera.SetFarPlane(100000.0f);

		// set camera initial position and orientation
		set_cam_dir(&fDi, &uDi, DEFAULT);
		set_cam_pos(&pos, &pos, &uDi);
		camera.SetPosition(&pos);
		camera.SetDirection(&fDi, &uDi);
	}
	BOOL4 want_to_move(){
		return
			FyCheckHotKeyStatus(FY_UP) ||
			FyCheckHotKeyStatus(FY_RIGHT) ||
			FyCheckHotKeyStatus(FY_LEFT) ||
			FyCheckHotKeyStatus(FY_DOWN);
	}
	ACTIONid want_to_do(){
		if (FyCheckHotKeyStatus(FY_Z))return attnID;
		if (FyCheckHotKeyStatus(FY_X))return aimmID;
		return want_to_move() ? runnID : idleID;
	}
	void gen_act(vector<BattleC*>&allBattleC){
		if (play_time != 0 || curpID == dieeID){
			return;
		}
		else if (curpID == damnID || (curpID == defeID && defe_time >= 60)){
			curpID = want_to_do();
		}
	}
	void gen_mov(vector<BattleC*>&allBattleC, OBJECTid tID){
		BOOL4 can_move = curpID == runnID;
		BOOL4 can_turn = curpID == runnID || curpID == attnID || curpID == aimmID;
		if (curpID == aimmID){
			BattleC*target = find_face_target(ALL(allBattleC));
			if (target != NULL){
				dot dvc = Cdvc(*target);
				SetDirection(&dvc, NULL);
			}
			else{
				can_turn = 1;
			}
		}
		FnCharacter&actor = *this;
		// get camera object
		FnCamera camera; camera.ID(cID);
		// get terrain object
		FnObject terrain; terrain.ID(tID);
		// camera & actor position
		float cam_pos[3], act_pos[3];
		// get camera & actor direction
		float cam_fDir[3], cam_uDir[3], act_fDir[3], act_uDir[3] = { 0.0, 0.0, 1.0 };
		camera.GetPosition(cam_pos);
		camera.GetDirection(cam_fDir, cam_uDir);
		actor.GetPosition(act_pos);
		actor.GetDirection(act_fDir, NULL);
		// dis-accuracy prevent
		const float e = EPSILON;

		// z value by cam.dir x act.dir
		float z = cam_fDir[0] * act_fDir[1] - cam_fDir[1] * act_fDir[0],
			dot = cam_fDir[0] * act_fDir[0] + cam_fDir[1] * act_fDir[1];

		int hotkey = (int)FyCheckHotKeyStatus(FY_UP) + (int)FyCheckHotKeyStatus(FY_DOWN)
				   + (int)FyCheckHotKeyStatus(FY_RIGHT) + (int)FyCheckHotKeyStatus(FY_LEFT);
		float Speed = ( hotkey > 1 )? speed/dou(sqrt(2)) : speed;

		// ***** Up *****
		if (FyCheckHotKeyStatus(FY_UP)) {
			camera.GetPosition(cam_pos);
			camera.GetDirection(cam_fDir, cam_uDir);
			actor.GetPosition(act_pos);
			actor.GetDirection(act_fDir, NULL);

			// if not facing up, face to up
			if (can_turn && (dot < 0 || z < -e || z > e)){
				set_act_dir(cam_fDir, act_fDir);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			if (can_move) {
				if ( actor.MoveForward(Speed, TRUE, FALSE, 0.0f, TRUE) != BOUNDARY) {
					if ( distance(cam_pos, act_pos ) > dis ) {
						camera.SetDirection(act_fDir, act_uDir);
						camera.MoveForward(Speed, TRUE, FALSE, 0.0f, TRUE);
					}
				}
			}
		}

		// ***** Down *****
		if (FyCheckHotKeyStatus(FY_DOWN)) {
			camera.GetPosition(cam_pos);
			camera.GetDirection(cam_fDir, cam_uDir);
			actor.GetPosition(act_pos);
			actor.GetDirection(act_fDir, NULL);

			// if not facing down, face to down
			if (can_turn && (dot > 0 || z < -e || z > e)){
				set_act_dir(cam_fDir, act_fDir, DOWN);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			if (can_move) {
				if ( actor.MoveForward(Speed, TRUE, FALSE, 0.0f, TRUE) != BOUNDARY) {
					if ( distance(cam_pos, act_pos ) > dis ) {
						camera.SetDirection(act_fDir, act_uDir);
						camera.MoveForward(Speed, TRUE, FALSE, 0.0f, TRUE);
					}
				}
			}
		}

		// ***** Right *****
		if (FyCheckHotKeyStatus(FY_RIGHT)) {
			camera.GetPosition(cam_pos);
			camera.GetDirection(cam_fDir, cam_uDir);
			actor.GetPosition(act_pos);
			actor.GetDirection(act_fDir, NULL);
			// if not facing right, face to right
			if (can_turn && (z > 0 || dot < -e || dot > e)) {
				set_act_dir(cam_fDir, act_fDir, RIGHT);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			if (can_turn)
				actor.TurnRight(THETA);
			if (can_move){
				if (actor.MoveForward(Speed, TRUE, FALSE, 0.0f, TRUE) == BOUNDARY) {
					// camera move
					float Dir[3] = { 0.0f, 0.0f, 1.0f };
					camera.SetDirection(NULL, Dir);
					camera.TurnRight(-90.0f + THETA);
					camera.MoveForward(Speed, TRUE, FALSE, 0.0f, TRUE);
				}
			}
		}

		// ***** Left *****
		if (FyCheckHotKeyStatus(FY_LEFT)) {
			camera.GetPosition(cam_pos);
			camera.GetDirection(cam_fDir, cam_uDir);
			actor.GetPosition(act_pos);
			actor.GetDirection(act_fDir, NULL);
			// if not facing left, face to right
			if (can_turn && (z < 0 || dot < -e || dot > e)) {
				set_act_dir(cam_fDir, act_fDir, LEFT);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			if (can_turn)
				actor.TurnRight(-THETA);
			if (can_move){
				if (actor.MoveForward(Speed, TRUE, FALSE, 0.0f, TRUE) == BOUNDARY) {
					// cemara move
					float Dir[3] = { 0.0f, 0.0f, 1.0f };
					camera.SetDirection(NULL, Dir);
					camera.TurnRight(90.0f - THETA);
					camera.MoveForward(Speed, TRUE, FALSE, 0.0f, TRUE);
				}
			}
		}

		actor.GetPosition(act_pos);
		camera.GetPosition(cam_pos);
		// decide camera position
		cam_fDir[0] = act_pos[0] - cam_pos[0];
		cam_fDir[1] = act_pos[1] - cam_pos[1];
		float d = sqrt ( cam_fDir[0] * cam_fDir[0] + cam_fDir[1] * cam_fDir[1] );
		cam_fDir[0] /= d, cam_fDir[1] /= d;
		float cur_dis = 0;
		float i = MAX_APPROACH;
		float cur_pos[3] = { act_pos[0], act_pos[1], cam_pos[2] };
		float dir[3] = { 0.0f, 0.0f, -1.0f };

		while ( cur_dis < DISTANCE_P && i >= POS_APPROACH_MIN ) {
			if ( cur_dis + i > DISTANCE_P ) {
				i /= 2;
				continue;
			}

			float pos_try[3] = { cur_pos[0] - cam_fDir[0] * i, cur_pos[1] - cam_fDir[1] * i, cur_pos[2] };

			if ( terrain.HitTest ( pos_try, dir ) <= 0 )
				i /= 2;
			else {
				cur_pos[0] = pos_try[0], cur_pos[1] = pos_try[1];
				cur_dis += i;
			}
		}

		cur_pos[2] = (cur_dis * (DEFAULT_HEIGHT-MAX_HEIGHT) + DISTANCE_P*MAX_HEIGHT) / DISTANCE_P;
		camera.SetPosition(cur_pos);


		// camera facing actor
		cam_fDir[0] = act_pos[0] - cur_pos[0];
		cam_fDir[1] = act_pos[1] - cur_pos[1];
		cam_fDir[2] = act_pos[2] - cur_pos[2]+42.946793f;
		set_cam_dir(cam_fDir, cam_uDir);
		camera.SetDirection(cam_fDir, cam_uDir);


		// ***** Double hot key pressed direction *****
		// assume no more key pressed at the same time
		// up and right
		if (FyCheckHotKeyStatus(FY_UP) && FyCheckHotKeyStatus(FY_RIGHT)) {
			mix_dir(cam_fDir, act_fDir, UP_RIGHT);
			if (can_turn)
				actor.SetDirection(act_fDir, NULL);
		}
		// up and left
		else if ( FyCheckHotKeyStatus(FY_UP) && FyCheckHotKeyStatus(FY_LEFT)) {
			mix_dir(cam_fDir, act_fDir, UP_LEFT);
			if (can_turn)
				actor.SetDirection(act_fDir, NULL);
		}
		// down and right
		else if ( FyCheckHotKeyStatus(FY_DOWN) && FyCheckHotKeyStatus(FY_RIGHT)) {
			mix_dir(cam_fDir, act_fDir, DOWN_RIGHT);
			if (can_turn)
				actor.SetDirection(act_fDir, NULL);
		}
		// down and left
		else if ( FyCheckHotKeyStatus(FY_DOWN) && FyCheckHotKeyStatus(FY_LEFT)) {
			mix_dir(cam_fDir, act_fDir, DOWN_LEFT);
			if (can_turn)
				actor.SetDirection(act_fDir, NULL);
		}
	}
};