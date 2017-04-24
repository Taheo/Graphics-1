// opengl_wektory_normalne.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*
(c) Janusz Ganczarski
http://www.januszg.hg.pl
JanuszG@enter.net.pl
*/

#include <GL/glut.h>
#include "glext.h"
#ifndef WIN32
#define GLX_GLXEXT_LEGACY
#include <GL/glx.h>
#define wglGetProcAddress glXGetProcAddressARB
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "materials.h"
#include "colors.h"



//#include "VectorMath.cpp"

// Some data types
typedef GLfloat GLTVector2[2];      // Two component floating point vector
typedef GLfloat GLTVector3[3];      // Three component floating point vector
typedef GLfloat GLTVector4[4];      // Four component floating point vector
typedef GLfloat GLTMatrix[16];      // A column major 4x4 matrix of type GLfloat

#pragma region MyRegion
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
#pragma endregion





PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;

// wska�nik dost�pno�ci rozszerzenia EXT_rescale_normal

bool rescale_normal = false;

// sta�e do obs�ugi menu podr�cznego

enum
{
	BRASS,                // mosi�dz
	BRONZE,               // br�z
	POLISHED_BRONZE,      // polerowany br�z
	CHROME,               // chrom
	COPPER,               // mied�
	POLISHED_COPPER,      // polerowana mied�
	GOLD,                 // z�oto
	POLISHED_GOLD,        // polerowane z�oto
	PEWTER,               // grafit (cyna z o�owiem)
	SILVER,               // srebro
	POLISHED_SILVER,      // polerowane srebro
	EMERALD,              // szmaragd
	JADE,                 // jadeit
	OBSIDIAN,             // obsydian
	PEARL,                // per�a
	RUBY,                 // rubin
	TURQUOISE,            // turkus
	BLACK_PLASTIC,        // czarny plastik
	BLACK_RUBBER,         // czarna guma

	KIERUNKOWE,
	OTACZAJ�CE,
	REFLEKTOR,




	NORMALS_SMOOTH,       // jeden wektor normalny na wierzcho�ek
	NORMALS_FLAT,         // jeden wektor normalny na �cian�
	FULL_WINDOW,          // aspekt obrazu - ca�e okno
	ASPECT_1_1,           // aspekt obrazu 1:1
	EXIT                  // wyj�cie
};

// aspekt obrazu

int aspect = FULL_WINDOW;

// usuni�cie definicji makr near i far

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

// rozmiary bry�y obcinania

const GLdouble left = -1.0;
const GLdouble right = 1.0;
const GLdouble bottom = -1.0;
const GLdouble top = 1.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;

// k�ty obrotu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wska�nik naci�ni�cia lewego przycisku myszki

int button_state = GLUT_UP;

// po�o?enie kursora myszki

int button_x, button_y;

// wsp�czynnik skalowania

GLfloat scale = 1.0;

// w�a�ciwo�ci materia�u - domy�lnie mosi�dz
//////////////////////////////////////////////////////////////////////////////////////////////
//Tu ustala si� kolor
const GLfloat *ambient = GoldAmbient;
const GLfloat *diffuse = GoldDiffuse;
const GLfloat *specular = GoldSpecular;
GLfloat shininess = GoldShininess;


///////////////////////////////////////////////////////////////////////////////////////////////


GLfloat light_position[4] =
{
	0.0,0.0,2.0,0.0
};

GLfloat ambient_light[4] =
{
	0.2,0.2,0.2,1.0
};

// k�ty obrotu kierunku �r�d�a �wiat�a

GLfloat light_rotatex = 0.0;
GLfloat light_rotatey = 0.0;

// k�ty obrotu po�o�enia �r�d�a �wiat�a

// kierunek reflektora

GLfloat spot_direction[3] =
{
	0.0,0.0,-1.0
};

// k�t odciecia reflektora

GLfloat spot_cutoff = 180.0;

// wyk�adnik t�umienia k�towego reflektora

GLfloat spot_exponent = 128.0;

