/**
 *
 * Vaishak Ramesh Vellore
 * Chiranjeevi Ramamurthy
 * Ananya Dutta
 * Fall 2018
 */

#define _CRT_SECURE_NO_DEPRECATE
#define PI 3.1415927

#include "core.h"
#include "fire.h"
#include "particles.h"
#include "timer.h"
#include "car.h"
#include "laser.h"
#include "explosion.h"
#include "island.h"
#include "Tree.h"
#include "TreeNext.h"
#include<Windows.h>
#include <GL/glut.h>

 // global variables for Shadows
GLfloat light_position[4];
GLfloat shadow_matrix[4][4];
Vec3f floor_normal;
vector<Vec3f> dot_vertex_floor;
int renderShadow = 1;

//light parameters
float lightAngle = 0.0, lightHeight = 1200;

//camera movement
float cameraX = 0, cameraY = 0;

//planes movement parameters
float F16X = 0, F16Y = 0;
float paceSetter = 0.01f;
boolean planeExplode = false;
float enemyAngle = 0.0f;
float output[36][3];//curve points 
float output_camera[36][3];//curve points for camera 
float enemyPlaneCollision[2][2];
float planeCollision[2][2];
int counter = 0;
int enemyPlane = -1;
int enemyPlaneDestroyed = 0;
float explosionTimer = 0;

// calculate floor normal
void calculate_floor_normal(Vec3f *plane, vector<Vec3f> dot_floor) {
	Vec3<GLfloat> AB = dot_floor[1] - dot_floor[0];
	Vec3<GLfloat> AC = dot_floor[2] - dot_floor[0];
	*plane = AB.cross(AC);
}

// Create a matrix that will project the desired shadow
void shadowMatrix(GLfloat shadowMat[4][4], Vec3f plane_normal, GLfloat lightpos[4]) {
	GLfloat dot;
	Vec3f lightpos_v; lightpos_v.x = lightpos[0]; lightpos_v.y = lightpos[1]; lightpos_v.z = lightpos[2];
	dot = plane_normal.dot(lightpos_v);
	shadowMat[0][0] = dot - lightpos[0] * plane_normal[0];
	shadowMat[1][0] = 0.f - lightpos[0] * plane_normal[1];
	shadowMat[2][0] = 0.f - lightpos[0] * plane_normal[2];
	shadowMat[3][0] = 0.f - lightpos[0] * plane_normal[3];
	shadowMat[0][1] = 0.f - lightpos[1] * plane_normal[0];
	shadowMat[1][1] = dot - lightpos[1] * plane_normal[1];
	shadowMat[2][1] = 0.f - lightpos[1] * plane_normal[2];
	shadowMat[3][1] = 0.f - lightpos[1] * plane_normal[3];
	shadowMat[0][2] = 0.f - lightpos[2] * plane_normal[0];
	shadowMat[1][2] = 0.f - lightpos[2] * plane_normal[1];
	shadowMat[2][2] = dot - lightpos[2] * plane_normal[2];
	shadowMat[3][2] = 0.f - lightpos[2] * plane_normal[3];
	shadowMat[0][3] = 0.f - lightpos[3] * plane_normal[0];
	shadowMat[1][3] = 0.f - lightpos[3] * plane_normal[1];
	shadowMat[2][3] = 0.f - lightpos[3] * plane_normal[2];
	shadowMat[3][3] = dot - lightpos[3] * plane_normal[3];
}

