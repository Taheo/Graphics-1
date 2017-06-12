// zajecia10.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h> // plik nag³ówkowy dla we/wy standardowego
#include <stdlib.h> // plik nag³ówkowy dla bibioteki standardowej
//wariant 1
#include <GL/glut.h>
#include <GL/glu.h>
#include "glext.h"
#ifndef WIN32
#define GLX_GLXEXT_LEGACY
#include <GL/glx.h>
#define wglGetProcAddress glXGetProcAddressARB
#endif

#include "colors.h"
#include "materials.h"

// wskaŸnik na funkcjê glWindowPos2i

PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;

// sta³e do obs³ugi menu podrêcznego

enum
{
	HOLE, // dziura

		  // materia³y
	BRASS, // mosi¹dz
	BRONZE, // br¹z
	POLISHED_BRONZE, // polerowany br¹z
	CHROME, // chrom
	COPPER, // miedŸ
	POLISHED_COPPER, // polerowana miedŸ
	GOLD, // z³oto
	POLISHED_GOLD, // polerowane z³oto
	PEWTER, // grafit (cyna z o³owiem)
	SILVER, // srebro
	POLISHED_SILVER, // polerowane srebro
	EMERALD, // szmaragd
	JADE, // jadeit
	OBSIDIAN, // obsydian
	PEARL, // per³a
	RUBY, // rubin
	TURQUOISE, // turkus
	BLACK_PLASTIC, // czarny plastik
	BLACK_RUBBER, // czarna guma

