// OpenGL_TRIANGLE_ConsoleApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "math.h"

#include "stdafx.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define GL_PI 3.1415f

enum
{
	EXIT                            // wyjêcie
};

// Wielkoœæ obrotów
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

static GLboolean iCull, iDepth, iOutline;

// Funkcja wykonuje wszystkie konieczne inicjalizacje kontekstu renderowania
void SetupRC()
{
	// Czarne t³o
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor rysuj¹cy - zielony
	glColor3f(0.0f, 1.0f, 0.0f);
	// Model cieniowania kolorów - p³aski
	glShadeModel(GL_FLAT);
	// Wielok¹ty o nawiniêciu zgodnym z ruchem wskazówek zegara traktowane s¹
	// jako skierowane do przodu. Takie ustawienie jest konieczne, poniewa¿
	// korzystamy z wachlarzy trójk¹tów.
	glFrontFace(GL_CW);
}
// Wywo³ywana w celu przerysowania sceny
void RenderScene(void)
{
	GLfloat x, y, angle; // Przechowuj¹ wartoœci wspó³rzêdnych i k¹ta
	int iPivot = 1; // Do oznaczania zamiennych kolorów
					// Wyczyszczenie okna i bufora g³êbi
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// W³¹czenie lub wy³¹czenie mechanizmu eliminowania ukrytych powierzchni
	if (iCull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	// W³¹czenie lub wy³¹czenie mechanizmu sprawdzania g³êbi
	if (iDepth)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	// Je¿eli ten znacznik bêdzie ustawiony, to wielok¹ty zwrócone
	// ty³em do widza bêd¹ rysowane tylko w postaci szkieletu
	if (iOutline)
		glPolygonMode(GL_BACK, GL_LINE);
	else
		glPolygonMode(GL_BACK, GL_FILL);
	// Zapisanie stanu macierzy i wykonanie obrotu
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	// Rozpoczêcie rysowania wachlarza trójk¹tów
	glBegin(GL_TRIANGLE_FAN);
	// Czubek stoka jest wspólnym wierzcho³kiem wszystkich trójk¹tów z wachlarza
	// wysuniêtym do góry w osi z. W ten sposób zamiast ko³a powstanie sto¿ek.
	glVertex3f(0.0f, 0.0f, 75.0f);
	// Wykonujemy obrót w oko³o i oznaczamy w równych odstêpach wierzcho³ki
	// tworz¹ce wachlarz trójk¹tów.
	for (angle = 0.0f; angle < (2.0f*GL_PI); angle += (GL_PI / 2.f))
	{
		// Wyliczenie wspó³rzêdnych x i y kolejnego wierzcho³ka
		x = 50.0f*sin(angle);
		y = 50.0f*cos(angle);
		// Wybieranie koloru - zielony lub czerwony
		if ((iPivot % 2) == 0)
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(1.0f, 0.0f, 0.0f);
		// Inkrementacja zmiennej okrelaj¹cej rodzaj koloru
		iPivot++;
		// Definiowanie kolejnego wierzcho³ka w wachlarzu trójk¹tów
		glVertex2f(x, y);


	}
	// Zakoczenie rysowania trójk¹tów sto¿ka
	glEnd();
	// Rozpoczêcie rysowania kolejnego wachlarza trójk¹tów
	// zakrywaj¹cego podstaw stoka
	glBegin(GL_TRIANGLE_FAN);
	// œrodek wachlarza znajduje siê na pocz¹tku uk³adu wspó³rzêdnych
	glVertex2f(0.0f, 0.0f);
	for (angle = 0.0f; angle < (2.0f*GL_PI); angle += (GL_PI / 2.f))
	{
		// Wyliczenie wspó³rzêdnych x i y kolejnego wierzcho³ka
		x = 50.0f*sin(angle);
		y = 50.0f*cos(angle);
		// Wybieranie koloru - zielony lub czerwony
		if ((iPivot % 2) == 0)
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(1.0f, 0.0f, 0.0f);
		// Inkrementacja zmiennej okrelaj¹cej rodzaj koloru
		iPivot++;
		// Definiowanie kolejnego wierzcho³ka w wachlarzu trójk¹tów
		glVertex2f(x, y);
	}
	// Zakoczenie rysowania trójk¹tów podstawy stoka
	glEnd();
	// Odtworzenie macierzy przekszta³ceñ
	glPopMatrix();
	// Wys³anie poleceñ do wykonania
	glutSwapBuffers();
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
	// Odœwie¿enie zawartoœci okna
	glutPostRedisplay();
}
void ChangeSize(int w, int h)
{
	GLfloat nRange = 100.0f;
	// Zabezpieczenie przed dzieleniem przez zero
	if (h == 0)
		h = 1;
	// Ustalenie wymiarów widoku na zgodnych z wymiarami okna
	glViewport(0, 0, w, h);
	// Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Utworzenie przestrzeni ograniczaj¹cej (lewo, prawo, dó³, góra, blisko, daleko)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);
	// Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Menu(int value)
{
	switch (value)
	{
		//
	case GL_CULL_FACE:
		iCull = !iCull;
		glutSetWindowTitle("Stozek - GL_CULL_FACE");
		RenderScene();
		break;

		//
	case GL_DEPTH_TEST:
		iDepth = !iDepth;
		glutSetWindowTitle("Stozek - GL_DEPTH_TEST");
		RenderScene();
		break;

		//
	case GL_LINE:
		iOutline = true;
		glutSetWindowTitle("Stozek - GL_LINE");
		RenderScene();
		break;

		//
	case GL_FILL:
		iOutline = false;
		glutSetWindowTitle("Stozek - GL_LINE");
		RenderScene();
		break;

		// wyjêcie
	case EXIT:
		exit(0);
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Triangles Constructing Fan Example");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	// utworzenie podmenu - Prymitywy
	int MenuPrimitive = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_CULL_FACE", GL_CULL_FACE);
	glutAddMenuEntry("GL_DEPTH_TEST", GL_DEPTH_TEST);
	glutAddMenuEntry("Polygon mode LINE", GL_LINE);
	glutAddMenuEntry("Polygon mode FILL", GL_FILL);
	glutAddMenuEntry("Wyjecie", EXIT);

	// okreœlenie przycisku myszki obs³uguj¹cego menu podrêczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}