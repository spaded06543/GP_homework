/*==============================================================
  character movement testing using Fly2

  - Load a scene
  - Generate a terrain object
  - Load a character
  - Control a character to move
  - Change poses

  (C)2012-2015 Chuan-Chang Wang, All Rights Reserved
  Created : 0802, 2012

  Last Updated : 1004, 2015, Kevin C. Wang
 ===============================================================*/
#include "../Include/FlyWin32.h"
#include "DefFunction.h"
#include "template_3D.h"
#include<vector>
#include<map>
#include<algorithm>
#include <assert.h>

VIEWPORTid vID;                     // the major viewport
SCENEid sID;                        // the 3D scene
OBJECTid cID, tID;                  // the main camera and the terrain for terrain following
CHARACTERid actorID;                // the major character
ACTIONid idleID, runID, curPoseID, natkID;  // three actions

ROOMid terrainRoomID = FAILED_ID;
TEXTid textID = FAILED_ID;

string cname[3] = {
	"Donzo2",
	"Lyubu2",
	"Robber02"
};
char sIdle[3][20] = {
	"Idle",
	"Idle",
	"CombatIdle"
};
map<string, char*>aIdle = {
	{ cname[0], sIdle[0] },
	{ cname[1], sIdle[1] },
	{ cname[2], sIdle[2] }
};
char sDamN[3][20] = {
	"DamageL",
	"LeftDamaged",
	"Damage1"
};
map<string, char*>aDamN = {
	{ cname[0], sDamN[0] },
	{ cname[1], sDamN[1] },
	{ cname[2], sDamN[2] }
};
char sDiee[3][20] = {
	"Die",
	"Die",
	"Dead"
};
map<string, char*>aDiee = {
	{ cname[0], sDiee[0] },
	{ cname[1], sDiee[1] },
	{ cname[2], sDiee[2] }
};
typedef struct BattleC : public FnCharacter {
	int life;
	int dead_time, wudi_time;
	char name[20];
	BattleC(){}
	BattleC(char* name_, SCENEid sID, ROOMid terrainRoomID, dot pos, dot fDi, dot uDi, int life_){
		{//strcopy
			int tl = 0;
			for (; name_[tl]; tl++)name[tl] = name_[tl];
			name[tl] = 0;
		}
		FnScene scene; scene.ID(sID);
		FnObject terrain; terrain.ID(terrainRoomID);
		CHARACTERid ID = scene.LoadCharacter(name);
		(*this).ID(ID);
		(*this).SetDirection(fDi.c_array(), uDi.c_array());
		(*this).SetTerrainRoom(terrainRoomID, 10.0f);
		BOOL4 beOK = (*this).PutOnTerrain(pos.c_array());
		if (beOK == 0){
			fprintf(stderr,"load %s failed!",name);
		}
		assert(beOK);
		(*this).SetCurrentAction(NULL, 0, (*this).GetBodyAction(NULL, aIdle[name]));
		(*this).Play(START, 0.0f, FALSE, TRUE);
		life = life_;
		dead_time = wudi_time = 0;
	}
}BattleC;
vector<BattleC*>allBattleC;
BattleC donzos[20], robbrs[20];


// some globals
int frame = 0;
int oldX, oldY, oldXM, oldYM, oldXMM, oldYMM;

// hotkey callbacks
void QuitGame ( BYTE, BOOL4 );
void Movement ( BYTE, BOOL4 );

// timer callbacks
void GameAI ( int );
void RenderIt ( int );

// mouse callbacks
void InitPivot ( int, int );
void PivotCam ( int, int );
void InitMove ( int, int );
void MoveCam ( int, int );
void InitZoom ( int, int );
void ZoomCam ( int, int );


/*------------------
  the main program
  C.Wang 1010, 2014
 -------------------*/
