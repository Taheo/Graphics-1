#include "stdafx.h"


/*
(c) Janusz Ganczarski
http://www.januszg.hg.pl
JanuszG@enter.net.pl
*/

#include <GL/glut.h>
#include <stdlib.h>
#include "colors.h"


typedef GLfloat GLTVector2[2];
typedef GLfloat GLTVector3[3];
typedef GLfloat GLTVector4[4];
typedef GLfloat GLTMatrix[16];
// A column major 4x4 matrix of type GLfloat
#include <math.h>

// Adds two vectors together
void gltAddVectors(const GLTVector3 vFirst, const GLTVector3 vSecond, GLTVector3 vResult) {
	vResult[0] = vFirst[0] + vSecond[0];
	vResult[1] = vFirst[1] + vSecond[1];
	vResult[2] = vFirst[2] + vSecond[2];
}

// Subtract one vector from another
void gltSubtractVectors(const GLTVector3 vFirst, const GLTVector3 vSecond, GLTVector3 vResult)
{
	vResult[0] = vFirst[0] - vSecond[0];
	vResult[1] = vFirst[1] - vSecond[1];
	vResult[2] = vFirst[2] - vSecond[2];
}

// Scales a vector by a scalar
void gltScaleVector(GLTVector3 vVector, const GLfloat fScale)
{
	vVector[0] *= fScale; vVector[1] *= fScale; vVector[2] *= fScale;
}

// Gets the length of a vector squared
GLfloat gltGetVectorLengthSqrd(const GLTVector3 vVector)
{
	return (vVector[0] * vVector[0]) + (vVector[1] * vVector[1]) + (vVector[2] * vVector[2]);
}

// Gets the length of a vector
GLfloat gltGetVectorLength(const GLTVector3 vVector)
{
	return (GLfloat)sqrt(gltGetVectorLengthSqrd(vVector));
}

// Scales a vector by it's length - creates a unit vector
void gltNormalizeVector(GLTVector3 vNormal)
{
	GLfloat fLength = 1.0f / gltGetVectorLength(vNormal);
	gltScaleVector(vNormal, fLength);
}

// Copies a vector
void gltCopyVector(const GLTVector3 vSource, GLTVector3 vDest)
{
	memcpy(vDest, vSource, sizeof(GLTVector3));
}

// Get the dot product between two vectors
GLfloat gltVectorDotProduct(const GLTVector3 vU, const GLTVector3 vV)
{
	return vU[0] * vV[0] + vU[1] * vV[1] + vU[2] * vV[2];
}

// Calculate the cross product of two vectors
void gltVectorCrossProduct(const GLTVector3 vU, const GLTVector3 vV, GLTVector3 vResult)
{
	vResult[0] = vU[1] * vV[2] - vV[1] * vU[2];
	vResult[1] = -vU[0] * vV[2] + vV[0] * vU[2];
	vResult[2] = vU[0] * vV[1] - vV[0] * vU[1];
}



// Given three points on a plane in counter clockwise order, calculate the unit normal
void gltGetNormalVector(const GLTVector3 vP1, const GLTVector3 vP2, const GLTVector3 vP3, GLTVector3 vNormal)
{
	GLTVector3 vV1, vV2;

	gltSubtractVectors(vP2, vP1, vV1);
	gltSubtractVectors(vP3, vP1, vV2);

	gltVectorCrossProduct(vV1, vV2, vNormal);
	gltNormalizeVector(vNormal);
}



// Transform a point by a 4x4 matrix
void gltTransformPoint(const GLTVector3 vSrcVector, const GLTMatrix mMatrix, GLTVector3 vOut)
{
	vOut[0] = mMatrix[0] * vSrcVector[0] + mMatrix[4] * vSrcVector[1] + mMatrix[8] * vSrcVector[2] + mMatrix[12];
	vOut[1] = mMatrix[1] * vSrcVector[0] + mMatrix[5] * vSrcVector[1] + mMatrix[9] * vSrcVector[2] + mMatrix[13];
	vOut[2] = mMatrix[2] * vSrcVector[0] + mMatrix[6] * vSrcVector[1] + mMatrix[10] * vSrcVector[2] + mMatrix[14];
}