//Flag creation
GLUnurbsObj *wave;
void waveAFlag() {
	GLfloat control[4][4][3] = {
		{ { 0., 3., 0. },{ 1., 3., 0. },{ 2., 3., 0 },{ 3., 3., 0. } },
		{ { 0., 2., 0. },{ 1., 2., 0. },{ 2., 2., 0 },{ 3., 2., 0. } },
		{ { 0., 1., 0. },{ 1., 1., 0. },{ 2., 1., 0 },{ 3., 1., 0. } },
		{ { 0., 0., 0. },{ 1., 0., 0. },{ 2., 0., 0 },{ 3., 0., 0. } }
	};

	GLfloat ssknots[8] = { 0., 0., 0., 0., 1., 1., 1., 1. };
	GLfloat ttknots[8] = { 0., 0., 0., 0., 1., 1., 1., 1. };

	// Materials
	GLfloat mat_diffuse[] = { 0.396f, 0.74151f, 0.69102f, 0.3 };
	GLfloat mat_specular[] = { 0.297254f, 0.30829f, 0.306678f, 0.3 };
	GLfloat mat_ambient[] = { 0.1f, 0.18725f, 0.1745f, 0.3 };
	GLfloat mat_shininess[] = { 12.8 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	glPushMatrix();
	static GLfloat angle = 0.0;
	int i, j;
	// wave the flag by rotating Z coords though a sine wave
	for (i = 1; i < 4; i++)
		for (j = 0; j < 4; j++)
			control[i][j][2] = sin((GLfloat)i + angle);
	angle += 0.1;
	glPushMatrix();
	glTranslatef(950.0, 900.0, 8800.0);
	glScalef(150, 50, 50);
	glRotatef(270, 0., 0., 1.);
	gluBeginSurface(wave);
	gluNurbsSurface(wave, 8, ssknots, 8, ttknots,
		12, 3,
		&control[0][0][0], 4, 4, GL_MAP2_VERTEX_3);
	gluEndSurface(wave);
	glPopMatrix();

	// Materials
	GLfloat mat_diffuse_new[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_specular_new[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_ambient_new[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat mat_shininess_new[] = { 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient_new);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_new);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular_new);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess_new);
	glPopMatrix();
}

// draw particles
void drawParticles() {
	Particle* curr = ps.particle_head;
	while (curr) {
		glPushMatrix();
		glScalef(400.0, 400.0, 400.0);
		glTranslatef(curr->position[0], curr->position[1], curr->position[2]);
		glScalef(0.001, 0.001, 0.001);
		if (enemyPlaneDestroyed <= 100.)
			glCallList(waterFallDisplay);
		else
			glCallList(lavaFallDisplay);
		glPopMatrix();
		curr = curr->next;
	}

}

void drawParticlesLava() {
	Fire* curr = lavaps.particle_head;
	while (curr) {
		glPushMatrix();
		glScalef(400.0, 400.0, 400.0);
		glTranslatef(curr->position[0], curr->position[1], curr->position[2]);
		glScalef(0.001, 0.001, 0.001);
		glCallList(lavaDisplay);
		glPopMatrix();
		curr = curr->next;
	}

}

void drawParticlesLaser() {
	Laser* curr = laserps.particle_head;
	while (curr) {
		glPushMatrix();
		glScalef(400.0, 400.0, 400.0);
		glTranslatef(curr->position[0], curr->position[1], curr->position[2]);
		glScalef(0.001, 0.001, 0.001);
		glCallList(lavaDisplay);
		glPopMatrix();
		curr = curr->next;
	}

}

void drawParticlesExplosion() {
	Explosion* curr = explosionps.particle_head;
	while (curr) {
		glPushMatrix();
		glScalef(200.0, 200.0, 200.0);
		glTranslatef(curr->position[0], curr->position[1], curr->position[2]);
		glScalef(0.001, 0.001, 0.001);
		glCallList(lavaDisplay);
		glPopMatrix();
		curr = curr->next;
	}

}

void planeLocator() {
	float Points[4][3] = {
		{-500,750,11000},
		{-500,1000,11000},
		{500,1000,11000},
		{500,750,11000}
	};

	for (int i = 0; i != 36; ++i) {

		float t = (float)i / (35);
		// the t value inverted
		float it = 1.0f - t;
		// calculate blending functions
		float b0 = it * it*it / 6.0f;
		float b1 = (3 * t*t*t - 6 * t*t + 4) / 6.0f;
		float b2 = (-3 * t*t*t + 3 * t*t + 3 * t + 1) / 6.0f;
		float b3 = t * t*t / 6.0f;
		// sum the control points mulitplied by their respective blending functions
		output[i][0] = b0 * Points[0][0] + b1 * Points[1][0] + b2 * Points[2][0] + b3 * Points[3][0];
		output[i][1] = b0 * Points[0][1] + b1 * Points[1][1] + b2 * Points[2][1] + b3 * Points[3][1];
		output[i][2] = b0 * Points[0][2] + b1 * Points[1][2] + b2 * Points[2][2] + b3 * Points[3][2];
		// specify the point

	}

}

void cameraLocator() {
	float Points[4][3] = {
		{-490,950,11500},
		{-490,1200,11500},
		{560,1200,11500},
		{560,950,11500}
	};

	for (int i = 0; i != 36; ++i) {

		float t = (float)i / (35);
		// the t value inverted
		float it = 1.0f - t;
		// calculate blending functions
		float b0 = it * it*it / 6.0f;
		float b1 = (3 * t*t*t - 6 * t*t + 4) / 6.0f;
		float b2 = (-3 * t*t*t + 3 * t*t + 3 * t + 1) / 6.0f;
		float b3 = t * t*t / 6.0f;
		// sum the control points mulitplied by their respective blending functions
		output_camera[i][0] = b0 * Points[0][0] + b1 * Points[1][0] + b2 * Points[2][0] + b3 * Points[3][0];
		output_camera[i][1] = b0 * Points[0][1] + b1 * Points[1][1] + b2 * Points[2][1] + b3 * Points[3][1];
		output_camera[i][2] = b0 * Points[0][2] + b1 * Points[1][2] + b2 * Points[2][2] + b3 * Points[3][2];
		// specify the point

	}

}

//hermite calculation and drawing for river
void drawHermitePlane(float z1, float z2, float tan1, float tan2) {
	//Hermite curve's coordinates & Tangents
	float Geometry[4][3] = {
		{ 800 , 4 , z1 },	//  Point 1
		{ 800, 4, z2 },	//	Point 2
		{tan1, 0, 0 },		//	Tangent1
		{tan2, 0, 0 }		//	Tangent2
	};
	float points[20][3];
	float points2[20][3];

	// use the parametric time value 0 to 1
	for (int i = 0; i != 20; ++i) {
		float t = (float)i / (19);
		// calculate blending functions
		float b0 = 2 * t*t*t - 3 * t*t + 1;
		float b1 = -2 * t*t*t + 3 * t*t;
		float b2 = t * t*t - 2 * t*t + t;
		float b3 = t * t*t - t * t;
		// calculate the x, y and z of the curve point
		float x = b0 * Geometry[0][0] + b1 * Geometry[1][0] + b2 * Geometry[2][0] + b3 * Geometry[3][0];
		float y = b0 * Geometry[0][1] + b1 * Geometry[1][1] + b2 * Geometry[2][1] + b3 * Geometry[3][1];
		float z = b0 * Geometry[0][2] + b1 * Geometry[1][2] + b2 * Geometry[2][2] + b3 * Geometry[3][2];
		// specify the point
		points[i][0] = x;
		points[i][1] = y;
		points[i][2] = z;
		points2[i][0] = x - 1600;
		points2[i][1] = y;
		points2[i][2] = z;
	}

	for (int i = 0; i < 19; i++) {
		//drawing
		//Start filling the 'curve' with triangles
		glPushMatrix();

		glBegin(GL_TRIANGLES);
		if (enemyPlaneDestroyed > 100) {
			glColor3f(0.9, 0.9, 0.0);
		}
		else {
			glColor3f(0.2, 0.2, 2);
		}
		
		//for (int i = 0; i < 19; i++) {
		glVertex3f(points[i][0], points[i][1], points[i][2]);
		glVertex3f(points[i + 1][0], points[i + 1][1], points[i + 1][2]);
		glVertex3f(points2[i][0], points2[i][1], points2[i][2]);
		//}
		glEnd();
		glPopMatrix();
		glPushMatrix();

		glBegin(GL_TRIANGLES);
		if (i % 5 == 0) {
			if (enemyPlaneDestroyed > 100) {
				glColor3f(1, 0.1, 0.1);
			}
			else {
				glColor3f(0.0, 0.1, 0.5);
			}
		}
			
		else {
			if (enemyPlaneDestroyed > 100) {
				glColor3f(i % 5, 0.1, 0.1);
			}
			else {
				glColor3f(0.4, 0.4, i % 10);
			}
		}
			
		//for (int i = 0; i < 19; i++) {
		glVertex3f(points[i + 1][0], points[i + 1][1], points[i + 1][2]);
		glVertex3f(points2[i + 1][0], points2[i + 1][1], points2[i + 1][2]);
		glVertex3f(points2[i][0], points2[i][1], points2[i][2]);
		//}
		glEnd();
		glPopMatrix();
	}

}
//end off river

// draw an arrow to visualize the source of light
void drawLightArrow() {
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 0.0);
	// draw arrowhead. 
	glTranslatef(light_position[0], light_position[1], light_position[2]);
	glRotatef(lightAngle * -180.0 / 3.141592, 0, 1, 0);
	glRotatef(atan(lightHeight / 500) * 180.0 / 3.141592, 0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	glVertex3f(20, 10, 10);
	glVertex3f(20, -10, 10);
	glVertex3f(20, -10, -10);
	glVertex3f(20, 10, -10);
	glVertex3f(20, 10, 10);
	glEnd();
	// draw a  line from light direction
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);
	glEnd();
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void displayShadows() {
	// draw shadows
	glPushMatrix();
	// Tell GL new light source position
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	// Shadows
	if (renderShadow) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	}
	// Draw floor using blending to blend in reflection
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 1.0, 1.0, 0.3);
	glPushMatrix();
	glDisable(GL_LIGHTING);
	//glTranslatef(-900, 0, -900);
	//glRotatef(180, 0, 0, 1);
	if (riverCurve) {
		drawHermitePlane(0, 2000, 500, -500);
		drawHermitePlane(2000, 4000, -500, 500);
		drawHermitePlane(4000, 6000, 500, -500);
		drawHermitePlane(6000, 8000, -500, 500);
		drawHermitePlane(8000, 10000, 500, -500);
		drawHermitePlane(10000, 12000, -500, 500);
	}

	//draw riverbed 
	glPushMatrix();
	glTranslatef(-2000, 3, 0);
	glCallList(riverDisplay);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-2000, 3, 4000);
	glCallList(riverDisplay);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-2000, 3, 8000);
	glCallList(riverDisplay);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glPopMatrix();
	glDisable(GL_BLEND);
	// Shadows

	if (renderShadow) {
		glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
		glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);  /* Force the 50% black. */
		glColor4f(0.0, 0.0, 0.0, 0.5);
		glPushMatrix();
		// Project the shadow
		glMultMatrixf((GLfloat *)shadow_matrix);
		// boxes
		glDisable(GL_DEPTH_TEST);
		if (shadow) {
			glPushMatrix();
			glTranslatef(0, 3, enemyAngle * 100);
			glRotatef(180, 0, 0, 1);
			glScalef(50, 50, 50);
			glCallList(f16Display);
			glPopMatrix();
		}
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		// To eliminate depth buffer artifacts, use glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_STENCIL_TEST);
	}
	glPopMatrix();
}

