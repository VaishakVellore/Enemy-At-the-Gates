#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "core.h"
#include <Windows.h>
#include <glaux.h>
#include <GL/glut.h>

#define V_NUMPOINTS    4
#define U_NUMPOINTS	   4
#define V_NUMKNOTS    (V_NUMPOINTS + 4)
#define U_NUMKNOTS    (U_NUMPOINTS + 4)

// Knot sequences for cubic bezier surface and trims

GLfloat sknots[V_NUMKNOTS] = { 0., 0., 0., 0., 1., 1., 1., 1. };

GLfloat tknots[U_NUMKNOTS] = { 0., 0., 0., 0., 1., 1., 1., 1. };

time_t timer;



const int V_size = 4;

const int U_size = 4;

const int ORDER = 4;



// Control points for the flag. The Z values are modified to make it wave

GLfloat ctlpoints[V_NUMPOINTS][U_NUMPOINTS][3] = {

		   { { -1000.0, 13., 0. },{ -333.3, 13., 0. },{ 333.3, 13., 0 },{ 1000.0, 13., 0. } },

		   { { -1000.0, 12., 666.6 },{ -333.3, 12., 666.6 },{ 333.3, 12., 666.6 },{ 1000.0, 12., 666.6 } },

		   { { -1000.0, 11., 1333.3 },{ -333.3, 11., 1333.3 },{ 333.3, 11., 1333.3 },{ 1000.0, 11., 1333.3 } },

		   { { -1000.0, 10., 2000.0 },{ -333.3, 10., 2000.0 },{ 333.3, 10., 2000.0 },{ 1000.0, 10., 2000.0 } }

};