// Rotates a vector using a 4x4 matrix. Translation column is ignored
void gltRotateVector(const GLTVector3 vSrcVector, const GLTMatrix mMatrix, GLTVector3 vOut)
{
	vOut[0] = mMatrix[0] * vSrcVector[0] + mMatrix[4] * vSrcVector[1] + mMatrix[8] * vSrcVector[2];
	vOut[1] = mMatrix[1] * vSrcVector[0] + mMatrix[5] * vSrcVector[1] + mMatrix[9] * vSrcVector[2];
	vOut[2] = mMatrix[2] * vSrcVector[0] + mMatrix[6] * vSrcVector[1] + mMatrix[10] * vSrcVector[2];
}


// Gets the three coefficients of a plane equation given three points on the plane.
void gltGetPlaneEquation(GLTVector3 vPoint1, GLTVector3 vPoint2, GLTVector3 vPoint3, GLTVector3 vPlane)
{
	// Get normal vector from three points. The normal vector is the first three coefficients
	// to the plane equation...
	gltGetNormalVector(vPoint1, vPoint2, vPoint3, vPlane);

	// Final coefficient found by back substitution
	vPlane[3] = -(vPlane[0] * vPoint3[0] + vPlane[1] * vPoint3[1] + vPlane[2] * vPoint3[2]);
}

// Determine the distance of a point from a plane, given the point and the
// equation of the plane.
GLfloat gltDistanceToPlane(GLTVector3 vPoint, GLTVector4 vPlane)
{
	return vPoint[0] * vPlane[0] + vPoint[1] * vPlane[1] + vPoint[2] * vPlane[2] + vPlane[3];
}









// rozmiary bry³y obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near_ = 3.0;
const GLdouble far_ = 7.0;

// sta³e do obs³ugi menu podrêcznego

enum
{
	CUTTING_PLANE, // p³aszczyzna przekroju
	POLYGON_OFFSET, // przesuniêcie wartoœci g³êbi
	EXIT // wyjœcie
};

// k¹t obrotu kuli

GLfloat angle = 0.0;

// k¹ty obrotu szeœcianu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wskaŸnik rysowania p³aszczyzna przekroju

bool cutting_plane = true;

// wskaŸnik przesuniêcia wartoœci g³êbi

bool polygon_offset = true;

// wskaŸnik naciœniêcia lewego przycisku myszki

int button_state = GLUT_UP;

// po³o¿enie kursora myszki

int button_x, button_y;

// funkcja generuj¹ca scenê 3D

#define GL_PI 3.1415f

