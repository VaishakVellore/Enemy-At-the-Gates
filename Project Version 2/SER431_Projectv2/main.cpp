/**
 * Javier Gonzalez-Sanchez
 * Fall 2018
 *
 * http://javiergs.com/teaching/ser431
 */
#define _CRT_SECURE_NO_DEPRECATE
#include "core.h"
#include "fire.h"
#include "particles.h"
#include "timer.h"
#include<Windows.h>
 //#include "noise.h"

#include <GL/glut.h>

#define V_NUMPOINTS    4
#define U_NUMPOINTS	   4
#define V_NUMKNOTS    (V_NUMPOINTS + 4)
#define U_NUMKNOTS    (U_NUMPOINTS + 4)

 // Knot sequences for cubic bezier surface and trims 
GLfloat sknots[V_NUMKNOTS] = { 0., 0., 0., 0., 1., 1., 1., 1. };
GLfloat tknots[U_NUMKNOTS] = { 0., 0., 0., 0., 1., 1., 1., 1. };

GLboolean decFlag = false;
//GLboolean fog = false;

GLboolean leftTrigger = false;
GLboolean rightTrigger = false;
GLboolean fuelPodFlag = false, fuelPodLeft = false, fuelPodRight = false;
float globalTimer = 0.0f;
int fuelPodY = 2000;
float fuelPodCollision[2][3];
float planeCollision[2][2];

// Control points for the flag. The Z values are modified to make it wave
GLfloat ctlpoints[V_NUMPOINTS][U_NUMPOINTS][3] = {
	{ { -1000.0, 13., 0. },{ -333.3, 13., 0. },{ 333.3, 13., 0 },{ 1000.0, 13., 0. } },
	{ { -1000.0, 12., 666.6 },{ -333.3, 12., 666.6 },{ 333.3, 12., 666.6 },{ 1000.0, 12., 666.6 } },
	{ { -1000.0, 11., 1333.3 },{ -333.3, 11., 1333.3 },{ 333.3, 11., 1333.3 },{ 1000.0, 11., 1333.3 } },
	{ { -1000.0, 10., 2000.0 },{ -333.3, 10., 2000.0 },{ 333.3, 10., 2000.0 },{ 1000.0, 10., 2000.0 } }
};

GLUnurbsObj *nurbsflag;


// Shadows
GLfloat light_position[4];
GLfloat shadow_matrix[4][4];
Vec3f floor_normal;
vector<Vec3f> dot_vertex_floor;
float lightAngle = 0.0, lightHeight = 1200;
float cameraX = 0, cameraY = 0;
float F16X = 0, F16Y = 0;
int renderShadow = 1;

float output[36][3];//curve points 
float output_camera[36][3];//curve points for camera 
int counter = 0;

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

// draw particles
void drawParticles() {
	Particle* curr = ps.particle_head;
	// glPointSize(2);
	// glBegin(GL_POINTS);
	// while (curr) {
	//   glVertex3fv(curr->position);
	//	 curr = curr->next;
	// }
	// glEnd();
	while (curr) {
		glPushMatrix();
		glScalef(400.0, 400.0, 400.0);
		//glTranslatef(0, 800, 6000);
		glTranslatef(curr->position[0], curr->position[1], curr->position[2]);
		glScalef(0.001, 0.001, 0.001);
		glCallList(waterFallDisplay);
		glPopMatrix();
		curr = curr->next;
	}

}

