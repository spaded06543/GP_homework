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
#include "FlyWin32.h"
#include "DefFunction.h"
#include "template_3D.h"
#include<vector>
#include<map>
#include<algorithm>
#include<assert.h>
// some globals
int frame = 0;
int oldX, oldY, oldXM, oldYM, oldXMM, oldYMM;
#include "menumode.h"
#include "optionmode.h"
#include "selectmode.h"
#include "fightmode.h"

OBJECTid cID;
int now_mode = 1, pre_mode = 0;
//1 menu mode
//2 option mode
//3 select mode
//4 load to fight mode
//5 fight mode
//pre mode 0 -- bgm001, 1 -- bgm002

// hotkey callbacks
void QuitGame ( BYTE, BOOL4 );
void Movement ( BYTE, BOOL4 );

// timer callbacks
void GameAI ( int );
void RenderIt ( int );
void PlaySound(int skip);

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
  BOOL4 beOK = FyStartFlyWin32 ( "NTU@2014 Homework #01 - Use Fly2", 0, 0, 800, 600, FALSE );

  ini_actions_name();
  MM.load();
  OM.load();
  SM.load();
  FM.load();
  cID = MM.cID;
  // set Hotkeys
  FyDefineHotKey ( FY_ESCAPE, QuitGame, FALSE );  // escape for quiting the game
  FyDefineHotKey ( FY_UP, Movement, FALSE );      // Up for moving forward
  FyDefineHotKey ( FY_RIGHT, Movement, FALSE );   // Right for turning right
  FyDefineHotKey ( FY_LEFT, Movement, FALSE );    // Left for turning left
  FyDefineHotKey ( FY_DOWN, Movement, FALSE );    // Down for moving backward
  FyDefineHotKey ( FY_C, Movement, FALSE);	      // defence
  FyDefineHotKey ( FY_X, Movement, FALSE );	      // aim
  FyDefineHotKey ( FY_Z, Movement, FALSE );	      // normal attack
  FyDefineHotKey ( FY_L, Movement, FALSE );	      // something

  // define some mouse functions
  FyBindMouseFunction ( LEFT_MOUSE, InitPivot, PivotCam, NULL, NULL );
  FyBindMouseFunction ( MIDDLE_MOUSE, InitZoom, ZoomCam, NULL, NULL );
  FyBindMouseFunction ( RIGHT_MOUSE, InitMove, MoveCam, NULL, NULL );

  // bind timers, frame rate = 30 fps
  FyBindTimer(0, FPS, GameAI, TRUE);
  FyBindTimer(1, FPS, RenderIt, TRUE);
  FyBindTimer(2, FPS, PlaySound, TRUE);
  // invoke the system
  FyInvokeFly ( TRUE );
}
/*-------------------------------------------------------------
Play background music && loop
--------------------------------------------------------------*/
void PlaySound(int skip){
	FnAudio sd;
	sd.ID(OM.BGMid);
	if (now_mode == 5 && pre_mode == 0){
		sd.Stop();
		if (sd.Load("Bgm/bgm002") == FALSE){
			exit(3);
		}
		//sd.SetVolume(OM.volume);
		OM.Movement(FY_RIGHT, TRUE);
		OM.Movement(FY_LEFT, TRUE);
		pre_mode = 1;
	}
	else if (pre_mode == 1 && now_mode == 1){
		sd.Stop();
		if (sd.Load("Bgm/bgm001") == FALSE){
			exit(3);
		}
		//sd.SetVolume(OM.volume);
		OM.Movement(FY_RIGHT, TRUE);
		OM.Movement(FY_LEFT, TRUE);
		pre_mode = 0;
	}
	if (!(sd.IsPlaying())){
		sd.Play(LOOP);
		//sd.SetVolume(OM.volume);
		OM.Movement(FY_RIGHT, TRUE);
		OM.Movement(FY_LEFT, TRUE);
	}
}
/*-------------------------------------------------------------
  30fps timer callback in fixed frame rate for major game loop
 --------------------------------------------------------------*/
void GameAI ( int skip )
{
	if (now_mode == 1)MM.GameAI(skip);
	if (now_mode == 2)OM.GameAI(skip);
	if (now_mode == 3)SM.GameAI(skip);
	if (now_mode == 4)return;
	if (now_mode == 5)FM.GameAI(skip);
}


/*----------------------
  perform the rendering
  C.Wang 0720, 2006
 -----------------------*/
void RenderIt( int skip )
{
	if (now_mode == 1)MM.RenderIt(skip);
	if (now_mode == 2)OM.RenderIt(skip);
	if (now_mode == 3)SM.RenderIt(skip);
	if (now_mode == 4 || now_mode == 5)FM.RenderIt(skip);
	// swap buffer
	FySwapBuffers ();
}


/*------------------
  movement control
 -------------------*/
void Movement ( BYTE code, BOOL4 value )
{
	int next_mode = now_mode;
	if (now_mode == 1)next_mode = MM.Movement(code, value);
	if (now_mode == 2)next_mode = OM.Movement(code, value);
	if (now_mode == 3)next_mode = SM.Movement(code, value);
	if (now_mode == 4)return;
	if (now_mode == 5)next_mode = FM.Movement(code, value);
	if (!(1 <= next_mode && next_mode <= 5)){
		fprintf(stderr, "mode %d return wrong mode %d", now_mode, next_mode);
	}
	assert(1 <= next_mode && next_mode <= 5);
	now_mode = next_mode;
	if (now_mode == 1)cID = MM.cID;
	if (now_mode == 2)cID = OM.cID;
	if (now_mode == 3)cID = SM.cID;
	if (now_mode == 4){
		FM.true_load(SM.fighter_mode, cname[SM.select_character].c_str(), cname[SM.select_enemy].c_str());
		now_mode = 5;
	}
	if (now_mode == 5)cID = FM.cID;
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
	float dx = float(x - oldXM);
	float dy = float(y - oldYM);
	float dd = sqrt(dx*dx + dy*dy);
	if (sgn(dd) == 0)return;
	dx /= dd;
	dy /= dd;
	FnObject model;
	if ( x != oldXM ) {
		model.ID ( cID );
		model.Translate(0.1f*dx, 0.0f, 0.0f, LOCAL);
		oldXM = x;
	}
	if ( y != oldYM ) {
		model.ID ( cID );
		model.Translate(0.0f, 0.1f*dy, 0.0f, LOCAL);
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
    model.Translate ( 0.0f, 0.0f, (float) (x - oldXMM)*1.0f, LOCAL );
    oldXMM = x;
    oldYMM = y;
  }
}