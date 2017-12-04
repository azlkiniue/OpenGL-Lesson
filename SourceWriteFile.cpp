#include <glut.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

float sudut = 0.0, sudut2 = 0.0;
const int MAXSIZE = 1500;

typedef struct { float x; float y; }Point2D_t;
typedef struct { int x; int y; }Point2D_i;
typedef struct { float v[4]; } Vector3D_t;
typedef struct { float m[3][3]; } matrix3D_t;
typedef struct { int m[3][3]; } matrix3D_i;
typedef struct { float x, y, z; }Point3D_t;
//typedef struct { float x, y, z, r, g, b; }Point3D_t;
typedef struct { float r; float g; float b; } color_t;
typedef struct {
	int numofVertices;
	long int pnt[32];
}face_t;
typedef struct {
	int numofVertices;
	Point3D_t pnt[600];
	int numofFaces;
	face_t fc[1200];
}object3D_t;
Point3D_t point[MAXSIZE]; face_t face[MAXSIZE]; color_t color[MAXSIZE];
Vector3D_t vec[MAXSIZE], vecbuff[MAXSIZE], NormalVector;
Point2D_t titik2D[MAXSIZE];

void setColor(color_t col) {
	float r = col.r / 255;
	float g = col.g / 255;
	float b = col.b / 255;
	glColor3f(r, g, b);
}

void initColor() {
	for (int i = 0; i < MAXSIZE; i++)
	{
		color[i].r = (float)(rand() % 255);
		color[i].g = (float)(rand() % 255);
		color[i].b = (float)(rand() % 255);
	}
}

matrix3D_t createIdentity() {
	matrix3D_t rotate;
	rotate.m[0][0] = 0.0;
	rotate.m[0][1] = 0.0;
	rotate.m[0][2] = 0.0;
	rotate.m[1][0] = 0.0;
	rotate.m[1][1] = 0.0;
	rotate.m[1][2] = 0.0;
	rotate.m[2][0] = 0.0;
	rotate.m[2][1] = 0.0;
	rotate.m[2][1] = 0.0;
	return rotate;
}
matrix3D_t rotationX(float teta) {
	matrix3D_t rotate = createIdentity();
	rotate.m[0][0] = 1.0;
	rotate.m[0][1] = 0.0;
	rotate.m[0][2] = 0.0;
	rotate.m[1][0] = 0.0;
	rotate.m[1][1] = cos(teta / 57.3);
	rotate.m[1][2] = -sin(teta / 57.3);
	rotate.m[2][0] = 0.0;
	rotate.m[2][1] = sin(teta / 57.3);
	rotate.m[2][2] = cos(teta / 57.3);
	return rotate;
}
matrix3D_t rotationY(float teta) {
	matrix3D_t rotate = createIdentity();
	rotate.m[0][0] = cos(teta / 57.3);
	rotate.m[0][1] = 0.0;
	rotate.m[0][2] = sin(teta / 57.3);
	rotate.m[1][0] = 0.0;
	rotate.m[1][1] = 1.0;
	rotate.m[1][2] = 0.0;
	rotate.m[2][0] = -sin(teta / 57.3);
	rotate.m[2][1] = 0.0;
	rotate.m[2][2] = cos(teta / 57.3);
	return rotate;
}
matrix3D_t rotationZ(float teta) {
	matrix3D_t rotate = createIdentity();
	rotate.m[0][0] = cos(teta / 57.3);
	rotate.m[0][1] = -sin(teta / 57.3);
	rotate.m[0][2] = 0.0;
	rotate.m[1][0] = sin(teta / 57.3);
	rotate.m[1][1] = cos(teta / 57.3);
	rotate.m[1][2] = 0.0;
	rotate.m[2][0] = 0.0;
	rotate.m[2][1] = 0.0;
	rotate.m[2][2] = 1.0;
	return rotate;
}
Vector3D_t operator +(Vector3D_t a, Vector3D_t b) {
	Vector3D_t c;
	for (int i = 0; i<3; i++) {
		c.v[i] = a.v[i] + b.v[i];
	}
	return c;
}
Vector3D_t operator -(Vector3D_t a, Vector3D_t b) {
	Vector3D_t c;
	for (int i = 0; i<3; i++) {
		c.v[i] = a.v[i] - b.v[i];
	}
	return c;
}
Vector3D_t operator *(matrix3D_t a, Vector3D_t b) {
	Vector3D_t c;
	for (int i = 0; i<3; i++) {
		c.v[i] = 0;
		for (int j = 0; j<3; j++) {
			c.v[i] += a.m[i][j] * b.v[j];
		}
	}
	return c;
}
Vector3D_t operator ^(Vector3D_t a, Vector3D_t b)
{
	Vector3D_t c;//c=a*b
	c.v[0] = a.v[1] * b.v[2] - a.v[2] * b.v[1];
	c.v[1] = a.v[2] * b.v[0] - a.v[0] * b.v[2];
	c.v[2] = a.v[0] * b.v[1] - a.v[1] * b.v[0];
	c.v[3] = 1.;
	return c;
}
matrix3D_t operator * (matrix3D_t a, matrix3D_t b)
{
	matrix3D_t c;//c=a*b
	int i, j, k;
	for (i = 0; i<3; i++) for (j = 0; j<3; j++) {
		c.m[i][j] = 0;
		for (k = 0; k<3; k++)
			c.m[i][j] += a.m[i][k] * b.m[k][j];
	}
	return c;
}
Point2D_t Vector2Point2D(Vector3D_t vec)
{
	Point2D_t pnt;
	pnt.x = vec.v[0];
	pnt.y = vec.v[1];
	return pnt;
}
Vector3D_t Point2Vector(Point3D_t pnt)
{
	Vector3D_t vec;
	vec.v[0] = pnt.x;
	vec.v[1] = pnt.y;
	vec.v[2] = pnt.z;
	vec.v[3] = 1.;
	return vec;
}
void drawPolygon(Point2D_t point[], int n) {
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(point[i].x, point[i].y);
	}
	glEnd();
	//glFlush();
}
void colorizePolygon(Point2D_t point[], int n, color_t colour[]) {
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		setColor(colour[i]);
		glVertex2f(point[i].x, point[i].y);
	}
	glEnd();
	//glFlush();
}

void timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

void create3DObject(object3D_t object, color_t col) {
	for (int i = 0; i<object.numofFaces; i++) {
		setColor(col);
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j<object.fc[i].numofVertices; j++) {
			int p = object.fc[i].pnt[j];
			float x = object.pnt[p].x;
			float y = object.pnt[p].y;
			glVertex3f(x, y, 0.0);
		}
		glEnd();
	}
}
void create3DObjectVisCol(object3D_t object) {
	matrix3D_t tilting = rotationX(sudut2) * rotationZ(sudut2);
	Vector3D_t vec[MAXSIZE], vecbuff[MAXSIZE], NormalVector;
	Point2D_t titik2D[MAXSIZE];
	color_t warna[MAXSIZE];
	float normalzi;
	for (int i = 0; i<object.numofVertices; i++)
	{
		vec[i] = Point2Vector(object.pnt[i]);
		vec[i] = tilting*vec[i];
	}
	for (int i = 0; i<object.numofFaces; i++)
	{
		for (int j = 0; j<object.fc[i].numofVertices; j++)
			vecbuff[j] = vec[object.fc[i].pnt[j]];
		NormalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = NormalVector.v[2];
		if (normalzi>0.)
		{
			//setColor(colFore);
			for (int j = 0; j < object.fc[i].numofVertices; j++) {
				titik2D[j] = Vector2Point2D(vec[object.fc[i].pnt[j]]);
				warna[j] = color[object.fc[i].pnt[j]];
			}
			colorizePolygon(titik2D, object.fc[i].numofVertices, warna);
		}
	}
}
void create3DObjectVis(object3D_t object, color_t colFore) {
	matrix3D_t tilting = rotationX(sudut2);
	Vector3D_t vec[MAXSIZE], vecbuff[MAXSIZE], NormalVector;
	Point2D_t titik2D[MAXSIZE];
	float normalzi;
	for (int i = 0; i<object.numofVertices; i++)
	{
		vec[i] = Point2Vector(object.pnt[i]);
		vec[i] = tilting*vec[i];
	}
	for (int i = 0; i<object.numofFaces; i++)
	{
		for (int j = 0; j<object.fc[i].numofVertices; j++)
			vecbuff[j] = vec[object.fc[i].pnt[j]];
		NormalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = NormalVector.v[2];
		if (normalzi>0.)
		{
			setColor(colFore);
			for (int j = 0; j<object.fc[i].numofVertices; j++)
				titik2D[j] = Vector2Point2D(vec[object.fc[i].pnt[j]]);
			drawPolygon(titik2D, object.fc[i].numofVertices);
		}
	}
}