void drawReflections() {
	//reflections
	glPushMatrix();
	glEnable(GL_STENCIL_TEST); //Start using the stencil
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //Disable writing colors in frame buffer
	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF); //Place a 1 where rendered
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); 	//Replace where rendered
	// PLAIN for the stencil
	glPushMatrix();
	//glRotatef(180, 0, 0, 1);
	if (riverCurve) {
		drawHermitePlane(0, 2000, 500, -500);
		drawHermitePlane(2000, 4000, -500, 500);
		drawHermitePlane(4000, 6000, 500, -500);
		drawHermitePlane(6000, 8000, -500, 500);
		drawHermitePlane(8000, 10000, 500, -500);
		drawHermitePlane(10000, 12000, -500, 500);
	}
	glPopMatrix();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //Reenable color
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //Keep the pixel

	//f-16 reflections
	if (reflection) {
		glPushMatrix();
		if (!(leftTrigger || rightTrigger)) {
			glTranslatef(0, 300, 9200);
			glRotatef(180, 0, 1, 0);
			glScalef(20.0, -20.0, 20.0);
			glCallList(f16Display);
		}
		else
		{
			glTranslatef(F16X, 300, 9200);
			glRotatef(180, 0, 1, 0);
			glScalef(20, -20, 20);
			glCallList(f16Display);
		}
		glPopMatrix();
	}
	if(shadow){
		glDisable(GL_AUTO_NORMAL);
		glDisable(GL_NORMALIZE);
		glPushMatrix();
		if (!(leftTrigger || rightTrigger)) {
			glTranslatef(-800, 3, 8000);
			glRotatef(180, 0, 1, 0);
			glScalef(50.0, 50.0, 75.0);
			glCallList(f16Display);

		}
		else
		{
			glTranslatef(F16X - 800, 3, 8000);
			glRotatef(180, 0, 1, 0);
			glScalef(50+(F16X/10), 50, 75);
			glCallList(f16Display);
		}
		glPopMatrix();
	}
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);
	// STENCIL-STEP 4. disable it
	glDisable(GL_STENCIL_TEST);
	glPopMatrix();
	//end reflections
}

