#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include "glext.h"
#ifndef WIN32
#define GLX_GLXEXT_LEGACY
#include <GL/glx.h>
#define wglGetProcAddress glXGetProcAddressARB
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "colors.h"
#include "materials.h"

// wskaŸnik na funkcj? glWindowPos2i

PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;

// sta³e do obs³ugi menu podr?cznego

enum
{
	// obiekty
	SPHERE,               // kula
	TEAPOT,               // czajnik
	CONE,                 // sto¿ek
	TORUS,                // torus
	CUBE,                 // szeœcian
	DODECAHEDRON,         // dwunastoœcian
	OCTAHEDRON,           // oœmioœcian
	TETRAHEDRON,          // czworoœcian
	ICOSAHEDRON,          // dwudziestoœcian

						  // materia³y
						  BRASS,                // mosi¹dz
						  BRONZE,               // br¹z
						  POLISHED_BRONZE,      // polerowany br¹z
						  CHROME,               // chrom
						  COPPER,               // miedŸ
						  POLISHED_COPPER,      // polerowana miedŸ
						  GOLD,                 // z³oto
						  POLISHED_GOLD,        // polerowane z³oto
						  PEWTER,               // grafit (cyna z o³owiem)
						  SILVER,               // srebro
						  POLISHED_SILVER,      // polerowane srebro
						  EMERALD,              // szmaragd
						  JADE,                 // jadeit
						  OBSIDIAN,             // obsydian
						  PEARL,                // per³a
						  RUBY,                 // rubin
						  TURQUOISE,            // turkus
						  BLACK_PLASTIC,        // czarny plastik
						  BLACK_RUBBER,         // czarna guma

												// obszar renderingu
												FULL_WINDOW,          // aspekt obrazu - ca³e okno
												ASPECT_1_1,           // aspekt obrazu 1:1
												EXIT                  // wyjœcie
};

// aspekt obrazu

int aspect = FULL_WINDOW;

// usuni?cie definicji makr near i far

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

// wskaŸnik naciœni?cia lewego przycisku myszki

int button_state = GLUT_UP;

// po³o¿enie kursora myszki

int button_x, button_y;

// wspó³czynnik skalowania

GLfloat scale = 1.0;

// w³aœciwoœci materia³u - domyœlnie mosi¹dz

const GLfloat *ambient = BrassAmbient;
const GLfloat *diffuse = BrassDiffuse;
const GLfloat *specular = BrassSpecular;
GLfloat shininess = BrassShininess;

// wyœwietlany obiekt 3D

int object = SPHERE;

// kierunek Ÿród³a œwiat³a

GLfloat light_position[4] =
{
	0.0,0.0,2.0,1.0
};

// k¹ty obrotu po³o¿enia Ÿród³a œwiat³a

GLfloat light_rotatex = 0.0;
GLfloat light_rotatey = 0.0;

// kierunek reflektora

GLfloat spot_direction[3] =
{
	0.0,0.0,-1.0
};

// k¹t odci?cia reflektora

GLfloat spot_cutoff = 180.0;

// wyk³adnik t³umienia k¹towego reflektora

GLfloat spot_exponent = 128.0;

// sta³y wspó³czynnik t³umienia œwiat³a

GLfloat constant_attenuation = 1.0;

// liniowy wspó³czynnik t³umienia œwiat³a

GLfloat linear_attenuation = 0.0;

// kwadratowy wspó³czynnik t³umienia œwiat³a

GLfloat quadratic_attenuation = 0.0;

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

// funkcja generuj¹ca scen? 3D