void FyMain ( int argc, char **argv )
{
  // create a new world
  BOOL4 beOK = FyStartFlyWin32 ( "NTU@2014 Homework #01 - Use Fly2", 0, 0, 1024, 768, FALSE );

  // setup the data searching paths
  FySetShaderPath   ( "Data\\NTU6\\Shaders" );
  FySetModelPath    ( "Data\\NTU6\\Scenes" );
  FySetTexturePath  ( "Data\\NTU6\\Scenes\\Textures" );
  FySetScenePath    ( "Data\\NTU6\\Scenes" );

  // create a viewport
  vID = FyCreateViewport ( 0, 0, 1024, 768 );
  FnViewport vp;
  vp.ID ( vID );

  // create a 3D scene
  sID = FyCreateScene ( 10 );
  FnScene scene;
  scene.ID ( sID );

  // load the scene
  scene.Load ( "gameScene02" );
  scene.SetAmbientLights ( 1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.6f );

  // load the terrain
  tID = scene.CreateObject ( OBJECT );
  FnObject terrain;
  terrain.ID ( tID );
  BOOL beOK1 = terrain.Load ( "terrain" );
  terrain.Show ( FALSE );

  // set terrain environment
  terrainRoomID = scene.CreateRoom ( SIMPLE_ROOM, 10 );
  FnRoom room;
  room.ID ( terrainRoomID );
  room.AddObject ( tID );

  // load the character
  FySetModelPath ( "Data\\NTU6\\Characters" );
  FySetTexturePath ( "Data\\NTU6\\Characters" );
  FySetCharacterPath ( "Data\\NTU6\\Characters" );
  actorID = scene.LoadCharacter("Lyubu2");

  // put the character on terrain
  float pos[3], fDir[3], uDir[3];
  pos[0] = 3569.0f; pos[1] = -3208.0f; pos[2] = 1000.0f;
  fDir[0] = 0.0f; fDir[1] = -1.0f; fDir[2] = 0.0f;
  uDir[0] = 0.0f; uDir[1] = 0.0f; uDir[2] = 1.0f;
  {
	  FnCharacter actor;
	  actor.ID(actorID);
	  actor.SetDirection(fDir, uDir);

	  actor.SetTerrainRoom(terrainRoomID, 10.0f);
	  beOK = actor.PutOnTerrain(pos);

	  // Get two character actions pre-defined at Lyubu2
	  idleID = actor.GetBodyAction(NULL, "Idle");
	  runID = actor.GetBodyAction(NULL, "Run");
	  natkID = actor.GetBodyAction(NULL, "NormalAttack1");

	  // set the character to idle action
	  curPoseID = idleID;
	  actor.SetCurrentAction(NULL, 0, curPoseID);
	  actor.Play(START, 0.0f, FALSE, TRUE);
  }
  //load donzo
  for (int i = 0; i < 8; i++){
	  dot fDi = dot(cos(2 * pi*i / 8), sin(2 * pi*i / 8), 0);
	  dot uDi = dot(0, 0, 1);
	  dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 150 * fDi;
	  donzos[i]=BattleC("Donzo2", sID, terrainRoomID, pos, fDi, uDi, 200);
	  allBattleC.push_back(&donzos[i]);
  }
  //load robber
  for (int i = 0; i < 16; i++){
	  dot fDi = dot(cos(2 * pi*i / 16), sin(2 * pi*i / 16), 0);
	  dot uDi = dot(0, 0, 1);
	  dot pos = dot(3569.0f, -3208.0f, 1000.0f) - 250 * fDi;
	  robbrs[i] = BattleC("Robber02", sID, terrainRoomID, pos, fDi, uDi, 80);
	  allBattleC.push_back(&robbrs[i]);
  }

  // translate the camera
  cID = scene.CreateObject ( CAMERA );
  FnCamera camera;
  camera.ID ( cID );
  camera.SetNearPlane ( 5.0f );
  camera.SetFarPlane ( 100000.0f );

  // set camera initial position and orientation
  set_cam_dir ( fDir, uDir );
  set_cam_pos ( pos, pos, uDir );
  camera.SetPosition ( pos );
  camera.SetDirection ( fDir, uDir );

  // lighting
  float mainLightPos[3] = { -4579.0f, -714.0f, 15530.0f };
  float mainLightFDir[3] = { 0.276f, 0.0f, -0.961f };
  float mainLightUDir[3] = { 0.961f, 0.026f, 0.276f };

  FnLight lgt;
  lgt.ID ( scene.CreateObject ( LIGHT ) );
  lgt.Translate ( mainLightPos[0], mainLightPos[1], mainLightPos[2], REPLACE );
  lgt.SetDirection ( mainLightFDir, mainLightUDir );
  lgt.SetLightType ( PARALLEL_LIGHT );
  lgt.SetColor ( 1.0f, 1.0f, 1.0f );
  lgt.SetName ( "MainLight" );
  lgt.SetIntensity ( 0.4f );

  // create a text object for displaying messages on screen
  textID = FyCreateText ( "Trebuchet MS", 18, FALSE, FALSE );

  // set Hotkeys
  FyDefineHotKey ( FY_ESCAPE, QuitGame, FALSE );  // escape for quiting the game
  FyDefineHotKey ( FY_UP, Movement, FALSE );      // Up for moving forward
  FyDefineHotKey ( FY_RIGHT, Movement, FALSE );   // Right for turning right
  FyDefineHotKey ( FY_LEFT, Movement, FALSE );    // Left for turning left
  FyDefineHotKey ( FY_DOWN, Movement, FALSE );    // Down for moving backward
  FyDefineHotKey ( FY_W, Movement, FALSE );       // same as Up
  FyDefineHotKey ( FY_D, Movement, FALSE );	      // same as Right
  FyDefineHotKey ( FY_A, Movement, FALSE );	      // same as Left
  FyDefineHotKey ( FY_S, Movement, FALSE );	      // same as Down
  FyDefineHotKey ( FY_Z, Movement, FALSE );	      // normal attack

  // define some mouse functions
  FyBindMouseFunction ( LEFT_MOUSE, InitPivot, PivotCam, NULL, NULL );
  FyBindMouseFunction ( MIDDLE_MOUSE, InitZoom, ZoomCam, NULL, NULL );
  FyBindMouseFunction ( RIGHT_MOUSE, InitMove, MoveCam, NULL, NULL );

  // bind timers, frame rate = 30 fps
  FyBindTimer ( 0, FPS, GameAI, TRUE );
  FyBindTimer ( 1, FPS, RenderIt, TRUE );

  // invoke the system
  FyInvokeFly ( TRUE );
}