				  // obszar renderingu
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

// k¹ty obrotu obiektu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wskaŸnik naciœniêcia lewego przycisku myszki

int button_state = GLUT_UP;

// po³o¿enie kursora myszki

int button_x, button_y;

// wspó³czynnik skalowania

GLfloat scale = 1.0;

// w³aœciwoœci materia³u - domyœlnie mosi¹dz

const GLfloat * ambient = BrassAmbient;
const GLfloat * diffuse = BrassDiffuse;
const GLfloat * specular = BrassSpecular;
GLfloat shininess = BrassShininess;

// metoda podzia³u powierzchni NURBS na wielok¹ty

int sampling_method = GLU_PATH_LENGTH;

// sposób renderowania powierzchni NURBS

int display_mode = GLU_FILL;

// wspó³rzêdne punktów kontrolnych powierzchni

GLfloat points[5 * 5 * 3] =
{
	//ryosowanie jest od lewej do prawej
	-1.0,	-1.0,	0.0,
	-1.0,	-0.5,	0.0,
	-1.0,	0.0,	0.0,
	-1.0,	0.5,	0.0,
	-1.0,	1.0,	0.0,

	-0.5,	-1.0,	0.5,
	-0.5,	-0.5,	1,
	-0.5,	0.0,	1,
	-0.5,	0.5,	1,
	-0.5,	1.0,	0.5,

	0.0,	-1.0,	0.5,
	0.0,	-0.5,	1,
	0.0,	0.0,	1,
	0.0,	0.5,	1,
	0.0,	1.0,	0.5,

	0.5,	-1.0,	0.0,
	0.5,	-0.5,	0.0,
	0.5,	0.0,	0.0,
	0.5,	0.5,	0.0,
	0.5,	1.0,	0.0,

	2.0,	-1.0,	0.0,
	2.0,	-0.5,	0.0,
	2.0,	0.0,	0.0,
	2.0,	0.5,	0.0,
	2.0,	1.0,	0.0,

};
// wêz³y

GLfloat knots[11] =
{
	0,0,0,0,0,
	1,1,1,1,1,1
};
int i = 5;
// znacznik dostêpnoœci biblioteki GLU w wersji 1.3
bool GLU_1_3 = false;
// definicje metod podzia³u powierzchni NURBS na
// wielok¹ty wprowadzone w wersji 1.3 biblioteki GLU
#ifndef  GLU_OBJECT_PARAMETRIC_ERROR
#define GLU_OBJECT_PARAMETRIC_ERROR        100208
#endif
#ifndef GLU_OBJECT_PATH_LENGTH             100209
#define GLU_OBJECT_PATH_LENGTH             100209
#endif
// znacznik czy wycinaæ fragment powierzchni NURBS
bool hole = false;
// funkcja rysuj¹ca napis w wybranym miejscu
// (wersja korzystaj¹ca z funkcji glWindowPos2i)
void DrawString(GLint x, GLint y, char * string)
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
	// w³¹czenie testu bufora g³êbokoœci
	glEnable(GL_DEPTH_TEST);
	// przesuniêcie uk³adu wspó³rzêdnych obiektu do œrodka bry³y odcinania
	glTranslatef(0, 0, -(near + far) / 2);
	// obroty obiektu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);
	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);
	// w³¹czenie efektów oœwietlenia, gry renderowana jest wype³niona powierzchnia
	if (display_mode == GLU_FILL)
	{
		// w³¹czenie oœwietlenia
		glEnable(GL_LIGHTING);
		// w³¹czenie œwiat³a GL_LIGHT0 z parametrami domyœlnymi
		glEnable(GL_LIGHT0);
		// w³¹czenie automatycznej normalizacji wektorów normalnych
		glEnable(GL_NORMALIZE);
		// w³aœciwoœci materia³u
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		// w³¹czenie automatycznego generowania wektorów normalnych
		glEnable(GL_AUTO_NORMAL);
	}
	// kolor krawêdzi
	glColor3fv(Black);
	// utworzenie obiektu NURBS
	GLUnurbsObj * nurbs = gluNewNurbsRenderer();
	// pocz¹tek definicji powierzchni NURBS
	gluBeginSurface(nurbs);
	// sposób renderowania powierzchni NURBS
	gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, display_mode);
	// metoda podzia³u powierzchni NURBS na wielok¹ty
	gluNurbsProperty(nurbs, GLU_SAMPLING_METHOD, sampling_method);
	// narysowanie powierzchni
	gluNurbsSurface(nurbs, 10, knots, 10, knots, 5 * 3, 3, points, 5, 5, GL_MAP2_VERTEX_3);
	// rysowanie dziury w powierzchni NURBS

	// koniec definicji powierzchni
	gluEndSurface(nurbs);

	// usuniêcie obiektu NURBS
	gluDeleteNurbsRenderer(nurbs);

	// wy³¹czenie automatycznego generowania wektorów normalnych
	glDisable(GL_AUTO_NORMAL);

	// wy³¹czenie automatycznej normalizacji wektorów normalnych
	glDisable(GL_NORMALIZE);

	// wy³¹czenie œwiat³a GL_LIGHT0
	glDisable(GL_LIGHT0);

	// wy³aczenie oœwietlenia
	glDisable(GL_LIGHTING);

	// kolor punktów
	glColor3fv(Blue);

	// rozxmiar punktów
	glPointSize(6.0);

	// narysowanie punktów kontrolnych
	glBegin(GL_POINTS);
	for (int i = 0; i < 5 * 5; i++)
		glVertex3fv(points + i * 3);
	glEnd();
	// wyœwietlenie informacji o wybranych w³aœciwoœciach powierzchni NURBS
	glColor3fv(Black);

	// metoda podzia³u powierzchni NURBS na wielok¹ty
	if (sampling_method == GLU_PATH_LENGTH)
		DrawString(2, 2, "GLU_SAMPLING_METHOD = GLU_PATH_LENGTH");
	else
		if (sampling_method == GLU_PARAMETRIC_ERROR)
			DrawString(2, 2, "GLU_SAMPLING_METHOD = GLU_PARAMETRIC_ERROR");
		else
			if (sampling_method == GLU_DOMAIN_DISTANCE)
				DrawString(2, 2, "GLU_SAMPLING_METHOD = GLU_DOMAIN_DISTANCE");

	// metody podzia³u wprowadzone w wersji 1.3 biblioteki GLU
	if (GLU_1_3)
	{
		if (sampling_method == GLU_OBJECT_PARAMETRIC_ERROR)
			DrawString(2, 2, "GLU_SAMPLING_METHOD = GLU_OBJECT_PARAMETRIC_ERROR");
		else
			if (sampling_method == GLU_OBJECT_PATH_LENGTH)
				DrawString(2, 2, "GLU_SAMPLING_METHOD = GLU_OBJECT_PATH_LENGTH");

	}
	// sposób renderowania powierzchni NURBS
	if (display_mode == GLU_FILL)
		DrawString(2, 16, "GLU_DISPLAY_MODE = GLU_FILL");
	else
		if (display_mode == GLU_OUTLINE_PATCH)
			DrawString(2, 16, "GLU_DISPLAY_MODE = GLU_OUTLINE_PATCH");
		else
			if (display_mode == GLU_OUTLINE_POLYGON)
				DrawString(2, 16, "GLU_DISPLAY_MODE = GLU_OUTLINE_POLYGON");
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
		// wysokoœæ okna wiêksza od wysokoœci okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom * height / width, top * height / width, near, far);
		else

			// szerokoœæ okna wiêksza lub równa wysokoœci okna
			if (width >= height && height > 0)
				glFrustum(left * width / height, right * width / height, bottom, top, near, far);

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
	if (key == '+') {
		scale += 0.05;
		i++;
	}
	else {
		// klawisz -
		if (key == '-' && scale > 0.05)
		{
			i--;
			scale -= 0.05;
		}
	}
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
		// GLU_DISPLAY_MODE
	case GLU_FILL:
	case GLU_OUTLINE_PATCH:
	case GLU_OUTLINE_POLYGON:
		display_mode = value;
		DisplayScene();
		break;

		// GLU_SAMPLING_METHOD
	case GLU_PATH_LENGTH:
	case GLU_PARAMETRIC_ERROR:
	case GLU_DOMAIN_DISTANCE:
	case GLU_OBJECT_PARAMETRIC_ERROR:
	case GLU_OBJECT_PATH_LENGTH:
		sampling_method = value;
		DisplayScene();
		break;

		// dziura w³¹cz/wy³¹cz
	case HOLE:
		hole = !hole;
		DisplayScene();
		break;

		// materia³ - mosi¹dz
	case BRASS:
		ambient = BrassAmbient;
		diffuse = BrassDiffuse;
		specular = BrassSpecular;
		shininess = BrassShininess;
		DisplayScene();
		break;

		// materia³ - br¹z
	case BRONZE:
		ambient = BronzeAmbient;
		diffuse = BronzeDiffuse;
		specular = BronzeSpecular;
		shininess = BronzeShininess;
		DisplayScene();
		break;

		// materia³ - polerowany br¹z
	case POLISHED_BRONZE:
		ambient = PolishedBronzeAmbient;
		diffuse = PolishedBronzeDiffuse;
		specular = PolishedBronzeSpecular;
		shininess = PolishedBronzeShininess;
		DisplayScene();
		break;

		// materia³ - chrom
	case CHROME:
		ambient = ChromeAmbient;
		diffuse = ChromeDiffuse;
		specular = ChromeSpecular;
		shininess = ChromeShininess;
		DisplayScene();
		break;

		// materia³ - miedŸ
	case COPPER:
		ambient = CopperAmbient;
		diffuse = CopperDiffuse;
		specular = CopperSpecular;
		shininess = CopperShininess;
		DisplayScene();
		break;

		// materia³ - polerowana miedŸ
	case POLISHED_COPPER:
		ambient = PolishedCopperAmbient;
		diffuse = PolishedCopperDiffuse;
		specular = PolishedCopperSpecular;
		shininess = PolishedCopperShininess;
		DisplayScene();
		break;

		// materia³ - z³oto
	case GOLD:
		ambient = GoldAmbient;
		diffuse = GoldDiffuse;
		specular = GoldSpecular;
		shininess = GoldShininess;
		DisplayScene();
		break;

		// materia³ - polerowane z³oto
	case POLISHED_GOLD:
		ambient = PolishedGoldAmbient;
		diffuse = PolishedGoldDiffuse;
		specular = PolishedGoldSpecular;
		shininess = PolishedGoldShininess;
		DisplayScene();
		break;

		// materia³ - grafit (cyna z o³owiem)
	case PEWTER:
		ambient = PewterAmbient;
		diffuse = PewterDiffuse;
		specular = PewterSpecular;
		shininess = PewterShininess;
		DisplayScene();
		break;

		// materia³ - srebro
	case SILVER:
		ambient = SilverAmbient;
		diffuse = SilverDiffuse;
		specular = SilverSpecular;
		shininess = SilverShininess;
		DisplayScene();
		break;

		// materia³ - polerowane srebro
	case POLISHED_SILVER:
		ambient = PolishedSilverAmbient;
		diffuse = PolishedSilverDiffuse;
		specular = PolishedSilverSpecular;
		shininess = PolishedSilverShininess;
		DisplayScene();
		break;

		// materia³ - szmaragd
	case EMERALD:
		ambient = EmeraldAmbient;
		diffuse = EmeraldDiffuse;
		specular = EmeraldSpecular;
		shininess = EmeraldShininess;
		DisplayScene();
		break;

		// materia³ - jadeit
	case JADE:
		ambient = JadeAmbient;
		diffuse = JadeDiffuse;
		specular = JadeSpecular;
		shininess = JadeShininess;
		DisplayScene();
		break;

		// materia³ - obsydian
	case OBSIDIAN:
		ambient = ObsidianAmbient;
		diffuse = ObsidianDiffuse;
		specular = ObsidianSpecular;
		shininess = ObsidianShininess;
		DisplayScene();
		break;

		// materia³ - per³a
	case PEARL:
		ambient = PearlAmbient;
		diffuse = PearlDiffuse;
		specular = PearlSpecular;
		shininess = PearlShininess;
		DisplayScene();
		break;

		// metaria³ - rubin
	case RUBY:
		ambient = RubyAmbient;
		diffuse = RubyDiffuse;
		specular = RubySpecular;
		shininess = RubyShininess;
		DisplayScene();
		break;

		// materia³ - turkus
	case TURQUOISE:
		ambient = TurquoiseAmbient;
		diffuse = TurquoiseDiffuse;
		specular = TurquoiseSpecular;
		shininess = TurquoiseShininess;
		DisplayScene();
		break;

		// materia³ - czarny plastik
	case BLACK_PLASTIC:
		ambient = BlackPlasticAmbient;
		diffuse = BlackPlasticDiffuse;
		specular = BlackPlasticSpecular;
		shininess = BlackPlasticShininess;
		DisplayScene();
		break;

		// materia³ - czarna guma
	case BLACK_RUBBER:
		ambient = BlackRubberAmbient;
		diffuse = BlackRubberDiffuse;
		specular = BlackRubberSpecular;
		shininess = BlackRubberShininess;
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

