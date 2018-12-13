/**
* Javier Gonzalez-Sanchez
* Fall 2018
*
* http://javiergs.com/teaching/ser431
*/

#pragma once
#include <vector>
#include <Imathvec.h>
#include <GL/glut.h>
#include<iostream>
#include<fstream>
#include<string>
#include "noise.h"
typedef unsigned int UINT;

using namespace std;
using namespace Imath;

typedef Vec3<float> Vec3f;
typedef Vec2<float> Vec2f;

float bearing = 0.0f;
float bearing_left = 0.0;
float bearingx = 0.0f;


int window_width = 800, window_height = 800;
const float window_ratio = 1.0;

// camera
float camera_x = 0.0;
float camera_y = 300.0;
float camera_z = 320.0;

float camera_viewing_x = 0.0;
float camera_viewing_y = 301.0;
float camera_viewing_z = 0.0;

float total_moving_angle = 0.0;

// mesh data structure
struct Mesh {
	// vertex
	vector<Vec3f> dot_vertex;
	vector<Vec3f> dot_normalPerFace;
	vector<Vec3f> dot_normalPerVertex;
	vector<Vec2f> dot_texture;
	// faces
	vector<int> face_index_vertex;
	vector<int> face_index_normalPerFace;
	vector<int> face_index_normalPerVertex;
	vector<int> face_index_texture;
};

// global 
int width = 1200;
int height = 600;
float ratio = 1.0;
GLuint mountainDisplay, skyboxDisplay, riverDisplay, f16Display, waterFallDisplay, lavaDisplay, fuelCellDisplay;

// meshes
Mesh* mountainMesh;
Mesh* skybox;
Mesh* riverBedMesh;
Mesh* f16Mesh;
Mesh* waterFallMesh;
Mesh* lavaDisplayMesh;
Mesh* fuelCellDisplayMesh;

// controling parameters
int mouse_button;
int mouse_x = 0;
int mouse_y = 0;
float scale = 0.3;
float x_angle = 0.0;
float y_angle = 0.0;

GLboolean light = true, fog = true, noise = true, terrain = true, bbox = true, sbox = true, reflection = true, shadow = true;
GLboolean particles = true, riverCurve = true, menu = true;
//GLfloat light_position[] = { 0.0, 50.0, 0.0, 0.0 };
float altitude = 600;
//unsigned int width, height;

// textures
GLuint textureArray[8];



void loadBMP_custom(GLuint textureArray[], const char * imagepath, int n) {

	printf("Reading image %s\n", imagepath);
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	//unsigned int width, height;
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath, "rb");
	if (!file) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
		getchar();
		return;
	}
	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		fclose(file);
		return;
	}
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		fclose(file);
		return;
	}
	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    fclose(file); return; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    fclose(file); return; }
	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	// Create one OpenGL texture
	glGenTextures(1, &textureArray[n]);
	glBindTexture(GL_TEXTURE_2D, textureArray[n]);

	glGenTextures(1, &textureArray[n]);
	glBindTexture(GL_TEXTURE_2D, textureArray[n]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // must set to 1 for compact data
										   // glTexImage2D Whith size and minification
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	delete[] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

}

// OBJ file - str to int
int StrToInt(const string &str) {
	int i;
	if (sscanf_s(str.c_str(), "%i", &i) == 1) return i;
	else return 0;
}

// OBJ file - split string
vector<string> split_string(const string& str, const string& split_str) {
	vector<string> stlv_string;
	string part_string("");
	string::size_type i;
	i = 0;
	while (i < str.size()) {
		if (split_str.find(str[i]) != string::npos) {
			stlv_string.push_back(part_string);
			part_string = "";
			while (split_str.find(str[i]) != string::npos) {
				++i;
			}
		}
		else {
			part_string += str[i];
			++i;
		}
	}
	if (!part_string.empty())
		stlv_string.push_back(part_string);
	return stlv_string;
}


// t_scale
float t_scale(float x) {
	// assuming noise report values -2.xxx to 2.xxx
	return (x + 2) / 4;
}


// marbleMap
Vec3f marbleMap(float a) {
	Vec3f black = Vec3f(0, 0, 0);
	Vec3f color = Vec3f(1, 1, 1);
	return ((1 - a) * color + a * black);
}

