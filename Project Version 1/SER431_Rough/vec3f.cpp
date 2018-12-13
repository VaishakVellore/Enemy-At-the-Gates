/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* File for "Terrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <math.h>

#include "vec3f.h"

using namespace std;

Vec3fl::Vec3fl() {
	
}

Vec3fl::Vec3fl(float x, float y, float z) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

float &Vec3fl::operator[](int index) {
	return v[index];
}

float Vec3fl::operator[](int index) const {
	return v[index];
}

Vec3fl Vec3fl::operator*(float scale) const {
	return Vec3fl(v[0] * scale, v[1] * scale, v[2] * scale);
}

Vec3fl Vec3fl::operator/(float scale) const {
	return Vec3fl(v[0] / scale, v[1] / scale, v[2] / scale);
}

Vec3fl Vec3fl::operator+(const Vec3fl &other) const {
	return Vec3fl(v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2]);
}

Vec3fl Vec3fl::operator-(const Vec3fl &other) const {
	return Vec3fl(v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2]);
}

Vec3fl Vec3fl::operator-() const {
	return Vec3fl(-v[0], -v[1], -v[2]);
}

const Vec3fl &Vec3fl::operator*=(float scale) {
	v[0] *= scale;
	v[1] *= scale;
	v[2] *= scale;
	return *this;
}

const Vec3fl &Vec3fl::operator/=(float scale) {
	v[0] /= scale;
	v[1] /= scale;
	v[2] /= scale;
	return *this;
}

const Vec3fl &Vec3fl::operator+=(const Vec3fl &other) {
	v[0] += other.v[0];
	v[1] += other.v[1];
	v[2] += other.v[2];
	return *this;
}

const Vec3fl &Vec3fl::operator-=(const Vec3fl &other) {
	v[0] -= other.v[0];
	v[1] -= other.v[1];
	v[2] -= other.v[2];
	return *this;
}

float Vec3fl::magnitude() const {
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float Vec3fl::magnitudeSquared() const {
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

Vec3fl Vec3fl::normalize() const {
	float m = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	return Vec3fl(v[0] / m, v[1] / m, v[2] / m);
}

float Vec3fl::dot(const Vec3fl &other) const {
	return v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2];
}

Vec3fl Vec3fl::cross(const Vec3fl &other) const {
	return Vec3fl(v[1] * other.v[2] - v[2] * other.v[1],
				 v[2] * other.v[0] - v[0] * other.v[2],
				 v[0] * other.v[1] - v[1] * other.v[0]);
}

Vec3fl operator*(float scale, const Vec3fl &v) {
	return v * scale;
}

ostream &operator<<(ostream &output, const Vec3fl &v) {
	cout << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
	return output;
}