// sprawdzenie i przygotowanie obs³ugi wybranych rozszerzeñ

void ExtensionSetup()
{
	// pobranie numeru wersji biblioteki OpenGL
	const char * version = (char *)glGetString(GL_VERSION);

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

// sprawdzenie numeru wersji biblioteki GLU

void GLUSetup()
{
	// pobranie numeru wersji biblioteki GLU
	const char * version = (char *)gluGetString(GLU_VERSION);

	// sprawdzenie numeru wersji biblioteki GLU
	int major = 0, minor = 0;
	if (sscanf(version, "%d.%d", &major, &minor) != 2)
	{
#ifdef WIN32
		printf("B³êdny format wersji GLU\n");
#else

		printf("Bledny format wersji GLU\n");
#endif

		exit(0);
	}

	// sprawdzenie czy jest co najmniej wersja 1.3 biblioteki GLU
	if (major > 1 || minor >= 3)
		GLU_1_3 = true;

}

int main(int argc, char * argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// rozmiary g³ównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie g³ównego okna programu
	glutCreateWindow("Powierzchnia NURBS");

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

	// utworzenie menu podrêcznego
	glutCreateMenu(Menu);

	// sprawdzenie numeru wersji biblioteki GLU
	GLUSetup();

	// utworzenie podmenu - GLU_DISPLAY_MODE
	int MenuDisplayMode = glutCreateMenu(Menu);
	glutAddMenuEntry("GLU_FILL", GLU_FILL);
	glutAddMenuEntry("GLU_OUTLINE_PATCH", GLU_OUTLINE_PATCH);
	glutAddMenuEntry("GLU_OUTLINE_POLYGON", GLU_OUTLINE_POLYGON);

	// utworzenie podmenu - GLU_SAMPLING_METHOD
	int MenuSamplingMethod = glutCreateMenu(Menu);
	glutAddMenuEntry("GLU_PATH_LENGTH", GLU_PATH_LENGTH);
	glutAddMenuEntry("GLU_PARAMETRIC_ERROR", GLU_PARAMETRIC_ERROR);
	glutAddMenuEntry("GLU_DOMAIN_DISTANCE", GLU_DOMAIN_DISTANCE);

	// elementy munu dostêpne w wersji 1.3 biblioteki GLU
	if (GLU_1_3)
	{
		glutAddMenuEntry("GLU_OBJECT_PARAMETRIC_ERROR", GLU_OBJECT_PARAMETRIC_ERROR);
		glutAddMenuEntry("GLU_OBJECT_PATH_LENGTH", GLU_OBJECT_PATH_LENGTH);
	}

	// utworzenie podmenu - Materia³
	int MenuMaterial = glutCreateMenu(Menu);
#ifdef WIN32

	glutAddMenuEntry("Mosiadz", BRASS);
	glutAddMenuEntry("Braz", BRONZE);
	glutAddMenuEntry("Polerowany braz", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("Miedz", COPPER);
	glutAddMenuEntry("Polerowana miedz", POLISHED_COPPER);
	glutAddMenuEntry("Zloto", GOLD);
	glutAddMenuEntry("Polerowane zloto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z olowiem)", PEWTER);
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
#else

	glutAddMenuEntry("Mosiadz", BRASS);
	glutAddMenuEntry("Braz", BRONZE);
	glutAddMenuEntry("Polerowany braz", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("Miedz", COPPER);
	glutAddMenuEntry("Polerowana miedz", POLISHED_COPPER);
	glutAddMenuEntry("Zloto", GOLD);
	glutAddMenuEntry("Polerowane zloto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z olowiem)", PEWTER);
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
	glutAddSubMenu("GLU_DISPLAY_MODE", MenuDisplayMode);
	glutAddSubMenu("GLU_SAMPLING_METHOD", MenuSamplingMethod);

#ifdef WIN32

	glutAddMenuEntry("Dziura wlacz/wylacz", HOLE);
	glutAddSubMenu("Material", MenuMaterial);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);
#else

	glutAddMenuEntry("Dziura wlacz/wylacz", HOLE);
	glutAddSubMenu("Material", MenuMaterial);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// okreœlenie przycisku myszki obs³uguj¹cej menu podrêczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// sprawdzenie i przygotowanie obs³ugi wybranych rozszerzeñ
	ExtensionSetup();

	// wprowadzenie programu do obs³ugi pêtli komunikatów
	glutMainLoop();
	return 0;
}