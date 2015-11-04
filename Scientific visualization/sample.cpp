#include <stdio.h>
	// yes, I know stdio.h is not good C++, but I like the *printf( )
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glui.h"

#include "project.h"
#include "bmptotexture.h"
//
//
//	This is a sample OpenGL / GLUT / GLUI program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with radio buttons
//
//	The left mouse button allows rotation
//	The middle mouse button allows scaling
//	The glui window allows:
//		1. The 3d object to be transformed
//		2. The projection to be changed
//		3. The color of the axes to be changed
//		4. The axes to be turned on and off
//		5. The transformations to be reset
//		6. The program to quit
//
//	Author:			Joe Graphics
//
//  Latest update:	March 27, 2013
//


//
// constants:
//
// NOTE: There are a bunch of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.
//
//


// title of these windows:

const char *WINDOWTITLE = { "Project 08 -- Jun He" };
const char *GLUITITLE   = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// size of the box:

const float BOXSIZE = { 2.f };



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// able to use the left mouse for either rotation or scaling,
// in case have only a 2-button mouse:

enum LeftButton
{
	ROTATE,
	SCALE
};


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals
{
	RESET,
	QUIT
};


// window background color (rgba):

const float BACKCOLOR[ ] = { 0., 0., 0., 0. };


// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};


// the color definitions:
// this order must match the radio button order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };

//project 03 parameters

#define TEMP	0
#define XCOR	1
#define YCOR	2
#define ZCOR	3
#define RADI	4
#define GRAD	5
#define FLOW	6
#define SCALE	7

#define SPIN_XY			1
#define SPIN_YZ			2
#define SPIN_ZX			3
#define SPIN_NUMS		4
#define SPIN_STAR		5
#define SPIN_TIMESTEP	6
#define SPIN_SCALE		7
#define SPIN_K			8
#define SPIN_HEIGHT		9
#define SPIN_LIGHT		10

const char *	TEMPFORMAT = { "Temperature: %5.2f - %5.2f" };
const char *	XFORMAT = { "X: %5.2f - %5.2f" };
const char *	YFORMAT = { "Y: %5.2f - %5.2f" };
const char *	ZFORMAT = { "Z: %5.2f - %5.2f" };
const char *	RADIFORMAT = { "radius: %5.2f - %5.2f" };
const char *	GRADFORMAT = { "gradient: %5.2f - %5.2f" };



//
// non-constant global variables:
//
GLUI_HSlider *		TempSlider;
GLUI_HSlider *		XSlider;
GLUI_HSlider *		YSlider;
GLUI_HSlider *		ZSlider;
GLUI_HSlider *		RADISlider;
GLUI_HSlider *		GRADSlider;
GLUI_HSlider *		FlowSlider;
GLUI_HSlider *		ScaleSlider;

GLUI_StaticText *	TempLabel;
GLUI_StaticText *	XLabel;
GLUI_StaticText *	YLabel;
GLUI_StaticText *	ZLabel;
GLUI_StaticText *	RADILabel;
GLUI_StaticText *	GRADLabel;

GLUI_Spinner * XYSpin;
GLUI_Spinner * YZSpin;
GLUI_Spinner * ZXSpin;
GLUI_Spinner * numSSpin;
GLUI_Spinner * StarSpin;
GLUI_Spinner * TimeStepSpin;
GLUI_Spinner * ScaleSpin;
GLUI_Spinner * KSpin;
GLUI_Spinner * HeightSpin;
GLUI_Spinner * LightSpin;

GLUI_RadioGroup * radio_contour;
GLUI_RadioGroup * radio_probe;
GLUI_RadioGroup * radio_texture_filter; 
GLUI_RadioGroup * radio_texture_envi;

int PointOn;
int XYOn;
int YZOn;
int ZXOn;
int VectorOn;
int StreamlinesOn;
int PolarOn;
int HiDeOn;
int CloudOn;
int SmoothOn;

int IsosurfaceOn;
int is_contour;
int Which_Blob;
int Which_Thing;
int Which_Filter;
int Which_Envi;

int PauseOn = GLUIFALSE;

GLuint PointList;
GLuint XY_inter_List;
GLuint YZ_inter_List;
GLuint ZX_inter_List;
GLuint XY_contour_List;
GLuint YZ_contour_List;
GLuint ZX_contour_List;
GLuint Isosurface_List;
GLuint CubeEdge_List;
GLuint Vector_List;
GLuint Stream_List;
GLuint Ribbon_List;
GLuint Blob_List;
GLuint Probe_List;
GLuint Probe_Single_Stream_List;
GLuint Probe_Multi_Stream_List;
GLuint Terrain_Main_List;
GLuint TerrainTex;
GLuint RideTerrainTex;

struct node	Nodes[NX][NY][NZ];
struct node	XY[NX][NY];
struct node	YZ[NY][NZ];
struct node	ZX[NX][NZ];

struct LngLatHgt Points[NUMLATS][NUMLNGS];

float		TempLowHigh[2] = { TEMPMIN, TEMPMAX };
float		XLowHigh[2] = { XMIN, XMAX };
float		YLowHigh[2] = { YMIN, YMAX };
float		ZLowHigh[2] = { ZMIN, ZMAX };
float		RADILowHigh[2] = { RADIMIN, RADIMAX };
float		GRADLowHigh[2] = { GRADMIN, GRADMAX };
float		FlowLowHigh[2] = { FLOWMIN, FLOWMAX };


float		val_XY = 0;
float		val_YZ = 0;
float		val_ZX = 0;
float		val_TimeStep = 0.5;
float		ScaleValue = 0.1 ;

int			numS = 5;
float		val_S = 50.;

float		val_K = 10.;
float		HeightExag = 10.;
float		LightingHeightFactor = 3.;

unsigned char *Texture;
int Width, Height;
float Time = 0;
//

int	ActiveButton;			// current button that is down
GLuint	AxesList;			// list to hold the axes
int	AxesOn;					// != 0 means to draw the axes
int	DebugOn;				// != 0 means to print debugging info
int	DepthCueOn;				// != 0 means to use intensity depth cueing
GLUI *	Glui;				// instance of glui window
int	GluiWindow;				// the glut id for the glui window
int	LeftButton;				// either ROTATE or SCALE
GLuint	BoxList;			// object display list
int	MainWindow;				// window id for main graphics window
int GraphWindow;
int RideWindow;
GLfloat	RotMatrix[4][4];	// set by glui rotation widget
float	Scale, Scale2;		// scaling factors
int	WhichColor;				// index into Colors[ ]
int	WhichProjection;		// ORTHO or PERSP
int	Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;			// rotation angles in degrees
float	TransXYZ[3];		// set by glui translation widgets
float	ProbeXYZ[3];

int		Npts[68];
float	X[68][263];
float	Y[68][263];


//
// function prototypes:
//

void	Animate( void );
void	Buttons( int );
void	Display( void );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( void );
void	InitGlui( void );
void	InitGraphics( void );
void	InitLists( void );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( void );
void	Resize( int, int );
void	Sliders(int);
void	Spinners(int);
void	Translation(int);
void	Visibility( int );

void	Arrow( float [3], float [3] );
void	Cross( float [3], float [3], float [3] );
float	Dot( float [3], float [3] );
float	Unit( float [3], float [3] );
void	Axes( float );


void	Init_All_Planes();
void	ColorSquare(struct node *p1, struct node *p2, struct node *p3, struct node *p4);
void	ProcessQuad(struct node *p1, struct node *p2, struct node *p3, struct node *p4, float ts, float rgb[3]);
void	Draw_Interpolate(struct node ** plane, GLuint list, int numT, int numU);
void	Draw_Contour(struct node ** plane, GLuint list, int numT, int numU);
void	Draw_Isosurface(GLuint list);

void	Draw_CubeEdge();
void	Draw_Vector();
void	Streamline(float x, float y, float z);
void	Draw_Streamlines();
void	Draw_Ribbon(float x, float y, float z);
void	Draw_Quads(float p1[3], float p2[3], float p3[3], float p4[3]);
void	Draw_Blob(float x, float y, float z);
void	Draw_Probe();
void	Draw_Stream_Probe();
void	Draw_Single_Stream_Probe(float x, float y, float z);
void	Draw_Multi_Stream_Probe(float x, float y, float z);
void	Draw_US_Map();
void	Draw_HyperbolicLine(float px1, float py1, float px2, float py2);
void	Draw_Texture_Map(GLuint Tex);
void	Draw_Texture_Triangle(int i1, int j1, int i2, int j2, int i3, int j3);
void	Draw_Cloud(float time);

void	GraVec(int z, int x);

//Ride Window
void	RideDisplay();
void	GraphDisplay();

//
// main program:
//

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );


	// setup all the graphics stuff:

	InitGraphics( );


	// create the display structures that will not change:

	InitLists( );


	// init all the global variables used by Display( ):
	// this will also post a redisplay
	// it is important to call this before InitGlui( )
	// so that the variables that glui will control are correct
	// when each glui widget is created

	Reset( );


	// setup all the user interface stuff:

	InitGlui( );


	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutMainLoop( );

	getchar();
	// this is here to make the compiler happy:

	return 0;
}



