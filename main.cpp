#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

class Particle
{
public:
	static const int DEFAULT_LIFE_DURATION = 1800;
	float position[3];
	float direction[3];
	float color[3];
	float speed;
	float rotation_angel[3];
	float size;
	int age;
	int mat;
	Particle();
	Particle(float pX, float pY, float pZ, float dX, float dY, float dZ);
};

Particle::Particle()
{
	position[0] = 0;
	position[1] = 0;
	position[2] = 0;
	color[0] = 0;
	color[1] = 1;
	color[2] = 1;
	speed = 0;
	rotation_angel[0] = rand() % 360;
	rotation_angel[1] = rand() % 360;
	rotation_angel[2] = rand() % 360;
	size = 0.1;
	age = 1200;
	mat = 0;
}

Particle::Particle(float pX, float pY, float pZ, float dX, float dY, float dZ)
{
	position[0] = 0;
	position[1] = 0;
	position[2] = 0;
	color[0] = 0;
	color[1] = 1;
	color[2] = 1;
	speed = 0;
	float r[3] = {0, 0, 0};
	rotation_angel[0] = rand() % 360;
	rotation_angel[1] = rand() % 360;
	rotation_angel[2] = rand() % 360;
	size = 0.1;
	age = 1200;
	mat = rand() % 5 + 1;
	position[0] = pX;
	position[1] = pY;
	position[2] = pZ;
	direction[0] = dX;
	direction[1] = dY;
	direction[2] = dZ;
	speed = 0.2 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) / 5; // float: 0 to 12.
}

vector<Particle> parts;

int countdown = 10; //for addparticles. i.e. add a particle per 10 frames.

bool pause = false;
bool friction = false;
float floor1[3] = {-7.5, 0, 0};
float floor2[3] = {10, 0, -10};
float floor3[3] = {17.5, 0, 0};
float floor4[3] = {10, 0, 15};
float origin[3] = {0, 0, 0};
float hole[3] = {10, 0, 5};
float cols[6][3] = {{1, 0, 0}, {0, 1, 1}, {1, 1, 0}, {0, 1, 0}, {0, 0, 1}, {1, 0, 1}};
float camPos[] = {20, 20, 40};
float upPos[] = {0, 1, 0};
/* LIGHTING */
bool light_on = true;
int light_idx = 0;
float light_pos[4] = {10, 10, 20, 1};
float amb[4] = {1, 0.4, 0.4, 1};
float diff[4] = {0.5, 0.7, 0.7, 1};
float spec[4] = {1, 1, 1, 1};
float light_pos2[4] = {-3, -3, 1, 1};
float amb2[4] = {0.5, 1, 0.5, 1};
float diff2[4] = {0.5, 0.5, 0.5, 1};
float spec2[4] = {1, 1, 1, 1};
bool partcam = false;
bool catchpart = false;
bool holeswitch = true;
int partidx = -1;
/* Materials */
/*ground,particle1-particle5*/ //particel1 is turquoise,particle2 is gold,particel3 is purple,particle4 is black,particle5 is pearl
float ambMat[6][4] = {{0.5, 0.5, 0.5, 1}, {0.1, 0.18725, 0.1745, 1}, {0.24725, 0.1995, 0.0745, 1}, {0.2, 0.2, 0.6, 1}, {0.25, 0.20725, 0.20725, 1}, {0.05375, 0.05, 0.06625, 1}};
float diffMat[6][4] = {{0.5, 0, 0, 1}, {0.396, 0.74151, 0.69102, 1}, {0.75164, 0.60648, 0.22648, 1}, {1, 0, 1, 1}, {1, 0.829, 0.829, 1}, {0.18275, 0.17, 0.22525, 1}};
float specMat[6][4] = {{0, 0.5, 0, 1}, {0.297254, 0.30829, 0.306678, 1}, {0.628281, 0.555802, 0.366065, 1}, {1, 1, 1, 1}, {0.296648, 0.296648, 0.296648, 1}, {0.332741, 0.328634, 0.346435, 1}};

// vector<Particle> fire;
void firework(float x, float z)
{
	Particle newPart(x, 0, z, 0, 1, 0);
	newPart.age = 1800 - 120;
	newPart.speed = 0.1;
	newPart.size = 0.4;
	parts.push_back(newPart);
}

void firework1(float dx, float dy, float dz, int mat)
{
	for (int i = 0; i < 50; i++)
	{
		float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5;
		float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5;
		float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5;
		float length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		Particle newPart(dx, dy, dz, x / length, y / length, z / length);
		newPart.age = 1800 - 150;
		newPart.speed = 1;
		newPart.size = 0.2;
		newPart.mat = mat;
		parts.push_back(newPart);
	}
}

void addPart()
{
	float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5;
	float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5;
	float y = 10;
	float length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	Particle newpart(origin[0], origin[1], origin[2], x / length, y / length, z / length);
	if (catchpart)
	{
		parts.insert(parts.begin(), newpart);
		partidx = 0;
		catchpart = false;
	}
	parts.push_back(newpart);
}

