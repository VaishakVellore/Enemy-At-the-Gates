/**
 * Javier Gonzalez-Sanchez
 * Fall 2018
 *
 * http://javiergs.com/teaching/ser431
 */

#define _CRT_SECURE_NO_WARNINGS

#include "core.h"
#include "noise.h"
#include "texture.h"

 // Shadows
GLfloat light_position[4];
GLfloat shadow_matrix[4][4];
Vec3f floor_normal;
vector<Vec3f> dot_vertex_floor;
float lightAngle = 0.0, lightHeight = 100;
int renderShadow = 1;

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




// init
void init() {
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	// general
	ratio = (double)width / (double)height;
	mesh1 = createTerrain(2000, 2000, 200);
	mesh2 = createCube();
	mesh3 = createCube();
	mesh4 = loadFile("f-16.obj");
	mesh5 = createSkyBox(3000);
	mesh6 = createCube();
	mesh7 = createPlane(2000, 2000, 200);
	mesh8 = createPlane(3000, 3000, 200);
	calculateNormalPerFace(mesh1);
	calculateNormalPerFace(mesh2);
	calculateNormalPerFace(mesh3);
	calculateNormalPerFace(mesh4);
	calculateNormalPerFace(mesh5);
	calculateNormalPerFace(mesh6);
	calculateNormalPerFace(mesh7);
	calculateNormalPerFace(mesh8);
	calculateNormalPerVertex(mesh1);
	calculateNormalPerVertex(mesh2);
	calculateNormalPerVertex(mesh3);
	calculateNormalPerVertex(mesh4);
	calculateNormalPerVertex(mesh5);
	calculateNormalPerVertex(mesh6);
	calculateNormalPerVertex(mesh7);
	calculateNormalPerVertex(mesh8);
	// textures
	loadBMP_custom(textureArray, "grass.bmp", 0);
	loadBMP_custom(textureArray, "oldbox.bmp", 1);
	loadBMP_custom(textureArray, "cubesky.bmp", 2);
	codedTexture(textureArray, 3, 0);
	codedTexture(textureArray, 4, 1);
	codedTexture(textureArray, 5, 2);
	display1 = meshToDisplayList(mesh1, 1, 0);//ground
	display2 = meshToDisplayList(mesh2, 2, 3);//blue thing (perlin noise)
	display3 = meshToDisplayList(mesh3, 3, 4);//black and white thing(marble noise)
	display4 = meshToDisplayList(mesh4, 4, 1);//jet
	display5 = meshToDisplayList(mesh5, 5, 2);//skybox
	display6 = meshToDisplayList(mesh6, 6, 1);//oldbox
	display7 = meshToDisplayList(mesh7, 1, 0);//flat plain
	display8 = meshToDisplayList(mesh8, 7, 5);

	// shadow
	glClearStencil(0);
	// floor vertex
	dot_vertex_floor.push_back(Vec3<GLfloat>(-2000.0, 0.0, 2000.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(2000.0, 0.0, 2000.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(2000.0, 0.0, -2000.0));
	dot_vertex_floor.push_back(Vec3<GLfloat>(-2000.0, 0.0, -2000.0));
	calculate_floor_normal(&floor_normal, dot_vertex_floor);
	// light
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

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
	light_position[0] = 500 * cos(lightAngle);
	light_position[1] = lightHeight;
	light_position[2] = 500 * sin(lightAngle);
	light_position[3] = 0.0; // directional light
	lightAngle += 0.0005;
	// Calculate Shadow matrix
	shadowMatrix(shadow_matrix, floor_normal, light_position);
	// projection and view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(45.0, ratio, 10, 100000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// lookAt 
	gluLookAt(0.0f, 40.0f, 320.0, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	// camera
	glScalef(scale, scale, scale);
	glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);



	if (reflection) {
		// draw
		glPushMatrix();
		// Tell GL new light source position

		// ===== STENCIL DRAW ============================================================================

		glEnable(GL_STENCIL_TEST); //Start using the stencil
		glDisable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //Disable writing colors in frame buffer
		glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF); //Place a 1 where rendered
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); 	//Replace where rendered
		// PLAIN for the stencil
		if (terrain) {
			glPushMatrix();
			glTranslatef(-900, 0, -900);
			glCallList(display1);
			glPopMatrix();
		}
		else {
			glPushMatrix();
			glTranslatef(-900, 0, -900);
			glCallList(display7);
			glPopMatrix();
		}
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //Reenable color
		glEnable(GL_DEPTH_TEST);
		glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //Keep the pixel



		// boxes reflections
		if (noise) {
			glPushMatrix();
			glScalef(1.0, -1.0, 1.0);
			glCallList(display2);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(200, 0, 0);
			glScalef(1.0, -1.0, 1.0);
			glCallList(display3);
			glPopMatrix();
		}
		else {
			glPushMatrix();
			glScalef(1.0, -1.0, 1.0);
			glCallList(display6);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(200, 0, 0);
			glScalef(1.0, -1.0, 1.0);
			glCallList(display6);
			glPopMatrix();
		}
		glPushMatrix();
		glTranslatef(-200, 200, 0);
		glScalef(50.0, -50.0, 50.0);
		glCallList(display4);
		glPopMatrix();


		// STENCIL-STEP 4. disable it
		glDisable(GL_STENCIL_TEST);
	}


	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	if (shadow) {
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
		if (terrain) {
			glTranslatef(-900, 0, -900);
			glCallList(display1);
		}
		else {
			glTranslatef(-900, 0, -900);
			glCallList(display7);
		}
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
			glCallList(display2);
			glTranslatef(200, 0, 0);
			glCallList(display3);
			glTranslatef(-400, 0, 0);
			glScalef(50, 50, 50);
			glCallList(display4);
			glEnable(GL_DEPTH_TEST);
			glPopMatrix();
			glDisable(GL_BLEND);
			glEnable(GL_LIGHTING);
			// To eliminate depth buffer artifacts, use glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable(GL_STENCIL_TEST);
		}
	}
	else {
		if (terrain) {
			glEnable(GL_BLEND);
			glDisable(GL_LIGHTING);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.7, 0.0, 0.0, 0.3);
			glColor4f(1.0, 1.0, 1.0, 0.3);
			glPushMatrix();
			glTranslatef(-900, 0, -900);
			glCallList(display1);
			glPopMatrix();
			glEnable(GL_LIGHTING);
			glDisable(GL_BLEND);
		}
		else {
			glEnable(GL_BLEND);
			glDisable(GL_LIGHTING);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.7, 0.0, 0.0, 0.3);
			glColor4f(1.0, 1.0, 1.0, 0.3);
			glPushMatrix();
			glTranslatef(-900, 0, -900);
			glCallList(display7);
			glPopMatrix();
			glEnable(GL_LIGHTING);
			glDisable(GL_BLEND);
		}
		//glTranslatef(0, 0, 0);
	}

	if (noise) {
		// box 1
		glPushMatrix();
		glCallList(display2);
		glPopMatrix();
		// box 2
		glPushMatrix();
		glTranslatef(200, 0, 0);
		glCallList(display3);
		glPopMatrix();
	}
	else {
		// box 1
		glPushMatrix();
		glCallList(display6);
		glPopMatrix();
		// box 2
		glPushMatrix();
		glTranslatef(200, 0, 0);
		glCallList(display6);
		glPopMatrix();
	}
	// box 3
	glPushMatrix();
	glTranslatef(-200, 0, 0);
	glScalef(50, 50, 50);
	glCallList(display4);
	glPopMatrix();

	//lava texture
	glPushMatrix();
	glTranslatef(-1500, -1, -1500);
	//glScalef(50, 50, 50);
	glCallList(display8);
	glPopMatrix();
	//bounding box
	if (bbox) {
		glPushMatrix();
		glTranslatef(-295, 235, 0);
		glScalef(4, 2, 6);
		glutWireCube(100);
		glPopMatrix();
	}
	if (bbox && terrain) {
		glPushMatrix();
		glTranslatef(0, 250, -615);
		glScalef(0.9, 0.25, 0.30);
		glutWireCube(2000);
		glPopMatrix();
	}
	//skybox
	if (sbox) {
		glPushMatrix();
		glTranslatef(-1500, -2, -1500);
		glCallList(display5);
		glPopMatrix();
	}
	// draw the light arrow	
	drawLightArrow();
	//fog begin
	if (fog) {
		glPushMatrix();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		GLfloat fogColor[4] = { 0.5,0.5,0.5,1 };
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY,.85);
		glFogf(GL_FOG_START, 50.0);
		glFogf(GL_FOG_END, 2000.0);
		glFogf(GL_FOG_INDEX, 100);
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}
	else {
		glDisable(GL_FOG);
	}
	//fog end
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, window_width, 0, window_height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	renderBitmapString(0.0, window_height - 15.0f, 0.0f, "Use [t or T] to toggle Terrain");
	renderBitmapString(0.0, window_height - 30.0f, 0.0f, "Use [n or N] to toggle Noise");
	renderBitmapString(0.0, window_height - 45.0f, 0.0f, "Use [o or O] to toggle Shadows");
	renderBitmapString(0.0, window_height - 60.0f, 0.0f, "Use [r or R] to toggle Reflectoin");
	renderBitmapString(0.0, window_height - 75.0f, 0.0f, "Use [b or B] to toggle Bounding Box");
	renderBitmapString(0.0, window_height - 90.0f, 0.0f, "Use [z or Z] to toggle Skybox");
	renderBitmapString(0.0, window_height - 105.0f, 0.0f, "Use [f or F] to toggle Fog");
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopMatrix();
	glutSwapBuffers();
}



// main
void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("Shadows");
	// callbacks
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutSpecialFunc(specialkeys);
	glutKeyboardFunc(callbackKeyboard);
	init();
	glutMainLoop();
}