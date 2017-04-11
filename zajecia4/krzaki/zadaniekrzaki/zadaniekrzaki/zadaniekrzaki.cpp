#include "stdafx.h"
#include <GL/glut.h>
#include "glext.h"
#ifndef WIN32
#define GLX_GLXEXT_LEGACY
#include <GL/glx.h>
#define wglGetProcAddress glXGetProcAddressARB
#endif
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
#define TARGA_HEADER_SIZE 0x12
#define TARGA_UNCOMP_RGB_IMG 0x02
#define TARGA_UNCOMP_BW_IMG 0x03
// wskaŸnik na funkcjê glWindowPos2i
// wskaŸnik na funkcjê glWindowPos2i
PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;
// sta³e do obs³ugi menu podrêcznego
enum
{
	FULL_WINDOW, // aspekt obrazu - ca³e okno
	ASPECT_1_1, // aspekt obrazu 1:1
	EXIT // wyjœcie
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
// k¹ty obrotu
GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;
// wskaŸnik naciœniêcia lewego przycisku myszki
int button_state = GLUT_UP;
// po³o?enie kursora myszki
int button_x, button_y;
// wspó³czynnik skalowania
GLfloat scale = 1.5;
// identyfikatory tekstur
GLuint GRASS, TREE;
// identyfikatory list wyœwietlania
GLint GRASS_LIST, TREE_LIST;
// w³¹czenie/wy³¹czenie
bool alpha_test = true;
bool blend = false;
// funkcja rysuj¹ca napis w wybranym miejscu
// (wersja korzystaj¹ca z funkcji glWindowPos2i)
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
	// kolor t³a - zawartoœ? bufora koloru
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// wskazówki do korekcji perspektywy przy renderingu tekstur
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// ustawienie parametów œrodowiska tekstur
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// w³¹czenie/wy³¹czenie testowania kana³u alfa
	if (alpha_test == GL_TRUE)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5);
	}
	else
		glDisable(GL_ALPHA_TEST);
	// w³¹czenie/wy³¹czenie mieszania kolorów
	if (blend == GL_TRUE)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);
	// rysowanie trawy
	glBindTexture(GL_TEXTURE_2D, GRASS);
	glPushMatrix();
	glTranslatef(0.0, -0.5, 0.0);
	glCallList(GRASS_LIST);
	glPopMatrix();
	// rysowanie trzech drzew
	glBindTexture(GL_TEXTURE_2D, TREE);
	glPushMatrix();
	glTranslatef(-0.5, 0.0, 0.0);
	glScalef(0.5, 0.5, 0.5);
	glCallList(TREE_LIST);
	glTranslatef(1.0, 0.0, 0.5);
	glCallList(TREE_LIST);
	glTranslatef(1.0, 0.0, 0.5);
	glCallList(TREE_LIST);
	glPopMatrix();
	// wy³¹czenie teksturowania dwuwymiarowego
	glDisable(GL_TEXTURE_2D);
	// wyœwietlenie wybranych informacje
	char string[200];
	GLboolean var;
	glColor3fv(Black);
	// pobranie inforacji o w³¹czeniu testowania kana³u alfa
	glGetBooleanv(GL_ALPHA_TEST, &var);
	if (var == GL_TRUE)
		strcpy(string, "GL_ALPHA_TEST = GL_TRUE");
	else
		strcpy(string, "GL_ALPHA_TEST = GL_FALSE");
	DrawString(2, glutGet(GLUT_WINDOW_HEIGHT) - 17, string);
	// pobranie informacji o w³¹czeniu mieszania kolorów
	glGetBooleanv(GL_BLEND, &var);
	if (var == GL_TRUE)
		strcpy(string, "GL_BLEND = GL_TRUE");
	else
		strcpy(string, "GL_BLEND = GL_FALSE");
	DrawString(2, glutGet(GLUT_WINDOW_HEIGHT) - 33, string);
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
	if (aspect == ASPECT_1_1)
	{
		// wysokoœ? okna wiêksza od wysokoœci okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else
			// szerokoœ? okna wiêksza lub równa wysokoœci okna
			if (width >= height && height > 0)
				glFrustum(left*width / height, right*width / height, bottom, top, near,
					far);
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
	// wczytanie tekstury grass_color.tga
	GLboolean error = load_targa("ground1-2.tga", width, height, format, type, pixels);
	// b³¹d odczytu pliku
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku grass_color.tga");
		exit(0);
	}
	// utworzenie identyfikatora tekstury
	glGenTextures(1, &GRASS);
	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, GRASS);
	// w³¹czenie automatycznego generowania mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	// definiowanie tekstury (z mipmapami)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixels);
	// porz¹dki
	delete[](unsigned char*)pixels;
	// wczytanie tekstury branch.tga
	error = load_targa("shrub_medium_size.tga", width, height, format, type, pixels);
	// b³¹d odczytu pliku
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku branch.tga");
		exit(0);
	}
	// utworzenie identyfikatora tekstury
	glGenTextures(1, &TREE);
	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, TREE);
	// w³¹czenie automatycznego generowania mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	// definiowanie tekstury (z mipmapami)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, type, pixels);
	// porz¹dki
	delete[](unsigned char*)pixels;
}
// obs³uga menu podrêcznego
void Menu(int value)
{
	switch (value)
	{
		// test kana³u alfa: w³¹cz/wy³¹cz
	case GL_ALPHA_TEST:
		alpha_test = !alpha_test;
		DisplayScene();
		break;
		// mieszanie kolorów: w³¹cz/wy³¹cz
	case GL_BLEND:
		blend = !blend;
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
	GRASS_LIST = glGenLists(1);
	// pierwsza lista wyœwietlania - trawa
	glNewList(GRASS_LIST, GL_COMPILE);
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
	TREE_LIST = glGenLists(1);
	// druga lista wyœwietlania - drzewo
	glNewList(TREE_LIST, GL_COMPILE);
	// dwa trójk¹ty
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.0, 1.0, 0.0);
	glEnd();
	// koniec drugiej listy wyœwietlania
	glEndList();
}
// sprawdzenie i przygotowanie obs³ugi wybranych rozszerzeñ
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
		// pobranie wskaŸnika wybranej funkcji OpenGL 1.4
		glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress("glWindowPos2i");
	}
	else
		// sprawdzenie czy jest obs³ugiwane rozszerzenie ARB_window_pos
		if (glutExtensionSupported("GL_ARB_window_pos"))
		{
			// pobranie wskaŸnika wybranej funkcji rozszerzenia ARB_window_pos
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
	glutCreateWindow("Tekstura alfa");
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
	glutAddMenuEntry("Test kana³u alfa: w³¹cz/wy³¹cz", GL_ALPHA_TEST);
	glutAddMenuEntry("Mieszanie kolorów: w³¹cz/wy³¹cz", GL_BLEND);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjœcie", EXIT);
#else
	glutAddMenuEntry("Test kanalu alfa: wlacz/wylacz", GL_ALPHA_TEST);
	glutAddMenuEntry("Mieszanie kolorow: wlacz/wylacz", GL_BLEND);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif
	// okreœlenie przycisku myszki obs³uguj¹cego menu podrêczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	// utworzenie tekstur
	GenerateTextures();
	// sprawdzenie i przygotowanie obs³ugi wybranych rozszerzeñ
	ExtensionSetup();
	// utworzenie list wyœwietlania
	GenerateDisplayLists();
	// wprowadzenie programu do obs³ugi pêtli komunikatów
	glutMainLoop();
	return 0;
}