// sta�y wsp�czynnik t�umienia �wiat�a

GLfloat constant_attenuation = 1.0;

// liniowy wsp�czynnik t�umienia �wiat�a

GLfloat linear_attenuation = 0.0;

// kwadratowy wsp�czynnik t�umienia �wiat�a

GLfloat quadratic_attenuation = 0.0;

#define GL_PI 3.1415f
// Wielkoci obrot�w
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

// wektory normalne



int normals = NORMALS_FLAT;

// wsp�rz�dne wierzcho�k�w dwudziesto�cianu

GLfloat vertex[12 * 3] =
{
	0.000,  0.667,  0.500,   // v0
	0.000,  0.667, -0.500,   // v1
	0.000, -0.667, -0.500,   // v2
	0.000, -0.667,  0.500,   // v3
	0.667,  0.500,  0.000,   // v4
	0.667, -0.500,  0.000,   // v5
	-0.667, -0.500,  0.000,  // v6
	-0.667,  0.500,  0.000,  // v7
	0.500,  0.000,  0.667,   // v8
	-0.500,  0.000,  0.667,  // v9
	-0.500,  0.000, -0.667,  // v10
	0.500,  0.000, -0.667    // v11
};

// opis �cian dwudziesto�cianu

int triangles[20 * 3] =
{
	2, 10, 11,
	1, 11, 10,
	1, 10,  7,
	1,  4, 11,
	0,  1,  7,
	0,  4,  1,
	0,  9,  8,
	3,  8,  9,
	0,  7,  9,
	0,  8,  4,
	3,  9,  6,
	3,  5,  8,
	2,  3,  6,
	2,  5,  3,
	2,  6, 10,
	2, 11,  5,
	6,  7, 10,
	6,  9,  7,
	4,  5, 11,
	4,  8,  5
};

// obliczanie wektora normalnego dla wybranej �ciany

