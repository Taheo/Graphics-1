// opengl_mipmapy_2d.cpp : Defines the entry point for the console application.
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
#include "colors.h"
#include "targa.h"

// wska?nik na funkcjê glWindowPos2i

PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;

// sta³e do obs³ugi menu podrêcznego

enum
{
	PERSPECTIVE_CORRECTION_FASTEST,     // korekcja perspektywy - GL_FASTEST
	PERSPECTIVE_CORRECTION_DONT_CARE,   // korekcja perspektywy - GL_DONT_CARE
	PERSPECTIVE_CORRECTION_NICEST,      // korekcja perspektywy - GL_NICEST
	GENERATE_MIPMAP_FASTEST,            // jakoœæ generowania mipmap - GL_FASTEST
	GENERATE_MIPMAP_DONT_CARE,          // jakoœæ generowania mipmap - GL_DONT_CARE
	GENERATE_MIPMAP_NICEST,             // jakoœæ generowania mipmap - GL_NICEST
	FULL_WINDOW,                        // aspekt obrazu - ca³e okno
	ASPECT_1_1,                         // aspekt obrazu 1:1
	EXIT                                // wyjœcie
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

#define TARGA_HEADER_SIZE 0x12
#define TARGA_UNCOMP_RGB_IMG 0x02
#define TARGA_UNCOMP_BW_IMG 0x03
// rozmiary bry³y obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;

// k¹ty obrotu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wska?nik naciœniêcia lewego przycisku myszki

int button_state = GLUT_UP;

// po³o?enie kursora myszki

int button_x, button_y;

// wspó³czynnik skalowania

GLfloat scale = 1.5;

// identyfikatory tekstur

GLuint GROUND, WOOD, ROOF;

// identyfikatory list wyœwietlania

GLint GROUND_LIST, WOOD_LIST, ROOF_LIST;

// filtr pomniejszaj¹cy

GLint min_filter = GL_LINEAR_MIPMAP_LINEAR;

// wskazówki do korekcji perspektywy przy renderingu tekstur

GLint perspective_correction_hint = GL_DONT_CARE;

// wskazówki do automatycznego generowania mipmap

GLint mipmap_generation_hint = GL_DONT_CARE;

// funkcja rysuj¹ca napis w wybranym miejscu
// (wersja korzystaj¹ca z funkcji glWindowPos2i)

GLboolean load_targa(const char * filename, GLsizei & width, GLsizei & height, GLenum & format, GLenum & type, GLvoid *& pixels)
{
	pixels = NULL;
	width = 1024;
	height = 512;

	FILE * tga = fopen(filename, "rb");
	if (!tga)
		return GL_FALSE;
	unsigned char header[TARGA_HEADER_SIZE];
	fread(header, TARGA_HEADER_SIZE, 1, tga);
	fseek(tga, header[0], SEEK_CUR);
	width = header[12] + (header[13] << 8);
	height = header[14] + (header[15] << 8);
	if (header[2] == TARGA_UNCOMP_RGB_IMG && header[16] == 24)
	{
		pixels = new unsigned char[width * height * 3];
		fread((void *)pixels, width * height * 3, 1, tga);
		format = GL_BGR;
		type = GL_UNSIGNED_BYTE;
	}
	else
		if (header[2] == TARGA_UNCOMP_RGB_IMG && header[16] == 32)
		{
			pixels = new unsigned char[width * height * 4];
			fread((void *)pixels, width * height * 4, 1, tga);
			format = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
		}
		else
			if (header[2] == TARGA_UNCOMP_BW_IMG && header[16] == 8)
			{
				pixels = new unsigned char[width * height];
				fread((void *)pixels, width * height, 1, tga);
				format = GL_LUMINANCE;
				type = GL_UNSIGNED_BYTE;
			}
			else
				return GL_FALSE;
	fclose(tga);
	return GL_TRUE;
}

void DrawString(GLint x, GLint y, char *string)
{
	// po³o?enie napisu
	glWindowPos2i(x, y);

	// wyœwietlenie napisu
	int len = strlen(string);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
}

// funkcja generuj¹ca scenê 3D

void DisplayScene()
{
	// kolor t³a - zawartoœæ bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora g³êbokoœci
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesuniêcie uk³adu wspó³rzêdnych obiektów do œrodka bry³y odcinania
	glTranslatef(0.0, 0.0, -(near + far) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatey, 0.0, 1.0, 0.0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	// w³¹czenie testu bufora g³êbokoœci
	glEnable(GL_DEPTH_TEST);

	// w³¹czenie teksturowania dwuwymiarowego
	glEnable(GL_TEXTURE_2D);

	// filtr powiêkszaj¹cy
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// filtr pomniejszaj¹cy
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

	// wskazówki do korekcji perspektywy przy renderingu tekstur
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, perspective_correction_hint);

	// ustawienie parametów œrodowiska tekstur
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// rysowanie pod³o?a
	glBindTexture(GL_TEXTURE_2D, GROUND);
	glPushMatrix();
	glTranslatef(0.0, -1.0, 0.0);
	glCallList(GROUND_LIST);
	glPopMatrix();

	// rysowanie œcian domku
	glBindTexture(GL_TEXTURE_2D, WOOD);
	glPushMatrix();
	glTranslatef(0.0, -0.5, 0.0);
	glScalef(0.5, 0.5, 0.5);
	glCallList(WOOD_LIST);
	glPopMatrix();

	// rysowanie dachu domku
	glBindTexture(GL_TEXTURE_2D, ROOF);
	glPushMatrix();
	glTranslatef(0.0, -0.5, 0.0);
	glScalef(0.5, 0.5, 0.5);
	glCallList(ROOF_LIST);
	glPopMatrix();

	// wy³¹czenie teksturowania dwuwymiarowego
	glDisable(GL_TEXTURE_2D);

	// wyœwietlenie wybranych informacje
	char string[200];
	GLint var;
	glColor3fv(Black);

	// pobranie informacji o filtrze pomniejszaj¹cy
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &var);
	switch (var)
	{
		// filtr GL_NEAREST_MIPMAP_NEAREST
	case GL_NEAREST_MIPMAP_NEAREST:
		sprintf(string, "GL_TEXTURE_MIN_FILTER = GL_NEAREST_MIPMAP_NEAREST");
		break;

		// filtr GL_NEAREST_MIPMAP_LINEAR
	case GL_NEAREST_MIPMAP_LINEAR:
		sprintf(string, "GL_TEXTURE_MIN_FILTER = GL_NEAREST_MIPMAP_LINEAR");
		break;

		// filtr GL_LINEAR_MIPMAP_NEAREST
	case GL_LINEAR_MIPMAP_NEAREST:
		sprintf(string, "GL_TEXTURE_MIN_FILTER = GL_LINEAR_MIPMAP_NEAREST");
		break;

		// filtr GL_LINEAR_MIPMAP_LINEAR
	case GL_LINEAR_MIPMAP_LINEAR:
		sprintf(string, "GL_TEXTURE_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR");
		break;
	}
	DrawString(2, glutGet(GLUT_WINDOW_HEIGHT) - 17, string);