object3D_t makeObjKerucut() {
	int n = 32; float r = 0.5, h = 1;
	int numofVertices = n + 2; //int numofFaces = numofVertices;
	//Point3D_t point[600];
	int numofFaces = n * 2;
	//face_t face[1200];
	object3D_t obj;
	
	//make points
	point[0] = { 0,0,h };
	for (int i = 0; i < numofVertices-1; i++)
	{
		float s = i * 2 * M_PI / n;
		point[i + 1].x = r * cos(s);
		point[i + 1].y = r * sin(s);
		point[i + 1].z = 0;
	}	point[numofVertices - 1] = { 0,0,0 };

	//make faces
	for (int i = 1; i < n; i++)
	{
		face[i - 1] = { 3, { 0,i + 1,i } };
	}	face[n - 1] = { 3, { 0,1,n } };
	for (int i = 1; i < n; i++)
	{
		face[i + n - 1] = { 3, { numofVertices - 1,i,i + 1 } };
	}	face[numofFaces - 1] = { 3, { numofVertices - 1,n,1 } };

	//make object
	obj.numofFaces = numofFaces;
	for (int i = 0; i < numofFaces; i++)
	{
		obj.fc[i] = face[i];
	}
	obj.numofVertices = numofVertices;
	for (int i = 0; i < numofVertices; i++)
	{
		obj.pnt[i] = point[i];
	}

	return obj;
}

void writeObjectToFile(string filePath, object3D_t object) {
	ofstream myfile(filePath);
	float roundDigit = 1000000;
	if (myfile.is_open())
	{
		myfile << "COFF\n";
		myfile << object.numofVertices << " "
			<< object.numofFaces << " "
			<< 0 << endl;
		for (int i = 0; i < object.numofVertices; i++)
		{
			myfile << roundf(object.pnt[i].x * roundDigit)/roundDigit << " "
				<< roundf(object.pnt[i].y * roundDigit)/roundDigit << " "
				<< roundf(object.pnt[i].z * roundDigit)/roundDigit << " ";
			myfile << color[i].r << " "
				<< color[i].g << " "
				<< color[i].b << " ";
			myfile << 255 << endl;
		}
		for (int i = 0; i < object.numofFaces; i++)
		{
			myfile << object.fc[i].numofVertices;
			for (int j = 0; j < object.fc[i].numofVertices; j++)
			{
				myfile << " " << object.fc[i].pnt[j];
			}
			myfile << endl;
		}
		myfile.close();
	}
	else cout << "Unable to open file";
}

void DrawObject(string path) {
	object3D_t obj = makeObjKerucut();
	matrix3D_t matrix_X = rotationX(sudut);
	matrix3D_t matrix_Y = rotationY(sudut);
	matrix3D_t matrix_Z = rotationZ(sudut);
	for (int i = 0; i<obj.numofVertices; i++) {
		Vector3D_t p;
		p.v[0] = obj.pnt[i].x;
		p.v[1] = obj.pnt[i].y;
		p.v[2] = obj.pnt[i].z;
		 p = operator *(matrix_Y, p);
		 p = operator *(matrix_X, p);
		 p = operator *(matrix_Z, p);
		obj.pnt[i].x = p.v[0];
		obj.pnt[i].y = p.v[1];
		obj.pnt[i].z = p.v[2];
	}
	//createobj(obj, 8);
	color_t color1;
	color1.r = 255;
	color1.g = 255;
	color1.b = 255;
	//create3DObjectVis(obj, color1);
	create3DObjectVisCol(obj);
	sudut++; if (sudut >= 360.0) sudut = 0.0;
	glFlush();
	writeObjectToFile(path, obj);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	DrawObject("D:\\Nama\\Foldernya\\Diganti\\Ya\\Gan\\kerucut.off");
	glutSwapBuffers();
}

int main(int iArgc, char** cppArgv) {
	initColor();
	glutInit(&iArgc, cppArgv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("File Write");
	glClearColor(1.0, 1.0, 1.0, 0.0);
	gluOrtho2D(-1, 1, -1, 1);
	glutDisplayFunc(display);
	glutTimerFunc(1, timer, 0);
	glutMainLoop();

	return 0;
}
