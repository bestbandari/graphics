#include "project.h"


inline float SQR(float x)
{
	return x * x;
}

struct centers Centers[] =
{
	{ 1.00f, 0.00f, 0.00f, 90.00f },
	{ -1.00f, 0.30f, 0.00f, 120.00f },
	{ 0.00f, 1.00f, 0.00f, 120.00f },
	{ 0.00f, 0.40f, 1.00f, 170.00f },
};

float Temperature(float x, float y, float z)
{
	float t = 0.0;

	for (int i = 0; i <= 3; i++)
	{
		float dx = x - Centers[i].xc;
		float dy = y - Centers[i].yc;
		float dz = z - Centers[i].zc;
		float rsqd = SQR(dx) + SQR(dy) + SQR(dz);
		t += Centers[i].a * exp(-5.*rsqd);
	}

	if (t > TEMPMAX)
		t = TEMPMAX;

	return t;
}

void Vector(float x, float y, float z, float *vxp, float *vyp, float *vzp)
{
	*vxp = y * z * (y*y + z*z);
	*vyp = x * z * (x*x + z*z);
	*vzp = x * y * (x*x + y*y);


}

float Norm(float vx, float vy, float vz)
{
	return sqrt(vx*vx + vy*vy + vz*vz);
}

void Advect(float *x, float *y, float *z, float TimeStep)
{
	float xa = *x, ya = *y, za = *z, xb, yb, zb;
	float vxa, vya, vza, vxb, vyb, vzb, vx, vy, vz;
	Vector(xa, ya, za, &vxa, &vya, &vza);

	xb = xa + TimeStep * vxa;
	yb = ya + TimeStep * vya;
	zb = za + TimeStep * vza;

	Vector(xb, yb, zb, &vxb, &vyb, &vzb);

	vx = (vxa + vxb) / 2;
	vy = (vya + vyb) / 2;
	vz = (vza + vzb) / 2;

	*x = xa + TimeStep * vx;
	*y = ya + TimeStep * vy;
	*z = za + TimeStep * vz;
}

void InitPlane(struct node ** plane, float minT, float maxT, float minU, float maxU, int numT, int numU, float con, int mode)
{
	for (int i = 0; i < numT; i++)
	{
		float t = minT + (maxT - minT) * (float)i / (float)(numT - 1);
		for (int j = 0; j < numU; j++)
		{
			float u = minU + (maxU - minU) * (float)j / (float)(numU - 1);

			switch (mode)
			{
			case MODE_XY:
				((struct node*)plane + i*numU + j)->x = t;
				((struct node*)plane + i*numU + j)->y = u;
				((struct node*)plane + i*numU + j)->z = con;
				break;

			case MODE_YZ:
				((struct node*)plane + i*numU + j)->x = con;
				((struct node*)plane + i*numU + j)->y = t;
				((struct node*)plane + i*numU + j)->z = u;
				break;

			case MODE_ZX:
				((struct node*)plane + i*numU + j)->x = u;
				((struct node*)plane + i*numU + j)->y = con;
				((struct node*)plane + i*numU + j)->z = t;
				break;
			}
			
			((struct node*)plane + i*numU + j)->t = Temperature(((struct node*)plane + i*numU + j)->x, ((struct node*)plane + i*numU + j)->y, ((struct node*)plane + i*numU + j)->z);
			float hsv[3] = { 240. - 240 * (((struct node*)plane + i*numU + j)->t - TEMPMIN) / (TEMPMAX - TEMPMIN), 1., 1. };
			HsvRgb(hsv, ((struct node*)plane + i*numU + j)->rgb);
		}
	}

}

float Radius(float center[3], float point[3])
{
	return sqrt(pow(point[0] - center[0], 2) + pow(point[1] - center[1], 2) + pow(point[2] - center[2], 2));
}

float AbsGra(struct node Nodes[][NY][NZ], int x, int y, int z)
{
	float gradient[3] = { 0 };
	if (x == 0){
		gradient[0] = (Nodes[x][y][z].t - Nodes[x + 1][y][z].t) / (Nodes[x][y][z].x - Nodes[x + 1][y][z].x);
	}
	else{
		if (x == NX-1){
			gradient[0] = (Nodes[x - 1][y][z].t - Nodes[x][y][z].t) / (Nodes[x - 1][y][z].x - Nodes[x][y][z].x);
		}
		else{
			gradient[0] = (Nodes[x - 1][y][z].t - Nodes[x + 1][y][z].t) / (Nodes[x - 1][y][z].x - Nodes[x + 1][y][z].x);
		}
	}

	if (y == 0){
		gradient[1] = (Nodes[x][y][z].t - Nodes[x][y + 1][z].t) / (Nodes[x][y][z].y - Nodes[x][y + 1][z].y);
	}
	else{
		if (y == NY-1){
			gradient[1] = (Nodes[x][y - 1][z].t - Nodes[x][y][z].t) / (Nodes[x][y - 1][z].y - Nodes[x][y][z].y);
		}
		else{
			gradient[1] = (Nodes[x][y - 1][z].t - Nodes[x][y + 1][z].t) / (Nodes[x][y - 1][z].y - Nodes[x][y + 1][z].y);
		}
	}
	
	if (z == 0){
		gradient[2] = (Nodes[x][y][z].t - Nodes[x][y][z + 1].t) / (Nodes[x][y][z].z - Nodes[x][y][z + 1].z);
	}
	else{
		if (z == NZ-1){
			gradient[2] = (Nodes[x][y][z - 1].t - Nodes[x][y][z].t) / (Nodes[x][y][z - 1].z - Nodes[x][y][z].z);
		}
		else{
			gradient[2] = (Nodes[x][y][z - 1].t - Nodes[x][y][z + 1].t) / (Nodes[x][y][z - 1].z - Nodes[x][y][z + 1].z);
		}
	}
	return sqrt(pow(gradient[0], 2) + pow(gradient[1], 2) + pow(gradient[2], 2));
}


void
HsvRgb(float hsv[3], float rgb[3])
{
	float r, g, b;			// red, green, blue

	// guarantee valid input:

	float h = hsv[0] / 60.;
	while (h >= 6.)	h -= 6.;
	while (h <  0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;


	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}


	// get an rgb from the hue itself:

	float i = floor(h);
	float f = h - i;
	float p = v * (1. - s);
	float q = v * (1. - s*f);
	float t = v * (1. - (s * (1. - f)));

	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void Hyperbolic(float * x, float * y, float k, int mode){
	if (mode){
		Polar(x, y, k);
	}
	else{
		Cartesian(x, y, k);
	}
}


void Polar(float * x, float * y, float k)
{
	float r = sqrt((*x)*(*x) + (*y)*(*y));
	*x = *x / (r + k);
	*y = *y / (r + k);
}

void Cartesian(float * x, float * y, float k)
{
	*x = *x / sqrt((*x)*(*x) + k*k);
	*y = *y / sqrt((*y)*(*y) + k*k);
}

