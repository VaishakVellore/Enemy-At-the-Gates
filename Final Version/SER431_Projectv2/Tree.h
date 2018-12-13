#pragma once
#include <vector>
#include "Imathvec.h"
#include<string>
#include<fstream>
#include <math.h>
#include <limits.h>           
#include <float.h>          
#include <time.h>
#include "core.h"
#include "GL/glut.h"

#define random_number() (((float) rand())/((float) RAND_MAX))
#define MAXLEVEL 8
int Level = 4;
long TreeSeed;
int TREE = 0, STEM = 1, LEAF = 2, LEAF_MAT = 3, TREE_MAT = 4, STEMANDLEAVES = 5, FULLTREE = 10;

// create a tree as fractal
void FractalTree(int level) {
	long savedseed;
	if (level == Level) {
		glPushMatrix();
		glRotatef(random_number() * 180, 0, 1, 0);
		glCallList(STEMANDLEAVES);
		glPopMatrix();
	}
	else {
		glCallList(STEM);
		glPushMatrix();
		glRotatef(random_number() * 180, 0, 1, 0);
		glTranslatef(0, 1, 0);
		glScalef(0.7, 0.7, 0.7);
		glPushMatrix();
		glRotatef(110 + random_number() * 40, 0, 1, 0);
		glRotatef(30 + random_number() * 20, 0, 0, 1);
		FractalTree(level + 1);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-130 + random_number() * 40, 0, 1, 0);
		glRotatef(30 + random_number() * 20, 0, 0, 1);
		FractalTree(level + 1);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-20 + random_number() * 40, 0, 1, 0);
		glRotatef(30 + random_number() * 20, 0, 0, 1);
		FractalTree(level + 1);
		glPopMatrix();
		glPopMatrix();
	}
}

// Create display lists for a leaf, a set of leaves, and a stem
void CreateTreeLists(void) {
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
	glNewList(TREE_MAT, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tree_ambuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, tree_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, tree_shininess);
	glEndList();
	// leaf material
	glNewList(LEAF_MAT, GL_COMPILE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, leaf_ambuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, leaf_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, leaf_shininess);
	glEndList();
	// steam
	glNewList(STEM, GL_COMPILE);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluCylinder(cylquad, 0.1, 0.08, 1, 10, 2);
	glPopMatrix();
	glEndList();
	// leaf
	glNewList(LEAF, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	glNormal3f(-0.1, 0, 0.25);
	glVertex3f(0, 0, 0);
	glVertex3f(0.25, 0.25, 0.1);
	glVertex3f(0, 0.5, 0);
	glNormal3f(0.1, 0, 0.25);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0.5, 0);
	glVertex3f(-0.25, 0.25, 0.1);
	glEnd();
	glEndList();
	// steam&leaf
	glNewList(STEMANDLEAVES, GL_COMPILE);
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glCallList(STEM);
	glCallList(LEAF_MAT);
	for (i = 0; i < 3; i++) {
		glTranslatef(0, 0.333, 0);
		glRotatef(90, 0, 1, 0);
		glPushMatrix();
		glRotatef(0, 0, 1, 0);
		glRotatef(50, 1, 0, 0);
		glCallList(LEAF);
		glPopMatrix();
		glPushMatrix();
		glRotatef(180, 0, 1, 0);
		glRotatef(60, 1, 0, 0);
		glCallList(LEAF);
		glPopMatrix();
	}
	glPopAttrib();
	glPopMatrix();
	glEndList();
	//
	glNewList(FULLTREE, GL_COMPILE);
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glCallList(TREE_MAT);
	glTranslatef(0, -1, 0);
	FractalTree(0);
	glPopAttrib();
	glPopMatrix();
	glEndList();
}


