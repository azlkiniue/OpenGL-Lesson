#include <glut.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

float sudut = 0.0, sudut2 = 0.0;
const int MAXSIZE = 1500;

typedef struct { float x; float y; }Point2D_t;
typedef struct { int x; int y; }Point2D_i;
typedef struct { float v[4]; } Vector3D_t;
typedef struct { float m[3][3]; } matrix3D_t;
typedef struct { int m[3][3]; } matrix3D_i;
//typedef struct { float x, y, z; }Point3D_t;
typedef struct { float x, y, z; }Point3D_t;
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
void create3DObjectVis(object3D_t Theobject, color_t colFore) {
	object3D_t object = Theobject;
	matrix3D_t tilting = rotationX(sudut2) * rotationY(-sudut2);
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
void create3DObjectInvis(object3D_t object, color_t colBack) {
	matrix3D_t tilting = rotationX(sudut2) * rotationZ(sudut2);
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
		if (normalzi < 0.)
		{
			setColor(colBack);
			for (int j = 0; j<object.fc[i].numofVertices; j++)
				titik2D[j] = Vector2Point2D(vec[object.fc[i].pnt[j]]);
			drawPolygon(titik2D, object.fc[i].numofVertices);
		}
	}
}

object3D_t readFileToObject(string filePath) {
	ifstream file(filePath);
	if (!file)
	{
		cout << "Cannot open file";
		system("pause");
		exit(6);
	}

	char s[255]; string delimiter = " ", token;
	int numofVertices, numofFaces;
	float temp[10];

	file.getline(s, 255);

	file.getline(s, 255);
	stringstream stream(s);
	for (int i = 0; stream; i++) {
		int n;
		stream >> n;
		temp[i] = n;
	}
	numofVertices = temp[0];
	numofFaces = temp[1];

	while (!file.eof())
	{
		for (int i = 0; i < numofVertices; i++)
		{
			file.getline(s, 255);
			istringstream stream(s);
			for (int j = 0; stream; j++) {
				float n;
				stream >> n;
				temp[j] = n;
			}

			point[i] = { temp[0], temp[1], temp[2] };
			color[i] = { temp[3], temp[4], temp[5] };

			//cout << s << " \n";
			//cout << endl;
		}
		for (int i = 0; i < numofFaces; i++)
		{
			file.getline(s, 255);
			istringstream stream(s);
			for (int j = 0; stream; j++) {
				float n;
				stream >> n;
				temp[j] = n;
			}

			face_t fctemp;
			fctemp.numofVertices = temp[0];
			for (int j = 0; j < fctemp.numofVertices; j++)
			{
				fctemp.pnt[j] = temp[j + 1];
			}
			face[i] = fctemp;
		}

	}
	//cout << numofFaces << numofVertices << endl;
	object3D_t obj;
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
	//delete[] face; delete[] point; delete[] color;
	return obj;
}

void DrawObject(string path) {
	object3D_t obj = readFileToObject(path);
	matrix3D_t matrix_X = rotationX(sudut);
	matrix3D_t matrix_Y = rotationY(sudut);
	matrix3D_t matrix_Z = rotationZ(sudut);
	for (int i = 0; i<obj.numofVertices; i++) {
		/*obj[i] = operator *(matrix_X, obj[i]);
		obj[i] = operator *(matrix_Y, obj[i]);
		obj[i] = operator *(matrix_Z, obj[i]);*/
		Vector3D_t p;
		p.v[0] = obj.pnt[i].x;
		p.v[1] = obj.pnt[i].y;
		p.v[2] = obj.pnt[i].z;
		/*p = (matrix_Y)*(p);*/ p = operator *(matrix_Y, p);
		/*p = (matrix_X)*(p);*/ p = operator *(matrix_X, p);
		/*p = (matrix_Z)*(p);*/ p = operator *(matrix_Z, p);
		obj.pnt[i].x = p.v[0];
		obj.pnt[i].y = p.v[1];
		obj.pnt[i].z = p.v[2];
	}
	//createobj(obj, 8);
	color_t color1;
	color1.r = 1.0;
	color1.g = 1.0;
	color1.b = 1.0;
	//create3DObjectVis(obj, color1);
	create3DObjectVisCol(obj);
	sudut++; if (sudut >= 360.0) sudut = 0.0;
	glFlush();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	DrawObject("D:\\Nama\\Folder\\Diganti\\Ya\\Gan\\blablabla.off");
	glutSwapBuffers();
}

int main(int iArgc, char** cppArgv) {
	glutInit(&iArgc, cppArgv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("File Read");
	glClearColor(0.0, 0.0, 0.0, 0.0);
	gluOrtho2D(-1, 1, -1, 1);
	glutDisplayFunc(display);
	glutTimerFunc(1, timer, 0);
	glutMainLoop();
	return 0;
	//system("pause");
}