// skyMap
Vec3f skyMap(float a) {
	Vec3f white = Vec3f(1, 1, 1);
	Vec3f color = Vec3f(1, 0, 0);
	return (a * white + (1 - a)* color);  //
}

//lavaFire
Vec3f lavaFire(float a) {
	Vec3f red = Vec3f(0, 0, 3);
	Vec3f yellow = Vec3f(0, 3, 3);
	return (a * red + (1 - a)* yellow);  //lava colors
}

// Create texture from algorithm
void codedTexture(UINT textureArray[], int n, int type) {
	const int TexHeight = 128;
	const int TexWidth = 128;
	// create texture in memory
	GLubyte textureImage[TexHeight][TexWidth][3];
	ImprovedNoise noise;
	Vec3f pixelColor;
	for (int i = 0; i < TexHeight; i++)
		for (int j = 0; j < TexWidth; j++) {
			//pixelColor = skyMap(t_scale(noise.perlinMultiscale(i * 5, j * 5)));
			if (type == 0) pixelColor = skyMap(t_scale(noise.perlinMultiscale(i * 5, j * 5)));
			else if (type == 1) pixelColor = marbleMap(t_scale(noise.perlinMarble(i * 5, j * 5)));
			else pixelColor = lavaFire(t_scale(noise.perlinLava(i * 5, j * 5)));//lavaplanex

			textureImage[i][j][0] = pixelColor[0] * 255;
			textureImage[i][j][1] = pixelColor[1] * 255;
			textureImage[i][j][2] = pixelColor[2] * 255;
		}
	// setup texture
	glGenTextures(1, &textureArray[n]);
	glBindTexture(GL_TEXTURE_2D, textureArray[n]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // must set to 1 for compact data
										   // glTexImage2D Whith size and minification
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TexWidth, TexHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, textureImage); // BGRA to include alpha
}

// OBJ file - load file
Mesh* loadFile(const char* file) {
	Mesh *m = new Mesh;
	m->dot_vertex.clear();
	m->face_index_vertex.clear();
	ifstream infile(file);
	if (infile.fail()) {
		std::cout << "Error opening file " << file;
		return NULL;
	}
	char current_line[1024];
	while (!infile.eof()) {
		infile.getline(current_line, 1024);
		switch (current_line[0]) {
		case'v':
			float x, y, z;
			switch (current_line[1]) {
			case 'n':
				sscanf_s(current_line, "vn %f %f %f", &x, &y, &z);
				m->dot_normalPerFace.push_back(Vec3f(x, y, z));
				m->dot_normalPerVertex.push_back(Vec3f(x, y, z));
				break;
			case 't':
				sscanf_s(current_line, "vt %f %f", &x, &y);
				m->dot_texture.push_back(Vec2f(x, y));
				break;
			default:
				sscanf_s(current_line, "v %f %f %f", &x, &y, &z);
				m->dot_vertex.push_back(Vec3f(x, y, z));
				break;
			}
			break;
		case 'f': {
			vector<string> faces = split_string(current_line, " \t.\r\n");
			int vnt[3][3] = { { -1, -1, -1 },{ -1, -1, -1 },{ -1, -1, -1 } };
			string::size_type begin, end;
			for (int i = 0; i < 2; i++) {
				begin = 0;
				int j = 0;
				do {
					end = faces[i + 1].find_first_of("/", begin);
					if (begin < end) {
						vnt[i][j] = StrToInt(faces[i + 1].substr(begin, end - begin)) - 1;
					}
					else {
						vnt[i][j] = -1;
					}
					begin = end + 1;
					j++;
				} while (end != string::npos);
			}
			for (unsigned int i = 3; i < faces.size(); i++) {
				begin = 0;
				int j = 0;
				do {
					end = faces[i].find_first_of("/", begin);
					if (begin < end) {
						vnt[2][j] = StrToInt(faces[i].substr(begin, end - begin)) - 1;
					}
					else {
						vnt[2][j] = -1;
					}
					begin = end + 1;
					j++;
				} while (end != string::npos);
				for (int j = 0; j < 3; j++) {
					m->face_index_vertex.push_back(vnt[j][0]);
					if (vnt[j][1] != -1) m->face_index_texture.push_back(vnt[j][1]);
					if (vnt[j][2] != -1) m->face_index_normalPerFace.push_back(vnt[j][2]);
					if (vnt[j][2] != -1) m->face_index_normalPerVertex.push_back(vnt[j][2]);
				}
				memcpy(&vnt[1], &vnt[2], sizeof(int) * 3);
			}
		}
				  break;
		default:
			break;
		}
	}
	return m;
}