void Normal(GLfloat *n, int i)
{
	GLfloat v1[3], v2[3];

	// obliczenie wektor�w na podstawie wsp�rz�dnych wierzcho�k�w tr�jk�t�w
	v1[0] = vertex[3 * triangles[3 * i + 1] + 0] - vertex[3 * triangles[3 * i + 0] + 0];
	v1[1] = vertex[3 * triangles[3 * i + 1] + 1] - vertex[3 * triangles[3 * i + 0] + 1];
	v1[2] = vertex[3 * triangles[3 * i + 1] + 2] - vertex[3 * triangles[3 * i + 0] + 2];
	v2[0] = vertex[3 * triangles[3 * i + 2] + 0] - vertex[3 * triangles[3 * i + 1] + 0];
	v2[1] = vertex[3 * triangles[3 * i + 2] + 1] - vertex[3 * triangles[3 * i + 1] + 1];
	v2[2] = vertex[3 * triangles[3 * i + 2] + 2] - vertex[3 * triangles[3 * i + 1] + 2];

	// obliczenie waktora normalnego przy pomocy iloczynu wektorowego
	n[0] = v1[1] * v2[2] - v1[2] * v2[1];
	n[1] = v1[2] * v2[0] - v1[0] * v2[2];
	n[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

// normalizacja wektora

void Normalize(GLfloat *v)
{
	// obliczenie d�ugo�ci wektora
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	// normalizacja wektora
	if (d)
	{
		v[0] /= d;
		v[1] /= d;
		v[2] /= d;
	}
}

int light = KIERUNKOWE;
// funkcja generuj�ca scen� 3D

void Display()
{
	// kolor t�a - zawarto�� bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora g��boko�ci
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wyb�r macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesuni�cie uk�adu wsp�rz�dnych obiektu do �rodka bry�y odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	// w��czenie testu bufora g��boko�ci
	glEnable(GL_DEPTH_TEST);



	// w�a�ciwo�ci materia�u
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);


	switch (light)
	{
	case KIERUNKOWE:
		// w��czenie o�wietlenia
		glEnable(GL_LIGHTING);

		// w��czenie �wiat�a GL_LIGHT0 z parametrami domy�lnymi
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);


		// ustalenie kierunku �r�d�a �wiat�a
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		break;
	case OTACZAJ�CE:
		// w��czenie o�wietlenia
		glEnable(GL_LIGHTING);

		// w��czenie �wiat�a GL_LIGHT0 z parametrami domy�lnymi
		glEnable(GL_LIGHT0);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
		glDisable(GL_LIGHTING);
		GLfloat rgba[4];
		glColor3fv(Black);
		glGetFloatv(GL_LIGHT_MODEL_AMBIENT, rgba);
		break;
	case REFLEKTOR:
		// w��czenie o�wietlenia
		glEnable(GL_LIGHTING);

		// w��czenie �wiat�a GL_LIGHT0
		glEnable(GL_LIGHT0);
		// k�t odciecia reflektora
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);

		// wyk�adnik t�umienia k�towego reflektora
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);

		// sta�y wsp�czynnik t�umienia �wiat�a,
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, constant_attenuation);

		// liniowy wsp�czynnik t�umienia �wiat�a
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, linear_attenuation);

		// kwadratowy wsp�czynnik t�umienia �wiat�a
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadratic_attenuation);

		// zmiana po�o�enia �r�d�a �wiat�a jest wykonywana niezale�nie
		// od obrot�w obiektu, st�d od�o�enie na stos macierzy modelowania
		glPushMatrix();


		// macierz modelowania = macierz jednostkowa
		glLoadIdentity();

		// przesuniecie uk�adu wsp�rzednych �r�d�a �wiat�a do �rodka bry�y odcinania
		glTranslatef(0, 0, -(near + far) / 2);

		// obroty po�o�enia �r�d�a �wiat�a - klawisze kursora
		glRotatef(light_rotatex, 1.0, 0, 0);
		glRotatef(light_rotatey, 0, 1.0, 0);

		// przesuniecie �r�d�a �wiat�a
		glTranslatef(light_position[0], light_position[1], light_position[2]);

		// ustalenie pozycji �r�d�a �wiat�a
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		// ustalenie kierunku reflektora
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);

		// od�o�enie na stos zmiennych stanu zwi�zanych z o�wietleniem sceny
		glPushAttrib(GL_LIGHTING_BIT);
		break;


	default:
		break;
	}







	// w��czenie automatycznej normalizacji wektor�w normalnych
	// lub automatycznego skalowania jednostkowych wektor�w normalnych
	if (rescale_normal == true)
		glEnable(GL_RESCALE_NORMAL);
	else
		glEnable(GL_NORMALIZE);

	// pocz�tek definicji obiektu
	glBegin(GL_TRIANGLES);

	// generowanie obiektu g�adkiego - jeden u�redniony
	// wektor normalny na wierzcho�ek
	if (normals == NORMALS_SMOOTH)
		//for (int i = 0; i < 20; i++)
		//{
		//	// obliczanie wektora normalnego dla pierwszego wierzcho�ka
		//	GLfloat n[3];
		//	n[0] = n[1] = n[2] = 0.0;
		//	// wyszukanie wszystkich �cian posiadaj�cych bie?�cy wierzcho�ek
		//	for (int j = 0; j < 20; j++)
		//		if (3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 0] ||
		//			3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 1] ||
		//			3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 2])
		//		{
		//			// dodawanie wektor�w normalnych poszczeg�lnych �cian
		//			GLfloat nv[3];
		//			Normal(nv, j);
		//			n[0] += nv[0];
		//			n[1] += nv[1];
		//			n[2] += nv[2];
		//		}
		//	// u�redniony wektor normalny jest normalizowany tylko, gdy biblioteka
		//	// obs�uguje automatyczne skalowania jednostkowych wektor�w normalnych
		//	if (rescale_normal == true)
		//		Normalize(n);
		//	glNormal3fv(n);
		//	glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);
		//	// obliczanie wektora normalnego dla drugiego wierzcho�ka
		//	n[0] = n[1] = n[2] = 0.0;
		//	// wyszukanie wszystkich �cian posiadaj�cych bie?�cy wierzcho�ek
		//	for (int j = 0; j < 20; j++)
		//		if (3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 0] ||
		//			3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 1] ||
		//			3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 2])
		//		{
		//			// dodawanie wektor�w normalnych poszczeg�lnych �cian
		//			GLfloat nv[3];
		//			Normal(nv, j);
		//			n[0] += nv[0];
		//			n[1] += nv[1];
		//			n[2] += nv[2];
		//		}
		//	// u�redniony wektor normalny jest normalizowany tylko, gdy biblioteka
		//	// obs�uguje automatyczne skalowania jednostkowych wektor�w normalnych
		//	if (rescale_normal == true)
		//		Normalize(n);
		//	glNormal3fv(n);
		//	glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);
		//	// obliczanie wektora normalnego dla trzeciego wierzcho�ka
		//	n[0] = n[1] = n[2] = 0.0;
		//	// wyszukanie wszystkich �cian posiadaj�cych bie?�cy wierzcho�ek
		//	for (int j = 0; j < 20; j++)
		//		if (3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 0] ||
		//			3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 1] ||
		//			3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 2])
		//		{
		//			// dodawanie wektor�w normalnych poszczeg�lnych �cian
		//			GLfloat nv[3];
		//			Normal(nv, j);
		//			n[0] += nv[0];
		//			n[1] += nv[1];
		//			n[2] += nv[2];
		//		}
		//	// u�redniony wektor normalny jest normalizowany tylko, gdy biblioteka
		//	// obs�uguje automatyczne skalowania jednostkowych wektor�w normalnych
		//	if (rescale_normal == true)
		//		Normalize(n);
		//	glNormal3fv(n);
		//	glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
		//}

	{
		///////////////////////////////////////////////////////////////////////////////////////////////
		//tu si� ustala podstaw� ostros�upa
		GLint n = 5;
		GLTVector3 vNormal;
		GLTVector3 vCorners[12] =
		{
			{ 0.0f, 1.0f, 0.0f }, // G�ra 0
		};

		for (GLint i = 1; i <= n; i++)
		{

			vCorners[i][0] = vCorners[i - 1][0] + ((0.5*cos(i * 2 * GL_PI / n)) * 1.0f);
			vCorners[i][2] = vCorners[i - 1][2] + ((0.5*sin(i * 2 * GL_PI / n)) * 1.0f);

		}


		//wyznaczanie �rodka
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

		// Czyszczenie okna aktualnym kolorem czyszcz�cym
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Zapisanie stanu macierzy i wykonanie obrot�w
		glPushMatrix();
		// Cofni�cie obiekt�w
		glTranslatef(0.0f, -0.25f, -4.0f);
		glRotatef(xRot, 1.0f, 0.0f, 0.0f);
		glRotatef(yRot, 0.0f, 1.0f, 0.0f);
		// Rysowanie piramidy
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);

		glNormal3f(0.0f, -1.0f, 0.0f);

		for (GLint i = 1; i < n; i++)
		{
			glTexCoord2f(1.0f, 1.0f);
			glVertex3fv(vCorners[i]);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(vCorners[i + 1]);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3fv(vCorners[11]);
		}

		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv(vCorners[n]);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(vCorners[1]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[11]);


		for (GLint i = 1; i < n; i++)
		{
			gltGetNormalVector(vCorners[i], vCorners[i + 1], vCorners[0], vNormal);
			glNormal3fv(vNormal);
			glTexCoord2f(0.5f, 1.0f);
			glVertex3fv(vCorners[0]);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(vCorners[i + 1]);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3fv(vCorners[i]);
		}

		gltGetNormalVector(vCorners[n], vCorners[1], vCorners[0], vNormal);
		glNormal3fv(vNormal);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[0]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[1]);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[n]);
	}
	else
	{
		//// generowanie obiektu "p�askiego" - jeden wektor normalny na �cian�
		//for (int i = 0; i < 20; i++)
		//{
		//	GLfloat n[3];
		//	Normal(n, i);

		//	// u�redniony wektor normalny jest normalizowany tylko, gdy biblioteka
		//	// obs�uguje automatyczne skalowania jednostkowych wektor�w normalnych
		//	if (rescale_normal == true)
		//		Normalize(n);
		//	glNormal3fv(n);
		//	glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);
		//	glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);
		//	glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
		//}
		GLint n = 5;
		GLTVector3 vNormal;
		GLTVector3 vCorners[12] =
		{
			{ 0.0f, 1.0f, 0.0f }, // G�ra 0
		};

		for (GLint i = 1; i <= n; i++)
		{

			vCorners[i][0] = vCorners[i - 1][0] + ((0.5*cos(i * 2 * GL_PI / n)) * 1.0f);
			vCorners[i][2] = vCorners[i - 1][2] + ((0.5*sin(i * 2 * GL_PI / n)) * 1.0f);

		}


		//wyznaczanie �rodka
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

		// Czyszczenie okna aktualnym kolorem czyszcz�cym
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Zapisanie stanu macierzy i wykonanie obrot�w
		glPushMatrix();
		// Cofni�cie obiekt�w
		glTranslatef(0.0f, -0.25f, -4.0f);
		glRotatef(xRot, 1.0f, 0.0f, 0.0f);
		glRotatef(yRot, 0.0f, 1.0f, 0.0f);
		// Rysowanie piramidy
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);

		glNormal3f(0.0f, -1.0f, 0.0f);

		for (GLint i = 1; i < n; i++)
		{
			glTexCoord2f(1.0f, 1.0f);
			glVertex3fv(vCorners[i]);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(vCorners[i + 1]);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3fv(vCorners[11]);
		}

		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv(vCorners[n]);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(vCorners[1]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[11]);


		for (GLint i = 1; i < n; i++)
		{
			gltGetNormalVector(vCorners[i], vCorners[i + 1], vCorners[0], vNormal);
			glNormal3fv(vNormal);
			glTexCoord2f(0.5f, 1.0f);
			glVertex3fv(vCorners[0]);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(vCorners[i + 1]);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3fv(vCorners[i]);
		}

		gltGetNormalVector(vCorners[n], vCorners[1], vCorners[0], vNormal);
		glNormal3fv(vNormal);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[0]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[1]);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[n]);
	}


	// koniec definicji obiektu
	glEnd();

	// informacje o modyfikowanych warto�ciach
	// parametr�w �r�d�a �wiata�a GL_LIGHT0
	char string[200];
	GLfloat vec[4];
	glColor3fv(Black);

	// kierunek �r�d�a �wiat�a
	glGetLightfv(GL_LIGHT0, GL_POSITION, vec);
	sprintf(string, "GL_POSITION = (%f,%f,%f,%f)", vec[0], vec[1], vec[2], vec[3]);


	// k�ty obrotu kierunku �r�d�a �wiat�a
	sprintf(string, "light_rotatex = %f", light_rotatex);

	sprintf(string, "light_rotatey = %f", light_rotatey);
	;
	// skierowanie polece? do wykonania
	glFlush();

	// zamiana bufor�w koloru
	glutSwapBuffers();
}

