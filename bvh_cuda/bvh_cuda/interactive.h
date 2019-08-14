#pragma once
#ifndef INTERACTIVE_H
#define INTERACTIVE_H
#include <Windows.h>
#include <glew.h>
#include <freeglut.h>
#include "skeleton.h"

double move_x = 280, move_y = 280, move_z = 280;
int oldposx = 0, oldposy = 0;
double radius = 0.3;
double angle_x = 0, angle_y = 0;
extern double target_x, target_y, target_z;
int op = 0;
extern bool is_move;

float framesPerSecond = 0.0f;
float lastTime = 0.0f;
int fps;

extern Skeleton skeleton;

void mymouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		oldposx = x; oldposy = y;
	}

	glutPostRedisplay();
}

void rotate(int x, int y)
{
	int delta_x = x - oldposx;
	int delta_y = y - oldposy;
	angle_x += (double)delta_x;
	angle_y += (double)delta_y;
	if (angle_x >= 1080) angle_x -= 1080;
	if (angle_y >= 1080) angle_y -= 1080;
	oldposx = x;
	oldposy = y;
	glutPostRedisplay();
}

void setLight()
{
	GLfloat lightPosition1[] = { 0,100,160,0 };
	GLfloat light_ambient[] = { 1.0 , 1.0 , 1.0 , 1.0 };
	GLfloat light_diffuse[] = { 0.6 , 0.6 , 0.6 , 1.0 };
	GLfloat  light_specular[] = { 0.5, 0.5, 0.5, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);

	GLfloat lightPosition2[] = { 0,100,-160,0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}

void init()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("BVH_PLAY");
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	setLight();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLdouble)width / (GLdouble)height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void mykeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w': skeleton.joints[op]->channels[X_ROTATION] += 0.1; break;
	case 's': skeleton.joints[op]->channels[X_ROTATION] -= 0.1; break;
	case 'a': skeleton.joints[op]->channels[Y_ROTATION] += 0.1; break;
	case 'd': skeleton.joints[op]->channels[Y_ROTATION] -= 0.1; break;
	case 'q': skeleton.joints[op]->channels[Z_ROTATION] += 0.1; break;
	case 'e': skeleton.joints[op]->channels[Z_ROTATION] -= 0.1; break;
	case 'i':skeleton.joints[op]->offset.y() += 0.1; break;
	case 'k':skeleton.joints[op]->offset.y() -= 0.1; break;
	case 'j':skeleton.joints[op]->offset.x() += 0.1; break;
	case 'l':skeleton.joints[op]->offset.x() -= 0.1; break;
	case 'u':skeleton.joints[op]->offset.z() += 0.1; break;
	case 'o':skeleton.joints[op]->offset.z() -= 0.1; break;
	case 'm': is_move = ~is_move; break;
	case ',': {move_x += 10; move_y += 10; move_z += 10; }break;
	case '.': {move_x -= 10; move_y -= 10; move_z -= 10; }break;
	}
	glutPostRedisplay();
}

void switchop(GLint key, GLint x, GLint y)
{
	if (key == GLUT_KEY_UP)
	{
		op++;
		op = op % (skeleton.joints.size());
	}
	else if (key == GLUT_KEY_DOWN)
	{
		op--;
		if (op < 0) op += (skeleton.joints.size());
	}
	std::cout << op << " " << skeleton.joints[op]->name << std::endl;
	glutPostRedisplay();
}

void  drawMessage(int line_no, const char * message)
{
	int   i;
	if (message == NULL)
		return;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, 800, 800, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glColor3f(1.0, 0.0, 0.0);
	glRasterPos2i(8, 24 + 18 * line_no);
	for (i = 0; message[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void CalculateFrameRate()
{
	float currentTime = GetTickCount() * 0.001f;
	++framesPerSecond;
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		//printf("Current Frames Per Second: %d \n", int(framesPerSecond));
		fps = int(framesPerSecond);
		framesPerSecond = 0;
	}
	char message[128];
	sprintf(message, "FPS : %d ", fps);
	drawMessage(0, message);
}


#endif