//
// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it
//

void
Animate( void )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	Which_Blob = (Which_Blob + 1) % Blob_ITER;
	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms = ms % PERIOD_MS;
	Time = (float)ms / (float)(PERIOD_MS - 1);
	
	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );

	glutSetWindow(RideWindow);
	glutPostRedisplay();

	glutSetWindow(GraphWindow);
	glutPostRedisplay();
}




//
// glui buttons callback:
//

void
Buttons( int id )
{

	switch( id )
	{
		case RESET:
			Reset( );
			/*
			char str[128];
			TempLowHigh[0] = TEMPMIN;
			TempLowHigh[1] = TEMPMAX;
			sprintf(str, TEMPFORMAT, TempLowHigh[0], TempLowHigh[1]);
			TempLabel->set_text(str);
			XLowHigh[0] = XMIN;
			XLowHigh[1] = XMAX;
			sprintf(str, XFORMAT, XLowHigh[0], XLowHigh[1]);
			XLabel->set_text(str);
			YLowHigh[0] = YMIN;
			YLowHigh[1] = YMAX;
			sprintf(str, YFORMAT, YLowHigh[0], YLowHigh[1]);
			YLabel->set_text(str);
			ZLowHigh[0] = ZMIN;
			ZLowHigh[1] = ZMAX;
			sprintf(str, ZFORMAT, ZLowHigh[0], ZLowHigh[1]);
			ZLabel->set_text(str);
			RADILowHigh[0] = RADIMIN;
			RADILowHigh[1] = RADIMAX;
			sprintf(str, RADIFORMAT, RADILowHigh[0], RADILowHigh[1]);
			RADILabel->set_text(str);
			GRADLowHigh[0] = GRADMIN;
			GRADLowHigh[1] = GRADMAX;
			sprintf(str, GRADFORMAT, GRADLowHigh[0], GRADLowHigh[1]);
			GRADLabel->set_text(str);
			*/
			Glui->sync_live( );
			glutSetWindow( MainWindow );
			glutPostRedisplay( );
			break;

		case QUIT:
			// gracefully close the glui window:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:

			Glui->close( );
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Button ID %d\n", id );
	}

}



//
// draw the complete scene:
//

void
Display( void )
{
	if (DebugOn != 0)
	{
		fprintf(stderr, "Display\n");
	}

	glDisable(GL_DEPTH);
	// set which window we want to do the graphics into:

	glutSetWindow(MainWindow);


	// erase the background:

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);


	// specify shading to be flat:

	glShadeModel(GL_FLAT);


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );
	
	//gluOrtho2D(-10., 10., -10., 10.);
	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// set the eye position, look-at position, and up-vector:
	// IF DOING 2D, REMOVE THIS -- OTHERWISE ALL YOUR 2D WILL DISAPPEAR !

	gluLookAt( 0., -300., 1.,     0., 0., 0.,     0., 1., 0. );
	//glMatrixMode(GL_TEXTURE);


	// translate the objects in the scene:
	// note the minus sign on the z value
	// this is to make the appearance of the glui z translate
	// widget more intuitively match the translate behavior
	// DO NOT TRANSLATE IN Z IF YOU ARE DOING 2D !

	glTranslatef( (GLfloat)TransXYZ[0], (GLfloat)TransXYZ[1], -(GLfloat)TransXYZ[2] );


	// rotate the scene:
	// DO NOT ROTATE (EXCEPT ABOUT Z) IF YOU ARE DOING 2D !

	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );
	glMultMatrixf((const GLfloat *)RotMatrix);


	// uniformly scale the scene:

	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );
	GLfloat scale2 = 1. + Scale2;		// because glui translation starts at 0.
	if (scale2 < MINSCALE)
		scale2 = MINSCALE;
	glScalef( (GLfloat)scale2, (GLfloat)scale2, (GLfloat)scale2 );


	// set the fog parameters:
	// DON'T NEED THIS IF DOING 2D !

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}


	// possibly draw the axes:

	if (AxesOn != 0)
	{
		glColor3fv(&Colors[WhichColor][0]);
		glCallList(AxesList);
	}


	// set the color of the object:

	glColor3fv(Colors[WhichColor]);
	glDisable(GL_DEPTH_TEST);

	// draw the current object:
	Draw_Texture_Map(TerrainTex);
	if (CloudOn){
		Draw_Cloud(Time);
	}

	//glCallList( BoxList );
	/*
	if (PointOn){
		glCallList(PointList);
	}
	if (0){
		if (IsosurfaceOn)
		{
			glCallList(Isosurface_List);
		}
		else{
			if (is_contour){
				if (XYOn)
				{
					glCallList(XY_contour_List);
				}
				if (YZOn)
				{
					glCallList(YZ_contour_List);
				}
				if (ZXOn)
				{
					glCallList(ZX_contour_List);
				}
			}
			else{
				if (XYOn)
				{
					glCallList(XY_inter_List);
				}
				if (YZOn)
				{
					glCallList(YZ_inter_List);
				}
				if (ZXOn)
				{
					glCallList(ZX_inter_List);
				}
			}
		}
	}

	glCallList(CubeEdge_List);
	if (VectorOn){
		glCallList(Vector_List);
	}
	if (StreamlinesOn){
		glCallList(Stream_List);
	}
	
	glCallList(Probe_List);
	switch (Which_Thing){
	case 1:glCallList(Probe_Single_Stream_List); break;
	case 2:glCallList(Probe_Multi_Stream_List); break;
	case 3:glCallList(Ribbon_List); break;
	case 4:glCallList(Blob_List + Which_Blob); break;
	}
	*/

	//Draw_US_Map();
	// draw some gratuitous text that just rotates on top of the scene:
	
	glColor3f( 0., 1., 1. );
	//DoRasterString( 0., 1., 0., "Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100.,     0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1., 1., 1. );
	DoRasterString( 5., 5., 0., "Text That Doesn't" );


	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );

	//Animate();
}



//
// use glut to display a string of characters using a raster font:
//

void
DoRasterString( float x, float y, float z, char *s )
{
	char c;			// one character to print

	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}



//
// use glut to display a string of characters using a stroke font:
//

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	char c;			// one character to print

	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05 + 33.33 );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}



//
// return the number of seconds since the start of the program:
//

float
ElapsedSeconds( void )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.;
}



//
// initialize the glui window:
//