void lightPosition() {
	// light source position
		light_position[0] = 500;
		light_position[1] = 1500;
		light_position[2] = 500;
		light_position[3] = 0; // directional light
		lightAngle += 0.05;
		if (lightFlag)
		{
			// light
			GLfloat light_ambient[] = { 0.8, 0.8, 0.8, 1.0 };
			GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
			GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHTING);
		}
		else
		{
			// light
			GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 0.1 };
			GLfloat light_diffuse[] = { 0.1, 0.1, 0.1, 0.1 };
			GLfloat light_specular[] = { 0.1, 0.1, 0.1, 0.1 };
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHTING);
		}
}

//tree drawings
void treeCreations()
{
	if (fractal)
	{
		if (enemyPlaneDestroyed > 100) {
			// tree fractal
			glPushMatrix();
			glTranslatef(620, 370, 5500);
			glScalef(200, 200, 200);
			glCallList(TTREE);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-1000, 200, 7500);
			glScalef(200, 200, 200);
			glCallList(TTREE);
			glPopMatrix();
		}
		else {
			// tree fractal
			glPushMatrix();
			glTranslatef(620, 370, 5500);
			glScalef(200, 200, 200);
			glCallList(FULLTREE);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-1000, 200, 7500);
			glScalef(200, 200, 200);
			glCallList(FULLTREE);
			glPopMatrix();
		}
	}
}

//mountain on river
void surfaceMountain()
{

	GLfloat texpts[2][2][3] =
	{
	{ { 0.0f, 0.0f,0.0f }, { 0.0f, 1.0f,0.0f } },
	{ { 1.0f, 0.0f,0.0f }, { 1.0f, 1.0f,0.0f } }
	};

	glPushMatrix();

	if (Materials) {
		// Materials
		GLfloat mat_diffuse[] = { 0.0f, 0.5f, 0.31f, 1 };
		GLfloat mat_specular[] = { 0.1f, 0.5f, 0.5f, 1 };
		GLfloat mat_ambient[] = { 0.0f, 0.5f, 0.31f, 1 };
		GLfloat mat_shininess[] = { 100.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	}

	GLfloat ctlpoints[U_size][V_size][3] = {
		{ { 40, 3,   570} ,{ 60, 3,  570 },{ 60, 3, 570 }  ,{ 80, 3, 570 } },
		{ { 40, 3,   550 } ,{ 60, 30, 550 },{ 60, 30, 550 } ,{ 80, 3, 550 } },
		{ { 40, 3,   550 } ,{ 60, 30, 550 },{ 60, 30, 550 },{ 80, 3,  550 } },
		{ { 40, 3,   530 } ,{ 60, 3, 530 },{ 60, 3, 530 },{ 80, 3,  530 } }
	};
	GLfloat vknots[V_size + ORDER] = { 0.0, 0.0, 0.0, 0.0, 5.0, 5.0, 5.0, 5.0 };
	GLfloat uknots[U_size + ORDER] = { 0.0, 0.0, 0.0, 0.0, 5.0, 5.0, 5.0, 5.0 };

	GLUnurbsObj *theNurb;
	theNurb = gluNewNurbsRenderer();
	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);

	glPushMatrix();
	glScalef(10, 10, 10);
	gluBeginSurface(theNurb);
	gluNurbsSurface(theNurb,
		U_size + ORDER, uknots,
		V_size + ORDER, vknots,
		V_size * 3,
		3,
		&ctlpoints[0][0][0],
		ORDER, ORDER,
		GL_MAP2_VERTEX_3);
	gluEndSurface(theNurb);

	glEnd();
	glEnable(GL_LIGHTING);
	glPopMatrix();

	// Materials
	GLfloat mat_diffuse_new[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_specular_new[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_ambient_new[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat mat_shininess_new[] = { 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient_new);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_new);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular_new);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess_new);
	glPopMatrix();
}