// zmiana wielko�ci okna

void Reshape(int width, int height)
{
	// obszar renderingu - ca�e okno
	glViewport(0, 0, width, height);

	// wyb�r macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bry�y obcinania
	if (aspect == ASPECT_1_1)
	{
		// wysoko�� okna wi�ksza od wysoko�ci okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else

			// szeroko�� okna wi�ksza lub r�wna wysoko�ci okna
			if (width >= height && height > 0)
				glFrustum(left*width / height, right*width / height, bottom, top, near, far);
	}
	else
		glFrustum(left, right, bottom, top, near, far);

	// generowanie sceny 3D
	Display();
}

// obs�uga klawiatury

void Keyboard(unsigned char key, int x, int y)
{
	// klawisz +
	if (key == '+')
		scale += 0.05;
	else

		// klawisz -
		if (key == '-' && scale > 0.05)
			scale -= 0.05;

	// narysowanie sceny
	Display();
}

// obs�uga klawiszy funkcyjnych i klawiszy kursora

void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
		// kursor w lewo
	case GLUT_KEY_LEFT:
		rotatey -= 1;
		break;

		// kursor w g�r�
	case GLUT_KEY_UP:
		rotatex -= 1;
		break;

		// kursor w prawo
	case GLUT_KEY_RIGHT:
		rotatey += 1;
		break;

		// kursor w d�
	case GLUT_KEY_DOWN:
		rotatex += 1;
		break;
	}

	// odrysowanie okna
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