void
InitGlui( void )
{
	GLUI_Panel *panel;
	GLUI_RadioGroup *group;
	GLUI_Rotation *rot;
	GLUI_Translation *trans, *scale;

	char str[128];
	// setup the glui window:

	glutInitWindowPosition( INIT_WINDOW_SIZE + 400, 0 );
	Glui = GLUI_Master.create_glui( (char *) GLUITITLE );


	Glui->add_statictext( (char *) GLUITITLE );
	Glui->add_separator( );
	/*
	Glui->add_checkbox("High Definition", &HiDeOn);
	Glui->add_checkbox("Polar", &PolarOn);
	KSpin = Glui->add_spinner("K", GLUI_SPINNER_FLOAT, &val_K, SPIN_K, (GLUI_Update_CB)Spinners);
	KSpin->set_float_limits(0., 100.);
	*/
	
	Glui->add_checkbox( "Axes", &AxesOn );

	Glui->add_checkbox( "Perspective", &WhichProjection );

	Glui->add_checkbox( "Intensity Depth Cue", &DepthCueOn );


	Glui->add_checkbox("Cloud", &CloudOn);
	panel = Glui->add_panel("Texture Filtering");
	radio_texture_filter = Glui->add_radiogroup_to_panel(panel, &Which_Filter);
	Glui->add_radiobutton_to_group(radio_texture_filter, "GL_NEAREST");
	Glui->add_radiobutton_to_group(radio_texture_filter, "GL_LINEAR");

	panel = Glui->add_panel("Texture Environment");
	radio_texture_envi = Glui->add_radiogroup_to_panel(panel, &Which_Envi);
	Glui->add_radiobutton_to_group(radio_texture_envi, "NO Texture");
	Glui->add_radiobutton_to_group(radio_texture_envi, "GL_REPLACE");
	Glui->add_radiobutton_to_group(radio_texture_envi, "GL_MODULATE");

	Glui->add_checkbox("Use Smooth Lighting", &SmoothOn);

	HeightSpin = Glui->add_spinner("Height Exaggeration", GLUI_SPINNER_FLOAT, &HeightExag, SPIN_HEIGHT, (GLUI_Update_CB)Spinners);
	HeightSpin->set_float_limits(HEIGHTMIN, HEIGHTMAX);
	LightSpin = Glui->add_spinner("Lighting Height Factor", GLUI_SPINNER_FLOAT, &LightingHeightFactor, SPIN_LIGHT, (GLUI_Update_CB)Spinners);
	LightSpin->set_float_limits(LIGHTMIN, LIGHTMAX);

	/*
	Glui->add_checkbox("Point", &PointOn); 
	Glui->add_checkbox("Vector", &VectorOn);
	Glui->add_checkbox("Streamlines", &StreamlinesOn);

	panel = Glui->add_panel("Probe");
	radio_probe = Glui->add_radiogroup_to_panel(panel, &Which_Thing);
	Glui->add_radiobutton_to_group(radio_probe, "Off");
	Glui->add_radiobutton_to_group(radio_probe, "Single");
	Glui->add_radiobutton_to_group(radio_probe, "Multi");
	Glui->add_radiobutton_to_group(radio_probe, "Ribbon");
	Glui->add_radiobutton_to_group(radio_probe, "Blob");
	
	GLUI_Panel *probe_panel = Glui->add_panel_to_panel(panel, "");
	trans = Glui->add_translation_to_panel(probe_panel, "Trans XY", GLUI_TRANSLATION_XY, &ProbeXYZ[0], -1, (GLUI_Update_CB)Translation);
	trans->set_speed(0.05f);

	Glui->add_column_to_panel(probe_panel, GLUIFALSE);
	trans = Glui->add_translation_to_panel(probe_panel, "Trans YZ", GLUI_TRANSLATION_XY, &ProbeXYZ[1], -1, (GLUI_Update_CB)Translation);
	trans->set_speed(0.05f);

	Glui->add_column_to_panel(probe_panel, GLUIFALSE);
	trans = Glui->add_translation_to_panel(probe_panel, "Trans ZX", GLUI_TRANSLATION_XY, &ProbeXYZ[2], -1, (GLUI_Update_CB)Translation);
	trans->set_speed(0.05f);

	TimeStepSpin = Glui->add_spinner("Time Step", GLUI_SPINNER_FLOAT, &val_TimeStep, SPIN_TIMESTEP, (GLUI_Update_CB)Spinners);
	TimeStepSpin->set_float_limits(TIMESTEPMIN, TIMESTEPMAX);
	ScaleSpin = Glui->add_spinner("Vector Scale", GLUI_SPINNER_FLOAT, &ScaleValue, SPIN_SCALE, (GLUI_Update_CB)Spinners);
	ScaleSpin->set_float_limits(SCALEMIN, SCALEMAX);

	Glui->add_statictext("Flow Speed");
	FlowSlider = Glui->add_slider(true, GLUI_HSLIDER_FLOAT, FlowLowHigh, FLOW, (GLUI_Update_CB)Sliders);
	FlowSlider->set_float_limits(FLOWMIN, FLOWMAX);
	*/
	/*
	//sliders
	TempSlider = Glui->add_slider(true, GLUI_HSLIDER_FLOAT, TempLowHigh, TEMP, (GLUI_Update_CB)Sliders);
	TempSlider->set_float_limits(TEMPMIN, TEMPMAX);
	TempSlider->set_w(200);		// good slider width
	sprintf(str, TEMPFORMAT, TempLowHigh[0], TempLowHigh[1]);
	TempLabel = Glui->add_statictext(str);

	XSlider = Glui->add_slider(true, GLUI_HSLIDER_FLOAT, XLowHigh, XCOR, (GLUI_Update_CB)Sliders);
	XSlider->set_float_limits(XMIN, XMAX);
	XSlider->set_w(200);		// good slider width
	sprintf(str, XFORMAT, XLowHigh[0], XLowHigh[1]);
	XLabel = Glui->add_statictext(str);

	YSlider = Glui->add_slider(true, GLUI_HSLIDER_FLOAT, YLowHigh, YCOR, (GLUI_Update_CB)Sliders);
	YSlider->set_float_limits(YMIN, YMAX);
	YSlider->set_w(200);		// good slider width
	sprintf(str, YFORMAT, YLowHigh[0], YLowHigh[1]);
	YLabel = Glui->add_statictext(str);

	ZSlider = Glui->add_slider(true, GLUI_HSLIDER_FLOAT, ZLowHigh, ZCOR, (GLUI_Update_CB)Sliders);
	ZSlider->set_float_limits(ZMIN, ZMAX);
	ZSlider->set_w(200);		// good slider width
	sprintf(str, ZFORMAT, ZLowHigh[0], ZLowHigh[1]);
	ZLabel = Glui->add_statictext(str);

	RADISlider = Glui->add_slider(true, GLUI_HSLIDER_FLOAT, RADILowHigh, RADI, (GLUI_Update_CB)Sliders);
	RADISlider->set_float_limits(RADIMIN, RADIMAX);
	RADISlider->set_w(200);		// good slider width
	sprintf(str, RADIFORMAT, RADILowHigh[0], RADILowHigh[1]);
	RADILabel = Glui->add_statictext(str);

	GRADSlider = Glui->add_slider(true, GLUI_HSLIDER_FLOAT, GRADLowHigh, GRAD, (GLUI_Update_CB)Sliders);
	GRADSlider->set_float_limits(GRADMIN, GRADMAX);
	GRADSlider->set_w(200);		// good slider width
	sprintf(str, GRADFORMAT, GRADLowHigh[0], GRADLowHigh[1]);
	GRADLabel = Glui->add_statictext(str);

	panel = Glui->add_panel("Project 04 and 05");
	//spinners
	XYSpin = Glui->add_spinner_to_panel(panel, "XY->Z", GLUI_SPINNER_FLOAT, &val_XY, SPIN_XY, (GLUI_Update_CB)Spinners);
	XYSpin->set_float_limits(ZMIN, ZMAX);
	YZSpin = Glui->add_spinner_to_panel(panel, "YZ->X", GLUI_SPINNER_FLOAT, &val_YZ, SPIN_YZ, (GLUI_Update_CB)Spinners);
	YZSpin->set_float_limits(XMIN, XMAX);
	ZXSpin = Glui->add_spinner_to_panel(panel, "ZX->Y", GLUI_SPINNER_FLOAT, &val_ZX, SPIN_ZX, (GLUI_Update_CB)Spinners);
	ZXSpin->set_float_limits(YMIN, YMAX);
	numSSpin = Glui->add_spinner_to_panel(panel, "numS", GLUI_SPINNER_INT, &numS, SPIN_NUMS, (GLUI_Update_CB)Spinners);
	numSSpin->set_int_limits(SMIN, SMAX);
	//checkbox
	GLUI_Panel *check_panel = Glui->add_panel_to_panel(panel, "");
	Glui->add_checkbox_to_panel(check_panel, "XY", &XYOn);
	Glui->add_column_to_panel(check_panel, FALSE);
	Glui->add_checkbox_to_panel(check_panel, "YZ", &YZOn);
	Glui->add_column_to_panel(check_panel, FALSE);
	Glui->add_checkbox_to_panel(check_panel, "ZX", &ZXOn);
	//radios
	radio_contour = Glui->add_radiogroup_to_panel(panel, &is_contour);
	Glui->add_radiobutton_to_group(radio_contour, "Interpolated Color");
	Glui->add_radiobutton_to_group(radio_contour, "Contour Lines");

	Glui->add_separator_to_panel(panel);
	Glui->add_checkbox_to_panel(panel, "Isosurface On", &IsosurfaceOn);
	StarSpin = Glui->add_spinner_to_panel(panel, "S*", GLUI_SPINNER_FLOAT, &val_S, SPIN_STAR, (GLUI_Update_CB)Spinners);
	StarSpin->set_float_limits(TEMPMIN, TEMPMAX);
	

	panel = Glui->add_panel(  "Axes Color" );
		group = Glui->add_radiogroup_to_panel( panel, &WhichColor );
			Glui->add_radiobutton_to_group( group, "Red" );
			Glui->add_radiobutton_to_group( group, "Yellow" );
			Glui->add_radiobutton_to_group( group, "Green" );
			Glui->add_radiobutton_to_group( group, "Cyan" );
			Glui->add_radiobutton_to_group( group, "Blue" );
			Glui->add_radiobutton_to_group( group, "Magenta" );
			Glui->add_radiobutton_to_group( group, "White" );
			Glui->add_radiobutton_to_group( group, "Black" );
*/
	panel = Glui->add_panel( "Object Transformation" );

		rot = Glui->add_rotation_to_panel( panel, "Rotation", (float *) RotMatrix );

		// allow the object to be spun via the glui rotation widget:

		rot->set_spin( 1.0 );


		Glui->add_column_to_panel( panel, GLUIFALSE );
		scale = Glui->add_translation_to_panel( panel, "Scale",  GLUI_TRANSLATION_Y , &Scale2 );
		scale->set_speed( 0.005f );

		Glui->add_column_to_panel( panel, GLUIFALSE );
		trans = Glui->add_translation_to_panel( panel, "Trans XY", GLUI_TRANSLATION_XY, &TransXYZ[0] );
		trans->set_speed( 0.05f );

		Glui->add_column_to_panel( panel, GLUIFALSE );
		trans = Glui->add_translation_to_panel( panel, "Trans Z",  GLUI_TRANSLATION_Z , &TransXYZ[2] );
		trans->set_speed( 0.05f );

	Glui->add_checkbox( "Debug", &DebugOn );


	panel = Glui->add_panel( "", GLUIFALSE );

	Glui->add_button_to_panel( panel, "Reset", RESET, (GLUI_Update_CB) Buttons );

	Glui->add_column_to_panel( panel, GLUIFALSE );

	Glui->add_button_to_panel( panel, "Quit", QUIT, (GLUI_Update_CB) Buttons );


	// tell glui what graphics window it needs to post a redisplay to:

	Glui->set_main_gfx_window( MainWindow );


	// set the graphics window's idle function if needed:

	GLUI_Master.set_glutIdleFunc( Animate );
}