// create a triangulated box
Mesh* createSkyBox(int w) {
	Mesh *mesh = new Mesh;
	// Vertexes
	int BOX_WIDTH = w;
	int BOX_HEIGHT = w;
	int BOX_DEPTH = w;
	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, BOX_HEIGHT, BOX_DEPTH));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, BOX_HEIGHT, BOX_DEPTH));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, 0.0, BOX_DEPTH));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, 0.0, BOX_DEPTH));

	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, BOX_HEIGHT, 0.0));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, BOX_HEIGHT, 0.0));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, 0.0, 0.0));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, 0.0, 0.0));

	// face 1
	mesh->face_index_vertex.push_back(1);//0
	mesh->face_index_vertex.push_back(2);//1
	mesh->face_index_vertex.push_back(0);//2
	mesh->face_index_vertex.push_back(2);//0
	mesh->face_index_vertex.push_back(3);//2
	mesh->face_index_vertex.push_back(0);//3
										 // face 2
	mesh->face_index_vertex.push_back(4);//0
	mesh->face_index_vertex.push_back(5);//1
	mesh->face_index_vertex.push_back(0);//5
	mesh->face_index_vertex.push_back(5);//0
	mesh->face_index_vertex.push_back(1);//5
	mesh->face_index_vertex.push_back(0);//4
										 // face 3
	mesh->face_index_vertex.push_back(5);//1
	mesh->face_index_vertex.push_back(6);//2
	mesh->face_index_vertex.push_back(1);//6
	mesh->face_index_vertex.push_back(6);//1
	mesh->face_index_vertex.push_back(2);//6
	mesh->face_index_vertex.push_back(1);//5
										 // face 4
	mesh->face_index_vertex.push_back(6);//2
	mesh->face_index_vertex.push_back(7);//3
	mesh->face_index_vertex.push_back(2);//7
	mesh->face_index_vertex.push_back(7);//2
	mesh->face_index_vertex.push_back(3);//7
	mesh->face_index_vertex.push_back(2);//6
										 // face 5
	mesh->face_index_vertex.push_back(0);//0
	mesh->face_index_vertex.push_back(3);//3
	mesh->face_index_vertex.push_back(4);//7
	mesh->face_index_vertex.push_back(3);//0
	mesh->face_index_vertex.push_back(7);//7
	mesh->face_index_vertex.push_back(4);//4
										 // face 6
	mesh->face_index_vertex.push_back(4);//4 
	mesh->face_index_vertex.push_back(7);//5
	mesh->face_index_vertex.push_back(5);//6
	mesh->face_index_vertex.push_back(7);//4
	mesh->face_index_vertex.push_back(6);//6
	mesh->face_index_vertex.push_back(5);//7
	// ---------------------------------------------------------------------------------------
	//face with number 2
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 2.0 / 3.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.25, 2.0 / 3.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.25, 1.0 / 3.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0 / 3.0));
	mesh->face_index_texture.push_back(1);//0
	mesh->face_index_texture.push_back(2);//1
	mesh->face_index_texture.push_back(0);//2
	mesh->face_index_texture.push_back(2);//0
	mesh->face_index_texture.push_back(3);//2
	mesh->face_index_texture.push_back(0);//3
	//face with number 1
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.25, 1.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.50, 1.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.50, 0.66));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.25, 0.66));
	mesh->face_index_texture.push_back(5);//0
	mesh->face_index_texture.push_back(6);//1
	mesh->face_index_texture.push_back(4);//2
	mesh->face_index_texture.push_back(6);//0
	mesh->face_index_texture.push_back(7);//2
	mesh->face_index_texture.push_back(4);//3
	 //face with number 3
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.25, 2.0 / 3.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.50, 2.0 / 3.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.50, 1.0 / 3.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.25, 1.0 / 3.0));
	mesh->face_index_texture.push_back(9);//0
	mesh->face_index_texture.push_back(10);//1
	mesh->face_index_texture.push_back(8);//2
	mesh->face_index_texture.push_back(10);//0
	mesh->face_index_texture.push_back(11);//2
	mesh->face_index_texture.push_back(8);//3
	//face with number 6
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.25, 1.0 / 3));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.50, 1.0 / 3));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.50, 0.0 / 3));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.25, 0.0 / 3));
	mesh->face_index_texture.push_back(13);//0
	mesh->face_index_texture.push_back(14);//1
	mesh->face_index_texture.push_back(12);//2
	mesh->face_index_texture.push_back(14);//0
	mesh->face_index_texture.push_back(15);//2
	mesh->face_index_texture.push_back(12);//3
	//face with number 5
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.75, 2.0 / 3));
	mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 2.0 / 3));
	mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0 / 3));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.75, 1.0 / 3));

	mesh->face_index_texture.push_back(17);//0
	mesh->face_index_texture.push_back(18);//1
	mesh->face_index_texture.push_back(16);//2
	mesh->face_index_texture.push_back(18);//0
	mesh->face_index_texture.push_back(19);//2
	mesh->face_index_texture.push_back(16);//3
	//face with number 4
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.50, 2.0 / 3));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.75, 2.0 / 3));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.75, 1.0 / 3));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.50, 1.0 / 3));
	mesh->face_index_texture.push_back(21);//0
	mesh->face_index_texture.push_back(22);//1
	mesh->face_index_texture.push_back(20);//2
	mesh->face_index_texture.push_back(22);//0
	mesh->face_index_texture.push_back(23);//2
	mesh->face_index_texture.push_back(20);//3

	// ---------------------------------------------------------------------------------------
	return mesh;
}