/*-------------------------------------------------------------
  30fps timer callback in fixed frame rate for major game loop
 --------------------------------------------------------------*/
void GameAI ( int skip )
{
  // play character pose
  FnCharacter actor;
  actor.ID ( actorID );
  actor.Play ( LOOP, (float) skip, FALSE, TRUE );

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
  // get camera object
  FnCamera camera; camera.ID ( cID );
  // get terrain object
  FnObject terrain; terrain.ID ( tID );
  // camera & actor position
  float cam_pos[3], act_pos[3];
  // get camera & actor direction
  float cam_fDir[3], cam_uDir[3], act_fDir[3];
  camera.GetDirection ( cam_fDir, cam_uDir );
  actor.GetDirection ( act_fDir, NULL );

  int is_atking = curPoseID == natkID;

  if (is_atking){
	  dot a_pos, fDi;
	  actor.GetPosition(a_pos.c_array(), NULL);
	  actor.GetDirection(fDi.c_array(), NULL);
	  for (BattleC* pointer : allBattleC)if (pointer->life != 0 && pointer->wudi_time == 0){
		  BattleC& beatenC = *pointer;
		  dot b_pos;
		  beatenC.GetPosition(b_pos.c_array(), NULL);
		  dot dd = b_pos - a_pos;
		  dot dandd = dan(dd);
		  dou sint = my_dis(dan(fDi)*dandd);
		  BOOL4 isbeaten = my_dis(dd) < 140 && fDi%dandd >= 0 && asin(fabs(sint)) <= pi*25.0 / 180;
		  if (isbeaten == 0)continue;

		  beatenC.life = max(beatenC.life - 10, 0);
		  if (sgn(dd%dd)){
			  beatenC.SetDirection(((-1)*dandd).c_array(), NULL);
			  beatenC.SetPosition((b_pos + 5 * dandd).c_array());
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

  // dis-accuracy prevent
  const float e = EPSILON;

  // z value by cam.dir x act.dir
  float z   = cam_fDir[0] * act_fDir[1] - cam_fDir[1] * act_fDir[0],
        dot = cam_fDir[0] * act_fDir[0] + cam_fDir[1] * act_fDir[1];

  // ***** Up *****
  if ( FyCheckHotKeyStatus ( FY_W ) || FyCheckHotKeyStatus ( FY_UP ) ) {

    // if not facing up, face to up
    if ( dot < 0 || z < -e || z > e ) {
      set_act_dir ( cam_fDir, act_fDir );
      actor.SetDirection ( act_fDir, NULL );
    }

    // actor move
	if (is_atking==0)actor.MoveForward(12.0f, TRUE, FALSE, 0.0f, TRUE);
    camera.GetPosition ( cam_pos );
    actor.GetPosition ( act_pos );

    // if too far, camera foward
    if ( distance ( cam_pos, act_pos ) > dis ) {
      set_cam_pos ( cam_pos, act_pos, cam_fDir );
      camera.SetPosition ( cam_pos );
    }
  }

  // ***** Down *****
  if ( FyCheckHotKeyStatus ( FY_S ) || FyCheckHotKeyStatus ( FY_DOWN ) ) {

    // if not facing down, face to down
    if ( dot > 0 || z < -e || z > e ) {
      set_act_dir ( cam_fDir, act_fDir, DOWN );
      actor.SetDirection ( act_fDir, NULL );
    }

    // actor move
	if (is_atking == 0)actor.MoveForward(12.0f, TRUE, FALSE, 0.0f, TRUE);
    camera.GetPosition ( cam_pos );
    actor.GetPosition ( act_pos );

    // if too close, camera backward
    if ( distance ( cam_pos, act_pos ) < dis ) {
      set_cam_pos ( cam_pos, act_pos, cam_fDir );
      camera.SetPosition ( cam_pos );
    }
  }

  // ***** Right *****
  if ( FyCheckHotKeyStatus ( FY_D ) || FyCheckHotKeyStatus ( FY_RIGHT ) ) {
    // if not facing right, face to right
    if ( z > 0 || dot < -e || dot > e ) {
      set_act_dir ( cam_fDir, act_fDir, RIGHT );
      actor.SetDirection ( act_fDir, NULL );
    }

    // actor move
    actor.GetPosition ( act_pos );
    camera.GetPosition ( cam_pos );
    actor.TurnRight ( THETA );
	if (is_atking == 0){
		if (actor.MoveForward(2 * DISTANCE_P * sin(degree2rad(THETA)), TRUE, FALSE, 0.0f, TRUE) == BOUNDARY) {
			float Dir[3] = { 0.0f, 0.0f, 1.0f };
			camera.SetDirection(NULL, Dir);
			camera.TurnRight(-90.0f + THETA);
			camera.MoveForward(12.0f, TRUE, FALSE, 0.0f, TRUE);
		}
	}

    // camera facing actor
    actor.GetPosition ( act_pos );
    camera.GetPosition ( cam_pos );
    cam_fDir[0] = act_pos[0] - cam_pos[0];
    cam_fDir[1] = act_pos[1] - cam_pos[1];
    set_cam_dir ( cam_fDir, cam_uDir );
    camera.SetDirection ( cam_fDir, cam_uDir );
  }

  // ***** Left *****
  if ( FyCheckHotKeyStatus ( FY_A ) || FyCheckHotKeyStatus ( FY_LEFT ) ) {
    // if not facing left, face to right
    if ( z < 0 || dot < -e || dot > e ) {
      set_act_dir ( cam_fDir, act_fDir, LEFT );
      actor.SetDirection ( act_fDir, NULL );
    }

    // actor move
    actor.GetPosition ( act_pos );
    camera.GetPosition ( cam_pos );
    actor.TurnRight ( -THETA );
	if (is_atking == 0){
		if (actor.MoveForward(2 * DISTANCE_P * sin(degree2rad(THETA)), TRUE, FALSE, 0.0f, TRUE) == BOUNDARY) {
			float Dir[3] = { 0.0f, 0.0f, 1.0f };
			camera.SetDirection(NULL, Dir);
			camera.TurnRight(90.0f - THETA);
			camera.MoveForward(12.0f, TRUE, FALSE, 0.0f, TRUE);
		}
	}

    // camera facing actor
    actor.GetPosition ( act_pos );
    camera.GetPosition ( cam_pos );
    cam_fDir[0] = act_pos[0] - cam_pos[0];
    cam_fDir[1] = act_pos[1] - cam_pos[1];
    set_cam_dir ( cam_fDir, cam_uDir );
    camera.SetDirection ( cam_fDir, cam_uDir );
  }


  // ***** Double hot key pressed *****
  // assume no more key pressed at the same time
  // up and right
  if ( ( FyCheckHotKeyStatus ( FY_W ) || FyCheckHotKeyStatus ( FY_UP )  ) &&
       ( FyCheckHotKeyStatus ( FY_D ) || FyCheckHotKeyStatus ( FY_RIGHT ) ) ) {
    mix_dir ( cam_fDir, act_fDir, UP_RIGHT );
    actor.SetDirection ( act_fDir, NULL );
  }
  // up and left
  else if ( ( FyCheckHotKeyStatus ( FY_W ) || FyCheckHotKeyStatus ( FY_UP )  ) &&
            ( FyCheckHotKeyStatus ( FY_A ) || FyCheckHotKeyStatus ( FY_LEFT ) ) ) {
    mix_dir ( cam_fDir, act_fDir, UP_LEFT );
    actor.SetDirection ( act_fDir, NULL );
  }
  // down and right
  else if ( ( FyCheckHotKeyStatus ( FY_S ) || FyCheckHotKeyStatus ( FY_DOWN )  ) &&
            ( FyCheckHotKeyStatus ( FY_D ) || FyCheckHotKeyStatus ( FY_RIGHT ) ) ) {
    mix_dir ( cam_fDir, act_fDir, DOWN_RIGHT );
    actor.SetDirection ( act_fDir, NULL );
  }
  // down and left
  else if ( ( FyCheckHotKeyStatus ( FY_S ) || FyCheckHotKeyStatus ( FY_DOWN )  ) &&
            ( FyCheckHotKeyStatus ( FY_A ) || FyCheckHotKeyStatus ( FY_LEFT ) ) ) {
    mix_dir ( cam_fDir, act_fDir, DOWN_LEFT );
    actor.SetDirection ( act_fDir, NULL );
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


/*----------------------
  perform the rendering
  C.Wang 0720, 2006
 -----------------------*/
void RenderIt( int skip )
{
  FnViewport vp;

  // render the whole scene
  vp.ID ( vID );
  vp.Render3D ( cID, TRUE, TRUE );

  // get camera's data
  FnCamera camera;
  camera.ID ( cID );

  float pos[3], fDir[3], uDir[3];
  camera.GetPosition ( pos );
  camera.GetDirection ( fDir, uDir );

  // show frame rate
  static char string[128];
  if ( frame == 0 ) {
    FyTimerReset ( 0 );
  }

  if ( frame/10*10 == frame ) {
    float curTime;

    curTime = FyTimerCheckTime ( 0 );
    sprintf ( string, "Fps: %6.2f", frame/curTime );
  }

  frame += skip;
  if ( frame >= 1000 ) {
    frame = 0;
  }
  FnText text;
  text.ID ( textID );

  text.Begin ( vID );
  text.Write ( string, 20, 20, 255, 0, 0 );

  char posS[256], fDirS[256], uDirS[256];
  sprintf ( posS, "pos: %8.3f %8.3f %8.3f", pos[0], pos[1], pos[2] );
  sprintf ( fDirS, "facing: %8.3f %8.3f %8.3f", fDir[0], fDir[1], fDir[2] );
  sprintf ( uDirS, "up: %8.3f %8.3f %8.3f", uDir[0], uDir[1], uDir[2] );

  text.Write ( posS, 20, 35, 255, 255, 0 );
  text.Write ( fDirS, 20, 50, 255, 255, 0 );
  text.Write ( uDirS, 20, 65, 255, 255, 0 );

  char tts[256];
  for (int i = 0; i < 8; i++){
	  sprintf(tts,"donzos[%d] life = %d",i,donzos[i].life);
	  text.Write(tts, 20, 95+15*i, 255, 255, 0);
  }
  /*
  dot d_pos, l_pos;
  FnCharacter actor;
  actor.ID(actorID);
  actor.GetPosition(l_pos.c_array(), NULL);
  BattleC&donzo = *donzos;
  donzo.GetPosition(d_pos.c_array(), NULL);
  dot dd = d_pos - l_pos;
  dot fD;
  actor.GetDirection(fD.c_array(), NULL);
  char tts[256];
  sprintf(tts, "theta = %.3f",asin(fabs(my_dis(fD*dd) / my_dis(fD) / my_dis(dd))));
  text.Write(tts, 20, 80, 255, 255, 0);
  sprintf(tts, "maxtheta = %.3f", pi*20 / 180);
  text.Write(tts, 20, 95, 255, 255, 0);
  sprintf(tts, "donzolife=%d", donzo.life);
  text.Write(tts, 20, 110, 255, 255, 0);
  sprintf(tts, "dis=%.3f", my_dis(dd));
  text.Write(tts, 20, 125, 255, 255, 0);
  int is_in = my_dis(dd) < 100 && fD%dd >= 0 && asin(fabs(my_dis(fD*dd) / my_dis(fD) / my_dis(dd))) <= pi * 20.0 / 180;
  if (my_dis(dd) < 140)text.Write("dis ok", 20, 140, 255, 255, 0);
  if (fD%dd >= 0      )text.Write("face ok", 20, 155, 255, 255, 0);
  if (asin(fabs(my_dis(fD*dd) / my_dis(fD) / my_dis(dd))) <= pi * 20.0 / 180)text.Write("theta ok", 20, 170, 255, 255, 0);
  if (is_in           )text.Write("attack ok", 20, 185, 255, 255, 0);

  sprintf(tts, "donzo_dead_time=%d", donzo.dead_time);
  text.Write(tts, 300, 50, 255, 255, 0);
  sprintf(tts, "donzo.wudi_time=%d", donzo.wudi_time);
  text.Write(tts, 300, 65, 255, 255, 0);
  */

  text.End ();
  
  // swap buffer
  FySwapBuffers ();
}


/*------------------
  movement control
 -------------------*/
void Movement ( BYTE code, BOOL4 value )
{
	FnCharacter actor;
	actor.ID ( actorID );
	int is_moving_code = 
		code == FY_UP    || code == FY_W ||
		code == FY_RIGHT || code == FY_D ||
		code == FY_LEFT  || code == FY_A ||
		code == FY_DOWN  || code == FY_S;
	int has_moving_key =
		FyCheckHotKeyStatus(FY_UP)    || FyCheckHotKeyStatus(FY_W) ||
		FyCheckHotKeyStatus(FY_RIGHT) || FyCheckHotKeyStatus(FY_D) ||
		FyCheckHotKeyStatus(FY_LEFT)  || FyCheckHotKeyStatus(FY_A) ||
		FyCheckHotKeyStatus(FY_DOWN)  || FyCheckHotKeyStatus(FY_S);
	if ( value ) {
		if (is_moving_code) {
			if ( curPoseID == idleID ) {
				curPoseID = runID;
				actor.SetCurrentAction ( 0, NULL, curPoseID, 5.0f );
				actor.Play ( START, 0.0f, FALSE, TRUE );
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
			if ( curPoseID == runID ) {
				if (has_moving_key==0) {
					curPoseID = idleID;
					actor.SetCurrentAction ( 0, NULL, curPoseID, 5.0f );
					actor.Play ( START, 0.0f, FALSE, TRUE );
				}
			}
		}
		else if (code == FY_Z){
			curPoseID = has_moving_key?runID:idleID;
			actor.SetCurrentAction(0, NULL, curPoseID, 5.0f);
			actor.Play(START, 0.0f, FALSE, TRUE);
		}
	}
}


/*------------------
  quit the demo
  C.Wang 0327, 2005
 -------------------*/
void QuitGame ( BYTE code, BOOL4 value )
{
  if ( code == FY_ESCAPE )
    if ( value )
      FyQuitFlyWin32 ();
}



/*-----------------------------------
  initialize the pivot of the camera
  C.Wang 0329, 2005
 ------------------------------------*/
void InitPivot ( int x, int y )
{
  oldX = x;
  oldY = y;
  frame = 0;
}


/*------------------
  pivot the camera
  C.Wang 0329, 2005
 -------------------*/
void PivotCam ( int x, int y )
{
  FnObject model;

  if ( x != oldX ) {
    model.ID ( cID );
    model.Rotate ( Z_AXIS, (float) (x - oldX), GLOBAL );
    oldX = x;
  }

  if ( y != oldY ) {
    model.ID ( cID );
    model.Rotate ( X_AXIS, (float) (y - oldY), GLOBAL );
    oldY = y;
  }
}


/*----------------------------------
  initialize the move of the camera
  C.Wang 0329, 2005
 -----------------------------------*/
void InitMove ( int x, int y )
{
  oldXM = x;
  oldYM = y;
  frame = 0;
}


/*------------------
  move the camera
  C.Wang 0329, 2005
 -------------------*/
void MoveCam ( int x, int y )
{
  if ( x != oldXM ) {
    FnObject model;

    model.ID ( cID );
    model.Translate ( (float) (x - oldXM)*2.0f, 0.0f, 0.0f, LOCAL );
    oldXM = x;
  }
  if ( y != oldYM ) {
    FnObject model;

    model.ID ( cID );
    model.Translate ( 0.0f, (float) (oldYM - y)*2.0f, 0.0f, LOCAL );
    oldYM = y;
  }
}


/*----------------------------------
  initialize the zoom of the camera
  C.Wang 0329, 2005
 -----------------------------------*/
void InitZoom ( int x, int y )
{
  oldXMM = x;
  oldYMM = y;
  frame = 0;
}


/*------------------
  zoom the camera
  C.Wang 0329, 2005
 -------------------*/
void ZoomCam ( int x, int y )
{
  if ( x != oldXMM || y != oldYMM ) {
    FnObject model;

    model.ID ( cID );
    model.Translate ( 0.0f, 0.0f, (float) (x - oldXMM)*10.0f, LOCAL );
    oldXMM = x;
    oldYMM = y;
  }
}