// obs�uga przycisk�w myszki

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapami�tanie stanu lewego przycisku myszki
		button_state = state;

		// zapami�tanie po�o?enia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

// obs�uga ruchu kursora myszki

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

// obs�uga menu podr�cznego

void Menu(int value)
{
	switch (value)
	{
		// materia� - mosi�dz
	case BRASS:
		ambient = BrassAmbient;
		diffuse = BrassDiffuse;
		specular = BrassSpecular;
		shininess = BrassShininess;
		Display();
		break;

		// materia� - br�z
	case BRONZE:
		ambient = BronzeAmbient;
		diffuse = BronzeDiffuse;
		specular = BronzeSpecular;
		shininess = BronzeShininess;
		Display();
		break;

		// materia� - polerowany br�z
	case POLISHED_BRONZE:
		ambient = PolishedBronzeAmbient;
		diffuse = PolishedBronzeDiffuse;
		specular = PolishedBronzeSpecular;
		shininess = PolishedBronzeShininess;
		Display();
		break;

		// materia� - chrom
	case CHROME:
		ambient = ChromeAmbient;
		diffuse = ChromeDiffuse;
		specular = ChromeSpecular;
		shininess = ChromeShininess;
		Display();
		break;

		// materia� - mied�
	case COPPER:
		ambient = CopperAmbient;
		diffuse = CopperDiffuse;
		specular = CopperSpecular;
		shininess = CopperShininess;
		Display();
		break;

		// materia� - polerowana mied�
	case POLISHED_COPPER:
		ambient = PolishedCopperAmbient;
		diffuse = PolishedCopperDiffuse;
		specular = PolishedCopperSpecular;
		shininess = PolishedCopperShininess;
		Display();
		break;

		// materia� - z�oto
	case GOLD:
		ambient = GoldAmbient;
		diffuse = GoldDiffuse;
		specular = GoldSpecular;
		shininess = GoldShininess;
		Display();
		break;

		// materia� - polerowane z�oto
	case POLISHED_GOLD:
		ambient = PolishedGoldAmbient;
		diffuse = PolishedGoldDiffuse;
		specular = PolishedGoldSpecular;
		shininess = PolishedGoldShininess;
		Display();
		break;

		// materia� - grafit (cyna z o�owiem)
	case PEWTER:
		ambient = PewterAmbient;
		diffuse = PewterDiffuse;
		specular = PewterSpecular;
		shininess = PewterShininess;
		Display();
		break;

		// materia� - srebro
	case SILVER:
		ambient = SilverAmbient;
		diffuse = SilverDiffuse;
		specular = SilverSpecular;
		shininess = SilverShininess;
		Display();
		break;

		// materia� - polerowane srebro
	case POLISHED_SILVER:
		ambient = PolishedSilverAmbient;
		diffuse = PolishedSilverDiffuse;
		specular = PolishedSilverSpecular;
		shininess = PolishedSilverShininess;
		Display();
		break;

		// materia� - szmaragd
	case EMERALD:
		ambient = EmeraldAmbient;
		diffuse = EmeraldDiffuse;
		specular = EmeraldSpecular;
		shininess = EmeraldShininess;
		Display();
		break;

		// materia� - jadeit
	case JADE:
		ambient = JadeAmbient;
		diffuse = JadeDiffuse;
		specular = JadeSpecular;
		shininess = JadeShininess;
		Display();
		break;

		// materia� - obsydian
	case OBSIDIAN:
		ambient = ObsidianAmbient;
		diffuse = ObsidianDiffuse;
		specular = ObsidianSpecular;
		shininess = ObsidianShininess;
		Display();
		break;

		// materia� - per�a
	case PEARL:
		ambient = PearlAmbient;
		diffuse = PearlDiffuse;
		specular = PearlSpecular;
		shininess = PearlShininess;
		Display();
		break;

		// metaria� - rubin
	case RUBY:
		ambient = RubyAmbient;
		diffuse = RubyDiffuse;
		specular = RubySpecular;
		shininess = RubyShininess;
		Display();
		break;

		// materia� - turkus
	case TURQUOISE:
		ambient = TurquoiseAmbient;
		diffuse = TurquoiseDiffuse;
		specular = TurquoiseSpecular;
		shininess = TurquoiseShininess;
		Display();
		break;

		// materia� - czarny plastik
	case BLACK_PLASTIC:
		ambient = BlackPlasticAmbient;
		diffuse = BlackPlasticDiffuse;
		specular = BlackPlasticSpecular;
		shininess = BlackPlasticShininess;
		Display();
		break;

		// materia� - czarna guma
	case BLACK_RUBBER:
		ambient = BlackRubberAmbient;
		diffuse = BlackRubberDiffuse;
		specular = BlackRubberSpecular;
		shininess = BlackRubberShininess;
		Display();
		break;

	case REFLEKTOR:
		light = REFLEKTOR;
		Display();
		break;
	case KIERUNKOWE:
		light = KIERUNKOWE;
		Display();
		break;
	case OTACZAJ�CE:
		light = OTACZAJ�CE;
		Display();
		break;


		// wektory normalne - GLU_SMOOTH
	case NORMALS_SMOOTH:
		normals = NORMALS_SMOOTH;
		Display();
		break;

		// wektory normalne - GLU_FLAT
	case NORMALS_FLAT:
		normals = NORMALS_FLAT;
		Display();
		break;

		// obszar renderingu - ca�e okno
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyj�cie
	case EXIT:
		exit(0);
	}
}