void planeDraw()
{
	if (explosionTimer <= 0) {
		planeExplode = false;
		
	}
	//f16 plane movement along curve
	if (decFlag) {
		if (leftTrigger) {
			cameraX = output_camera[17 - counter][0];
			cameraY = output_camera[17 - counter][1];

			F16X = output[17 - counter][0];
			F16Y = output[17 - counter][1];

			counter++;
			Sleep(10);
		}
		else if (rightTrigger) {

			cameraX = output_camera[counter + 18][0];
			cameraY = output_camera[18 + counter][1];

			F16X = output[18 + counter][0];
			F16Y = output[18 + counter][1];
			counter++;
			Sleep(10);
		}
	}
	else {
		if (leftTrigger) {

			cameraX = output_camera[17 - counter][0];
			cameraY = output_camera[17 - counter][1];

			F16X = output[17 - counter][0];
			F16Y = output[17 - counter][1];
			counter--;
			Sleep(10);
		}
		else if (rightTrigger) {

			cameraX = output_camera[counter + 18][0];
			cameraY = output_camera[18 + counter][1];

			F16X = output[18 + counter][0];
			F16Y = output[18 + counter][1];
			counter--;
			Sleep(10);
		}
	}

	// translation based on curve
	if (counter == 17) {
		decFlag = false;
	}

	if (counter == 0 && poisionFlag == false) {
		decFlag = true;
		leftTrigger = false;
		rightTrigger = false;

		if (bbox) {
			glPushMatrix();
			glTranslatef(50, 1125, 11000);
			glScalef(1, 0.5, 1);
			glutWireCube(250);
			glPopMatrix();
		}
		planeCollision[0][0] = -125; //xmin
		planeCollision[0][1] = 1000; //ymin
		planeCollision[1][0] = 125;	//xmax
		planeCollision[1][1] = 1125;//ymax

		glPushMatrix();
		glTranslatef(0, 1000, 11000);
		glScalef(30, 30, 30);
		glRotatef(180, 0, 1, 0);
		glCallList(f16Display);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(15, 1100, 11120);
		glScalef(0.2, 0.2, 0.2);
		drawParticlesLava();
		glPopMatrix();

		//laser
		glPushMatrix();
		glTranslatef(15, 1100, 11120);
		glScalef(0.2, 0.2, 0.2);
		drawParticlesLaser();
		glPopMatrix();

	}
	else if (poisionFlag == false) {
		if (bbox) {
			glPushMatrix();
			glTranslatef(50 + F16X, 125 + F16Y, 11000);
			glScalef(1, 0.5, 1);
			glutWireCube(250);
			glPopMatrix();
		}
		planeCollision[0][0] = -125 + F16X; //xmin
		planeCollision[0][1] = 1000 + F16Y; //ymin
		planeCollision[1][0] = 125 + F16X;	//xmax
		planeCollision[1][1] = 1125 + F16Y;//ymax

		glPushMatrix();
		glTranslatef(F16X, F16Y, 11000);
		glScalef(30, 30, 30);
		glRotatef(180, 0, 1, 0);
		glCallList(f16Display);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(F16X + 15, F16Y + 100, 11120);
		glScalef(0.2, 0.2, 0.2);
		drawParticlesLava();
		glPopMatrix();

		//laser
		glPushMatrix();
		glTranslatef(F16X + 15, F16Y + 100, 11120);
		glScalef(0.2, 0.2, 0.2);
		drawParticlesLaser();
		glPopMatrix();
	}

	else if (counter == 0 && poisionFlag == true) {
		decFlag = true;
		leftTrigger = false;
		rightTrigger = false;

		if (bbox) {
			glPushMatrix();
			glTranslatef(50, 1125, 11000);
			glScalef(1, 0.5, 1);
			glutWireCube(250);
			glPopMatrix();
		}
		planeCollision[0][0] = -125; //xmin
		planeCollision[0][1] = 1000; //ymin
		planeCollision[1][0] = 125;	//xmax
		planeCollision[1][1] = 1125;//ymax

		glPushMatrix();
		glTranslatef(50, 1050, 10500);
		glScalef(100, 100, 100);
		glCallList(carDisplay);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(15, 1100, 11120);
		glScalef(0.2, 0.2, 0.2);
		drawParticlesLava();
		glPopMatrix();
	}

	else if (poisionFlag == true) {
		if (bbox) {
			glPushMatrix();
			glTranslatef(50 + F16X, 125 + F16Y, 11000);
			glScalef(1, 0.5, 1);
			glutWireCube(250);
			glPopMatrix();
		}
		planeCollision[0][0] = -125 + F16X; //xmin
		planeCollision[0][1] = 1000 + F16Y; //ymin
		planeCollision[1][0] = 125 + F16X;	//xmax
		planeCollision[1][1] = 1125 + F16Y;//ymax

		glPushMatrix();
		glTranslatef(F16X, F16Y, 10500);
		glScalef(100, 100, 100);
		glCallList(carDisplay);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(F16X + 15, F16Y + 100, 11120);
		glScalef(0.2, 0.2, 0.2);
		drawParticlesLava();
		glPopMatrix();
	}
	//draw enemy plane
	glPushMatrix();
	glTranslatef(400 * (enemyPlane)+10 * cos(enemyAngle), 1000 + 10 * sin(enemyAngle), enemyAngle * 100);
	if (bbox) {
		glutWireCube(500);
		enemyPlaneCollision[0][0] = 400 * (enemyPlane)+10 * cos(enemyAngle);
		enemyPlaneCollision[0][1] = 1000 + 10 * sin(enemyAngle);
	}
	glPopMatrix();

	enemyAngle = enemyAngle + 0.1f + paceSetter;
	paceSetter += 0.001;
	if (enemyAngle >= 115) {
		enemyAngle = 0;
		if (enemyPlane == 1) {
			enemyPlane = -1;
		}
		else {
			enemyPlane = 1;
		}
	}

	//collision detection for left trigger
	if (laserShooting == true
		&& bbox == true
		&& leftTrigger == true
		&& enemyPlaneCollision[0][0] >= -400
		&& enemyPlaneCollision[0][0] <= -390
		&& enemyPlaneCollision[0][1] >= 1000
		&& enemyPlaneCollision[0][0] <= 1010) {
		planeExplode = true;
		explosionTimer = 0.2f;
		enemyPlaneDestroyed += 1;
		
		enemyAngle = 0;
		if (enemyPlane == 1)
		{
			enemyPlane = -1;
		}
		else
			enemyPlane = 1;
	}

	//collision detection for right trigger
	else if (laserShooting == true
		&& bbox == true
		&& rightTrigger == true
		&& enemyPlaneCollision[0][0] >= 400
		&& enemyPlaneCollision[0][0] <= 410
		&& enemyPlaneCollision[0][1] >= 1000
		&& enemyPlaneCollision[0][0] <= 1010) {
		planeExplode = true;
		explosionTimer = 0.2f;
		enemyPlaneDestroyed += 1;
		enemyAngle = 0;
		if (enemyPlane == 1)
		{
			enemyPlane = -1;
		}
		else
			enemyPlane = 1;
	}

	else if (bbox == true && enemyAngle >= 113)
	{
		gameOver = true;
	}
	//draw plane normally
	else {
		glPushMatrix();
		glTranslatef(400 * (enemyPlane)+10 * cos(enemyAngle), 1000 + 10 * sin(enemyAngle), enemyAngle * 100);
		glRotatef(enemyAngle * 10, 0, 0, 1);
		glScalef(50, 50, 50);
		glCallList(enemyPlaneDisplay);
		glPopMatrix();
	}
}