Mesh* createRiver(int arena_width, int arena_depth, int arena_cell) {
	Mesh *me = new Mesh;
	int n = (arena_cell + arena_width) / arena_cell;
	int m = (arena_cell + arena_depth) / arena_cell;

	// vertices
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {

			me->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, 0, j*arena_cell));//100*y_scale(noise.perlinMarble(i,j))



		}
	}
	//texture
	me->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));
	me->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
	me->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
	me->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
	// faces
	for (int i = 0; i < (n*m) - m; i++) {
		if ((i + 1) % n == 0) continue;
		me->face_index_vertex.push_back(i); me->face_index_vertex.push_back(i + 1);
		me->face_index_vertex.push_back(i + n);
		me->face_index_vertex.push_back(i + 1); me->face_index_vertex.push_back(i + n + 1);
		me->face_index_vertex.push_back(i + n);
		// index for texture
		for (int t = 0; t < 6; t++) {
			me->face_index_texture.push_back(3);//0
			me->face_index_texture.push_back(2);//2
			me->face_index_texture.push_back(1);//1
			me->face_index_texture.push_back(2);//0
			me->face_index_texture.push_back(0);//3
			me->face_index_texture.push_back(1);//2
		}
	}
	return me;
}

// create a triangulated diamond
Mesh* createCube() {
	Mesh *mesh = new Mesh;
	// Vertexes
	int BOX_WIDTH = 100;
	int BOX_HEIGHT = 100;
	int BOX_DEPTH = 100;

	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, BOX_HEIGHT, BOX_DEPTH));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, BOX_HEIGHT, BOX_DEPTH));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, 0.0, BOX_DEPTH));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, 0.0, BOX_DEPTH));

	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, BOX_HEIGHT, 0.0));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, BOX_HEIGHT, 0.0));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, 0.0, 0.0));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, 0.0, 0.0));

	// face 1
	mesh->face_index_vertex.push_back(0);//0
	mesh->face_index_vertex.push_back(2);//1
	mesh->face_index_vertex.push_back(1);//2
	mesh->face_index_vertex.push_back(0);//0
	mesh->face_index_vertex.push_back(3);//2
	mesh->face_index_vertex.push_back(2);//3
										 // face 2
	mesh->face_index_vertex.push_back(0);//0
	mesh->face_index_vertex.push_back(5);//1
	mesh->face_index_vertex.push_back(4);//5
	mesh->face_index_vertex.push_back(0);//0
	mesh->face_index_vertex.push_back(1);//5
	mesh->face_index_vertex.push_back(5);//4
										 // face 3
	mesh->face_index_vertex.push_back(1);//1
	mesh->face_index_vertex.push_back(6);//2
	mesh->face_index_vertex.push_back(5);//6
	mesh->face_index_vertex.push_back(1);//1
	mesh->face_index_vertex.push_back(2);//6
	mesh->face_index_vertex.push_back(6);//5
										 // face 4
	mesh->face_index_vertex.push_back(2);//2
	mesh->face_index_vertex.push_back(7);//3
	mesh->face_index_vertex.push_back(6);//7
	mesh->face_index_vertex.push_back(2);//2
	mesh->face_index_vertex.push_back(3);//7
	mesh->face_index_vertex.push_back(7);//6
										 // face 5
	mesh->face_index_vertex.push_back(4);//0
	mesh->face_index_vertex.push_back(3);//3
	mesh->face_index_vertex.push_back(0);//7
	mesh->face_index_vertex.push_back(4);//0
	mesh->face_index_vertex.push_back(7);//7
	mesh->face_index_vertex.push_back(3);//4
										 // face 6
	mesh->face_index_vertex.push_back(5);//4 
	mesh->face_index_vertex.push_back(7);//5
	mesh->face_index_vertex.push_back(4);//6
	mesh->face_index_vertex.push_back(5);//4
	mesh->face_index_vertex.push_back(6);//6
	mesh->face_index_vertex.push_back(7);//7

										 // texture
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));
	for (int t = 0; t < 6; t++) {
		mesh->face_index_texture.push_back(0);//0
		mesh->face_index_texture.push_back(2);//1
		mesh->face_index_texture.push_back(1);//2
		mesh->face_index_texture.push_back(0);//0
		mesh->face_index_texture.push_back(3);//2
		mesh->face_index_texture.push_back(2);//3
	}

	return mesh;
}