	// pobranie informacji o wskazówkach do automatycznego generowania mipmap
	glGetIntegerv(GL_PERSPECTIVE_CORRECTION_HINT, &var);
	switch (var)
	{
	case GL_FASTEST:
		sprintf(string, "GL_PERSPECTIVE_CORRECTION_HINT = GL_FASTEST");
		break;
	case GL_DONT_CARE:
		sprintf(string, "GL_PERSPECTIVE_CORRECTION_HINT = GL_DONT_CARE");
		break;
	case GL_NICEST:
		sprintf(string, "GL_PERSPECTIVE_CORRECTION_HINT = GL_NICEST");
		break;
	}
	DrawString(2, glutGet(GLUT_WINDOW_HEIGHT) - 33, string);

	// pobranie informacji o wskazówkach do korekcji perspektywy przy renderingu tekstur
	glGetIntegerv(GL_GENERATE_MIPMAP_HINT, &var);
	switch (var)
	{
	case GL_FASTEST:
		sprintf(string, "GL_GENERATE_MIPMAP_HINT = GL_FASTEST");
		break;
	case GL_DONT_CARE:
		sprintf(string, "GL_GENERATE_MIPMAP_HINT = GL_DONT_CARE");
		break;
	case GL_NICEST:
		sprintf(string, "GL_GENERATE_MIPMAP_HINT = GL_NICEST");
		break;
	}
	DrawString(2, glutGet(GLUT_WINDOW_HEIGHT) - 49, string);

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
		// wysokoœæ okna wiêksza od wysokoœci okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else

