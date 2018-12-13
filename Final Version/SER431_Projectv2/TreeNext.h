#pragma once
#include "Tree.h"
#include "GL/glut.h"

#define random_number() (((float) rand())/((float) RAND_MAX))
int LLevel = 4;
long LTreeSeed;
int  LSTEM = 1, LLEAF = 2, LLEAF_MAT = 3, LTREE_MAT = 4, LSTEMANDLEAVES = 5, TTREE = 10;

// create a tree as fractal
void FractalTreeLocal(int level) {
	long savedseed;
	if (level == LLevel) {
		glPushMatrix();
		glRotatef(random_number() * 180, 0, 1, 0);
		glCallList(LSTEMANDLEAVES);
		glPopMatrix();
	}
	else {
		glCallList(LSTEM);
		glPushMatrix();
		glRotatef(random_number() * 180, 0, 1, 0);
		glTranslatef(0, 1, 0);
		glScalef(0.7, 0.7, 0.7);
		glPushMatrix();
		glRotatef(110 + random_number() * 40, 0, 1, 0);
		glRotatef(30 + random_number() * 20, 0, 0, 1);
		FractalTreeLocal(level + 1);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-130 + random_number() * 40, 0, 1, 0);
		glRotatef(30 + random_number() * 20, 0, 0, 1);
		FractalTreeLocal(level + 1);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-20 + random_number() * 40, 0, 1, 0);
		glRotatef(30 + random_number() * 20, 0, 0, 1);
		FractalTreeLocal(level + 1);
		glPopMatrix();
		glPopMatrix();
	}
}

// Create display lists for a leaf, a set of leaves, and a stem
void CreateTreeListsLocal(void) {
	GLUquadricObj *cylquad = gluNewQuadric();
	int i;
	// materials
	GLfloat tree_ambuse[] = { 0.4, 0.25, 0.1, 1.0 };
	GLfloat tree_specular[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat tree_shininess[] = { 0 };
	GLfloat leaf_ambuse[] = { 0.0, 0.8, 0.0, 1.0 };
	GLfloat leaf_specular[] = { 0.0, 0.8, 0.0, 1.0 };
	GLfloat leaf_shininess[] = { 10 };
	// tree material
	glNewList(LTREE_MAT, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tree_ambuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, tree_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, tree_shininess);
	glEndList();
	// leaf material
	glNewList(LLEAF_MAT, GL_COMPILE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, leaf_ambuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, leaf_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, leaf_shininess);
	glEndList();
	// steam
	glNewList(LSTEM, GL_COMPILE);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluCylinder(cylquad, 0.1, 0.08, 1, 10, 2);
	glPopMatrix();
	glEndList();

	glNewList(LSTEMANDLEAVES, GL_COMPILE);
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glCallList(LSTEM);
	glCallList(LLEAF_MAT);
	for (i = 0; i < 3; i++) {
		glTranslatef(0, 0.333, 0);
		glRotatef(90, 0, 1, 0);
		glPushMatrix();
		glRotatef(0, 0, 1, 0);
		glRotatef(50, 1, 0, 0);

		glPopMatrix();
		glPushMatrix();
		glRotatef(180, 0, 1, 0);
		glRotatef(60, 1, 0, 0);

		glPopMatrix();
	}
	glPopAttrib();
	glPopMatrix();
	glEndList();
	//
	glNewList(TTREE, GL_COMPILE);
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glCallList(LTREE_MAT);
	glTranslatef(0, -1, 0);
	FractalTreeLocal(0);
	glPopAttrib();
	glPopMatrix();
	glEndList();
}

