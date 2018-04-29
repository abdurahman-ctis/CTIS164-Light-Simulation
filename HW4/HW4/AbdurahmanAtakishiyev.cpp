/*****
CTIS164 - Lab12
Vectors: Light Simulation
*****/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"


#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

typedef struct {
	float r, g, b;
} color_t;

typedef struct {
	vec_t   pos;
	color_t color;
	vec_t   vel;
} light_t;

typedef struct {
	vec_t pos;
	vec_t N;
} vertex_t;

#define NUM 4

light_t light[4] =
{
	{ { 0, 0 },{ 1, 0, 0 },{ 3,  2 } },
	{ { 200, 0 },{ 0, 1, 0 },{ -2, -1 } },
	{ { -200, 0 },{ 0, 0, 1 },{ 3, -2 } },
	{ {0 ,0} ,{1,1,1},{0,0}}
};

color_t mulColor(float k, color_t c) {
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}

color_t addColor(color_t c1, color_t c2) {
	color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
	return tmp;
}

// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 350 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d) {
	return (-1.0 / 350.0) * d + 1.0;
}

color_t calculateColor(light_t source, vertex_t v) {
	vec_t L = subV(source.pos, v.pos);
	vec_t uL = unitV(L);
	float factor = dotP(uL, v.N) * distanceImpact(magV(L));
	return mulColor(factor, source.color);
}


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, char *string, void *font)
{
	int len, i;
	 
	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

//
// To display onto window using OpenGL commands
//
void temp(float x, float y, float r) {
#define PI 3.1415
	float angle;
	glBegin(GL_TRIANGLE_FAN);	glColor3ub(47, 79, 79);

	glVertex2f(x, y);
	for (int i = 0; i <= 100; i++)
	{
		

		angle = 2 * PI*i / 100;
		vertex_t P = { { 100 - x + r*cos(angle), 250  - y + r*sin(angle) },{ 0, 1 } };

		color_t res = { 0, 0, 0 };
		for (int i = 0; i < NUM; i++) {
			res = addColor(res, calculateColor(light[i], P));
		}
		glColor3f(res.r, res.g, res.b);
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}
void drawFilledCircle(float x, float y, float radius) {
	int i;
	int triangleAmount = 40; //# of triangles used to draw circle

							 //GLfloat radius = 0.8f; //radius
	float twicePi = 2.0f * PI;

	glBegin(GL_TRIANGLE_FAN);

	glColor3ub(47, 79, 79);
	glVertex2f(x, y); // center of circle

	for (i = 0; i <= triangleAmount; i++) {
		vertex_t P = { { x + (radius * cos(i *  twicePi / triangleAmount)), y + (radius * sin(i * twicePi / triangleAmount)) },
		{ 0, 300-y+(radius * sin(i * twicePi / triangleAmount)) } };

		color_t res = { 0, 0, 0 };
		for (int i = 0; i < NUM; i++) {
			res = addColor(res, calculateColor(light[i], P));
		}
		glColor3f(res.r, res.g, res.b);
		glVertex2f(
			x + (radius * cos(i *  twicePi / triangleAmount)),
			y + (radius * sin(i * twicePi / triangleAmount))
		);
	}
	glEnd();
}
void display() {

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// r,g,b sources 
	for (int i = 0; i < NUM - 1; i++) {
		glColor3f(light[i].color.r, light[i].color.g, light[i].color.b);
		circle(light[i].pos.x, light[i].pos.y, 10);
	}

	// sun
	glColor3f(light[3].color.r, light[3].color.g, light[3].color.b);

	circle(light[3].pos.x, light[3].pos.y, 30);

	
	drawFilledCircle(0, 300, 20);
	glColor3f(1, 0, 0);

	temp(100, 250, 20);

	for (int x = -400; x <= 400; x++) {
		vertex_t P = { { x, -100 },{ 0, 1 } };

		color_t res = { 0, 0, 0 };
		for (int i = 0; i < NUM; i++) {
			res = addColor(res, calculateColor(light[i], P));
		}


		glBegin(GL_LINES);
		glColor3f(res.r, res.g, res.b);
		glVertex2f(x, -100);

		glColor3f(0, 0, 0);
		glVertex2f(x, -400);
		glEnd();
	}

	glutSwapBuffers();

}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.
	x = x - winWidth / 2;
	y = winHeight / 2 - y;
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
		printf("%d %d\n", x, y);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

void onMove(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	for (int i = 0; i < NUM; i++) {
		light[i].pos = addV(light[i].pos, light[i].vel);

		// Reflection from Walls.
		if (light[i].pos.x > 390 || light[i].pos.x < -390) {
			light[i].vel.x *= -1;
		}

		if (light[i].pos.y > 390 || light[i].pos.y < -90) {
			light[i].vel.y *= -1;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char *argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Light Simulation");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}