// creating a triangulated plane
Mesh* createPlane(int arena_width, int arena_depth, int arena_cell) {
	Mesh *me = new Mesh;
	int n = arena_width / arena_cell;
	int m = arena_depth / arena_cell;

	// vertices
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (i >= (5 * m / 12) && i <= (7 * m / 12)) {
				me->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, 0.0, j*arena_cell));
			}
			else {
				me->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, rand() % 1000, j*arena_cell));
			}
		}
	}
	//texture
	me->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));
	me->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
	me->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
	me->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
	// faces
	for (int i = 0; i < (n*m) - m; i++) {
		if ((i + 1) % n == 0) continue;
		me->face_index_vertex.push_back(i); me->face_index_vertex.push_back(i + 1);
		me->face_index_vertex.push_back(i + n);
		me->face_index_vertex.push_back(i + 1); me->face_index_vertex.push_back(i + n + 1);
		me->face_index_vertex.push_back(i + n);
		// index for texture
		for (int t = 0; t < 6; t++) {
			me->face_index_texture.push_back(3);//0
			me->face_index_texture.push_back(2);//2
			me->face_index_texture.push_back(1);//1
			me->face_index_texture.push_back(2);//0
			me->face_index_texture.push_back(0);//3
			me->face_index_texture.push_back(1);//2
		}
	}
	return me;
}

// creating a triangulated plane
Mesh* createTerrain(int arena_width, int arena_depth, int arena_cell) {
	Mesh *me = new Mesh;
	int n = (arena_cell + arena_width) / arena_cell;
	int m = (arena_cell + arena_depth) / arena_cell;

	// vertices
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (i < ((5 * n) / 12) || i>(7 * n / 12)) {
				me->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, rand() % 500, j*arena_cell));//100*y_scale(noise.perlinMarble(i,j))
			}
			else {
				me->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, 0, j*arena_cell));//100*y_scale(noise.perlinMarble(i,j))
			}

		}
	}
	//texture
	me->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));
	me->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
	me->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
	me->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
	// faces
	for (int i = 0; i < (n*m) - m; i++) {
		if ((i + 1) % n == 0) continue;
		me->face_index_vertex.push_back(i); me->face_index_vertex.push_back(i + 1);
		me->face_index_vertex.push_back(i + n);
		me->face_index_vertex.push_back(i + 1); me->face_index_vertex.push_back(i + n + 1);
		me->face_index_vertex.push_back(i + n);
		// index for texture
		for (int t = 0; t < 6; t++) {
			me->face_index_texture.push_back(3);//0
			me->face_index_texture.push_back(2);//2
			me->face_index_texture.push_back(1);//1
			me->face_index_texture.push_back(2);//0
			me->face_index_texture.push_back(0);//3
			me->face_index_texture.push_back(1);//2
		}
	}
	return me;
}