/* drawPolygon - takes 4 indices and an array of vertices
 *   and draws a polygon using the vertices indexed by the indices
 */
void drawPolygon(int a, int b, int c, int d, float v[8][3])
{
	glBegin(GL_POLYGON);
	glVertex3fv(v[a]);
	glVertex3fv(v[b]);
	glVertex3fv(v[c]);
	glVertex3fv(v[d]);
	glEnd();
}

/* cube - takes an array of 8 vertices, and draws 6 faces
 *  with drawPolygon, making up a box
 */
void cube(float v[8][3])
{
	//  faces[6][4];
	glColor3f(ambMat[4][0], ambMat[4][1], ambMat[4][2]);
	//	glNormal3f(0,1,0);
	drawPolygon(0, 3, 2, 1, v);

	// glColor3fv(cols[2]);
	drawPolygon(1, 0, 4, 5, v);

	// glColor3fv(cols[3]);
	drawPolygon(5, 1, 2, 6, v);

	// glColor3fv(cols[4]);
	drawPolygon(2, 3, 7, 6, v);

	// glColor3fv(cols[5]);
	drawPolygon(6, 5, 4, 7, v);

	// glColor3fv(cols[0]);
	drawPolygon(4, 0, 3, 7, v);
}

/* drawBox - takes centre point, width, height and depth of a box,
 *  calculates its corner vertices, and draws it with the cube function
 */
void drawBox(float *c, float w, float h, float d)
{
	float vertices[8][3] = {{c[0] - w / 2, c[1] - h / 2, c[2] + d / 2},
							{c[0] - w / 2, c[1] + h / 2, c[2] + d / 2},
							{c[0] + w / 2, c[1] + h / 2, c[2] + d / 2},
							{c[0] + w / 2, c[1] - h / 2, c[2] + d / 2},
							{c[0] - w / 2, c[1] - h / 2, c[2] - d / 2},
							{c[0] - w / 2, c[1] + h / 2, c[2] - d / 2},
							{c[0] + w / 2, c[1] + h / 2, c[2] - d / 2},
							{c[0] + w / 2, c[1] - h / 2, c[2] - d / 2}};

	cube(vertices);
}
void keyboard(unsigned char key, int x, int y)
{
	//cout << key << endl;
	/* key presses move the cube, if it isn't at the extents (hard-coded here) */
	switch (key)
	{
	case 'a':
		firework(rand() % 10, rand() % 10);
		break;
	case 'p':
		if (partcam)
		{
			partidx = -1;
			catchpart = false;
		}
		else
		{
			catchpart = true;
		}
		partcam = !partcam;
		break;
	case 'q':
	case 27:
		exit(0);
		break;
	case 'f':
		friction = !friction;
		break;
	case 'F':
		friction = !friction;
		break;

	case 'r':
		while (parts.size() != 0)
		{
			parts.pop_back();
		}
		break;
	case 'R':
		while (parts.size() != 0)
		{
			parts.pop_back();
		}
		break;
	case 32:
		pause = !pause;
		break;
	case 'l':
		if (light_on)
		{
			glDisable(GL_LIGHTING);
		}
		if (!light_on)
		{
			glEnable(GL_LIGHTING);
			light_idx = 0;
		}
		light_on = !light_on;
		break;
	case 49:
		if (light_idx == 1)
		{
			light_idx = 0;
		}
		else
		{
			light_idx = 1;
		}
		break;
	case 50:
		if (light_idx == 2)
		{
			light_idx = 0;
		}
		else
		{
			light_idx = 2;
		}
		break;
	case 'h':
		holeswitch = !holeswitch;
		break;
	}

	glutPostRedisplay();
}

//rotate camera
float scaley = 0;
float scalex = 0;
void special(int key, int x, int y)
{
	float radius = 15 * sqrt(6);
	switch (key)
	{
		//camera rotate about y axis
	case GLUT_KEY_RIGHT:
		scaley = scaley + 0.03;
		camPos[0] = radius * sin(scaley);
		camPos[2] = radius * cos(scaley);
		light_pos[0] = camPos[0];
		//light_pos[2] = camPos[2];
		break;
	}
	switch (key)
	{
	case GLUT_KEY_LEFT:
		scaley = scaley - 0.03;
		camPos[0] = radius * sin(scaley);
		camPos[2] = radius * cos(scaley);
		break;
	}
	switch (key)
	{
		//camera rotate about x axis
	case GLUT_KEY_UP:
		scalex = scalex + 0.03;
		camPos[1] = radius * sin(scalex);
		camPos[2] = radius * cos(scalex);
		break;
	}
	switch (key)
	{
	case GLUT_KEY_DOWN:
		scalex = scalex - 0.03;
		camPos[1] = radius * sin(scalex);
		camPos[2] = radius * cos(scalex);
		break;
	}
	glutPostRedisplay();
}