//
// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions
//

void
InitGraphics( void )
{
	// setup the display mode:
	// ( *must* be done before call to glutCreateWindow( ) )
	// ask for color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );


	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );
	MainWindow = glutCreateWindow( WINDOWTITLE );

	glutInitWindowPosition(650, 0);
	glutInitWindowSize(300, 300);
	RideWindow = glutCreateWindow("Ride Window");

	glutInitWindowPosition(650, 400);
	glutInitWindowSize(300, 300);
	GraphWindow = glutCreateWindow("Graph Window");

	//Ride Window Initialization
	glutSetWindow(RideWindow);
	glutDisplayFunc(RideDisplay);

	//Graph Window Initialization
	glutSetWindow(GraphWindow);
	glutDisplayFunc(GraphDisplay);

	/*
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(NULL);
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(0, NULL, 0);
	*/
	// setup the clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );


	// setup the callback routines:


	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( 0, NULL, 0 );

	// DO NOT SET THE GLUT IDLE FUNCTION HERE !!
	// glutIdleFunc( NULL );
	// let glui take care of it in InitGlui( )

	Init_All_Planes();
	FILE * fp;
	
	for (int i = 0; i < NX; i++)
	{
		float x = -1. + 2. * (float)i / (float)(NX - 1);
		for (int j = 0; j < NY; j++)
		{
			float y = -1. + 2. * (float)j / (float)(NY - 1);
			for (int k = 0; k < NZ; k++)
			{
				float z = -1. + 2. * (float)k / (float)(NZ - 1);
				Nodes[i][j][k].x = x;
				Nodes[i][j][k].y = y;
				Nodes[i][j][k].z = z;

				Nodes[i][j][k].t = Temperature(x, y, z);
				float hsv[3] = { 240. - 240 * (Nodes[i][j][k].t - TEMPMIN) / (TEMPMAX - TEMPMIN), 1., 1. };
				HsvRgb(hsv, Nodes[i][j][k].rgb);
			}
		}
	}
	for (int i = 0; i < NX; ++i){
		for (int j = 0; j < NY; ++j){
			for (int k = 0; k < NZ; ++k){
				float center[3] = { 0 };
				Nodes[i][j][k].rad = Radius(center, &Nodes[i][j][k].x);

				Nodes[i][j][k].grad = AbsGra(Nodes, i, j, k);
			}
		}
	}
	


}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( void )
{
	float dx = BOXSIZE / 2.;
	float dy = BOXSIZE / 2.;
	float dz = BOXSIZE / 2.;

	// create the object:
	/*
	BoxList = glGenLists( 1 );
	glNewList( BoxList, GL_COMPILE );
	
		glBegin( GL_QUADS );

			glColor3f( 0., 0., 1. );
			glNormal3f( 0., 0.,  1. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f( -dx,  dy,  dz );

			glNormal3f( 0., 0., -1. );
				glTexCoord2f( 0., 0. );
				glVertex3f( -dx, -dy, -dz );
				glTexCoord2f( 0., 1. );
				glVertex3f( -dx,  dy, -dz );
				glTexCoord2f( 1., 1. );
				glVertex3f(  dx,  dy, -dz );
				glTexCoord2f( 1., 0. );
				glVertex3f(  dx, -dy, -dz );

			glColor3f( 1., 0., 0. );
			glNormal3f(  1., 0., 0. );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f(  dx,  dy,  dz );

			glNormal3f( -1., 0., 0. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f( -dx,  dy, -dz );
				glVertex3f( -dx, -dy, -dz );

			glColor3f( 0., 1., 0. );
			glNormal3f( 0.,  1., 0. );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f( -dx,  dy, -dz );

			glNormal3f( 0., -1., 0. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx, -dy, -dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx, -dy,  dz );

		glEnd( );

	glEndList( );
	*/
	PointList = glGenLists(1);
	glNewList(PointList, GL_COMPILE);
	float delx = (XMAX - XMIN) / (float)(NX - 1);
	float dely = (YMAX - YMIN) / (float)(NY - 1);
	float delz = (ZMAX - ZMIN) / (float)(NZ - 1);

	glPointSize(0.1);
	glBegin(GL_POINTS);
	float x = XMIN;
	for (int i = 0; i < NX; i++, x += delx)
	{
		if (x < XLowHigh[0] || x > XLowHigh[1])
			continue;

		float y = YMIN;
		for (int j = 0; j < NY; j++, y += dely)
		{
			if (y < YLowHigh[0] || y > YLowHigh[1])
				continue;

			float z = ZMIN;
			for (int k = 0; k < NZ; k++, z += delz)
			{
				if (z < ZLowHigh[0] || z > ZLowHigh[1])
					continue;

				if (Nodes[i][j][k].t < TempLowHigh[0] || Nodes[i][j][k].t > TempLowHigh[1])
					continue;

				if (Nodes[i][j][k].rad < RADILowHigh[0] || Nodes[i][j][k].rad > RADILowHigh[1])
					continue;

				if (Nodes[i][j][k].grad < GRADLowHigh[0] || Nodes[i][j][k].grad > GRADLowHigh[1])
					continue;

				glColor3f(Nodes[i][j][k].rgb[0], Nodes[i][j][k].rgb[1], Nodes[i][j][k].rgb[2]);
				glVertex3f(Nodes[i][j][k].x, Nodes[i][j][k].y, Nodes[i][j][k].z);
			}
		}
	}
	glEnd();
	glEndList();

	XY_inter_List = glGenLists(1);
	YZ_inter_List = glGenLists(1);
	ZX_inter_List = glGenLists(1);

	XY_contour_List = glGenLists(1);
	YZ_contour_List = glGenLists(1);
	ZX_contour_List = glGenLists(1);

	Isosurface_List = glGenLists(1);

	CubeEdge_List = glGenLists(1);
	Draw_CubeEdge();

	Vector_List = glGenLists(1);
	Draw_Vector();

	Stream_List = glGenLists(1);
	Draw_Streamlines();

	Probe_List = glGenLists(1);
	Probe_Multi_Stream_List = glGenLists(1);
	Probe_Single_Stream_List = glGenLists(1);
	Ribbon_List = glGenLists(1);
	Blob_List = glGenLists(Blob_ITER);


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}



//
// the keyboard callback:
//

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
			PauseOn = !PauseOn;
			if (PauseOn)
				GLUI_Master.set_glutIdleFunc(NULL);
			else
				GLUI_Master.set_glutIdleFunc(Animate);
			break;

		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			Buttons( QUIT );	// will not return here
			break;			// happy compiler

		case 'r':
		case 'R':
			LeftButton = ROTATE;
			break;

		case 's':
		case 'S':
			LeftButton = SCALE;
			break;

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}


	// synchronize the GLUI display with the variables:

	Glui->sync_live( );


	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}



//
// called when the mouse button transitions down or up:
//

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}



//
// called when the mouse moves while a button is down:
//

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		switch( LeftButton )
		{
			case ROTATE:
				Xrot += ( ANGFACT*dy );
				Yrot += ( ANGFACT*dx );
				break;

			case SCALE:
				Scale += SCLFACT * (float) ( dx - dy );
				if( Scale < MINSCALE )
					Scale = MINSCALE;
				break;
		}
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	for (int i = 0; i < 68; i++)
	{
		for (int j = 0; j < Npts[i]; j++)
		{
			X[i][j] += dx / 50.;
			Y[i][j] -= dy / 50.;
		}
	}
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}