// normal per face
void calculateNormalPerFace(Mesh* m) {
	Vec3<float> v1, v2, v3, v4, v5;
	for (int i = 0; i < m->face_index_vertex.size(); i += 3) {
		v1 = m->dot_vertex[m->face_index_vertex[i]];
		v2 = m->dot_vertex[m->face_index_vertex[i + 1]];
		v3 = m->dot_vertex[m->face_index_vertex[i + 2]];
		v4 = (v2 - v1);
		v5 = (v3 - v1);
		v4 = v4.cross(v5);
		v4.normalize();
		m->dot_normalPerFace.push_back(v4);
		int pos = m->dot_normalPerFace.size() - 1;
		// same normal for all vertex in this face
		m->face_index_normalPerFace.push_back(pos);
		m->face_index_normalPerFace.push_back(pos);
		m->face_index_normalPerFace.push_back(pos);
	}
}

// calculate normal per vertex
void calculateNormalPerVertex(Mesh* m) {
	m->dot_normalPerVertex.clear();
	m->face_index_normalPerVertex.clear();
	Vec3<float> suma; suma.x = 0; suma.y = 0; suma.z = 0;
	//initialize
	for (unsigned int val = 0; val < m->dot_vertex.size(); val++) {
		m->dot_normalPerVertex.push_back(suma);
	}
	// calculate sum for vertex
	for (long pos = 0; pos < m->face_index_vertex.size(); pos++) {
		m->dot_normalPerVertex[m->face_index_vertex[pos]] +=
			m->dot_normalPerFace[m->face_index_normalPerFace[pos]];
	}
	// normalize for vertex 
	for (unsigned int val = 0; val < m->dot_normalPerVertex.size(); val++) {
		m->dot_normalPerVertex[val] = m->dot_normalPerVertex[val].normalize();
	}
	//normalVertexIndex is the same that vertexIndex
	for (unsigned int pos = 0; pos < m->face_index_vertex.size(); pos++) {
		m->face_index_normalPerVertex.push_back(m->face_index_vertex[pos]);
	}
}

// mesh to display list
GLuint meshToDisplayList(Mesh* m, int id, int textureId) {
	GLuint listID = glGenLists(id);
	glNewList(listID, GL_COMPILE);
	//

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, textureArray[textureId]);

	//
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < m->face_index_vertex.size(); i++) {
		// PER VERTEX NORMALS
		if ((!m->dot_normalPerVertex.empty() && !m->face_index_normalPerVertex.empty())) {
			glNormal3fv(&m->dot_normalPerVertex[m->face_index_normalPerVertex[i]].x);
		}
		// TEXTURES
		if (!m->dot_texture.empty() && !m->face_index_texture.empty()) {
			glTexCoord2fv(&m->dot_texture[m->face_index_texture[i]].x);
		}
		// COLOR
		//Vec3f offset = (m->dot_vertex[m->face_index_vertex[i]]);
		// VERTEX
		//glColor3f(fabs(sin(offset.x)), fabs(cos(offset.y)), fabs(offset.z));
		glVertex3fv(&m->dot_vertex[m->face_index_vertex[i]].x);
	}
	glEnd();
	//
	glDisable(GL_TEXTURE_2D);
	//
	glEndList();
	return listID;
}

// reshape
void reshape(int w, int h) {
	width = w;
	height = h;
	if (h == 0) h = 1;
	ratio = 1.0f * w / h;
	glutPostRedisplay();
}

// mouse
void mouse(int button, int state, int x, int y) {
	mouse_x = x;
	mouse_y = y;
	mouse_button = button;
}

