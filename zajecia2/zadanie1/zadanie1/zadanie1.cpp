#include "stdafx.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
// Definiujemy sta� o wartoci liczby PI
#define GL_PI 3.1415f
// Wielko obrot�w
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
GLfloat x, y, z, angle;
// Wywo�ywana w celu przerysowania sceny
void RenderScene(void)
{
	// Zmienne przechowujce wsp�rzdne i kty  x11
	// Wyczyszczenie okna aktualnym kolorem czyszczcym
	glClear(GL_COLOR_BUFFER_BIT);
	// Zapisanie stanu macierzy i wykonanie obrotu
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	// Wywo�a tylko raz, przed rysowaniem wszystkich punkt�w
	glBegin(GL_POINTS);
	x = -50.0f;

	GLfloat sizes[2]; // Do zapisania dopuszczalnego zakresu wielko�ci
	GLfloat step; //Zapami�tuje obs�ugiwany przyrost rozmiaru
				  // Pobranie dopuszczalnych rozmiar�w punkt�w
	glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
	glGetFloatv(GL_POINT_SIZE_GRANULARITY, &step);
	GLfloat curSize = sizes[0];
	for (angle = 0.0f; angle <= (2.0f*GL_PI)*11.0f; angle += 0.1f)
	{
		z = 50.0f*sin(angle);
		y = 50.0f*cos(angle);
		// Okrelenie punktu i przesunicie wsp�rzdnej Z
		glVertex3f(x, y, z);
		x += 0.52f;
		glPointSize(curSize);
		curSize += step;

	}
	// Zakoczenie rysowania punkt�w
	glEnd();
	// Odtworzenie macierzy przekszta�ce
	glPopMatrix();
	// Wykonanie polece rysowania
	glutSwapBuffers();
}
// Ta funkcja wykonuje wszystkie konieczne inicjalizacje kontekstu renderowania
void SetupRC()
{
	// Czarne t�o
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Bdziemy rysowa kolorem zielonym
	glColor3f(0.0f, 1.0f, 0.0f);
}
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		xRot -= 5.0f;
	if (key == GLUT_KEY_DOWN)
		xRot += 5.0f;
	if (key == GLUT_KEY_LEFT)
		yRot -= 5.0f;
	if (key == GLUT_KEY_RIGHT)
		yRot += 5.0f;
	if (key > 356.0f)
		xRot = 0.0f;
	if (key < -1.0f)
		xRot = 355.0f;
	if (key > 356.0f)
		yRot = 0.0f;
	if (key < -1.0f)
		yRot = 355.0f;
	// Odwieenie zawartoci okna
	glutPostRedisplay();
}
void ChangeSize(int w, int h)
{
	GLfloat nRange = 100.0f;
	// Zabezpieczenie przed dzieleniem przez zero
	if (h == 0)
		h = 1;
	// Ustalenie wymiar�w widoku na zgodnych z wymiarami okna
	glViewport(0, 0, w, h);
	// Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Utworzenie przestrzeni ograniczajcej (lewo, prawo, d�, g�ra, blisko, daleko)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);
	// Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Points Example");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();
	return 0;
}