#define _USE_MATH_DEFINES   // to use cmath headfile
#include <cmath>

#define FPS 30.0f                 // frame per sec
#define EPSILON 0.0001f           // mis-accuraccy

#define DISTANCE_P 1000.0f        // default dis between (projection of cam on xy plane) & act
#define DEFAULT_HEIGHT 140.0f     // default cam height
#define DISTANCE sqrt ( DISTANCE_P*DISTANCE_P + DEFAULT_HEIGHT*DEFAULT_HEIGHT )
#define DEFAULT_DEGREE 4.7f       // default cam look down degree

#define ACT_DEGREEperFRAME 0.25f  // act turned degree per frame
#define CAMUPDEGREE 1.0f          // cam raise angle if hit terrain

// used in function set_act_dir
#define UP    0
#define DOWN  1
#define RIGHT 2
#define LEFT  3

// used in function mix_dir
#define UP_RIGHT    0
#define UP_LEFT     1
#define DOWN_RIGHT  2
#define DOWN_LEFT   3

float dis = DISTANCE;
float height = DEFAULT_HEIGHT;
float THETA = ACT_DEGREEperFRAME;
float PHI = CAMUPDEGREE;
float cam_angle = DEFAULT_DEGREE;
float cam_height = DEFAULT_HEIGHT;


float degree2rad ( float degree )
{
  return degree * 2 * M_PI / 360;
}

float distance ( float *a, float *b )
{
  float x = a[0] - b[0],
        y = a[1] - b[1],
        z = a[2] - b[2];
  return sqrt ( x * x + y * y + z * z );
}

void set_act_dir ( float *cam_fDir, float *act_fDir, int direction = UP )
{
  float d = sqrt ( 1 - cam_fDir[2] * cam_fDir[2] );

  act_fDir[0] = cam_fDir[0] / d;
  act_fDir[1] = cam_fDir[1] / d;
  act_fDir[2] = 0;

  if ( direction == DOWN )
    act_fDir[0] *= -1, act_fDir[1] *= -1;
  else if ( direction == RIGHT ) {
    float f = act_fDir[0];
    act_fDir[0] = act_fDir[1];
    act_fDir[1] = -f;
  }
  else if ( direction == LEFT ) {
    float f = act_fDir[0];
    act_fDir[0] = -act_fDir[1];
    act_fDir[1] = f;
  }
}

void set_cam_dir ( float *fDir, float *uDir )
{
  float d = sqrt ( fDir[0] * fDir[0] + fDir[1] * fDir[1] );
  float Dir[3] = { fDir[0] / d, fDir[1] / d, 0 };

  fDir[2] = -sin ( degree2rad ( cam_angle ) );
  uDir[2] =  cos ( degree2rad ( cam_angle ) );

  float fc = sqrt ( 1 - fDir[2] * fDir[2] ),
        uc = sqrt ( 1 - uDir[2] * uDir[2] );

  uDir[0] = Dir[0] * uc;
  uDir[1] = Dir[1] * uc;

  fDir[0] = Dir[0] * fc;
  fDir[1] = Dir[1] * fc;
}

void set_cam_pos ( float *cam_pos, float *act_pos, float *cam_fDir )
{
  float d = sqrt ( 1 - cam_fDir[2] * cam_fDir[2] );
  cam_pos[0] = act_pos[0] - DISTANCE_P * cam_fDir[0] / d;
  cam_pos[1] = act_pos[1] - DISTANCE_P * cam_fDir[1] / d;
  cam_pos[2] = height;
}

void mix_dir ( float *cam_fDir, float *act_fDir, int dir )
{
  float Dir1[3], Dir2[3];

  if ( dir == UP_RIGHT ) {
    Dir1[0] = cam_fDir[0], Dir1[1] =  cam_fDir[1];
    Dir2[0] = cam_fDir[1], Dir2[1] = -cam_fDir[0];
  }
  else if ( dir == UP_LEFT ) {
    Dir1[0] =  cam_fDir[0], Dir1[1] = cam_fDir[1];
    Dir2[0] = -cam_fDir[1], Dir2[1] = cam_fDir[0];
  }
  else if ( dir == DOWN_RIGHT ) {
    Dir1[0] = -cam_fDir[0], Dir1[1] = -cam_fDir[1];
    Dir2[0] =  cam_fDir[1], Dir2[1] = -cam_fDir[0];
  }
  else {
    Dir1[0] = -cam_fDir[0], Dir1[1] = -cam_fDir[1];
    Dir2[0] = -cam_fDir[1], Dir2[1] =  cam_fDir[0];
  }

  float d = sqrt ( 2 );
  act_fDir[0] = ( Dir1[0] + Dir2[0] ) / d;
  act_fDir[1] = ( Dir1[1] + Dir2[1] ) / d;
  act_fDir[2] = 0;
}