// motion
void motion(int x, int y) {
	if (mouse_button == GLUT_LEFT_BUTTON) {
		y_angle += (float(x - mouse_x) / width) *360.0;
		x_angle += (float(y - mouse_y) / height)*360.0;
	}
	if (mouse_button == GLUT_RIGHT_BUTTON) {
		scale += (y - mouse_y) / 100.0;
		if (scale < 0.1) scale = 0.1;
		if (scale > 7)	scale = 7;
	}
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}

// text
void renderBitmapString(float x, float y, float z, const char *string) {
	const char *c;
	glRasterPos3f(x, y, z);   // fonts position
	for (c = string; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
}

// rotate what the user see
void rotate_point(float angle) {
	float s = sin(angle);
	float c = cos(angle);
	// translate point back to origin:
	camera_viewing_x -= camera_x;
	camera_viewing_z -= camera_z;
	// rotate point
	float xnew = camera_viewing_x * c - camera_viewing_z * s;
	float znew = camera_viewing_x * s + camera_viewing_z * c;
	// translate point back:
	camera_viewing_x = xnew + camera_x;
	camera_viewing_z = znew + camera_z;
}

// callback function for keyboard (alfanumeric keys)
//void callbackKeyboard(unsigned char key, int x, int y) {
//	switch (key) {
//	case 'w': case 'W':
//		camera_viewing_y += (10);
//		camera_x += (100) * sin(total_moving_angle);//*0.1;
//		camera_z += (100) * -cos(total_moving_angle);//*0.1;
//		camera_viewing_x += (100) * sin(total_moving_angle);//*0.1;
//		camera_viewing_z += (100) * -cos(total_moving_angle);//*0.1;
//		break;
//	case 's': case 'S':
//		camera_viewing_y -= (10);
//		camera_x += (-100) * sin(total_moving_angle);//*0.1;
//		camera_z += (-100) * -cos(total_moving_angle);//*0.1;
//		camera_viewing_y -= 10;
//		camera_viewing_x += (-100) * sin(total_moving_angle);//*0.1;
//		camera_viewing_z += (-100) * -cos(total_moving_angle);//*0.1;
//		break;
//	case 'a': case 'A':
//		camera_viewing_x -= 100;
//		bearing_left -= 1.0f;
//
//		break;
//	case 'D': case 'd':
//		camera_viewing_x += 100;
//		bearing_left += 1.0f;
//
//		break;
//	case 'l': case 'L':
//		if (light) {
//			light = false;
//		}
//		else {
//			light = true;
//		}
//		break;
//	case 'f': case 'F':
//		if (fog) {
//			fog = false;
//		}
//		else {
//			fog = true;
//		}
//		break;
//	case 'n': case 'N':
//		if (noise) {
//			noise = false;
//		}
//		else {
//			noise = true;
//		}
//		break;
//	case 't':case 'T':
//		if (terrain) {
//			terrain = false;
//		}
//		else {
//			terrain = true;
//		}
//		break;
//	case 'b':case 'B':
//		if (bbox) {
//			bbox = false;
//		}
//		else {
//			bbox = true;
//		}
//		break;
//	case 'z':case 'Z':
//		if (sbox) {
//			sbox = false;
//		}
//		else {
//			sbox = true;
//		}break;
//	case 'r':case 'R':
//		if (reflection) {
//			reflection = false;
//		}
//		else {
//			reflection = true;
//		}
//		break;
//	case 'o':case 'O':
//		if (shadow) {
//			shadow = false;
//		}
//		else {
//			shadow = true;
//		}
//		break;
//	}
//
//}

// callback function for arrows
void specialkeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		//total_moving_angle += -0.01;
		//rotate_point(-0.01);
		//camera_viewing_x -= 100;
		bearing -= 1;
	}
	else if (key == GLUT_KEY_RIGHT) {
		//total_moving_angle += 0.01;
		//rotate_point(0.01);
		//camera_viewing_x += 100;
		bearing += 1;

	}
	else if (key == GLUT_KEY_DOWN) {
		//printf("Down key is pressed\n");
		//camera_z += 10;
		camera_viewing_y += (10);
		bearingx++;
		altitude = 600 + 10 * bearingx;
	}
	else if (key == GLUT_KEY_UP) {
		//printf("Up key is pressed\n");
		//camera_z -= 10;
		camera_viewing_y -= (10);
		bearingx--;
		altitude = 600 + 10 * bearingx;

		//camera_viewing_y += 10;
	}
}