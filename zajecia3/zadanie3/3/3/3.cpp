#include "stdafx.h"
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
#include <GLFW/glfw3.h>

#define TARGA_HEADER_SIZE 0x12
#define TARGA_UNCOMP_RGB_IMG 0x02
#define TARGA_UNCOMP_BW_IMG 0x03
// wska?nik na funkcjê glWindowPos2i

PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;

// sta³e do obs³ugi menu podrêcznego

enum
{
	VENUS_TEX,      // Wenus
	EARTH_TEX,      // Ziemia
	MARS_TEX,       // Mars
	MERKURY_TEX,
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

// k¹ty obrotu

GLfloat rotatex = 270.0;
GLfloat rotatez = 0.0;

// wska?nik naciœniêcia lewego przycisku myszki

int button_state = GLUT_UP;

// po³o¿enie kursora myszki

int button_x, button_y;

// wspó³czynnik skalowania

GLfloat scale = 1.5;

// identyfikatory tekstur

GLuint VENUS, EARTH, MARS, MERKURY;

// identyfikator bie¿¹cej tekstury

GLuint texture;

// filtr pomniejszaj¹cy

GLint min_filter = GL_LINEAR_MIPMAP_LINEAR;

// funkcja rysuj¹ca napis w wybranym miejscu
// (wersja korzystaj¹ca z funkcji glWindowPos2i)

void DrawString(GLint x, GLint y, char *string)
{
	// po³o¿enie napisu
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
	glRotatef(rotatez, 0.0, 0.0, 1.0);

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

	// dowi¹zanie wybranej tekstury
	glBindTexture(GL_TEXTURE_2D, texture);

	// ustawienie parametów œrodowiska tekstur
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// utworzenie kwadryki
	GLUquadricObj *quadobj = gluNewQuadric();

	// styl (wygl¹d) generowanej kwadryki
	gluQuadricDrawStyle(quadobj, GLU_FILL);

	// sposób generacji wektorów normalnych
	gluQuadricNormals(quadobj, GLU_SMOOTH);

	// na³o¿enie tekstury na kwadrykê
	gluQuadricTexture(quadobj, GL_TRUE);

	// narysowanie kuli
	gluSphere(quadobj, 1.0, 30, 30);

	// usuniêcie kwadryki
	gluDeleteQuadric(quadobj);

	// wy³¹czenie teksturowania dwuwymiarowego
	glDisable(GL_TEXTURE_2D);

	// informacje o wybranych parametrach bie¿¹cej tekstury
	char string[200];
	GLfloat var;
	glColor3fv(Black);

	// wartoœæ priorytetu tekstury
	glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, &var);
	sprintf_s(string, "GL_TEXTURE_PRIORITY = %f", var);
	DrawString(2, 2, string);

	// czy tekstura jest rezydentna
	glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_RESIDENT, &var);
	if (var == GL_FALSE)
		strcpy_s(string, "GL_TEXTURE_RESIDENT = GL_FALSE");
	else
		strcpy_s(string, "GL_TEXTURE_RESIDENT = GL_TRUE");
	DrawString(2, 16, string);

	// szerokoœæ tekstury (poziom 0)
	glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &var);
	sprintf_s(string, "GL_TEXTURE_WIDTH = %f", var);
	DrawString(2, 30, string);

	// wysokoœæ tekstury (poziom 0)
	glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &var);
	sprintf_s(string, "GL_TEXTURE_HEIGHT = %f", var);
	DrawString(2, 46, string);

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
		rotatez += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

// obs³uga menu podrêcznego

void Menu(int value)
{
	switch (value)
	{
		// Wenus
	case VENUS_TEX:
		texture = VENUS;
		DisplayScene();
		break;

		// Ziemia
	case EARTH_TEX:
		texture = EARTH;
		DisplayScene();
		break;

		// Mars
	case MARS_TEX:
		texture = MARS;
		DisplayScene();
		break;
	case MERKURY_TEX:
		texture = MERKURY;
		DisplayScene();
		break;
		// filtr pomniejszaj¹cy
	case GL_NEAREST_MIPMAP_NEAREST:
	case GL_NEAREST_MIPMAP_LINEAR:
	case GL_LINEAR_MIPMAP_NEAREST:
	case GL_LINEAR_MIPMAP_LINEAR:
		min_filter = value;
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
// utworzenie tekstur
void GenerateTextures()
{
	//// zmienne u¿yte przy obs³udze plików TARGA
	GLsizei width, height;
	GLenum format, type;
	GLvoid *pixels;
	GLboolean error = true;


	//
	//// tryb upakowania bajtów danych tekstury
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	load_targa("mer0uu2.tga", width, height, format, type, pixels);

	// b³¹d odczytu pliku
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku ear0xuu2.tga");
		exit(0);
	}

	// utworzenie identyfikatora tekstury
	glGenTextures(1, &MERKURY);

	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, MERKURY);

	// utworzenie tekstury wraz z mipmapami
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, format, type, pixels);

	// porz¹dki
	delete[](unsigned char*)pixels;
	// identyfikator bie¿¹cej tekstury
	texture = MERKURY;
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
	glutCreateWindow("Tekstura 2D");

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

	// utworzenie podmenu - Planeta
	int MenuPlanet = glutCreateMenu(Menu);
	glutAddMenuEntry("Wenus", VENUS_TEX);
	glutAddMenuEntry("Ziemia", EARTH_TEX);
	glutAddMenuEntry("Mars", MARS_TEX);
	glutAddMenuEntry("Merkury", MERKURY_TEX);

	// utworzenie podmenu - Filtr pomniejszaj¹cy
	int MenuMinFilter = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR);

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
	glutAddSubMenu("Planeta", MenuPlanet);

#ifdef WIN32

	glutAddSubMenu("Filtr pomniejszaj¹cy", MenuMinFilter);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjœcie", EXIT);
#else

	glutAddSubMenu("Filtr pomniejszajacy", MenuMinFilter);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// okreœlenie przycisku myszki obs³uguj¹cego menu podrêczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// utworzenie tekstur
	GenerateTextures();

	// sprawdzenie i przygotowanie obs³ugi wybranych rozszerze?
	ExtensionSetup();

	// wprowadzenie programu do obs³ugi pêtli komunikatów
	glutMainLoop();
	return 0;
}