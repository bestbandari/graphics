#include <math.h>

#define NX	10
#define NY	10
#define NZ	10

#define MODE_XY		0
#define MODE_YZ		1
#define MODE_ZX		2

#define NUMLNGS		201
#define NUMLATS		105

const float LNGMIN = { -289.6f };
const float LNGMAX = { 289.6f };
const float LATMIN = { -197.5f };
const float LATMAX = { 211.2f };
const float HGTMIN = { 0.0f };
const float HGTMAX = { 2.891f };

struct LngLatHgt
{
	float lng, lat, hgt;
};


const float TEMPMIN = { 0.f };
const float TEMPMAX = { 100.f };

const float XMIN = -1.;
const float XMAX = 1.;
const float YMIN = -1.;
const float YMAX = 1.;
const float ZMIN = -1.;
const float ZMAX = 1.;
const int SMIN = 1;
const int SMAX = 100;
const float TIMESTEPMIN = 0.01;
const float TIMESTEPMAX = 1;

const float RADIMIN = 0.;
const float RADIMAX = sqrt(XMAX*XMAX + YMAX*YMAX + ZMAX*ZMAX);
const float GRADMIN = 0.;
const float GRADMAX = 300;
const float FLOWMIN = 0.;
const float FLOWMAX = sqrt(12.);
const float SCALEMIN = 0.01;
const float SCALEMAX = 0.5;
const float HEIGHTMIN = 1.;
const float HEIGHTMAX = 50.;
const float LIGHTMIN = 1.;
const float LIGHTMAX = 50.;

const float GRAYMIN = { 0.20f };
const float GRAYMAX = { 1.00f };


const int MAX_ITER = 200;
const float TOLERANCE = 0.01;
const float Stream_Del = 0.03;
const int Stream_Num = 5;

const int Rib_ITER = 20;
const int Rib_Size = 20;
const float Rib_Del = 0.01;

const float Blob_Del = 0.1;
const int Blob_ITER = 20;

const float TOR = 0.00001;

const int PERIOD_MS = 10 * 1000;
const float PI = 3.14159265;


struct centers
{
	float xc, yc, zc;       // center location
	float a;                // amplitude
};

struct node
{
	float x, y, z;          // location
	float t;                // temperature
	float rgb[3];			// the assigned color (to be used later)
	float rad;              // radius (to be used later)
	float grad;             // total gradient (to be used later)
};


float Temperature(float x, float y, float z);
float Radius(float center[3], float point[3]);
float AbsGra(struct node Nodes[][NY][NZ], int x, int y, int z);

void InitPlane(struct node ** plane, float minT, float maxT, float minU, float maxU, int numT, int numU, float con, int mode);
void Vector(float x, float y, float z, float *vxp, float *vyp, float *vzp);
float Norm(float vx, float vy, float vz);
void Advect(float *x, float *y, float *z, float TimeStep);

void HsvRgb(float hsv[3], float rgb[3]);

void Hyperbolic(float * x, float * y, float k, int mode);
void Polar(float * x, float * y, float k);
void Cartesian(float * x, float * y, float k);