void init(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glClearColor(0.5, 0.5, 0.5, 0);
	glColor3f(1, 1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//	glOrtho(-5, 5, -5, 5, -5, 80);
	gluPerspective(45, 1, 1, 100);
}

void drawparts()
{
	//srand(time(NULL));
	glPushMatrix();
	glTranslatef(0, 1, 0);
	for (int i = 0; i < parts.size(); ++i)
	{
		int mat = parts[i].mat;
		glPushMatrix();
		glTranslatef(parts[i].position[0], parts[i].position[1], parts[i].position[2]);
		glRotatef(parts[i].rotation_angel[0], 1, 0, 0);
		glRotatef(parts[i].rotation_angel[1], 0, 1, 0);
		glRotatef(parts[i].rotation_angel[2], 0, 0, 1);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[parts[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[parts[i].mat]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[parts[i].mat]);
		if (parts[i].mat == 4)
		{
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.25 * 128);
		}
		else
		{
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128 * 1.0 * parts[i].mat / 10);
		}
		glColor3f(ambMat[mat][0], ambMat[mat][1], ambMat[mat][2]);
		glutSolidSphere(0.2, 16, 16);

		glPopMatrix();
	}
	glPopMatrix();
}

/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	if (light_on)
	{
		switch (light_idx)
		{
		case 1:
			glEnable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			break;
		case 2:
			glEnable(GL_LIGHT1);
			glDisable(GL_LIGHT0);
			break;
		default:
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHT1);
			break;
		}
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (partcam && !catchpart)
	{
		gluLookAt(parts[0].position[0], parts[0].position[1], parts[0].position[2], 0, 0, 0, upPos[0], upPos[1], upPos[2]);
	}
	else
	{
		gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, upPos[0], upPos[1], upPos[2]);
	}
	glColor3f(1, 1, 1);

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos2);
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat[0]);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 27);
	//ground material
	drawBox(floor1, 25, 1, 40);
	drawBox(floor2, 10, 1, 20);
	drawBox(floor3, 5, 1, 40);
	drawBox(floor4, 10, 1, 10);

	if (!holeswitch)
	{

		drawBox(hole, 10, 1, 10);
	}

	drawparts();

	glutSwapBuffers();
}

void deleteParticle(int i)
{
	parts[i] = parts[parts.size() - 1];
	parts.pop_back();
}

void moveAll()
{
	for (int i = 0; i < parts.size(); i++)
	{
		parts[i].position[0] += parts[i].direction[0] * parts[i].speed;
		parts[i].position[1] += parts[i].direction[1] * parts[i].speed;
		parts[i].position[2] += parts[i].direction[2] * parts[i].speed;
		float x, y, z, length;
		x = parts[i].direction[0] * parts[i].speed;
		if (parts[i].mat != 5)
		{
			y = parts[i].direction[1] * parts[i].speed - 0.03;
		}
		else
		{
			y = parts[i].direction[1] * parts[i].speed;
		}
		z = parts[i].direction[2] * parts[i].speed;
		length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		parts[i].direction[0] = x / length;
		parts[i].direction[1] = y / length;
		parts[i].direction[2] = z / length;
		if (parts[i].position[1] < 0 && parts[i].direction[1] < 0)
		{
			if (parts[i].mat == 4)
			{
				parts[i].speed * 1.1;
			}
			if (parts[i].position[0] < 20 && parts[i].position[0] > -20 && parts[i].position[2] < 20 && parts[i].position[2] > -20)
			{
				if (parts[i].position[0] < 15 && parts[i].position[0] > 5 && parts[i].position[2] < 10 && parts[i].position[2] > 0 && holeswitch)
				{
				}
				else
				{
					parts[i].direction[1] = -parts[i].direction[1];
					if (!friction)
					{
						parts[i].speed *= 0.9;
					}
				}
			}
		}
	}
}

void FPS(int val)
{
	if (!pause)
	{
		if (countdown != 0)
		{
			countdown--;
		}
		else
		{
			addPart();
		}
	}

	moveAll();
	// moveAllFire();
	glutPostRedisplay();
	for (int i = 0; i < parts.size(); i++)
	{
		parts[i].age++;
		if (parts[i].age >= Particle::DEFAULT_LIFE_DURATION)
		{
			if (parts[i].size > 0.3)
			{
				firework1(parts[i].position[0], parts[i].position[1], parts[i].position[2], parts[i].mat);
			}
			deleteParticle(i);
		}
	}
	glutTimerFunc(17, FPS, 0);
}

/* main function - program entry point */
int main(int argc, char **argv)
{
	cout << "Press q to exit" << endl;
	cout << "Press r to reset" << endl;
	cout << "Press f to turn on/off the friction mode" << endl;
	cout << "Press l to turn on/off the light" << endl;
	cout << "Press 1,2 to show one local light only when light mode is on" << endl;
	cout << "Press spacebar to start/stop the simluation" << endl;
	cout << "Press arrow keys to rotate camera" << endl;
	cout << "Press arrow p to turn on/off the paricle camera" << endl;
	cout << "Press arrow h to turn on/off the hole on the floor" << endl;
	cout << "Press arrow a to rise a particle firework" << endl;
	glutInit(&argc, argv); //starts up GLUT

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Fountain"); //creates the window

	glutDisplayFunc(display); //registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutTimerFunc(17, FPS, 0);

	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

	init();

	glutMainLoop();
}