// reset the transformations and the colors:
//
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( void )
{
	ActiveButton = 0;
	AxesOn = GLUITRUE;
	DebugOn = GLUIFALSE;
	DepthCueOn = GLUIFALSE;
	LeftButton = ROTATE;
	Scale  = 1.0;
	Scale2 = 0.0;		// because we add 1. to it in Display( )
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
	TransXYZ[0] = TransXYZ[1] = TransXYZ[2] = 0.;

	                  RotMatrix[0][1] = RotMatrix[0][2] = RotMatrix[0][3] = 0.;
	RotMatrix[1][0]                   = RotMatrix[1][2] = RotMatrix[1][3] = 0.;
	RotMatrix[2][0] = RotMatrix[2][1]                   = RotMatrix[2][3] = 0.;
	RotMatrix[3][0] = RotMatrix[3][1] = RotMatrix[3][3]                   = 0.;
	RotMatrix[0][0] = RotMatrix[1][1] = RotMatrix[2][2] = RotMatrix[3][3] = 1.;

	PointOn = GLUIFALSE;
	//project 04
	XYOn = GLUITRUE;
	YZOn = GLUITRUE;
	ZXOn = GLUITRUE;
	IsosurfaceOn = GLUIFALSE;
	val_XY = 0;
	val_YZ = 0;
	val_ZX = 0;
	is_contour = 0;
	numS = 50;

	Init_All_Planes();
	Draw_Interpolate((struct node **)XY, XY_inter_List, NX, NY);
	Draw_Interpolate((struct node **)YZ, YZ_inter_List, NY, NZ);
	Draw_Interpolate((struct node **)ZX, ZX_inter_List, NZ, NX);

	Draw_Contour((struct node **)XY, XY_contour_List, NX, NY);
	Draw_Contour((struct node **)YZ, YZ_contour_List, NY, NZ);
	Draw_Contour((struct node **)ZX, ZX_contour_List, NZ, NX);

	Draw_Isosurface(Isosurface_List);

	VectorOn = GLUIFALSE;
	StreamlinesOn = GLUIFALSE;
	Which_Blob = 0;
	Draw_Probe();

	val_K = 10.;
	PolarOn = GLUIFALSE;
	HiDeOn = GLUIFALSE;
	CloudOn = GLUITRUE;
	Which_Filter = 1;
	Which_Envi = 0;
	SmoothOn = 1;
}



//
// called when user resizes the window:
//

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

//
// called when user resizes the sliders:
//

void
Sliders(int id)
{
	char str[128];

	switch (id)
	{
	case TEMP:
		sprintf(str, TEMPFORMAT, TempLowHigh[0], TempLowHigh[1]);
		TempLabel->set_text(str);
		break;

	case XCOR:
		sprintf(str, XFORMAT, XLowHigh[0], XLowHigh[1]);
		XLabel->set_text(str);
		break;

	case YCOR:
		sprintf(str, YFORMAT, YLowHigh[0], YLowHigh[1]);
		YLabel->set_text(str);
		break;

	case ZCOR:
		sprintf(str, ZFORMAT, ZLowHigh[0], ZLowHigh[1]);
		ZLabel->set_text(str);
		break;

	case RADI:
		sprintf(str, RADIFORMAT, RADILowHigh[0], RADILowHigh[1]);
		RADILabel->set_text(str);
		break;

	case GRAD:
		sprintf(str, GRADFORMAT, GRADLowHigh[0], GRADLowHigh[1]);
		GRADLabel->set_text(str);
		break;

	case FLOW:
		Draw_Vector();
		break;

	}

	//glutSetWindow(MainWindow);
	//glutPostRedisplay();
}

// called when user changes spinner

void
Spinners(int id)
{
	switch (id)
	{
	case SPIN_XY:
		InitPlane((struct node **)XY, XMIN, XMAX, YMIN, YMAX, NX, NY, val_XY, MODE_XY);
		Draw_Interpolate((struct node **)XY, XY_inter_List, NX, NY);
		Draw_Contour((struct node **)XY, XY_contour_List, NX, NY);
		break;
	case SPIN_YZ:
		InitPlane((struct node **)YZ, YMIN, YMAX, ZMIN, ZMAX, NY, NZ, val_YZ, MODE_YZ);
		Draw_Interpolate((struct node **)YZ, YZ_inter_List, NY, NZ);
		Draw_Contour((struct node **)YZ, YZ_contour_List, NY, NZ);
		break;
	case SPIN_ZX:
		InitPlane((struct node **)ZX, ZMIN, ZMAX, XMIN, XMAX, NZ, NX, val_ZX, MODE_ZX);
		Draw_Interpolate((struct node **)ZX, ZX_inter_List, NZ, NX);
		Draw_Contour((struct node **)ZX, ZX_contour_List, NZ, NX);
		break;
	case SPIN_NUMS:
		Draw_Contour((struct node **)XY, XY_contour_List, NX, NY);
		Draw_Contour((struct node **)YZ, YZ_contour_List, NY, NZ);
		Draw_Contour((struct node **)ZX, ZX_contour_List, NZ, NX);
		break;
	case SPIN_STAR:
		Draw_Isosurface(Isosurface_List);
		break;
	case SPIN_TIMESTEP:
		Draw_Streamlines();
		Draw_Probe();
		break;
	case SPIN_SCALE:
		Draw_Vector();
		break; 

	default:
		break;
	}

}

void Translation(int id)
{
	Draw_Probe();
}
//
// handle a change to the window's visibility:
//

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}




///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////

// size of wings as fraction of length:

#define WINGS	0.10


// axes:

#define X_AXIS	1
#define Y_AXIS	2
#define Z_AXIS	3


// x, y, z, axes:

static float axx[3] = { 1., 0., 0. };
static float ayy[3] = { 0., 1., 0. };
static float azz[3] = { 0., 0., 1. };


