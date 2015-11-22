#include<cmath>
using namespace std;
typedef long long int lnt;
typedef float dou;
const dou eps = dou( 1e-8 );
const dou pi  = dou( acos(-1) );
const dou inf = dou( 1e30 );
int sgn(dou k){return (k>-eps)-(k<eps);}
typedef struct dot{
	dou x,y,z;
	dot(){ x = y = z = 0; }
	dot(dou x_, dou y_, dou z_){
		x = x_, y = y_, z = z_;
	}
	float*c_array(){
		return (float*)this;
	}
}dot;
dot operator-(dot a,dot b){return dot( a.x-b.x,a.y-b.y,a.z-b.z );}
dot operator+(dot a,dot b){return dot( a.x+b.x,a.y+b.y,a.z+b.z );}
dot operator*(dou k,dot a){return dot( a.x*k,a.y*k,a.z*k );}
dot operator/(dot a,dou k){return dot( a.x/k,a.y/k,a.z/k );}

dot operator*(dot a,dot b){
	return dot(
		a.y*b.z-a.z*b.y,
		a.z*b.x-a.x*b.z,
		a.x*b.y-a.y*b.x
	);
}
dou operator%(dot a,dot b){return a.x*b.x+a.y*b.y+a.z*b.z;}
dou my_dis(dot a){return sqrt(a % a);}
dot dan(dot a){return a / my_dis(a);}

/////////////////////////////////////////
typedef struct{dot s,e;}line;
typedef struct{dot o;dou r;}cir;

