
#include "stdafx.h"


#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "glext.h"

// sta³e do obs³ugi menu podrêcznego

enum
{
	MAG_FILTER,     // filtr powiêkszaj¹cy
	MIN_FILTER,     // filtr pomniejszaj¹cy
	FULL_WINDOW,    // aspekt obrazu - ca³e okno
	ASPECT_1_1,     // aspekt obrazu 1:1
	EXIT            // wyjœcie
};

// aspekt obrazu

int aspect = FULL_WINDOW;

// usuniêcie definicji makr near i far

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

// rozmiary bry³y obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;

// wspó³czynnik skalowania

GLfloat scale = 1.05;

// identyfikatory list wyœwietlania

GLint RECT_LIST;
GLint TEXTURE_256_LIST, TEXTURE_128_LIST, TEXTURE_64_LIST;

// filtr powiêkszaj¹cy

GLint mag_filter = GL_NEAREST;

// filtr pomniejszaj¹cy

GLint min_filter = GL_NEAREST;

// funkcja generuj¹ca scenê 3D

void DisplayScene()
{
	// kolor t³a - zawartoœæ bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru
	glClear(GL_COLOR_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesuniêcie uk³adu wspó³rzêdnych obiektów do œrodka bry³y odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, 1.0, 1.0);

	// w³¹czenie teksturowania jednowymiarowego
	glEnable(GL_TEXTURE_1D);

	// tryb upakowania bajtów danych tekstury
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// filtr powiêkszaj¹cy
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, mag_filter);

	// filtr pomniejszaj¹cy
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, min_filter);

	// usunuiêcie b³êdów przy renderingu brzegu tekstury
	if (mag_filter == GL_LINEAR)
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	// wyœwietlenie tekstur
	glCallList(TEXTURE_256_LIST);
	glCallList(TEXTURE_128_LIST);
	glCallList(TEXTURE_64_LIST);

	// skierowanie polece? do wykonania
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
	if (aspect == ASPECT_1_1)
	{
		// wysokoœ? okna wiêksza od wysokoœci okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else

			// szerokoœ? okna wiêksza lub równa wysokoœci okna
			if (width >= height && height > 0)
				glFrustum(left*width / height, right*width / height, bottom, top, near, far);
	}
	else
		glFrustum(left, right, bottom, top, near, far);

	// generowanie sceny 3D
	DisplayScene();
}

// obs³uga klawiatury

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
	DisplayScene();
}

// obs³uga menu podrêcznego

void Menu(int value)
{
	switch (value)
	{
		// filtr powiêkszaj¹cy: GL_NEAREST/GL_LINEAR
	case MAG_FILTER:
		if (mag_filter == GL_NEAREST)
			mag_filter = GL_LINEAR;
		else
			mag_filter = GL_NEAREST;
		DisplayScene();
		break;

		// filtr pomniejszaj¹cy: GL_NEAREST/GL_LINEAR
	case MIN_FILTER:
		if (min_filter == GL_NEAREST)
			min_filter = GL_LINEAR;
		else
			min_filter = GL_NEAREST;
		DisplayScene();
		break;

		// obszar renderingu - ca³e okno
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyjœcie
	case EXIT:
		exit(0);
	}
}

// utworzenie list wyœwietlania

