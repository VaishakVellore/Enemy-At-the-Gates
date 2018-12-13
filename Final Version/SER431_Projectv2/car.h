#pragma once
#include <gl/glut.h>
#include "core.h"

//displaying surprise element
void displayCar(GLfloat x, GLfloat y, GLfloat z) {
	Mesh *car = loadFile("porsche.obj");
	calculateNormalPerFace(car);
	calculateNormalPerVertex(car);
	GLuint carDisplay = meshToDisplayList(car, 4, 2);
	glTranslatef(x, y, z);
	glCallList(carDisplay);
}