void drawParticlesLava() {
	Fire* curr = lavaps.particle_head;
	// glPointSize(2);
	// glBegin(GL_POINTS);
	// while (curr) {
	//   glVertex3fv(curr->position);
	//	 curr = curr->next;
	// }
	// glEnd();
	while (curr) {
		glPushMatrix();
		glScalef(400.0, 400.0, 400.0);
		//glTranslatef(0, 800, 6000);
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

// init
void init() {
	planeLocator();
	cameraLocator();
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
	f16Mesh = loadFile("C:/Users/chira/source/repos/SER431_Projectv2/SER431_Projectv2/f-16.obj");

	calculateNormalPerFace(mountainMesh);
	calculateNormalPerFace(skybox);
	calculateNormalPerFace(riverBedMesh);
	calculateNormalPerFace(f16Mesh);
	calculateNormalPerFace(waterFallMesh);
	calculateNormalPerFace(lavaDisplayMesh);
	calculateNormalPerFace(fuelCellDisplayMesh);

	calculateNormalPerVertex(mountainMesh);
	calculateNormalPerVertex(skybox);
	calculateNormalPerVertex(riverBedMesh);
	calculateNormalPerVertex(f16Mesh);
	calculateNormalPerVertex(waterFallMesh);
	calculateNormalPerVertex(lavaDisplayMesh);
	calculateNormalPerVertex(fuelCellDisplayMesh);

	// textures
	loadBMP_custom(textureArray, "C:/Users/chira/source/repos/SER431_Projectv2/SER431_Projectv2/grass.bmp", 0);
	loadBMP_custom(textureArray, "C:/Users/chira/source/repos/SER431_Projectv2/SER431_Projectv2/cubesky.bmp", 1);
	loadBMP_custom(textureArray, "C:/Users/chira/source/repos/SER431_Projectv2/SER431_Projectv2/brick.bmp", 2);
	codedTexture(textureArray, 3, 0);//for waterfall
	mountainDisplay = meshToDisplayList(mountainMesh, 1, 0);
	skyboxDisplay = meshToDisplayList(skybox, 2, 1);
	riverDisplay = meshToDisplayList(riverBedMesh, 3, 2);
	f16Display = meshToDisplayList(f16Mesh, 4, 2);
	waterFallDisplay = meshToDisplayList(waterFallMesh, 5, 3);
	codedTexture(textureArray, 4, 2);
	lavaDisplay = meshToDisplayList(lavaDisplayMesh, 6, 4);

	codedTexture(textureArray, 7, 1);//for marbel fuel cell
	fuelCellDisplay = meshToDisplayList(fuelCellDisplayMesh, 7, 7);
	// shadow
	glClearStencil(0);
	// floor vertex
	dot_vertex_floor.push_back(Vec3<GLfloat>(-1000.0, 2.0, 0.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(1000.0, 2.0, 0.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(1000.0, 2.0, 12000.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(-1000.0, 2.0, 12000.0));
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

}


//to draw a nice river
//hermite calculation and drawing
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
		glColor3f(0.2, 0.2, 2);
		//for (int i = 0; i < 19; i++) {
		glVertex3f(points[i][0], points[i][1], points[i][2]);
		glVertex3f(points[i + 1][0], points[i + 1][1], points[i + 1][2]);
		glVertex3f(points2[i][0], points2[i][1], points2[i][2]);
		//}
		glEnd();
		glPopMatrix();
		glPushMatrix();

		glBegin(GL_TRIANGLES);
		if (i % 5 == 0)
			glColor3f(0.0, 0.1, 0.5);
		else
			glColor3f(0.4, 0.4, i % 10);
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

// display
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// light source position
	light_position[0] = 0;//500 * cos(lightAngle);
	light_position[1] = 11500;
	light_position[2] = 11999;//800 *sin(lightAngle)+11000;
	light_position[3] = 0; // directional light
	lightAngle += 0.05;
	// Calculate Shadow matrix
	shadowMatrix(shadow_matrix, floor_normal, light_position);
	// projection and view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(45.0, ratio, 1, 100000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// lookAt 
	if (leftTrigger || rightTrigger)
		gluLookAt(cameraX, cameraY, 11500.0, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	else
		gluLookAt(60.0f, 1200.0f, 11500.0, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// camera
	//glScalef(scale, scale, scale);
	glRotatef(y_angle, 0.0f, 300.0f, 0.0f); //detects mouse' movement along x-axis
	glRotatef(x_angle, 300.0f, 0.0f, 0.0f); //detects mouse' movement along y-axis
	glTranslatef(0.0f, 0.0f, 0.0f);

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
	if(riverCurve){
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
	if(reflection){
		glPushMatrix();

		if (!(leftTrigger || rightTrigger)) {
			glTranslatef(0, 300, 9500);
			glRotatef(180, 0, 1, 0);
			glScalef(20.0, -20.0, 20.0);
			glCallList(f16Display);

		}
		else
		{
			glTranslatef(F16X, 300, 9500);
			glRotatef(180, 0, 1, 0);
			glScalef(20, -20, 20);
			glCallList(f16Display);
		}
		glPopMatrix();
	}
	if(shadow){
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

	// STENCIL-STEP 4. disable it
	glDisable(GL_STENCIL_TEST);
	glPopMatrix();
	//end reflections

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
		//glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); 
		//  To eliminate depth buffer artifacts, use glEnable(GL_POLYGON_OFFSET_FILL);
		// Render 50% black shadow color on top of whatever the floor appareance is
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);  /* Force the 50% black. */
		glColor4f(0.0, 0.0, 0.0, 0.5);
		glPushMatrix();
		// Project the shadow
		glMultMatrixf((GLfloat *)shadow_matrix);
		// boxes
		glDisable(GL_DEPTH_TEST);
		if (shadow){
			glPushMatrix();
			glTranslatef(0, 4, 7000);
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

	//fog starts
	if (fog)
	{
		glPushMatrix();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		GLfloat fogColor[4] = { .7,1.0,1.5,1 };
		glFogfv(GL_FOG_COLOR, fogColor);
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


	////particles for waterfall
	if(particles){
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
	}
	// particles for waterfall end

	if(sbox){//skybox
		glPushMatrix();
		glTranslatef(-6000, -1, 0);
		glCallList(skyboxDisplay);
		glPopMatrix();
	}

	//mountain
	if(terrain){
		glPushMatrix();
		glTranslatef(-6000, 1, 0);
		glCallList(mountainDisplay);
		glPopMatrix();
		
	}
	else {
		particles = false;
	}
	
	
	//fuel pods
	if (fuelPodFlag) {

		glPushMatrix();
		glTranslatef(100, fuelPodY, 11000);//*(pow(-1,rand()%10))

		glScalef(0.1, 0.1, 0.1);
		glCallList(fuelCellDisplay);

		glPopMatrix();

		//bounding sphere for fuel pods
		glPushMatrix();
		glTranslatef(105, fuelPodY + 5, 11005);//*(pow(-1,rand()%10))
		glutSolidSphere(8.66, 36, 36);

		glPopMatrix();
		fuelPodCollision[0][0] = 105 - 8.66;			//xmin
		fuelPodCollision[1][0] = 105 + 8.66;			//xmax
		fuelPodCollision[0][1] = fuelPodY - 3.66;		//ymin
		fuelPodCollision[1][1] = fuelPodY + 13.66;		//ymax
		fuelPodCollision[0][2] = 11005 - 8.66;			//zmin
		fuelPodCollision[1][2] = 11005 + 8.66;			//zmax
		fuelPodY -= 10;
		if (fuelPodY < 10) {
			fuelPodFlag = false;
			fuelPodLeft = false;
			fuelPodRight = false;
		}
	}

	if (globalTimer >= 20.0&&globalTimer <= 20.01) {
		fuelPodFlag = true;
		fuelPodLeft = true;
		fuelPodY = 8000;
	}
	//end fuel pods

	////particles lava
	lavaps.add();
	lavaps.update(0.01);
	lavaps.remove();

	// translation based on curve
	if (counter == 17) {
		decFlag = false;
	}
	if (counter == 0) {
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

	}
	else {
		if (bbox) {
			glPushMatrix();
			glTranslatef(50+ F16X, 125+ F16Y, 11000);
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
	}

	//float transF16[][] = (float)planeLocator(planepoints);

	// draw the light arrow	
	drawLightArrow();
	glPopMatrix();

	
		//menu
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, window_width, 0, window_height);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glColor3f(1.0, 1.0, 1.0);
		renderBitmapString(0.0, window_height - 15.0f, 0.0f, "Use [m or M] to toggle Menu");
		if (menu)
		{
			renderBitmapString(0.0, window_height - 30.0f, 0.0f, "Use [t or T] to toggle Terrain");
			renderBitmapString(0.0, window_height - 45.0f, 0.0f, "Use [n or N] to toggle Noise");
			renderBitmapString(0.0, window_height - 60.0f, 0.0f, "Use [s or S] to toggle Shadows");
			renderBitmapString(0.0, window_height - 75.0f, 0.0f, "Use [r or R] to toggle Reflection");
			renderBitmapString(0.0, window_height - 90.0f, 0.0f, "Use [b or B] to toggle Bounding Box");
			renderBitmapString(0.0, window_height - 105.0f, 0.0f, "Use [x or X] to toggle Skybox");
			renderBitmapString(0.0, window_height - 120.0f, 0.0f, "Use [f or F] to toggle Fog");
			renderBitmapString(0.0, window_height - 135.0f, 0.0f, "Use [p or P] to toggle Particles");
			renderBitmapString(0.0, window_height - 150.0f, 0.0f, "Use [c or C] to toggle Surface made by Curves");
			renderBitmapString(0.0, window_height - 165.0f, 0.0f, "Use [a/d or A/D] to toggle Object/Camera movement Along Curve");
		}
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

	glutSwapBuffers();
	globalTimer += 0.01;
}

// callback function for keyboard (alfanumeric keys)
void callbackKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a': case 'A':
		leftTrigger = true;
		break;
	case 'd': case 'D':
		rightTrigger = true;
		break;
	case 'f': case 'F':
		if (fog)
			fog = false;
		else
			fog = true;
		break;
	case 's': case 'S':
		if (shadow)
			shadow = false;
		else
			shadow = true;
		break;
	case 'r': case 'R':
		if (reflection)
			reflection = false;
		else
			reflection = true;
		break;
	case 'x': case 'X':
		if (sbox)
			sbox = false;
		else
			sbox = true;
		break;
	case 'p': case 'P':
		if (particles)
			particles = false;
		else
			particles = true;
		break;
	case 'c': case 'C':
		if (riverCurve)
			riverCurve = false;
		else
			riverCurve = true;
		break;
	case 't': case 'T':
		if (terrain)
			terrain = false;
		else
			terrain = true;
		break;
	case 'n': case 'N':
		if (noise)
			noise = false;
		else
			noise = true;
		break;
	case 'm': case 'M':
		if (menu)
			menu = false;
		else
			menu = true;
		break;
	case 'b': case 'B':
		if (bbox)
			bbox = false;
		else
			bbox = true;
		break;
	}
}



// main
void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("Fighter Jets");
	// callbacks
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(callbackKeyboard);
	init();
	glutMainLoop();
}