void GenerateDisplayLists()
{
	// generowanie identyfikatora listy wyœwietlania
	RECT_LIST = glGenLists(1);

	// lista wyœwietlania - prostok¹t
	glNewList(RECT_LIST, GL_COMPILE);

	// na³o?enie tekstury na prostok¹t
	glBegin(GL_QUADS);
	glTexCoord1f(1.0);
	glVertex2f(1.5, 0.7);
	glTexCoord1f(0.0);
	glVertex2f(-1.5, 0.7);
	glTexCoord1f(0.0);
	glVertex2f(-1.5, -0.7);
	glTexCoord1f(1.0);
	glVertex2f(1.5, -0.7);
	glEnd();

	// koniec listy wyœwietlania
	glEndList();

	// sprawdzenie czy implementacja biblioteki obs³uguje tekstury o wymiarze 256
	GLint size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
	if (size < 256)
	{
		printf("Rozmiar tekstur mniejszy od 256");
		exit(0);
	}

	// dane tekstury
	GLubyte texture[256 * 3];

	// przygotowanie danych tekstury RGB
	for (int i = 0; i < 256; i++)
	{
		texture[3 * i + 0] = i;
		texture[3 * i + 1] = i;
		texture[3 * i + 2] = i;
	}

	// generowanie identyfikatora listy wyœwietlania
	TEXTURE_256_LIST = glGenLists(1);

	// lista wyœwietlania - tekstura o szerokoœci 256 tekseli
	glNewList(TEXTURE_256_LIST, GL_COMPILE);

	// definiowanie tekstury
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	// od³o?enie macierzy modelowania na stos
	glPushMatrix();

	// przesuniêcie prostok¹ta do góry o dwie jednostki
	glTranslatef(0.0, 2.0, 0.0);

	// na³o?enie tekstury na prostok¹t
	glCallList(RECT_LIST);

	// zdjêcie macierzy modelowania ze stosu
	glPopMatrix();

	// koniec listy wyœwietlania
	glEndList();

	//  przygotowanie danych tekstury LUMINANCE
	for (int i = 0; i < 128; i++)
	{
		texture[i] = i * 2;
	}

	// generowanie identyfikatora listy wyœwietlania
	TEXTURE_128_LIST = glGenLists(1);

	// lista wyœwietlania - tekstura o szerokoœci 128 tekseli
	glNewList(TEXTURE_128_LIST, GL_COMPILE);

	// definiowanie tekstury
	glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE, 64, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texture);

	// na³o?enie tekstury na prostok¹t
	glCallList(RECT_LIST);

	// koniec listy wyœwietlania
	glEndList();

	// przygotowanie danych tekstury INTENSITY
	for (int i = 0; i < 64; i++)
	{
		texture[3 * i] = i * 4;
	}

	// generowanie identyfikatora listy wyœwietlania
	TEXTURE_64_LIST = glGenLists(1);

	// lista wyœwietlania - tekstura o szerokoœci 64 tekseli
	glNewList(TEXTURE_64_LIST, GL_COMPILE);

	// definiowanie tekstury
	glTexImage1D(GL_TEXTURE_1D, 0, GL_INTENSITY, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	// od³o?enie macierzy modelowania na stos
	glPushMatrix();

	// przesuniêcie prostok¹ta do do³u o dwie jednostki
	glTranslatef(0.0, -2.0, 0.0);

	// na³o?enie tekstury na prostok¹t
	glCallList(RECT_LIST);

	// zdjêcie macierzy modelowania ze stosu
	glPopMatrix();

	// koniec listy wyœwietlania
	glEndList();
}

// sprawdzenie i przygotowanie obs³ugi wybranych rozszerze?

void ExtensionSetup()
{
	// pobranie numeru wersji biblioteki OpenGL
	const char *version = (char*)glGetString(GL_VERSION);

	// odczyt wersji OpenGL
	int major = 0, minor = 0;
	if (sscanf_s(version, "%d.%d", &major, &minor) != 2)
	{
#ifdef WIN32
		printf("B³êdny format wersji OpenGL\n");
#else

		printf("Bledny format wersji OpenGL\n");
#endif

		exit(0);
	}

	// sprawdzenie czy jest co najmniej wersja 1.2 OpenGL lub
	// czy jest obs³ugiwane rozszerzenie GL_SGIS_texture_edge_clamp
	if (!(major > 1 || minor >= 2) &&
		!glutExtensionSupported("GL_SGIS_texture_edge_clamp"))
	{
		printf("Brak rozszerzenia GL_SGIS_texture_edge_clamp!\n");
		exit(0);
	}
}

int main(int argc, char *argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	// rozmiary g³ównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie g³ównego okna programu
	glutCreateWindow("Tekstura 1D");

	// do³¹czenie funkcji generuj¹cej scenê 3D
	glutDisplayFunc(DisplayScene);

	// do³¹czenie funkcji wywo³ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// do³¹czenie funkcji obs³ugi klawiatury
	glutKeyboardFunc(Keyboard);

	// utworzenie podmenu - Aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);
#ifdef WIN32

	glutAddMenuEntry("Aspekt obrazu - ca³e okno", FULL_WINDOW);
#else

	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
#endif

	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu g³ówne
	glutCreateMenu(Menu);

#ifdef WIN32

	glutAddMenuEntry("Filtr powiêkszaj¹cy: GL_NEAREST/GL_LINEAR", MAG_FILTER);
	glutAddMenuEntry("Filtr pomniejszaj¹cy: GL_NEAREST/GL_LINEAR", MIN_FILTER);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjœcie", EXIT);
#else

	glutAddMenuEntry("Filtr powiekszajacy: GL_NEAREST/GL_LINEAR", MAG_FILTER);
	glutAddMenuEntry("Filtr pomniejszajacy: GL_NEAREST/GL_LINEAR", MIN_FILTER);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// okreœlenie przycisku myszki obs³uguj¹cego menu podrêczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// utworzenie list wyœwietlania
	GenerateDisplayLists();

	// sprawdzenie i przygotowanie obs³ugi wybranych rozszerze?
	ExtensionSetup();

	// wprowadzenie programu do obs³ugi pêtli komunikatów
	glutMainLoop();
	return 0;
}
