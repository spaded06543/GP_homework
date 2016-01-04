#ifndef _battleC_h_
#include"battleC.h"
#endif
struct HumanCC : public BattleC {
	OBJECTid cID;
	HumanCC() {}
	HumanCC(const char* name_, SCENEid sID, ROOMid tRID, dot pos, dot fDi, dot uDi, int group_) : BattleC(name_, sID, tRID, pos, fDi, uDi, group_, 0){
		FnScene scene;scene.ID(sID);
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
	}
	BOOL4 want_to_move(){
		return
			FyCheckHotKeyStatus(FY_UP) || FyCheckHotKeyStatus(FY_W) ||
			FyCheckHotKeyStatus(FY_RIGHT) || FyCheckHotKeyStatus(FY_D) ||
			FyCheckHotKeyStatus(FY_LEFT) || FyCheckHotKeyStatus(FY_A) ||
			FyCheckHotKeyStatus(FY_DOWN) || FyCheckHotKeyStatus(FY_S);
	}
	void gen_act(vector<BattleC*>&allBattleC){
		if (play_time != 0 || curpID == dieeID){
			return;
		}
		else if (curpID == damnID){
			curpID = want_to_move() ? runnID : idleID;
		}
	}
	void gen_mov(vector<BattleC*>&allBattleC, OBJECTid tID){
		BOOL4 can_move = curpID != dieeID && curpID != damnID && curpID != attnID;
		BOOL4 can_turn = curpID != dieeID && curpID != damnID;
		FnCharacter&actor = *this;
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
			if (can_turn && (dot < 0 || z < -e || z > e)){
				set_act_dir(cam_fDir, act_fDir);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			if (can_move)actor.MoveForward(6.0f, TRUE, FALSE, 0.0f, TRUE);
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
			if (can_turn && (dot > 0 || z < -e || z > e)){
				set_act_dir(cam_fDir, act_fDir, DOWN);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			if (can_move)actor.MoveForward(6.0f, TRUE, FALSE, 0.0f, TRUE);
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
			if (can_turn && (z > 0 || dot < -e || dot > e)) {
				set_act_dir(cam_fDir, act_fDir, RIGHT);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			actor.GetPosition(act_pos);
			camera.GetPosition(cam_pos);
			if (can_turn)actor.TurnRight(THETA);
			if (can_move){
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
			if (can_turn && (z < 0 || dot < -e || dot > e)) {
				set_act_dir(cam_fDir, act_fDir, LEFT);
				actor.SetDirection(act_fDir, NULL);
			}

			// actor move
			actor.GetPosition(act_pos);
			camera.GetPosition(cam_pos);
			if (can_turn)actor.TurnRight(-THETA);
			if (can_move){
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

		if (!(FyCheckHotKeyStatus(FY_D) || FyCheckHotKeyStatus(FY_RIGHT) || FyCheckHotKeyStatus(FY_A) || FyCheckHotKeyStatus(FY_LEFT))) {
			if (FyCheckHotKeyStatus(FY_W) || FyCheckHotKeyStatus(FY_UP) || FyCheckHotKeyStatus(FY_S) || FyCheckHotKeyStatus(FY_DOWN)) {
				if (can_move)actor.MoveForward(6.0f, TRUE, FALSE, 0.0f, TRUE);
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
			if (can_turn)actor.SetDirection(act_fDir, NULL);
		}
		// up and left
		else if ((FyCheckHotKeyStatus(FY_W) || FyCheckHotKeyStatus(FY_UP)) &&
			(FyCheckHotKeyStatus(FY_A) || FyCheckHotKeyStatus(FY_LEFT))) {
			mix_dir(cam_fDir, act_fDir, UP_LEFT);
			if (can_turn)actor.SetDirection(act_fDir, NULL);
		}
		// down and right
		else if ((FyCheckHotKeyStatus(FY_S) || FyCheckHotKeyStatus(FY_DOWN)) &&
			(FyCheckHotKeyStatus(FY_D) || FyCheckHotKeyStatus(FY_RIGHT))) {
			mix_dir(cam_fDir, act_fDir, DOWN_RIGHT);
			if (can_turn)actor.SetDirection(act_fDir, NULL);
		}
		// down and left
		else if ((FyCheckHotKeyStatus(FY_S) || FyCheckHotKeyStatus(FY_DOWN)) &&
			(FyCheckHotKeyStatus(FY_A) || FyCheckHotKeyStatus(FY_LEFT))) {
			mix_dir(cam_fDir, act_fDir, DOWN_LEFT);
			if (can_turn)actor.SetDirection(act_fDir, NULL);
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
};