void fogSetting()
{
	//fog starts
	if (fog)
	{
		glPushMatrix();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		
		if (enemyPlaneDestroyed > 100) {
			GLfloat fogColor[4] = { 1.0, 1.0, 0.5, 1 };
			glFogfv(GL_FOG_COLOR, fogColor);
		}
		else {
			GLfloat fogColor[4] = { .7,1.0,1.5,1 };
			glFogfv(GL_FOG_COLOR, fogColor);
		}
		glFogf(GL_FOG_DENSITY, 0.13);
		glFogf(GL_FOG_START, 3000.0);
		glFogf(GL_FOG_END, 12000.0);
		glFogf(GL_FOG_INDEX, 100);
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}
	else {
		glDisable(GL_FOG);
	}
}

void drawSkybox()
{
	if (sbox) {//skybox
		glPushMatrix();
		glTranslatef(-6000, -1, 0);
		glCallList(skyboxDisplay);
		glPopMatrix();
	}
}

void drawMountainTerrain()
{
	//mountain
	if (terrain) {
		if (enemyPlaneDestroyed <= 100) {
			glPushMatrix();
			glTranslatef(-6000, 1, 0);
			glCallList(mountainDisplay);
			glPopMatrix();
		}
		
		else {
			glPushMatrix();
			glTranslatef(-6000, 1, 0);
			glCallList(sandDisplay);
			glPopMatrix();
		}
	}
	else {
		particles = false;
	}
}

void drawPaticleSystem()
{
	////particles lava
	lavaps.add();
	lavaps.update(0.01);
	lavaps.remove();

	//particles for laser
	if (laserShooting) {
		laserps.add();
		laserps.update(0.01);
		laserps.remove();
		glPushMatrix();
		glTranslatef(1000, 1000, 11000);
		drawParticles();
		glPopMatrix();
	}
	else {

		laserps.removeLaser();
	}

	////particles for waterfall
	if (particles) {
		glPushMatrix();
		ps.add();
		ps.update(0.001);
		ps.remove();
		glPushMatrix();
		glTranslatef(1000, 900, 7500);
		drawParticles();
		glPopMatrix();

		////particles
		ps1.add();
		ps1.update(0.001);
		ps1.remove();
		glPushMatrix();
		glTranslatef(1100, 900, 3500);
		drawParticles();
		glPopMatrix();

		////particles
		ps2.add();
		ps2.update(0.001);
		ps2.remove();
		glPushMatrix();
		glTranslatef(-1300, 900, 4500);
		drawParticles();
		glPopMatrix();
		glPopMatrix();
	}
	// particles for waterfall end
}

void noiseTextures()
{
	//toggle noise for marbel,lava and sky textures
	if (noise) {
		fuelCellDisplay = meshToDisplayList(fuelCellDisplayMesh, 7, 7);
		waterFallDisplay = meshToDisplayList(waterFallMesh, 5, 3);
		lavaDisplay = meshToDisplayList(lavaDisplayMesh, 6, 4);
	}
	else {
		fuelCellDisplay = meshToDisplayList(fuelCellDisplayMesh, 7, 2);
		waterFallDisplay = meshToDisplayList(waterFallMesh, 5, 2);
		lavaDisplay = meshToDisplayList(lavaDisplayMesh, 6, 2);
	}
}