// sprawdzenie i przygotowanie obs�ugi wybranych rozszerze?

void ExtensionSetup()
{
	// pobranie numeru wersji biblioteki OpenGL
	const char *version = (char*)glGetString(GL_VERSION);

	// odczyt wersji OpenGL
	int major = 0, minor = 0;
	if (sscanf(version, "%d.%d", &major, &minor) != 2)
	{
#ifndef WIN32
		printf("B��dny format wersji OpenGL\n");
#else

		printf("Bledny format wersji OpenGL\n");
#endif

		exit(0);
	}

	// sprawdzenie czy jest co najmniej wersja 1.2
	if (major > 1 || minor >= 2)
		rescale_normal = true;
	else
		// sprawdzenie czy jest obs�ugiwane rozszerzenie EXT_rescale_normal
		if (glutExtensionSupported("GL_EXT_rescale_normal"))
			rescale_normal = true;
}

int main(int argc, char *argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// rozmiary g��wnego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie g��wnego okna programu
	glutCreateWindow("Wektory normalne");

	// do��czenie funkcji generuj�cej scen� 3D
	glutDisplayFunc(Display);

	// do��czenie funkcji wywo�ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// do��czenie funkcji obs�ugi klawiatury
	glutKeyboardFunc(Keyboard);

	// do��czenie funkcji obs�ugi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);

	// obs�uga przycisk�w myszki
	glutMouseFunc(MouseButton);

	// obs�uga ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie menu podr�cznego
	glutCreateMenu(Menu);

	// utworzenie podmenu - Materia�
	int MenuMaterial = glutCreateMenu(Menu);
#ifdef WIN32

	glutAddMenuEntry("Mosi�dz", BRASS);
	glutAddMenuEntry("Br�z", BRONZE);
	glutAddMenuEntry("Polerowany br�z", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("Mied�", COPPER);
	glutAddMenuEntry("Polerowana mied�", POLISHED_COPPER);
	glutAddMenuEntry("Z�oto", GOLD);
	glutAddMenuEntry("Polerowane z�oto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z o�owiem)", PEWTER);
	glutAddMenuEntry("Srebro", SILVER);
	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);
	glutAddMenuEntry("Szmaragd", EMERALD);
	glutAddMenuEntry("Jadeit", JADE);
	glutAddMenuEntry("Obsydian", OBSIDIAN);
	glutAddMenuEntry("Per�a", PEARL);
	glutAddMenuEntry("Rubin", RUBY);
	glutAddMenuEntry("Turkus", TURQUOISE);
	glutAddMenuEntry("Czarny plastik", BLACK_PLASTIC);
	glutAddMenuEntry("Czarna guma", BLACK_RUBBER);
#else

	glutAddMenuEntry("Mosiadz", BRASS);
	glutAddMenuEntry("Braz", BRONZE);
	glutAddMenuEntry("Polerowany braz", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("Miedz", COPPER);
	glutAddMenuEntry("Polerowana miedz", POLISHED_COPPER);
	glutAddMenuEntry("Zloto", GOLD);
	glutAddMenuEntry("Polerowane zloto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z o�owiem)", PEWTER);
	glutAddMenuEntry("Srebro", SILVER);
	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);
	glutAddMenuEntry("Szmaragd", EMERALD);
	glutAddMenuEntry("Jadeit", JADE);
	glutAddMenuEntry("Obsydian", OBSIDIAN);
	glutAddMenuEntry("Perla", PEARL);
	glutAddMenuEntry("Rubin", RUBY);
	glutAddMenuEntry("Turkus", TURQUOISE);
	glutAddMenuEntry("Czarny plastik", BLACK_PLASTIC);
	glutAddMenuEntry("Czarna guma", BLACK_RUBBER);
#endif

	// utworzenie podmenu - Wektory normalne
	int MenuNormals = glutCreateMenu(Menu);
#ifndef WIN32

	glutAddMenuEntry("Jeden wektor normalny na wierzcholek", NORMALS_SMOOTH);
	glutAddMenuEntry("Jeden wektor normalny na sciane", NORMALS_FLAT);
#else

	glutAddMenuEntry("Jeden wektor normalny na wierzcho�ek", NORMALS_SMOOTH);
	glutAddMenuEntry("Jeden wektor normalny na �cian�", NORMALS_FLAT);
#endif

	// utworzenie podmenu - aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);
#ifndef WIN32

	glutAddMenuEntry("Aspekt obrazu - ca�e okno", FULL_WINDOW);
#else

	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
#endif

	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	int MenuLight = glutCreateMenu(Menu);

	glutAddMenuEntry("Kierunkowe", KIERUNKOWE);
	glutAddMenuEntry("Otaczaj�ce", OTACZAJ�CE);
	glutAddMenuEntry("Reflektor", REFLEKTOR);


	// menu g��wne
	glutCreateMenu(Menu);

#ifdef WIN32

	glutAddSubMenu("Swiatlo", MenuLight);
#else

	glutAddSubMenu("Swiatlo", MenuLight);
#endif

#ifdef WIN32

	glutAddSubMenu("Materia�", MenuMaterial);
#else

	glutAddSubMenu("Material", MenuMaterial);
#endif

	glutAddSubMenu("Wektory normalne", MenuNormals);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
#ifndef WIN32

	glutAddMenuEntry("Wyj�cie", EXIT);
#else

	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// okre�lenie przycisku myszki obs�uguj�cej menu podr�czne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// sprawdzenie i przygotowanie obs�ugi wybranych rozszerze?
	ExtensionSetup();

	// wprowadzenie programu do obs�ugi p�tli komunikat�w
	glutMainLoop();
	return 0;
}