void
Arrow( float tail[3], float head[3] )
{
	float u[3], v[3], w[3];		// arrow coordinate system

	// set w direction in u-v-w coordinate system:

	w[0] = head[0] - tail[0];
	w[1] = head[1] - tail[1];
	w[2] = head[2] - tail[2];


	// determine major direction:

	int axis = X_AXIS;
	float mag = fabs( w[0] );
	if(  fabs( w[1] )  > mag  )
	{
		axis = Y_AXIS;
		mag = fabs( w[1] );
	}
	if(  fabs( w[2] )  > mag  )
	{
		axis = Z_AXIS;
		mag = fabs( w[2] );
	}


	// set size of wings and turn w into a Unit vector:

	float d = WINGS * Unit( w, w );


	// draw the shaft of the arrow:

	glBegin( GL_LINE_STRIP );
		glVertex3fv( tail );
		glVertex3fv( head );
	glEnd( );

	// draw two sets of wings in the non-major directions:

	float x, y, z;

	if (axis != X_AXIS)
	{
		Cross( w, axx, v );
		(void) Unit( v, v );
		Cross( v, w, u  );
		x = head[0] + d * ( u[0] - w[0] );
		y = head[1] + d * ( u[1] - w[1] );
		z = head[2] + d * ( u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd( );
		x = head[0] + d * ( -u[0] - w[0] );
		y = head[1] + d * ( -u[1] - w[1] );
		z = head[2] + d * ( -u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd( );
	}


	if (axis != Y_AXIS)
	{
		Cross( w, ayy, v );
		(void) Unit( v, v );
		Cross( v, w, u  );
		x = head[0] + d * ( u[0] - w[0] );
		y = head[1] + d * ( u[1] - w[1] );
		z = head[2] + d * ( u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd( );
		x = head[0] + d * ( -u[0] - w[0] );
		y = head[1] + d * ( -u[1] - w[1] );
		z = head[2] + d * ( -u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd( );
	}



	if (axis != Z_AXIS)
	{
		Cross( w, azz, v );
		(void) Unit( v, v );
		Cross( v, w, u  );
		x = head[0] + d * ( u[0] - w[0] );
		y = head[1] + d * ( u[1] - w[1] );
		z = head[2] + d * ( u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd( );
		x = head[0] + d * ( -u[0] - w[0] );
		y = head[1] + d * ( -u[1] - w[1] );
		z = head[2] + d * ( -u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd( );
	}
}



float
Dot( float v1[3], float v2[3] )
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}



void
Cross( float v1[3], float v2[3], float vout[3] )
{
	float tmp[3];

	tmp[0] = v1[1]*v2[2] - v2[1]*v1[2];
	tmp[1] = v2[0]*v1[2] - v1[0]*v2[2];
	tmp[2] = v1[0]*v2[1] - v2[0]*v1[1];

	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}



float
Unit( float vin[3], float vout[3] )
{
	float dist = vin[0]*vin[0] + vin[1]*vin[1] + vin[2]*vin[2];

	if( dist > 0.0 )
	{
		dist = sqrt( dist );
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}

	return dist;
}



// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};


static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};


static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};


// fraction of the length to use as height of the characters:

const float LENFRAC = 0.10f;


// fraction of length to use as start location of the characters:

const float BASEFRAC = 1.10f;


//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}




// routine to convert HSV to RGB
//
// Reference:  Foley, van Dam, Feiner, Hughes,
//		"Computer Graphics Principles and Practices,"

void Init_All_Planes()
{
	InitPlane((struct node **)XY, XMIN, XMAX, YMIN, YMAX, NX, NY, val_XY, MODE_XY);

	InitPlane((struct node **)YZ, YMIN, YMAX, ZMIN, ZMAX, NY, NZ, val_YZ, MODE_YZ);

	InitPlane((struct node **)ZX, ZMIN, ZMAX, XMIN, XMAX, NZ, NX, val_ZX, MODE_ZX);
}

void ColorSquare(struct node *p1, struct node *p2, struct node *p3, struct node *p4)
{
	glColor3f(p1->rgb[0], p1->rgb[1], p1->rgb[2]);
	glVertex3f(p1->x, p1->y, p1->z);

	glColor3f(p2->rgb[0], p2->rgb[1], p2->rgb[2]);
	glVertex3f(p2->x, p2->y, p2->z);

	glColor3f(p3->rgb[0], p3->rgb[1], p3->rgb[2]);
	glVertex3f(p3->x, p3->y, p3->z);

	glColor3f(p4->rgb[0], p4->rgb[1], p4->rgb[2]);

	glVertex3f(p4->x, p4->y, p4->z);
}

void Draw_Interpolate(struct node ** plane, GLuint list, int numT, int numU)
{
	glNewList(list, GL_COMPILE);

	glShadeModel(GL_SMOOTH);
	glBegin(GL_QUADS);

	for (int i = 0; i < numT - 1; ++i){
		for (int j = 0; j < numU - 1; ++j){
			ColorSquare(((struct node*)plane + i*numU + j), ((struct node*)plane + (i+1)*numU + j), ((struct node*)plane + (i+1)*numU + j+1), ((struct node*)plane + i*numU + j+1));
		}
	}
	glEnd();

	glEndList();
}

void ProcessQuad(struct node *p1, struct node *p2, struct node *p3, struct node *p4, float ts, float rgb[3])
{
	
	float x, y, z;
	float tar = (ts - p1->t) / (p2->t - p1->t);
	if (tar >= 0 && tar <= 1){
		x = p1->x + tar * (p2->x - p1->x);
		y = p1->y + tar * (p2->y - p1->y);
		z = p1->z + tar * (p2->z - p1->z);
		glColor3fv(rgb);
		glVertex3f(x, y, z);
	}
	tar = (ts - p2->t) / (p3->t - p2->t);
	if (tar >= 0 && tar <= 1){
		x = p2->x + tar * (p3->x - p2->x);
		y = p2->y + tar * (p3->y - p2->y);
		z = p2->z + tar * (p3->z - p2->z);
		glColor3fv(rgb);
		glVertex3f(x, y, z);
	}
	tar = (ts - p3->t) / (p4->t - p3->t);
	if (tar >= 0 && tar <= 1){
		x = p3->x + tar * (p4->x - p3->x);
		y = p3->y + tar * (p4->y - p3->y);
		z = p3->z + tar * (p4->z - p3->z);
		glColor3fv(rgb);
		glVertex3f(x, y, z);
	}
	tar = (ts - p4->t) / (p1->t - p4->t);
	if (tar >= 0 && tar <= 1){
		x = p4->x + tar * (p1->x - p4->x);
		y = p4->y + tar * (p1->y - p4->y);
		z = p4->z + tar * (p1->z - p4->z);
		glColor3fv(rgb);
		glVertex3f(x, y, z);
	}
}

void Draw_Contour(struct node ** plane, GLuint list, int numT, int numU)
{
	glNewList(list, GL_COMPILE);

	int is;
	float ts, del_s = (TEMPMAX - TEMPMIN) / numS;
	for (is = 0, ts = TEMPMIN; is < numS; ++is, ts += del_s){
		float hsv[3] = { 240. - 240 * (ts - TEMPMIN) / (TEMPMAX - TEMPMIN), 1., 1. }, rgb[3];
		HsvRgb(hsv, rgb);
		glBegin(GL_LINES);
		for (int i = 0; i < numT - 1; ++i){
			for (int j = 0; j < numU - 1; ++j){
				ProcessQuad(((struct node*)plane + i*numU + j), ((struct node*)plane + (i + 1)*numU + j), ((struct node*)plane + (i + 1)*numU + j + 1), ((struct node*)plane + i*numU + j + 1), ts, rgb);
			}
		}
		glEnd();
	}
	
	glEndList();
}

void Draw_Isosurface(GLuint list)
{
	float hsv[3] = { 240. - 240 * (val_S - TEMPMIN) / (TEMPMAX - TEMPMIN), 1., 1. }, rgb[3];
	HsvRgb(hsv, rgb);

	glNewList(list, GL_COMPILE);
	glBegin(GL_LINES);

	for (int i = 0; i < NX; ++i){
		for (int j = 0; j < NY - 1; ++j){
			for (int k = 0; k < NZ - 1; ++k){
				ProcessQuad(&Nodes[i][j][k], &Nodes[i][j+1][k], &Nodes[i][j+1][k+1], &Nodes[i][j][k+1], val_S, rgb);
			}
		}
	}

	for (int i = 0; i < NY; ++i){
		for (int j = 0; j < NZ - 1; ++j){
			for (int k = 0; k < NX - 1; ++k){
				ProcessQuad(&Nodes[k][i][j], &Nodes[k][i][j+1], &Nodes[k+1][i][j+1], &Nodes[k+1][i][j], val_S, rgb);
			}
		}
	}

	for (int i = 0; i < NZ; ++i){
		for (int j = 0; j < NX - 1; ++j){
			for (int k = 0; k < NY - 1; ++k){
				ProcessQuad(&Nodes[j][k][i], &Nodes[j+1][k][i], &Nodes[j+1][k+1][i], &Nodes[j][k+1][i], val_S, rgb);
			}
		}
	}
	
	glEnd();
	glEndList();
}

void Draw_Edge(int x, int y, int z)
{
	struct node n;

	n = Nodes[x][y][z];
	glVertex3f(n.x, n.y, n.z);
	n = Nodes[x][y][NZ - 1 - z];
	glVertex3f(n.x, n.y, n.z);

	n = Nodes[x][y][z];
	glVertex3f(n.x, n.y, n.z);
	n = Nodes[x][NY - 1 - y][z];
	glVertex3f(n.x, n.y, n.z);

	n = Nodes[x][y][z];
	glVertex3f(n.x, n.y, n.z);
	n = Nodes[NX - 1- x][y][z];
	glVertex3f(n.x, n.y, n.z);
}

void Draw_CubeEdge()
{
	glNewList(CubeEdge_List, GL_COMPILE);
	glBegin(GL_LINES); 
	
	float rgb[3] = { 1., 0, 0. };
	glColor3fv(rgb);
	glLineWidth(5.);

	Draw_Edge(0, 0, 0);
	Draw_Edge(0, NY - 1, NZ - 1);
	Draw_Edge(NX - 1, 0, NZ - 1);
	Draw_Edge(NX - 1, NY - 1, 0);

	glEnd();
	glEndList();
}

void Draw_Vector()
{
	glNewList(Vector_List, GL_COMPILE);

	for (int i = 0; i < NX; ++i){
		for (int j = 0; j < NY; ++j){
			for (int k = 0; k < NZ; ++k){
				float vx, vy, vz;
				struct node n = Nodes[i][j][k];
				Vector(n.x, n.y, n.z, &vx, &vy, &vz);

				float speed = Norm(vx, vy, vz);
				if  (speed<FlowLowHigh[0] || speed >FlowLowHigh[1]) continue;

				float head[3], tail[3];
				head[0] = n.x - ScaleValue * vx / 2;
				head[1] = n.y - ScaleValue * vy / 2;
				head[2] = n.z - ScaleValue * vz / 2;

				tail[0] = n.x + ScaleValue * vx / 2;
				tail[1] = n.y + ScaleValue * vy / 2;
				tail[2] = n.z + ScaleValue * vz / 2;

				float hsv[3] = { 240. - 120 * (speed - FLOWMIN) / (FLOWMAX - FLOWMIN), 1., 1. }, rgb[3];
				HsvRgb(hsv, rgb);
				glColor3fv(rgb);

				
				glLineWidth(2.);
				Arrow(head, tail);
			}
		}
	}
	glEndList();
}

void Streamline(float x, float y, float z)
{
	glLineWidth(2.);
	glBegin(GL_LINE_STRIP);
	
	for (int i = 0; i < MAX_ITER; ++i){
		if (x < XMIN || x > XMAX) break;
		if (y < YMIN || y > YMAX) break;
		if (z < ZMIN || z > ZMAX) break;
		
		glVertex3f(x, y, z);

		float vx, vy, vz;
		Vector(x, y, z, &vx, &vy, &vz);
		
		if (Norm(vx, vy, vz) < TOLERANCE) break;

		Advect(&x, &y, &z, val_TimeStep);
	}
	glEnd();
}

void Draw_Streamlines()
{
	const int num = 10;
	struct node n;
	glNewList(Stream_List, GL_COMPILE);
	glColor3fv(Colors[CYAN]);

	for (int i = 0; i < NX; i += 2){
		for (int j = 0; j < NY; j += 2){
			for (int k = 0; k < NZ; k += 2){
				n = Nodes[i][j][k];
				Streamline(n.x, n.y, n.z);
			}
		}
	}
	//Streamline(-0.82, -1., -1.);
	glEndList();
}

void Ribbon(float p[][3])
{
	float r[Rib_Size][3];

	glShadeModel(GL_SMOOTH);
	glBegin(GL_QUADS);

	glColor3fv(Colors[GREEN]);
	for (int i = 0; i < Rib_ITER; ++i){
		memcpy(r, p, sizeof(r));
		int j = 0;
		for (; j < Rib_Size; ++j){
			glVertex3f(p[j][0], p[j][1], p[j][2]);

			float vx, vy, vz;
			Vector(p[j][0], p[j][1], p[j][2], &vx, &vy, &vz);

			if (Norm(vx, vy, vz) < 0.01) break;

			Advect(&p[j][0], &p[j][1], &p[j][2], val_TimeStep);
			if (p[j][0] < XMIN || p[j][0] > XMAX) break;
			if (p[j][1] < YMIN || p[j][1] > YMAX) break;
			if (p[j][2] < ZMIN || p[j][2] > ZMAX) break;
		}

		if (j == Rib_Size){
			for (int j = 0; j < Rib_Size - 1; ++j){
				Draw_Quads(r[j], r[j + 1], p[j + 1], p[j]);
			}
		}
		else{
			break;
		}
	}
	glEnd();

}

void Draw_Ribbon(float x, float y, float z)
{
	
	float p[Rib_Size][3];
	p[0][0] = x;
	p[0][1] = y;
	p[0][2] = z;

	for (int i = 1; i < Rib_Size; ++i){
		p[i][0] = p[i - 1][0] + Rib_Del;
		p[i][1] = p[i - 1][1];
		p[i][2] = p[i - 1][2];
	}

	glNewList(Ribbon_List, GL_COMPILE);
	Ribbon(p);
	glEndList();
}

void Draw_Quads(float p1[3], float p2[3], float p3[3], float p4[3])
{
	glVertex3fv(p1);
	glVertex3fv(p2);
	glVertex3fv(p3);
	glVertex3fv(p4);
}

void Draw_Cube(float cube[8][3])
{
	Draw_Quads(cube[0], cube[1], cube[2], cube[3]);
	Draw_Quads(cube[4], cube[5], cube[6], cube[7]);

	Draw_Quads(cube[0], cube[1], cube[5], cube[4]);
	Draw_Quads(cube[1], cube[2], cube[6], cube[5]);
	Draw_Quads(cube[2], cube[3], cube[7], cube[6]);
	Draw_Quads(cube[3], cube[0], cube[4], cube[7]);
}

void Draw_Blob(float x, float y, float z)
{
	float blob[8][3], old[8][3];
	blob[0][0] = x;
	blob[0][1] = y;
	blob[0][2] = z;

	blob[1][0] = blob[0][0] + Blob_Del;
	blob[1][1] = blob[0][1];
	blob[1][2] = blob[0][2];

	blob[2][0] = blob[0][0] + Blob_Del;
	blob[2][1] = blob[0][1];
	blob[2][2] = blob[0][2] + Blob_Del;

	blob[3][0] = blob[0][0];
	blob[3][1] = blob[0][1];
	blob[3][2] = blob[0][2] + Blob_Del;

	blob[4][0] = blob[0][0];
	blob[4][1] = blob[0][1] + Blob_Del;
	blob[4][2] = blob[0][2];

	blob[5][0] = blob[0][0] + Blob_Del;
	blob[5][1] = blob[0][1] + Blob_Del;
	blob[5][2] = blob[0][2] ;

	blob[6][0] = blob[0][0] + Blob_Del;
	blob[6][1] = blob[0][1] + Blob_Del;
	blob[6][2] = blob[0][2] + Blob_Del;

	blob[7][0] = blob[0][0];
	blob[7][1] = blob[0][1] + Blob_Del;
	blob[7][2] = blob[0][2] + Blob_Del;

	//Draw_Cube(blob);

	int end = 0;
	for (int i = 0; i < Rib_ITER; ++i){
		memcpy(old, blob, sizeof(blob));
		int j = 0;
		for (; j < 8; ++j){

			if (blob[j][0] < XMIN || blob[j][0] > XMAX) break;
			if (blob[j][1] < YMIN || blob[j][1] > YMAX) break;
			if (blob[j][2] < ZMIN || blob[j][2] > ZMAX) break;

			glVertex3f(blob[j][0], blob[j][1], blob[j][2]);

			float vx, vy, vz;
			Vector(blob[j][0], blob[j][1], blob[j][2], &vx, &vy, &vz);

			if (Norm(vx, vy, vz) < TOLERANCE) break;

			Advect(&blob[j][0], &blob[j][1], &blob[j][2], val_TimeStep);
		}

		glNewList(Blob_List + i, GL_COMPILE);
		if (j == 8 && end == 0){
			glShadeModel(GL_SMOOTH);
			glBegin(GL_QUADS);
			glColor3fv(Colors[GREEN]);

			Draw_Cube(old);

			glEnd();
		}
		else{ end = 1; }
		glEndList();
	}


}

void Draw_Single_Stream_Probe(float x, float y, float z)
{
	glNewList(Probe_Single_Stream_List, GL_COMPILE);
	glColor3fv(Colors[GREEN]);

	Streamline(x, y, z);

	glEndList();
}

void Draw_Multi_Stream_Probe(float x, float y, float z)
{
	float s[Stream_Num * 3][3];
	s[0][0] = x;
	s[0][1] = y;
	s[0][2] = z;
	for (int i = 1; i < 3 * Stream_Num; ++i){
		memcpy(s[i], s[0], sizeof(s[0]));
	}

	for (int i = 1; i < Stream_Num; ++i){
		s[i][0] = s[i - 1][0] + Stream_Del;
	}
	for (int i = Stream_Num; i < 2 * Stream_Num; ++i){
		s[i][1] = s[i - 1][1] + Stream_Del;
	}
	for (int i = 2 * Stream_Num; i < 3 * Stream_Num; ++i){
		s[i][2] = s[i - 1][2] + Stream_Del;
	}

	glNewList(Probe_Multi_Stream_List, GL_COMPILE);
	glColor3fv(Colors[GREEN]);
	for (int i = 0; i < Stream_Num * 3; ++i){
		Streamline(s[i][0], s[i][1], s[i][2]);
	}
	glEndList();

}

void Draw_Probe()
{
	glNewList(Probe_List, GL_COMPILE);
	glShadeModel(GL_SMOOTH);
	glBegin(GL_QUADS);
	glColor3fv(Colors[BLUE]);

	float probe[4][3];
	for (int i = 0; i < 4; memcpy(probe[i], ProbeXYZ, sizeof(ProbeXYZ)), i++);
	probe[1][0] += Blob_Del;
	probe[2][0] += Blob_Del;
	probe[2][1] += Blob_Del;
	probe[3][1] += Blob_Del;
	Draw_Quads(probe[0], probe[1], probe[2], probe[3]);

	glEnd();
	glEndList();

	Draw_Ribbon(ProbeXYZ[0], ProbeXYZ[1], ProbeXYZ[2]);

	Draw_Multi_Stream_Probe(ProbeXYZ[0], ProbeXYZ[1], ProbeXYZ[2]);
	Draw_Single_Stream_Probe(ProbeXYZ[0], ProbeXYZ[1], ProbeXYZ[2]);

	Draw_Blob(ProbeXYZ[0], ProbeXYZ[1], ProbeXYZ[2]);

}

void Draw_US_Map()
{
	for (int i = 0; i < 68; i++)
	{
		glBegin(GL_LINE_STRIP);
		glColor3fv(Colors[WhichColor]);
		for (int j = 0; j < Npts[i]; j++)
		{
			if (HiDeOn && j < Npts[i] - 1){
				Draw_HyperbolicLine(X[i][j], Y[i][j], X[i][j + 1], Y[i][j + 1]);
			}
			else{
				float x = X[i][j];
				float y = Y[i][j];
				Hyperbolic(&x, &y, val_K, PolarOn);
				glVertex2f(x, y);
			}
		}
		glEnd();
	}
}

void Draw_HyperbolicLine(float px1, float py1, float px2, float py2)
{
	float ax = (px1 + px2) / 2;
	float ay = (py1 + py2) / 2;

	float _ax = ax, _ay = ay;
	Hyperbolic(&_ax, &_ay, val_K, PolarOn);

	float _px1 = px1, _py1 = py1, _px2 = px2, _py2 = py2;
	Hyperbolic(&_px1, &_py1, val_K, PolarOn);
	Hyperbolic(&_px2, &_py2, val_K, PolarOn);

	float _bx = (_px1 + _px2) / 2;
	float _by = (_py1 + _py2) / 2;

	if (sqrt(pow((_ax - _bx), 2) + pow((_ay - _by), 2)) < TOR){
		glVertex2f(_px1, _py1);
		glVertex2f(_px2, _py2);
	}
	else{
		Draw_HyperbolicLine(px1, py1, ax, ay);
		Draw_HyperbolicLine(ax, ay, px2, py2);
	}

}

void Draw_Texture_Map(GLuint Tex )
{
	

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, Tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	if (Which_Filter){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	switch (Which_Envi){
	case 0: glDisable(GL_TEXTURE_2D); break;
	case 1:
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);
		glEnable(GL_TEXTURE_2D);
		break;
	case 2:
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);
		glEnable(GL_TEXTURE_2D);
		break;
	}


	glShadeModel(GL_SMOOTH);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < NUMLATS-1; ++i){
		for (int j = 0; j < NUMLNGS-1; ++j){

			Draw_Texture_Triangle(i, j, i + 1, j, i + 1, j + 1);
			Draw_Texture_Triangle(i, j, i, j + 1, i + 1, j + 1);

		}
	}
	glEnd();


	glDisable(GL_TEXTURE_2D);

}

void Draw_Texture_Triangle(int i1, int j1, int i2, int j2, int i3, int j3)
{
	float s, t;
	s = (Points[i1][j1].lng - LNGMIN) / (LNGMAX - LNGMIN);	
	t = (Points[i1][j1].lat - LATMIN) / (LATMAX - LATMIN);
	t = t * 0.718;
	GraVec(i1, j1);
	glTexCoord2f(s, t);
	glVertex3f(Points[i1][j1].lng, -HeightExag*Points[i1][j1].hgt, Points[i1][j1].lat);

	s = (Points[i2][j2].lng - LNGMIN) / (LNGMAX - LNGMIN);
	t = (Points[i2][j2].lat - LATMIN) / (LATMAX - LATMIN);
	t = t * 0.718;
	GraVec(i2, j2);
	glTexCoord2f(s, t);
	glVertex3f(Points[i2][j2].lng, -HeightExag*Points[i2][j2].hgt, Points[i2][j2].lat);

	s = (Points[i3][j3].lng - LNGMIN) / (LNGMAX - LNGMIN);
	t = (Points[i3][j3].lat - LATMIN) / (LATMAX - LATMIN);
	t = t * 0.718;
	GraVec(i3, j3);
	glTexCoord2f(s, t);
	glVertex3f(Points[i3][j3].lng, -HeightExag*Points[i3][j3].hgt, Points[i3][j3].lat);

}

void GraVec(int z, int x)
{
	float v01[3], v02[3], norm[3];
	if (x == 0){
		v01[0] = Points[z][x + 1].lng - Points[z][x].lng;
		v01[1] = HeightExag*LightingHeightFactor*(Points[z][x + 1].hgt - Points[z][x].hgt);
	}
	else{
		if (x == NUMLNGS)
		{
			v01[0] = Points[z][x].lng - Points[z][x - 1].lng;
			v01[1] = HeightExag*LightingHeightFactor*(Points[z][x].hgt - Points[z][x - 1].hgt);
		}
		else{
			v01[0] = Points[z][x + 1].lng - Points[z][x - 1].lng;
			v01[1] = HeightExag*LightingHeightFactor*(Points[z][x + 1].hgt - Points[z][x - 1].hgt);
		}
	}
	v01[2] = 0;

	v02[0] = 0.;
	if (x == 0){
		v02[1] = HeightExag*LightingHeightFactor*(Points[z + 1][x].hgt - Points[z][x].hgt);
		v02[2] = Points[z + 1][x].lat - Points[z][x].lat;
	}
	else{
		if (x == NUMLNGS)
		{
			v02[1] = HeightExag*LightingHeightFactor*(Points[z][x].hgt - Points[z - 1][x].hgt);
			v02[2] = Points[z][x].lat - Points[z - 1][x].lat;
		}
		else{
			v02[1] = HeightExag*LightingHeightFactor*(Points[z + 1][x].hgt - Points[z - 1][x].hgt);
			v02[2] = Points[z + 1][x].lat - Points[z - 1][x].lat;
		}
	}

	Cross(v01, v02, norm);
	Unit(norm, norm);

	float inten = fabs(norm[1]);
	glColor3f(inten, inten, inten);
	glNormal3fv(norm);
}

void Draw_Cloud(float time)
{
	float si = sin(time * 2 * PI), co = cos(time * 2 * PI);
	float radius = 170.;
	float pos[3] = { radius * co, -10., radius * si };

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);

	glColor4f(0.9, 0.5, 0.5, 0.5);

	float range = 50.;
	glVertex3f(pos[0] + range, pos[1], pos[2]);
	glVertex3f(pos[0], pos[1], pos[2] + range);
	glVertex3f(pos[0] - range, pos[1], pos[2]);
	glVertex3f(pos[0], pos[1], pos[2] - range);

	glEnd();
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void
RideDisplay()
{
	if (DebugOn != 0)
	{
		fprintf(stderr, "Display\n");
	}

	glDisable(GL_DEPTH);
	// set which window we want to do the graphics into:

	glutSetWindow(RideWindow);


	// erase the background:

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);


	// specify shading to be flat:

	glShadeModel(GL_FLAT);


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (WhichProjection == ORTHO)
		glOrtho(-3., 3., -3., 3., 0.1, 1000.);
	else
		gluPerspective(90., 1., 0.1, 1000.);

	//gluOrtho2D(-10., 10., -10., 10.);
	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// set the eye position, look-at position, and up-vector:
	// IF DOING 2D, REMOVE THIS -- OTHERWISE ALL YOUR 2D WILL DISAPPEAR !
	float si = sin(Time * 2 * PI), co = cos(Time * 2 * PI);
	float radius = 170.;
	float pos1[3] = { radius * co, -30., radius * si };
	si = sin(Time * 2 * PI + 0.5); co = cos(Time * 2 * PI + 0.5);
	float pos2[3] = { radius * co, 0., radius * si };

	gluLookAt(pos1[0], pos1[1], pos1[2],pos2[0], pos2[1], pos2[2], 0., -1., 0.);
/*
	glTranslatef((GLfloat)TransXYZ[0], (GLfloat)TransXYZ[1], -(GLfloat)TransXYZ[2]);


	// rotate the scene:
	// DO NOT ROTATE (EXCEPT ABOUT Z) IF YOU ARE DOING 2D !
	
	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);
	glMultMatrixf((const GLfloat *)RotMatrix);


	// uniformly scale the scene:

	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	GLfloat scale2 = 1. + Scale2;		// because glui translation starts at 0.
	if (scale2 < MINSCALE)
		scale2 = MINSCALE;
	glScalef((GLfloat)scale2, (GLfloat)scale2, (GLfloat)scale2);
*/

	// set the fog parameters:
	// DON'T NEED THIS IF DOING 2D !

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}


	// set the color of the object:

	glColor3fv(Colors[WhichColor]);
	glDisable(GL_DEPTH_TEST);

	// draw the current object:
	Draw_Texture_Map(RideTerrainTex);
	if (CloudOn){
		Draw_Cloud(Time);
	}



	// swap the double-buffered framebuffers:

	glutSwapBuffers();


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();

}

