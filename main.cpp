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
#include<assert.h>
// some globals
int frame = 0;
int oldX, oldY, oldXM, oldYM, oldXMM, oldYMM;
#include "menumode.h"
#include "selectmode.h"
#include "fightmode.h"

OBJECTid cID;
int now_mode = 1;
//1 menu mode
//2 select mode
//3 load to fight mode
//4 fight mode

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
  BOOL4 beOK = FyStartFlyWin32 ( "NTU@2014 Homework #01 - Use Fly2", 0, 0, 800, 600, FALSE );

  MM.load();
  //FM.load();
  SM.load();
  cID = MM.cID;
  // set Hotkeys
  FyDefineHotKey ( FY_ESCAPE, QuitGame, FALSE );  // escape for quiting the game
  FyDefineHotKey ( FY_UP, Movement, FALSE );      // Up for moving forward
  FyDefineHotKey ( FY_RIGHT, Movement, FALSE );   // Right for turning right
  FyDefineHotKey ( FY_LEFT, Movement, FALSE );    // Left for turning left
  FyDefineHotKey ( FY_DOWN, Movement, FALSE );    // Down for moving backward
  FyDefineHotKey ( FY_Z, Movement, FALSE );	      // normal attack
  FyDefineHotKey ( FY_L, Movement, FALSE );	      // something

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
	if (now_mode == 1)MM.GameAI(skip);
	if (now_mode == 2 || now_mode == 3)SM.GameAI(skip);
	if (now_mode == 4)FM.GameAI(skip);
}


/*----------------------
  perform the rendering
  C.Wang 0720, 2006
 -----------------------*/
void RenderIt( int skip )
{
	if (now_mode == 1)MM.RenderIt(skip);
	if (now_mode == 2 || now_mode == 3)SM.RenderIt(skip);
	if (now_mode == 4)FM.RenderIt(skip);
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
	if (now_mode == 2)next_mode = SM.Movement(code, value);
	if (now_mode == 3)return;
	if (now_mode == 4)next_mode = FM.Movement(code, value);
	if (!(1 <= next_mode && next_mode <= 4)){
		fprintf(stderr, "mode %d return wrong mode %d", now_mode, next_mode);
	}
	assert(1 <= next_mode && next_mode <= 4);
	now_mode = next_mode;
	if (now_mode == 1)cID = MM.cID;
	if (now_mode == 2)cID = SM.cID;
	if (now_mode == 3){
		FM.load(FIGHT1P, "Lyubu2", "Donzo2");
		now_mode = 4;
	}
	if (now_mode == 4)cID = FM.cID;
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