void DisplayScene()
{
	// kolor t³a - zawartoœ? bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora g³?bokoœci
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// w³¹czenie testu bufora g³?bokoœci
	glEnable(GL_DEPTH_TEST);

	// w³¹czenie oœwietlenia
	glEnable(GL_LIGHTING);

	// w³¹czenie œwiat³a GL_LIGHT0
	glEnable(GL_LIGHT0);

	// w³¹czenie automatycznej normalizacji wektorów normalnych
	glEnable(GL_NORMALIZE);

	// przesuni?cie uk³adu wspó³rz?dnych obiektu do œrodka bry³y odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	// w³aœciwoœci materia³u
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	// k¹t odci?cia reflektora
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);

	// wyk³adnik t³umienia k¹towego reflektora
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);

	// sta³y wspó³czynnik t³umienia œwiat³a,
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, constant_attenuation);

	// liniowy wspó³czynnik t³umienia œwiat³a
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, linear_attenuation);

	// kwadratowy wspó³czynnik t³umienia œwiat³a
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadratic_attenuation);

	// zmiana po³o¿enia Ÿród³a œwiat³a jest wykonywana niezale¿nie
	// od obrotów obiektu, st¹d od³o¿enie na stos macierzy modelowania
	glPushMatrix();

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesuni?cie uk³adu wspó³rz?dnych Ÿród³a œwiat³a do œrodka bry³y odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty po³o¿enia Ÿród³a œwiat³a - klawisze kursora
	glRotatef(light_rotatex, 1.0, 0, 0);
	glRotatef(light_rotatey, 0, 1.0, 0);

	// przesuni?cie Ÿród³a œwiat³a
	glTranslatef(light_position[0], light_position[1], light_position[2]);

	// ustalenie pozycji Ÿród³a œwiat³a
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// ustalenie kierunku reflektora
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);

	// od³o¿enie na stos zmiennych stanu zwi¹zanych z oœwietleniem sceny
	glPushAttrib(GL_LIGHTING_BIT);

	// wy³acznie Ÿród³a œwiat³a GL_LIGHT0
	glDisable(GL_LIGHT0);

	// materia³ imituj¹cy œwiecenie kuli na czerwono
	glMaterialfv(GL_FRONT, GL_EMISSION, Red);

	// narysowanie kuli po³o¿onej w pocz¹tku Ÿród³a œwiat³a
	glutSolidSphere(0.1, 30, 20);

	// przywrócenie zmiennych stanu zwi¹zanych z oœwietleniem sceny
	glPopAttrib();

	// przywrócenie pierwotnej macierzy modelowania
	glPopMatrix();

	// rysowanie wybranego obiektu 3D
	switch (object)
	{
		// kula
	case SPHERE:
		glutSolidSphere(1.0, 50, 40);
		break;

		// czajnik
	case TEAPOT:
		glutSolidTeapot(1);
		break;

		// sto¿ek
	case CONE:
		glutSolidCone(1, 1, 50, 40);
		break;

		// torus
	case TORUS:
		glutSolidTorus(0.3, 1, 40, 50);
		break;

		// szeœcian
	case CUBE:
		glutSolidCube(1);
		break;

		// dwunastoœcian
	case DODECAHEDRON:
		glutSolidDodecahedron();
		break;

		// oœmioœcian
	case OCTAHEDRON:
		glutSolidOctahedron();
		break;

		// czworoœcian
	case TETRAHEDRON:
		glutSolidTetrahedron();
		break;

		// dwudziestoœcian
	case ICOSAHEDRON:
		glutSolidIcosahedron();
		break;
	}

	// informacje o wartoœciach modyfikowanych
	// parametrów Ÿród³a œwiata³a GL_LIGHT0
	char string[200];
	GLfloat vec[4];
	glColor3fv(Black);

	// kierunek Ÿród³a œwiat³a
	glGetLightfv(GL_LIGHT0, GL_POSITION, vec);
	sprintf(string, "GL_POSITION = (%f,%f,%f,%f)", vec[0], vec[1], vec[2], vec[3]);
	DrawString(2, 2, string);

	glGetLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, vec);
	sprintf(string, "GL_SPOT_DIRECTION = (%f,%f,%f)", vec[0], vec[1], vec[2]);
	DrawString(2, 16, string);

	// k¹t odci?cia reflektora
	glGetLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, vec);
	sprintf(string, "GL_SPOT_CUTOFF = %f", vec[0]);
	DrawString(2, 30, string);

	// wyk³adnik t³umienia k¹towego reflektora
	glGetLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, vec);
	sprintf(string, "GL_SPOT_EXPONENT = %f", vec[0]);
	DrawString(2, 44, string);

	// sta³y wspó³czynnik t³umienia œwiat³a,
	glGetLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, vec);
	sprintf(string, "GL_CONSTANT_ATTENUATION = %f", vec[0]);
	DrawString(2, 58, string);

	// liniowy wspó³czynnik t³umienia œwiat³a
	glGetLightfv(GL_LIGHT0, GL_LINEAR_ATTENUATION, vec);
	sprintf(string, "GL_LINEAR_ATTENUATION = %f", vec[0]);
	DrawString(2, 72, string);

	// kwadratowy wspó³czynnik t³umienia œwiat³a
	glGetLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, vec);
	sprintf(string, "GL_QUADRATIC_ATTENUATION = %f", vec[0]);
	DrawString(2, 86, string);

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
		// wysokoœ? okna wi?ksza od wysokoœci okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else

			// szerokoœ? okna wi?ksza lub równa wysokoœci okna
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
	switch (key)
	{
		// klawisz "+" - powi?kszenie obiektu
	case '+':
		scale += 0.05;
		break;

		// klawisz "-" - zmniejszenie obiektu
	case '-':
		if (scale > 0.05)
			scale -= 0.05;
		break;

		// klawisz "S" - zwi?kszenie k¹ta obci?cia reflektora
	case 'S':
		if (spot_cutoff == 90)
			spot_cutoff = 180;
		else
			if (spot_cutoff < 90)
				spot_cutoff++;
		break;

		// klawisz "s" - zmniejszenie k¹ta obci?cia reflektora
	case 's':
		if (spot_cutoff == 180)
			spot_cutoff = 90;
		else
			if (spot_cutoff > 0)
				spot_cutoff--;
		break;

		// klawisz "E" - zwi?kszenie wyk³adnika t³umienia k¹towego reflektora
	case 'E':
		if (spot_exponent < 128)
			spot_exponent++;
		break;

		// klawisz "e" - zmniejszenie wyk³adnika t³umienia k¹towego reflektora
	case 'e':
		if (spot_exponent > 0)
			spot_exponent--;
		break;

		// klawisz "C" - zwi?kszenie sta³ego wspó³czynnika t³umienia œwiat³a
	case 'C':
		constant_attenuation += 0.1;
		break;

		// klawisz "c" - zmniejszenie sta³ego wspó³czynnika t³umienia œwiat³a
	case 'c':
		if (constant_attenuation > 0)
			constant_attenuation -= 0.1;
		break;

		// klawisz "L" - zwi?kszenie liniowego wspó³czynnika t³umienia œwiat³a
	case 'L':
		linear_attenuation += 0.1;
		break;

		// klawisz "l" - zmniejszenie liniowego wspó³czynnika t³umienia œwiat³a
	case 'l':
		if (linear_attenuation > 0)
			linear_attenuation -= 0.1;
		break;

		// klawisz "Q" - zwi?kszenie kwadratowego wspó³czynnika t³umienia œwiat³a
	case 'Q':
		quadratic_attenuation += 0.1;
		break;

		// klawisz "q" - zmniejszenie kwadratowego wspó³czynnika t³umienia œwiat³a
	case 'q':
		if (quadratic_attenuation > 0)
			quadratic_attenuation -= 0.1;
		break;
	}

	// narysowanie sceny
	DisplayScene();
}