void
GraphDisplay()
{
	if (DebugOn != 0)
	{
		fprintf(stderr, "Display\n");
	}

	glDisable(GL_DEPTH);
	// set which window we want to do the graphics into:

	glutSetWindow(GraphWindow);


	// erase the background:

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);


	// specify shading to be flat:

	glShadeModel(GL_FLAT);

	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/*
	if (WhichProjection == ORTHO)
		glOrtho(-3., 3., -3., 3., 0.1, 1000.);
	else
		gluPerspective(90., 1., 0.1, 1000.);
		*/

	gluOrtho2D(-1., 11., -4., 4.);
	// place the objects into the scene:

	glColor3fv(Colors[YELLOW]);
	glLineWidth(1.);

	glBegin(GL_LINE_STRIP);

	glVertex2f(0., 3.);
	glVertex2f(0., 2.);
	glVertex2f(10., 2.);
	glVertex2f(0., 2.);
	glVertex2f(0., 1.);
	glVertex2f(10., 1.);

	glEnd();
	glBegin(GL_LINE_STRIP);

	glVertex2f(0., -1.);
	glVertex2f(0., -2.);
	glVertex2f(10., -2.);
	glVertex2f(0., -2.);
	glVertex2f(0., -3.);
	glVertex2f(10., -3.);


	glEnd();

	glShadeModel(GL_SMOOTH);
	glColor3fv(Colors[GREEN]);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 1000; ++i){
		glVertex2f(i / 100., cos(i / 1000. * 2 * PI) + 2.);
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 1000; ++i){
		glVertex2f(i / 100., sin(i / 1000. * 2 * PI) - 2.);
	}
	glEnd();
	
	glColor3fv(Colors[RED]);
	glBegin(GL_QUADS);
	float pos[2] = { cos(Time * 2 * PI) + 2., sin(Time * 2 * PI) - 2.};
	float range = 0.2;

	glVertex2f(Time * 10. + range, pos[0]);
	glVertex2f(Time * 10., pos[0] + range);
	glVertex2f(Time * 10. - range, pos[0]);
	glVertex2f(Time * 10., pos[0] - range);

	glVertex2f(Time * 10. + range, pos[1]);
	glVertex2f(Time * 10., pos[1] + range);
	glVertex2f(Time * 10. - range, pos[1]);
	glVertex2f(Time * 10., pos[1] - range);

	glEnd();

	glutSwapBuffers();


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();

}