void Display()
{
	GLint n = 5;
	GLTVector3 vNormal;
	GLTVector3 vCorners[12] =
	{
		{ 0.0f, 1.0f, 0.0f }, // Góra 0
	};

	for (GLint i = 1; i <= n; i++)
	{

		vCorners[i][0] = vCorners[i - 1][0] + ((0.5*cos(i * 2 * GL_PI / n)) * 1.0f);
		vCorners[i][2] = vCorners[i - 1][2] + ((0.5*sin(i * 2 * GL_PI / n)) * 1.0f);

	}


	//wyznaczanie œrodka
	GLfloat sx = 0, sz = 0;

	for (GLint i = 1; i <= n; i++)
	{
		sx = sx + vCorners[i][0];
		sz = sz + vCorners[i][2];
	}

	vCorners[0][0] = sx / n;
	vCorners[0][1] = 1.f;
	vCorners[0][2] = sz / n;

	vCorners[11][0] = sx / n;
	vCorners[11][1] = 0.f;
	vCorners[11][2] = sz / n;

	//xRot = vCorners[11][0];

	// Czyszczenie okna aktualnym kolorem czyszcz¹cym
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Zapisanie stanu macierzy i wykonanie obrotów
	glPushMatrix();
	// Cofniêcie obiektów
	glTranslatef(0.0f, -0.25f, -4.0f);
	glRotatef(rotatex, 1.0f, 0.0f, 0.0f);
	glRotatef(rotatey, 0.0f, 1.0f, 0.0f);
	// Rysowanie piramidy
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, -1.0f, 0.0f);

	/*for (GLint i = 1; i < n; i++)
	{
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[i]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[i+1]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[11]);
	}
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[n]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[1]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[11]);
	*/

	for (GLint i = 1; i < n; i++)
	{
		gltGetNormalVector(vCorners[i], vCorners[i + 1], vCorners[0], vNormal);
		glNormal3fv(vNormal);
		//glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[0]);
		//glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[i + 1]);
		//glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[i]);
	}

	gltGetNormalVector(vCorners[n], vCorners[1], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	//glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	//glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[1]);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[n]);

	for (GLint i = 1; i < n; i++)
	{
		gltGetNormalVector(vCorners[i], vCorners[i + 1], vCorners[0], vNormal);
		glNormal3fv(vNormal);
		//glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[0]);
		//glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[i]);
		//glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[i + 1]);
	}

	gltGetNormalVector(vCorners[n], vCorners[1], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	//glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	//glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[n]);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[1]);


	glEnd();;
	// Odtworzenie stanu macierzy
	glPopMatrix();
	// Zamiana buforów
	//glutSwapBuffers();




	// kolor t³a - zawartoœæ bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora g³êbokoœci
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesuniêcie uk³adu wspó³rzêdnych szeœcianu do œrodka bry³y odcinania
	glTranslatef(0, 0, -(near_ + far_) / 2);

	// obroty szeœcianu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// niewielkie powiêkszenie szeœcianu
	glScalef(1.15, 1.15, 1.15);

	// w³¹czenie testu bufora g³êbokoœci
	glEnable(GL_DEPTH_TEST);

	// najpierw rysujemy kulê obracaj¹c¹ siê wewn¹trz szeœcianu;
	// z uwagi na celowy brak efektów oœwietlenia, obrót kuli
	// podkreœla druga kula w wersji "szkieletowej"
	glPushMatrix();
	angle += 0.2;
	glRotatef(angle, 1.0, 1.0, 0.0);
	glColor3fv(Yellow);
	if (polygon_offset)
		glEnable(GL_POLYGON_OFFSET_FILL);

	glPolygonOffset(1.0, 1.0);
	glutSolidSphere(0.5, 10, 10);
	glColor3fv(Black);
	glutWireSphere(0.5, 10, 10);
	if (polygon_offset)
		glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();

	glPushMatrix();
	// Cofniêcie obiektów
	glTranslatef(0.0f, -0.25f, -4.0f);
	glRotatef(rotatex, 1.0f, 0.0f, 0.0f);
	glRotatef(rotatey, 0.0f, 1.0f, 0.0f);
	// Rysowanie piramidy
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, -1.0f, 0.0f);

	/*for (GLint i = 1; i < n; i++)
	{
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[i]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[i+1]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[11]);
	}
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[n]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[1]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[11]);
	*/

	for (GLint i = 1; i < n; i++)
	{
		gltGetNormalVector(vCorners[i], vCorners[i + 1], vCorners[0], vNormal);
		glNormal3fv(vNormal);
		//glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[0]);
		//glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[i + 1]);
		//glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[i]);
	}

	gltGetNormalVector(vCorners[n], vCorners[1], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	//glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	//glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[1]);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[n]);

	for (GLint i = 1; i < n; i++)
	{
		gltGetNormalVector(vCorners[i], vCorners[i + 1], vCorners[0], vNormal);
		glNormal3fv(vNormal);
		//glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[0]);
		//glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[i]);
		//glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[i + 1]);
	}

	gltGetNormalVector(vCorners[n], vCorners[1], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	//glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	//glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[n]);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[1]);


	glEnd();;
	// Odtworzeni



	// w drugiej kolejnoœci rysujemy wnêtrze szeœcianu;
	// rysowane s¹ tylko przednie strony wewnêtrznych œcian
	// no i nie jest rysowana œciana przednia, w której bêdzie otwór
	glEnable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	glColor3fv(Blue);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(1.0, 1.0, -1.0);
	glVertex3f(-1.0, 1.0, -1.0);

	glColor3fv(Lime);
	glVertex3f(-1.0, 1.0, 1.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(-1.0, 1.0, -1.0);

	glColor3fv(Cyan);
	glVertex3f(1.0, 1.0, 1.0);
	glVertex3f(-1.0, 1.0, 1.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glVertex3f(1.0, 1.0, -1.0);

	glColor3fv(Green);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(1.0, 1.0, 1.0);
	glVertex3f(1.0, 1.0, -1.0);
	glVertex3f(1.0, -1.0, -1.0);

	glColor3fv(Cyan);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glEnd();
	glDisable(GL_CULL_FACE);

	// rysowanie p³aszczyzny otworu w szeœcianie
	if (cutting_plane)
	{
		// wy³¹czenie rysowania w buforze kolorów
		glDrawBuffer(GL_NONE);

		// rysowanie kwadratu czêœciowo ods³aniaj¹cego wnêtrze szeœcianu
		// (kwadrat jest po³o¿ony o 0,001 jednostki nad bokiem szeœcianu)
		glBegin(GL_QUADS);
		glVertex3f(-0.6, -0.6, 1.001);
		glVertex3f(0.6, -0.6, 1.001);
		glVertex3f(0.6, 0.6, 1.001);
		glVertex3f(-0.6, 0.6, 1.001);
		glEnd();

		// w³¹czenie rysowania w buforze kolorów
		glDrawBuffer(GL_BACK);
	}

	// w³aœciwy szeœcian z obramowaniem, którego rysowanie wymusza brak oœwietlenia
	glColor3fv(Red);
	if (polygon_offset)
		glEnable(GL_POLYGON_OFFSET_FILL);

	glPolygonOffset(1.0, 1.0);
	glutSolidCube(2.0);
	glColor3fv(Black);
	glutWireCube(2.0);
	if (polygon_offset)
		glDisable(GL_POLYGON_OFFSET_FILL);

	// skierowanie poleceñ do wykonania
	glFlush();

	// zamiana buforów koloru
	glutSwapBuffers();
}

// zmiana wielkoœci okna

void Reshape(int width, int height)
{
	// obszar renderingu - ca³e okno
	glViewport(0, 0, width, height);

	// wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bry³y obcinania
	glFrustum(left, right, bottom, top, near_, far_);

	// generowanie sceny 3D
	Display();
}

// obs³uga klawiszy funkcyjnych i klawiszy kursora

void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
		// kursor w lewo
	case GLUT_KEY_LEFT:
		rotatey -= 1;
		break;

		// kursor w górê
	case GLUT_KEY_UP:
		rotatex -= 1;
		break;

		// kursor w prawo
	case GLUT_KEY_RIGHT:
		rotatey += 1;
		break;

		// kursor w dó³
	case GLUT_KEY_DOWN:
		rotatex += 1;
		break;
	}

	// odrysowanie okna
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

// obs³uga przycisków myszki

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiêtanie stanu lewego przycisku myszki
		button_state = state;

		// zapamiêtanie po³o¿enia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

// obs³uga ruchu kursora myszki

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) *(x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) *(button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

// obs³uga menu podrêcznego

void Menu(int value)
{
	switch (value)
	{
		// p³aszczyzna przekroju
	case CUTTING_PLANE:
		cutting_plane = !cutting_plane;
		Display();
		break;

		// przesuniêcie wartoœci g³êbi
	case POLYGON_OFFSET:
		polygon_offset = !polygon_offset;
		Display();
		break;

		// wyjœcie
	case EXIT:
		exit(0);
	}
}

int main(int argc, char * argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// rozmiary g³ównego okna programu
	glutInitWindowSize(750, 750);

	// utworzenie g³ównego okna programu
	glutCreateWindow("Z-bufor");

	// do³¹czenie funkcji generuj¹cej scenê 3D
	glutDisplayFunc(Display);

	// do³¹czenie funkcji wywo³ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// do³¹czenie funkcji obs³ugi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);

	// obs³uga przycisków myszki
	glutMouseFunc(MouseButton);

	// obs³uga ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie menu podrêcznego
	glutCreateMenu(Menu);

	// menu g³ówne
	glutCreateMenu(Menu);
#ifdef WIN32

	glutAddMenuEntry("P³aszczyzna przekroju: rysowana/nierysowana", CUTTING_PLANE);
	glutAddMenuEntry("Przesuniêcie wartoœci g³êbi: w³¹cz/wy³¹cz", POLYGON_OFFSET);
	glutAddMenuEntry("Wyjœcie", EXIT);
#else

	glutAddMenuEntry("Plaszczyzna przekroju: rysowana/nierysowana", CUTTING_PLANE);
	glutAddMenuEntry("Przesuniecie wartosci glebi: wlacz/wylacz", POLYGON_OFFSET);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// okreœlenie przycisku myszki obs³uguj¹cej menu podrêczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// do³¹czenie funkcji wywo³ywanej podczas "bezczynnoœci" systemu
	glutIdleFunc(Display);

	// wprowadzenie programu do obs³ugi pêtli komunikatów
	glutMainLoop();
	return 0;
}