			// szerokoœæ okna wiêksza lub równa wysokoœci okna
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

// obs³uga przycisków myszki

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiêtanie stanu lewego przycisku myszki
		button_state = state;

		// zapamiêtanie po³o?enia kursora myszki
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
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

// utworzenie tekstur

void GenerateTextures()
{
	// zmienne u?yte przy obs³udze plików TARGA
	GLsizei width, height;
	GLenum format, type;
	GLvoid *pixels;

	// tryb upakowania bajtów danych tekstury
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// wskazówki do automatycznego generowania mipmap
	glHint(GL_GENERATE_MIPMAP_HINT, mipmap_generation_hint);

	// wczytanie tekstury ground1-2.tga
	GLboolean error = load_targa("ground1-2.tga", width, height, format, type, pixels);

	// b³¹d odczytu pliku
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku ground1-2.tga");
		exit(1);
	}

	// utworzenie identyfikatora tekstury
	glGenTextures(1, &GROUND);

	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, GROUND);

	// w³¹czenie automatycznego generowania mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	// definiowanie tekstury (z mipmapami)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixels);

	// porz¹dki
	delete[](unsigned char*)pixels;

	// wczytanie tekstury wall_wood_verti_color.tga
	error = load_targa("r.tga", width, height, format, type, pixels);

	// b³¹d odczytu pliku
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku wall_wood_verti_color.tga");
		exit(2);
	}

	// utworzenie identyfikatora tekstury
	glGenTextures(1, &WOOD);

	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, WOOD);

	// w³¹czenie automatycznego generowania mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	// definiowanie tekstury (z mipmapami)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixels);

	// porz¹dki
	delete[](unsigned char*)pixels;

	// wczytanie tekstury roof_old_rectangle_color.tga
	error = load_targa("roof_old_rectangle_color.tga", width, height, format, type, pixels);

	// b³¹d odczytu pliku
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku roof_old_rectangle_color.tga");
		exit(3);
	}

	// utworzenie identyfikatora tekstury
	glGenTextures(1, &ROOF);

	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, ROOF);

	// w³¹czenie automatycznego generowania mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	// definiowanie tekstury (z mipmapami)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixels);

	// porz¹dki
	delete[](unsigned char*)pixels;
}

// obs³uga menu podrêcznego