// obs³uga przycisków myszki

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapami?tanie stanu lewego przycisku myszki
		button_state = state;

		// zapami?tanie po³o¿enia kursora myszki
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

// obs³uga klawiszy funkcyjnych i klawiszy kursora

void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
		// kursor w lewo
	case GLUT_KEY_LEFT:
		light_rotatey -= 5;
		break;

		// kursor w prawo
	case GLUT_KEY_RIGHT:
		light_rotatey += 5;
		break;

		// kursor w dó³
	case GLUT_KEY_DOWN:
		light_rotatex += 5;
		break;

		// kursor w gór?
	case GLUT_KEY_UP:
		light_rotatex -= 5;
		break;
	}

	// odrysowanie okna
	DisplayScene();
}

// obs³uga menu podr?cznego

void Menu(int value)
{
	switch (value)
	{
		// rysowany obiekt - kula
	case SPHERE:
		object = SPHERE;
		DisplayScene();
		break;

		// rysowany obiekt - czajnik
	case TEAPOT:
		object = TEAPOT;
		DisplayScene();
		break;

		// rysowany obiekt - sto¿ek
	case CONE:
		object = CONE;
		DisplayScene();
		break;

		// rysowany obiekt - torus
	case TORUS:
		object = TORUS;
		DisplayScene();
		break;

		// rysowany obiekt - szeœcian
	case CUBE:
		object = CUBE;
		DisplayScene();
		break;

		// rysowany obiekt - dwunastoœcian
	case DODECAHEDRON:
		object = DODECAHEDRON;
		DisplayScene();
		break;

		// rysowany obiekt - oœmioœcian
	case OCTAHEDRON:
		object = OCTAHEDRON;
		DisplayScene();
		break;

		// rysowany obiekt - czworoœcian
	case TETRAHEDRON:
		object = TETRAHEDRON;
		DisplayScene();
		break;

		// rysowany obiekt - dwudziestoœcian
	case ICOSAHEDRON:
		object = ICOSAHEDRON;
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

// sprawdzenie i przygotowanie obs³ugi wybranych rozszerze?

void ExtensionSetup()
{
	// pobranie numeru wersji biblioteki OpenGL
	const char *version = (char*)glGetString(GL_VERSION);

	// odczyt wersji OpenGL
	int major = 0, minor = 0;
	if (sscanf(version, "%d.%d", &major, &minor) != 2)
	{
#ifndef WIN32
		printf("B³?dny format wersji OpenGL\n");
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

int main(int argc, char *argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// rozmiary g³ównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie g³ównego okna programu
	glutCreateWindow("Reflektor");

	// do³¹czenie funkcji generuj¹cej scen? 3D
	glutDisplayFunc(DisplayScene);

	// do³¹czenie funkcji wywo³ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// do³¹czenie funkcji obs³ugi klawiatury
	glutKeyboardFunc(Keyboard);

	// do³¹czenie funkcji obs³ugi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);

	// obs³uga przycisków myszki
	glutMouseFunc(MouseButton);

	// obs³uga ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie menu podr?cznego
	glutCreateMenu(Menu);

	// utworzenie podmenu - obiekt
	int MenuObject = glutCreateMenu(Menu);
	glutAddMenuEntry("Kula", SPHERE);
	glutAddMenuEntry("Czajnik", TEAPOT);

#ifdef WIN32

	glutAddMenuEntry("Sto¿ek", CONE);
	glutAddMenuEntry("Torus", TORUS);
	glutAddMenuEntry("Szeœcian", CUBE);
	glutAddMenuEntry("Dwunastoœcian", DODECAHEDRON);
	glutAddMenuEntry("Oœmioœcian", OCTAHEDRON);
	glutAddMenuEntry("Czworoœcian", TETRAHEDRON);
	glutAddMenuEntry("Dwudziestoœcian", ICOSAHEDRON);
#else

	glutAddMenuEntry("Stozek", CONE);
	glutAddMenuEntry("Torus", TORUS);
	glutAddMenuEntry("Szescian", CUBE);
	glutAddMenuEntry("Dwunastoscian", DODECAHEDRON);
	glutAddMenuEntry("Osmioscian", OCTAHEDRON);
	glutAddMenuEntry("Czworoscian", TETRAHEDRON);
	glutAddMenuEntry("Dwudziestoscian", ICOSAHEDRON);
#endif

	// utworzenie podmenu - Materia³
	int MenuMaterial = glutCreateMenu(Menu);
#ifdef WIN32

	glutAddMenuEntry("Mosi¹dz", BRASS);
	glutAddMenuEntry("Br¹z", BRONZE);
	glutAddMenuEntry("Polerowany br¹z", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("MiedŸ", COPPER);
	glutAddMenuEntry("Polerowana miedŸ", POLISHED_COPPER);
	glutAddMenuEntry("Z³oto", GOLD);
	glutAddMenuEntry("Polerowane z³oto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z o³owiem)", PEWTER);
	glutAddMenuEntry("Srebro", SILVER);
	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);
	glutAddMenuEntry("Szmaragd", EMERALD);
	glutAddMenuEntry("Jadeit", JADE);
	glutAddMenuEntry("Obsydian", OBSIDIAN);
	glutAddMenuEntry("Per³a", PEARL);
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
	glutAddMenuEntry("Grafit (cyna z o³owiem)", PEWTER);
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
	glutAddSubMenu("Obiekt", MenuObject);

#ifdef WIN32

	glutAddSubMenu("Materia³", MenuMaterial);
#else

	glutAddSubMenu("Material", MenuMaterial);
#endif

	glutAddSubMenu("Aspekt obrazu", MenuAspect);
#ifdef WIN32

	glutAddMenuEntry("Wyjœcie", EXIT);
#else

	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// okreœlenie przycisku myszki obs³uguj¹cej menu podr?czne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// sprawdzenie i przygotowanie obs³ugi wybranych rozszerze?
	ExtensionSetup();

	// wprowadzenie programu do obs³ugi p?tli komunikatów
	glutMainLoop();
	return 0;
}

