#include <glew.h>
#include "skeleton.h"
#include "interactive.h"

using namespace std;

Skeleton skeleton;
Skeleton cmu_skeleton;
bool is_move = false;

int frame = 0;


void display()
{
	glClearColor(176 / 255.0, 196 / 255.0, 222 / 255.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	gluLookAt(move_x, move_y, move_z, skeleton.joints[0]->position.x(), skeleton.joints[0]->position.y(), skeleton.joints[0]->position.z(), 0, 1, 0);
	glRotatef(angle_x*radius, 0, 1, 0);
	glRotatef(angle_y*radius, 1, 0, 0);

	if (is_move == true)
	{
		skeleton.updateJoint((frame++) % 344);

		double ox, oz;
		float size = 30.0f;
		int num_x = 20, num_z = 20;

		ox = -(num_x*size) / 2.0;
		for (int x = 0; x < num_x; x++, ox += size)
		{
			oz = -(num_z*size) / 2;
			for (int z = 0; z < num_z; z++, oz += size)
			{
				if ((x + z) % 2 == 0)
				{
					GLfloat mat[] = { 0,0,0,1 };
					glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
					glBegin(GL_QUADS);
					glNormal3f(0, 1, 0);
					glVertex3d(ox, 88.836, oz);
					glVertex3d(ox, 88.836, oz + size);
					glVertex3d(ox + size, 88.836, oz + size);
					glVertex3d(ox + size, 88.836, oz);
					glEnd();
				}

			}
		}
	}
	else skeleton.updateJoint();
	skeleton.updateMesh();

	skeleton.joints[0]->draw();
	skeleton.drawMeshFace();
	skeleton.drawMeshWire();

	glColor3d(1, 1, 0);
	glPointSize(20.0f);
	glBegin(GL_POINTS);
	glVertex3d(skeleton.joints[op]->position.x(), skeleton.joints[op]->position.y(), skeleton.joints[op]->position.z());
	glEnd();

	glPopMatrix();

	CalculateFrameRate();

	glutSwapBuffers();
	glutPostRedisplay();

}


int main(int argc, char* argv[])
{
	glutInit(&argc, (char**)argv);
	init();

	skeleton.readBVH("F:/图华实习工程文件/ophuman.bvh");
	skeleton.readInfo("F:/图华实习工程文件/humaninfo.txt");
	cmu_skeleton.readBVH("F:/图华实习工程文件/1.bvh");

	for (int i = 0; i < skeleton.joints.size(); i++)
	{
		if ((i >= 8 && i <= 20) || (i >= 22 && i <= 34))
		{
			double length = skeleton.joints[i]->offset.norm();
			if (skeleton.joints[i]->name == "rShldr" || skeleton.joints[i]->name == "lShldr")
			{
				length *= 1.5;
			}
			cmu_skeleton.joints[i]->offset.normalize();
			skeleton.joints[i]->offset = cmu_skeleton.joints[i]->offset*length;
		}
	}
	skeleton.motion_block = cmu_skeleton.motion_block;
	skeleton.updateJoint();
	skeleton.mallocSpace();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(mykeyboard);
	glutSpecialFunc(switchop);
	glutMouseFunc(mymouse);
	glutMotionFunc(rotate);
	glutMainLoop();

	skeleton.freeSpace();

	return 0;

}