void Menu(int value)
{
	switch (value)
	{
		// filtr pomniejszaj¹cy
	case GL_NEAREST_MIPMAP_NEAREST:
	case GL_NEAREST_MIPMAP_LINEAR:
	case GL_LINEAR_MIPMAP_NEAREST:
	case GL_LINEAR_MIPMAP_LINEAR:
		min_filter = value;
		DisplayScene();
		break;

		// wskazówka GL_FASTEST do korekcji perspektywy przy renderingu tekstur
	case PERSPECTIVE_CORRECTION_FASTEST:
		perspective_correction_hint = GL_FASTEST;
		DisplayScene();
		break;

		// wskazówka GL_DONT_CARE do korekcji perspektywy przy renderingu tekstur
	case PERSPECTIVE_CORRECTION_DONT_CARE:
		perspective_correction_hint = GL_DONT_CARE;
		DisplayScene();
		break;

		// wskazówka GL_NICEST do korekcji perspektywy przy renderingu tekstur
	case PERSPECTIVE_CORRECTION_NICEST:
		perspective_correction_hint = GL_NICEST;
		DisplayScene();
		break;

		// wskazówka do automatycznego generowania mipmap
	case GENERATE_MIPMAP_FASTEST:
		GenerateTextures();
		mipmap_generation_hint = GL_FASTEST;
		DisplayScene();
		break;

		// wskazówka GL_DONT_CARE do automatycznego generowania mipmap
	case GENERATE_MIPMAP_DONT_CARE:
		GenerateTextures();
		mipmap_generation_hint = GL_DONT_CARE;
		DisplayScene();
		break;

		// wskazówka GL_NICEST do automatycznego generowania mipmap
	case GENERATE_MIPMAP_NICEST:
		GenerateTextures();
		mipmap_generation_hint = GL_NICEST;
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
	// generowanie identyfikatora pierwszej listy wyœwietlania
	GROUND_LIST = glGenLists(1);

	// pierwsza lista wyœwietlania - pod³o?e
	glNewList(GROUND_LIST, GL_COMPILE);

	// czworok¹t
	glBegin(GL_QUADS);
	glTexCoord2f(16.0, 0.0);
	glVertex3f(-8.0, 0.0, -8.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-8.0, 0.0, 8.0);
	glTexCoord2f(0.0, 16.0);
	glVertex3f(8.0, 0.0, 8.0);
	glTexCoord2f(16.0, 16.0);
	glVertex3f(8.0, 0.0, -8.0);
	glEnd();

	// koniec pierwszej listy wyœwietlania
	glEndList();

	// generowanie identyfikatora drugiej listy wyœwietlania
	WOOD_LIST = glGenLists(1);

	// druga lista wyœwietlania - œciany chatki
	glNewList(WOOD_LIST, GL_COMPILE);

	// seria trójk¹tów
	glBegin(GL_TRIANGLES);

	// przednia œciana
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(1.0, -1.0, 1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(-1.0, 1.0, 1.0);

	// prawa boczna œciana
	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.0, 1.0, -1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(1.0, -1.0, 1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.0, 1.0, -1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(1.0, -1.0, 1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(1.0, -1.0, -1.0);

	// lewa boczna œciana
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(-1.0, 1.0, 1.0);

	// tylna œciana
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(1.0, 1.0, -1.0);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(1.0, -1.0, -1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(1.0, 1.0, -1.0);

	// przedni szczyt
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, 1.0, 1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(1.0, 2.0);
	glVertex3f(0.0, 2.0, 1.0);

	// tylny szczyt
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(1.0, 2.0);
	glVertex3f(0.0, 2.0, -1.0);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(1.0, 1.0, -1.0);
	glEnd();

	// koniec drugiej listy wyœwietlania
	glEndList();

	// generowanie identyfikatora trzeciej listy wyœwietlania
	ROOF_LIST = glGenLists(1);

	// trzecia lista wyœwietlania - dach chatki
	glNewList(ROOF_LIST, GL_COMPILE);

	// dwa czworok¹ty
	glBegin(GL_QUADS);

	// lewa czêœæ dachu
	glTexCoord2f(2.0, 2.0);
	glVertex3f(0.0, 2.0, 1.2);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(0.0, 2.0, -1.2);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, 1.0, -1.2);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(-1.0, 1.0, 1.2);

	// prawa czêœæ dachu
	glTexCoord2f(2.0, 2.0);
	glVertex3f(0.0, 2.0, -1.2);
	glTexCoord2f(0.0, 2.0);
	glVertex3f(0.0, 2.0, 1.2);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.0, 1.0, 1.2);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(1.0, 1.0, -1.2);
	glEnd();

	// koniec trzeciej listy wyœwietlania
	glEndList();
}

// sprawdzenie i przygotowanie obs³ugi wybranych rozszerze?

void ExtensionSetup()
{
	// pobranie numeru wersji biblioteki OpenGL
	const char *version = (char*)glGetString(GL_VERSION);

	// odczyt wersji OpenGL
	int major = 0, minor = 0;
	if (sscanf(version, "%d.%d", &major, &minor) != 2)
	{
#ifdef WIN32
		printf("B³êdny format wersji OpenGL\n");
#else

		printf("Bledny format wersji OpenGL\n");
#endif

		exit(0);
	}

	// sprawdzenie czy jest co najmniej wersja 1.4 OpenGL lub
	// czy jest obs³ugiwane rozszerzenie GL_SGIS_generate_mipmap
	if (!(major > 1 || minor >= 4) &&
		!glutExtensionSupported("GL_SGIS_generate_mipmap"))
	{
		printf("Brak rozszerzenia GL_SGIS_generate_mipmap!\n");
		exit(0);
	}

	// sprawdzenie czy jest co najmniej wersja 1.4
	if (major > 1 || minor >= 4)
	{
		// pobranie wska?nika wybranej funkcji OpenGL 1.4
		glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress("glWindowPos2i");
	}
	else
		// sprawdzenie czy jest obs³ugiwane rozszerzenie ARB_window_pos
		if (glutExtensionSupported("GL_ARB_window_pos"))
		{
			// pobranie wska?nika wybranej funkcji rozszerzenia ARB_window_pos
			glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress
				("glWindowPos2iARB");
		}
		else
		{
			printf("Brak rozszerzenia ARB_window_pos!\n");
			exit(0);
		}
}

int main(int argc, char *argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// rozmiary g³ównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie g³ównego okna programu
	glutCreateWindow("Mipmapy 2D");

	// do³¹czenie funkcji generuj¹cej scenê 3D
	glutDisplayFunc(DisplayScene);

	// do³¹czenie funkcji wywo³ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// do³¹czenie funkcji obs³ugi klawiatury
	glutKeyboardFunc(Keyboard);

	// obs³uga przycisków myszki
	glutMouseFunc(MouseButton);

	// obs³uga ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie podmenu - Filtr pomniejszaj¹cy
	int MenuMinFilter = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR);

	// utworzenie podmenu - GL_PERSPECTIVE_CORRECTION_HINT
	int PerspectiveCorrectionHint = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_FASTEST", PERSPECTIVE_CORRECTION_FASTEST);
	glutAddMenuEntry("GL_DONT_CARE", PERSPECTIVE_CORRECTION_DONT_CARE);
	glutAddMenuEntry("GL_NICEST", PERSPECTIVE_CORRECTION_NICEST);

	// utworzenie podmenu - GL_GENERATE_MIPMAP_HINT
	int GenerateMipmapHint = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_FASTEST", GENERATE_MIPMAP_FASTEST);
	glutAddMenuEntry("GL_DONT_CARE", GENERATE_MIPMAP_DONT_CARE);
	glutAddMenuEntry("GL_NICEST", GENERATE_MIPMAP_NICEST);

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

	glutAddSubMenu("Filtr pomniejszaj¹cy", MenuMinFilter);
	glutAddSubMenu("GL_PERSPECTIVE_CORRECTION_HINT", PerspectiveCorrectionHint);
	glutAddSubMenu("GL_GENERATE_MIPMAP_HINT", GenerateMipmapHint);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjœcie", EXIT);
#else

	glutAddSubMenu("Filtr pomniejszajacy", MenuMinFilter);
	glutAddSubMenu("GL_PERSPECTIVE_CORRECTION_HINT", PerspectiveCorrectionHint);
	glutAddSubMenu("GL_GENERATE_MIPMAP_HINT", GenerateMipmapHint);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// okreœlenie przycisku myszki obs³uguj¹cego menu podrêczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// utworzenie tekstur
	GenerateTextures();

	// sprawdzenie i przygotowanie obs³ugi wybranych rozszerze?
	ExtensionSetup();

	// utworzenie list wyœwietlania
	GenerateDisplayLists();

	// wprowadzenie programu do obs³ugi pêtli komunikatów
	glutMainLoop();
	return 0;
}