void menuOptions()
{
	//menu
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, window_width, 0, window_height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	renderBitmapString(0.0, window_height - 15.0f, 0.0f, "Press \"Esc\" to Main Menu");
	renderBitmapString(0.0, window_height - 30.0f, 0.0f, "Use [m or M] to toggle Menu");
	if (menu)
	{
		
		renderBitmapString(0.0, window_height - 45.0f, 0.0f, "Use [a/d or A/D] to toggle Flight Movement");
		renderBitmapString(0.0, window_height - 60.0f, 0.0f,"Use \"Enter\" to turn on Machine Gun");
		renderBitmapString(0.0, window_height - 75.0f, 0.0f, "Use [r or R] to toggle Reflection");
		renderBitmapString(0.0, window_height - 90.0f, 0.0f, "Use [b or B] to toggle Bounding Box");
		renderBitmapString(0.0, window_height - 105.0f, 0.0f, "Use [x or X] to toggle Skybox");
		renderBitmapString(0.0, window_height - 120.0f, 0.0f, "Use [f or F] to toggle Fog");
		renderBitmapString(0.0, window_height - 135.0f, 0.0f, "Use [p or P] to toggle Particles");
		renderBitmapString(0.0, window_height - 150.0f, 0.0f, "Use [c or C] to toggle Surface Curves");
		renderBitmapString(0.0, window_height - 165.0f, 0.0f, "Use[n or N] to toggle Noise");
		renderBitmapString(0.0, window_height - 180.0f, 0.0f, "Use [t or T] to toggle Terrain");
		renderBitmapString(0.0, window_height - 195.0f, 0.0f, "Use [l or L] to toggle Light");
		renderBitmapString(0.0, window_height - 210.0f, 0.0f, "Use [i or I] to toggle Image Surface");
		renderBitmapString(0.0, window_height - 225.0f, 0.0f, "Use [o or O] to toggle Materials");
		renderBitmapString(0.0, window_height - 240.0f, 0.0f, "Use [j or J] to toggle Fractal Trees");
		renderBitmapString(0.0, window_height - 255.0f, 0.0f, "Use [s or S] to toggle Shadows");
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void cameraPosition()
{
	// lookAt 
	if (leftTrigger || rightTrigger)
		gluLookAt(cameraX, cameraY, 11500.0, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	else
		gluLookAt(60.0f, 1200.0f, 11500.0, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	// camera
	glRotatef(y_angle, 0.0f, 300.0f, 0.0f); //detects mouse' movement along x-axis
	glRotatef(x_angle, 300.0f, 0.0f, 0.0f); //detects mouse' movement along y-axis
	glTranslatef(0.0f, 0.0f, 0.0f);
}

// init
void init() {
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	// general
	ratio = (double)width / (double)height;
	mountainMesh = createPlane(12000, 12000, 200);//mountain terrain
	skybox = createSkyBox(12000); //skybox' mesh
	riverBedMesh = createRiver(4000, 4000, 200);//river
	waterFallMesh = createCube();//for waterfalls
	lavaDisplayMesh = createCube();
	fuelCellDisplayMesh = createCube();
	lavaMesh = createPlane(12000, 12000, 200);//mountain terrain
	lavaFallMesh = createCube();//for waterfalls
	lavaDisplayMesh = createCube();

	f16Mesh = loadFile("f-16.obj");
	carMesh = loadFile("mclaren_2001.obj");
	
	CreateTreeListsLocal();//trees for level2
	CreateTreeLists(); //fractal tree
	calculateNormalPerFace(mountainMesh);
	calculateNormalPerFace(skybox);
	calculateNormalPerFace(riverBedMesh);
	calculateNormalPerFace(f16Mesh);
	calculateNormalPerFace(waterFallMesh);
	calculateNormalPerFace(lavaDisplayMesh);
	calculateNormalPerFace(fuelCellDisplayMesh);
	calculateNormalPerFace(carMesh);
	calculateNormalPerFace(lavaMesh);
	calculateNormalPerFace(lavaFallMesh);
	calculateNormalPerFace(lavaDisplayMesh);

	calculateNormalPerVertex(mountainMesh);
	calculateNormalPerVertex(skybox);
	calculateNormalPerVertex(riverBedMesh);
	calculateNormalPerVertex(f16Mesh);
	calculateNormalPerVertex(waterFallMesh);
	calculateNormalPerVertex(lavaDisplayMesh);
	calculateNormalPerVertex(fuelCellDisplayMesh);
	calculateNormalPerVertex(carMesh);
	calculateNormalPerVertex(lavaMesh);
	calculateNormalPerVertex(lavaFallMesh);
	calculateNormalPerVertex(lavaDisplayMesh);
	
	// textures
	loadBMP_custom(textureArray, "grass.bmp", 0);
	loadBMP_custom(textureArray, "cubesky.bmp", 1);
	loadBMP_custom(textureArray, "brick.bmp", 2);
	codedTexture(textureArray, 3, 0);//for waterfall
	codedTexture(textureArray, 7, 1);//for marbel fuel cell
	codedTexture(textureArray, 4, 2);
	loadBMP_custom(textureArray, "sa.bmp", 5);
	codedTexture(textureArray, 6, 2);

	//displays
	mountainDisplay = meshToDisplayList(mountainMesh, 1, 0);
	skyboxDisplay = meshToDisplayList(skybox, 2, 1);
	riverDisplay = meshToDisplayList(riverBedMesh, 3, 2);
	f16Display = meshToDisplayList(f16Mesh, 4, 2);
	waterFallDisplay = meshToDisplayList(waterFallMesh, 5, 3);
	enemyPlaneDisplay = meshToDisplayList(f16Mesh, 4, 2);
	lavaDisplay = meshToDisplayList(lavaDisplayMesh, 6, 4);
	fuelCellDisplay = meshToDisplayList(fuelCellDisplayMesh, 7, 7);
	carDisplay = meshToDisplayList(carMesh, 4, 2);
	sandDisplay = meshToDisplayList(lavaMesh, 1, 5);
	riverLavaDisplay = meshToDisplayList(riverBedMesh, 3, 2);
	lavaFallDisplay = meshToDisplayList(lavaFallMesh, 8, 6);
	
	//object movements based on curves
	planeLocator();
	cameraLocator();

	// shadow
	glClearStencil(0);

	// floor vertex
	dot_vertex_floor.push_back(Vec3<GLfloat>(-1000.0, 3.0, 0.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(1000.0, 3.0, 0.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(1000.0, 3.0, 12000.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(-1000.0, 3.0, 12000.0));
	calculate_floor_normal(&floor_normal, dot_vertex_floor);
	// light
	GLfloat light_ambient[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	wave = gluNewNurbsRenderer();
	gluNurbsProperty(wave, GLU_SAMPLING_TOLERANCE, 100.0);
	gluNurbsProperty(wave, GLU_DISPLAY_MODE, GLU_FILL); //GLU_OUTLINE_POLYGON
}

// display
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// projection and view
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, width, height);
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, width, height);
	gluPerspective(45.0, ratio, 1, 100000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (startGame)
	{
		glPushMatrix();
		glColor3f(1, 1, 1);
		glLineWidth(5);
		renderStrokeString(-5.0, 0.0, -10.0, 0.01, GLUT_STROKE_ROMAN, "Enter 1 to start");
		renderStrokeString(-5.0, -2.0, -10.0, 0.01, GLUT_STROKE_ROMAN, "Enter 2 to view");
		renderStrokeString(-5.0, 2, -10.0, 0.01, GLUT_STROKE_ROMAN, "EnemyAtTheGates");
		glPopMatrix();
	}

	else if (gameMode)
	{
		glLineWidth(1);
		cameraPosition(); //camera movement settings
		lightPosition(); //light 
		shadowMatrix(shadow_matrix, floor_normal, light_position); // Calculate Shadow matrix
		treeCreations(); //tree fractals
		surfaceMountain();  //surface mountains on river
		planeDraw();   //fighter plane and enemy plane
		drawReflections(); //relections draw
		displayShadows(); //shadows draw
		fogSetting(); //setting fog parameters
		drawLightArrow();  // draw the light arrow	
		waveAFlag(); //flag creation
		drawSkybox(); //skybox setting
		drawMountainTerrain(); //mountain terrain settings
		drawPaticleSystem(); // draw all particles in the game
		noiseTextures(); //marbe,sky,lava textures
		menuOptions(); //menu using bitmapstring
		if (gameOver) {
			gameMode = false;
		}

		glPushMatrix();
		//setting up 2nd viewport
		glViewport( 1000, 600, 200, 200);
		glLoadIdentity();
		gluLookAt(0.0, -3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
		char message[32] = "";
		sprintf(message, "%d", enemyPlaneDestroyed);
		char display[32] = "Score: ";
		strcat(display, message);

		int LevelUpgrade = 1;
		if (enemyPlaneDestroyed > 100) {
			LevelUpgrade = 2;
		}
		char level[32] = "";
		sprintf(level, "%d", LevelUpgrade);
		char level_display[32] = "Level: ";
		strcat(level_display, level);

		glLineWidth(3);
		renderBitmapString(40.0, window_height - 100.0f, 0.0f, display);
		renderBitmapString(45.0, window_height - 400.0f, 0.0f, level_display);
		//2nd viewport ends
		glPopMatrix();
	}
	else if (viewMode) {
		bbox = false;
		laserShooting = false;
		leftTrigger = false;
		rightTrigger = false;
		poisionFlag = false;
		glLineWidth(1);
		cameraPosition(); //camera movement settings
		lightPosition(); //light 
		shadowMatrix(shadow_matrix, floor_normal, light_position); // Calculate Shadow matrix
		treeCreations(); //tree fractals
		surfaceMountain();  //surface mountains on river
		planeDraw();   //fighter plane and enemy plane
		drawReflections(); //relections draw
		displayShadows(); //shadows draw
		fogSetting(); //setting fog parameters
		drawSkybox(); //skybox setting
		drawMountainTerrain(); //mountain terrain settings
		drawPaticleSystem(); // draw all particles in the game
		noiseTextures(); //marbe,sky,lava textures
		menuOptions(); //menu using bitmapstring
		 
	}
	else if (gameOver) {

		glPushMatrix();
		glColor3f(1, 1, 1);
		glLineWidth(5);
		renderStrokeString(-5.0, -1.0, -10.0, 0.01, GLUT_STROKE_ROMAN, "");
		renderStrokeString(-5.0, 0, -10.0, 0.01, GLUT_STROKE_ROMAN, "GAME OVER!");
		char scoreDisplay[32] = "";
		sprintf(scoreDisplay, "%d", enemyPlaneDestroyed);
		char finalDisplay[32] = "Your Score: ";
		strcat(finalDisplay, scoreDisplay);
		renderStrokeString(-5.0, 2, -10.0, 0.01, GLUT_STROKE_ROMAN, finalDisplay);
		glPopMatrix();

		glPushMatrix();
		//setting up 2nd viewport
		glViewport(1000, 600, 200, 200);
		glLoadIdentity();
		gluLookAt(0.0, -3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
		char message[32] = "";
		sprintf(message, "%d", enemyPlaneDestroyed);
		char display[32] = "Score: ";
		strcat(display, message);
		glLineWidth(3);
		renderBitmapString(40.0, window_height - 100.0f, 0.0f, display);
		renderBitmapString(45.0, window_height - 400.0f, 0.0f, "High Score");
		//2nd viewport ends
		glPopMatrix();
	}
	glDisable(GL_SCISSOR_TEST);
	glPopMatrix();

	glutSwapBuffers();
}

// main
void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1200, 800);
	glutCreateWindow("Fighter Jets");
	// callbacks
	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(callbackKeyboard);
	glutMainLoop();
}