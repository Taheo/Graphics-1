/*
(c) Janusz Ganczarski
http://www.januszg.hg.pl
JanuszG@enter.net.pl
*/
#include "stdafx.h"

#include <GL/glut.h>
#include <GL/glu.h>
#include <stdlib.h>
#include "colors.h"
#include "materials.h"

// stałe do obsługi menu podręcznego

enum
{
	// materiały
	BRASS, // mosiądz
	BRONZE, // brąz
	POLISHED_BRONZE, // polerowany brąz
	CHROME, // chrom
	COPPER, // miedź
	POLISHED_COPPER, // polerowana miedź
	GOLD, // złoto
	POLISHED_GOLD, // polerowane złoto
	PEWTER, // grafit (cyna z ołowiem)
	SILVER, // srebro
	POLISHED_SILVER, // polerowane srebro
	EMERALD, // szmaragd
	JADE, // jadeit
	OBSIDIAN, // obsydian
	PEARL, // perła
	RUBY, // rubin
	TURQUOISE, // turkus
	BLACK_PLASTIC, // czarny plastik
	BLACK_RUBBER, // czarna guma

				  // obszar renderingu
	FULL_WINDOW, // aspekt obrazu - całe okno
	ASPECT_1_1, // aspekt obrazu 1:1
	EXIT // wyjście
};

// aspekt obrazu

int aspect = FULL_WINDOW;

// rozmiary bryły obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near_ = 3.0;
const GLdouble far_ = 7.0;

// kąty obrotu obiektu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wskaźnik naciśnięcia lewego przycisku myszki

int button_state = GLUT_UP;

// położenie kursora myszki

int button_x, button_y;

// współczynnik skalowania

GLfloat scale = 1.0;

// właściwości materiału - domyślnie mosiądz

const GLfloat * ambient = BrassAmbient;
const GLfloat * diffuse = BrassDiffuse;
const GLfloat * specular = BrassSpecular;
GLfloat shininess = BrassShininess;

// styl

int style = GL_FILL;

// współrzędne punktów kontrolnych powierzchni

GLfloat points[12 * 3] =
{
	-1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0,
	-1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0,
	-1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0
};

// współrzędne tekstury nałożonej na powierzchnię

GLfloat texture_coord[4 * 2] =
{
	0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0
};



GLubyte polish_flag[2 * 2 * 3] =
{
	0xD9, 0x1E, 0x41, 0xD9, 0x1E, 0x41,
	0xF2, 0xE6, 0xEF, 0xF2, 0xE6, 0xEF
};

// funkcja generująca scenę 3D

void Display()
{
	// kolor tła - zawartość bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// włączenie testu bufora głębokości
	glEnable(GL_DEPTH_TEST);

	// przesunięcie układu współrzędnych obiektu do środka bryły odcinania
	glTranslatef(0, 0, -(near_ + far_) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	// włączenie generowana współrzędnych (x,y,z) punktów powierzchni
	glEnable(GL_MAP2_VERTEX_3);

	// ewaluator dwuwymiarowy dla dwunastu punktów kontronych powierzchni
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 3, points);

	// utworzenie mapy (siatki) punktów reprezentujących powierzchnię
	glMapGrid2f(25, 0.0, 1.0, 25, 0.0, 1.0);

	// styl rysowania powierzchni - punkty
	if (style == GL_POINT)
	{
		// kolor punktów
		glColor3fv(Black);

		// rozmiar punktów
		glPointSize(2.0);

		// narysowanie siatki (mapy) powierzchni
		glEvalMesh2(GL_POINT, 0, 25, 0, 25);
	}

	// styl rysowania powierzchni - linie
	if (style == GL_LINE)
	{
		// kolor linii
		glColor3fv(Black);

		// szerokość linii
		glLineWidth(2.0);

		// narysowanie siatki (mapy) powierzchni
		glEvalMesh2(GL_LINE, 0, 25, 0, 25);
	}

	// styl rysowania powierzchni - wypełnienie
	if (style == GL_FILL)
	{
		// włączenie oświetlenia
		glEnable(GL_LIGHTING);

		// włączenie światła GL_LIGHT0 z parametrami domyślnymi
		glEnable(GL_LIGHT0);

		// włączenie automatycznej normalizacji wektorów normalnych
		glEnable(GL_NORMALIZE);

		// właściwości materiału
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

		// włączenie automatycznego generowania wektorów normalnych
		glEnable(GL_AUTO_NORMAL);

		// narysowanie siatki (mapy) powierzchni
		glEvalMesh2(GL_FILL, 0, 25, 0, 25);

		// wyłączenie automatycznego generowania wektorów normalnych
		glDisable(GL_AUTO_NORMAL);

		// wyłączenie automatycznej normalizacji wektorów normalnych
		glDisable(GL_NORMALIZE);

		// wyłaczenie oświetlenia
		glDisable(GL_LIGHTING);
	}

	// styl rysowania powierzchni - wypełnienie
	if (style == GL_TEXTURE)
	{
		// tryb upakowania bajtów danych tekstury
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// filtr powiększający
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// filtr pomniejszający
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// parametry środowiska tesktur
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		// tekstura dwuwymiarowa
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, polish_flag);

		// włączenie teksturowania dwuwymiarowego
		glEnable(GL_TEXTURE_2D);

		// włączenie generowana współrzędnych (s,t) tekstury
		glEnable(GL_MAP2_TEXTURE_COORD_2);

		// ewaluator dwuwymiarowy dla czterech punktów kontronych powierzchni
		glMap2f(GL_MAP2_TEXTURE_COORD_2, 0.0, 1.0, 2, 2, 0.0, 1.0, 4, 2, texture_coord);

		// utworzenie mapy (siatki) punktów reprezentujących powierzchnię
		glMapGrid2f(25, 0.0, 1.0, 25, 0.0, 1.0);

		// narysowanie siatki (mapy) powierzchni
		glEvalMesh2(GL_FILL, 0, 25, 0, 25);

		// wyłączenie generowana współrzędnych (s,t) tekstury
		glDisable(GL_MAP2_TEXTURE_COORD_2);

		// wyłączenie teksturowania dwuwymiarowego
		glDisable(GL_TEXTURE_2D);
	}

	// kolor punktów
	glColor3fv(Blue);

	// rozxmiar punktów
	glPointSize(6.0);

	// narysowanie punktów kontrolnych
	glBegin(GL_POINTS);
	for (int i = 0; i < 12; i++)
		glVertex3fv(points + i * 3);

	glEnd();

	// skierowanie poleceń do wykonania
	glFlush();

	// zamiana buforów koloru
	glutSwapBuffers();
}

// zmiana wielkości okna

void Reshape(int width, int height)
{
	// obszar renderingu - całe okno
	glViewport(0, 0, width, height);

	// wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bryły obcinania
	if (aspect == ASPECT_1_1)
	{
		// wysokość okna większa od wysokości okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom * height / width, top * height / width, near_, far_);
		else

			// szerokość okna większa lub równa wysokości okna
			if (width >= height && height > 0)
				glFrustum(left * width / height, right * width / height, bottom, top, near_, far_);

	}
	else
		glFrustum(left, right, bottom, top, near_, far_);

	// generowanie sceny 3D
	Display();
}

// obsługa klawiatury

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

// obsługa przycisków myszki

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiętanie stanu lewego przycisku myszki
		button_state = state;

		// zapamiętanie położenia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

// obsługa ruchu kursora myszki

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

// obsługa menu podręcznego

void Menu(int value)
{
	switch (value)
	{
		// styl
	case GL_POINT:
	case GL_LINE:
	case GL_FILL:
	case GL_TEXTURE:
		style = value;
		Display();
		break;

		// materiał - mosiądz
	case BRASS:
		ambient = BrassAmbient;
		diffuse = BrassDiffuse;
		specular = BrassSpecular;
		shininess = BrassShininess;
		Display();
		break;

		// materiał - brąz
	case BRONZE:
		ambient = BronzeAmbient;
		diffuse = BronzeDiffuse;
		specular = BronzeSpecular;
		shininess = BronzeShininess;
		Display();
		break;

		// materiał - polerowany brąz
	case POLISHED_BRONZE:
		ambient = PolishedBronzeAmbient;
		diffuse = PolishedBronzeDiffuse;
		specular = PolishedBronzeSpecular;
		shininess = PolishedBronzeShininess;
		Display();
		break;

		// materiał - chrom
	case CHROME:
		ambient = ChromeAmbient;
		diffuse = ChromeDiffuse;
		specular = ChromeSpecular;
		shininess = ChromeShininess;
		Display();
		break;

		// materiał - miedź
	case COPPER:
		ambient = CopperAmbient;
		diffuse = CopperDiffuse;
		specular = CopperSpecular;
		shininess = CopperShininess;
		Display();
		break;

		// materiał - polerowana miedź
	case POLISHED_COPPER:
		ambient = PolishedCopperAmbient;
		diffuse = PolishedCopperDiffuse;
		specular = PolishedCopperSpecular;
		shininess = PolishedCopperShininess;
		Display();
		break;

		// materiał - złoto
	case GOLD:
		ambient = GoldAmbient;
		diffuse = GoldDiffuse;
		specular = GoldSpecular;
		shininess = GoldShininess;
		Display();
		break;

		// materiał - polerowane złoto
	case POLISHED_GOLD:
		ambient = PolishedGoldAmbient;
		diffuse = PolishedGoldDiffuse;
		specular = PolishedGoldSpecular;
		shininess = PolishedGoldShininess;
		Display();
		break;

		// materiał - grafit (cyna z ołowiem)
	case PEWTER:
		ambient = PewterAmbient;
		diffuse = PewterDiffuse;
		specular = PewterSpecular;
		shininess = PewterShininess;
		Display();
		break;

		// materiał - srebro
	case SILVER:
		ambient = SilverAmbient;
		diffuse = SilverDiffuse;
		specular = SilverSpecular;
		shininess = SilverShininess;
		Display();
		break;

		// materiał - polerowane srebro
	case POLISHED_SILVER:
		ambient = PolishedSilverAmbient;
		diffuse = PolishedSilverDiffuse;
		specular = PolishedSilverSpecular;
		shininess = PolishedSilverShininess;
		Display();
		break;

		// materiał - szmaragd
	case EMERALD:
		ambient = EmeraldAmbient;
		diffuse = EmeraldDiffuse;
		specular = EmeraldSpecular;
		shininess = EmeraldShininess;
		Display();
		break;

		// materiał - jadeit
	case JADE:
		ambient = JadeAmbient;
		diffuse = JadeDiffuse;
		specular = JadeSpecular;
		shininess = JadeShininess;
		Display();
		break;

		// materiał - obsydian
	case OBSIDIAN:
		ambient = ObsidianAmbient;
		diffuse = ObsidianDiffuse;
		specular = ObsidianSpecular;
		shininess = ObsidianShininess;
		Display();
		break;

		// materiał - perła
	case PEARL:
		ambient = PearlAmbient;
		diffuse = PearlDiffuse;
		specular = PearlSpecular;
		shininess = PearlShininess;
		Display();
		break;

		// metariał - rubin
	case RUBY:
		ambient = RubyAmbient;
		diffuse = RubyDiffuse;
		specular = RubySpecular;
		shininess = RubyShininess;
		Display();
		break;

		// materiał - turkus
	case TURQUOISE:
		ambient = TurquoiseAmbient;
		diffuse = TurquoiseDiffuse;
		specular = TurquoiseSpecular;
		shininess = TurquoiseShininess;
		Display();
		break;

		// materiał - czarny plastik
	case BLACK_PLASTIC:
		ambient = BlackPlasticAmbient;
		diffuse = BlackPlasticDiffuse;
		specular = BlackPlasticSpecular;
		shininess = BlackPlasticShininess;
		Display();
		break;

		// materiał - czarna guma
	case BLACK_RUBBER:
		ambient = BlackRubberAmbient;
		diffuse = BlackRubberDiffuse;
		specular = BlackRubberSpecular;
		shininess = BlackRubberShininess;
		Display();
		break;

		// obszar renderingu - całe okno
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyjście
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

	// rozmiary głównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie głównego okna programu
#ifdef WIN32

	glutCreateWindow("Powierzchnia Béziera");
#else

	glutCreateWindow("Powierzchnia Beziera");
#endif

	// dołączenie funkcji generującej scenę 3D
	glutDisplayFunc(Display);

	// dołączenie funkcji wywoływanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// dołączenie funkcji obsługi klawiatury
	glutKeyboardFunc(Keyboard);

	// obsługa przycisków myszki
	glutMouseFunc(MouseButton);

	// obsługa ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie menu podręcznego
	glutCreateMenu(Menu);

	// utworzenie podmenu - Styl
	int MenuStyle = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_POINT", GL_POINT);
	glutAddMenuEntry("GL_LINE", GL_LINE);
	glutAddMenuEntry("GL_FILL", GL_FILL);
	glutAddMenuEntry("GL_FILL + tekstura", GL_TEXTURE);

	// utworzenie podmenu - Materiał
	int MenuMaterial = glutCreateMenu(Menu);
#ifdef WIN32

	glutAddMenuEntry("Mosiądz", BRASS);
	glutAddMenuEntry("Brąz", BRONZE);
	glutAddMenuEntry("Polerowany brąz", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("Miedź", COPPER);
	glutAddMenuEntry("Polerowana miedź", POLISHED_COPPER);
	glutAddMenuEntry("Złoto", GOLD);
	glutAddMenuEntry("Polerowane złoto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z ołowiem)", PEWTER);
	glutAddMenuEntry("Srebro", SILVER);
	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);
	glutAddMenuEntry("Szmaragd", EMERALD);
	glutAddMenuEntry("Jadeit", JADE);
	glutAddMenuEntry("Obsydian", OBSIDIAN);
	glutAddMenuEntry("Perła", PEARL);
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
	glutAddMenuEntry("Grafit (cyna z ołowiem)", PEWTER);
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

	glutAddMenuEntry("Aspekt obrazu - całe okno", FULL_WINDOW);
#else

	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
#endif

	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu główne
	glutCreateMenu(Menu);
	glutAddSubMenu("Styl", MenuStyle);

#ifdef WIN32

	glutAddSubMenu("Materiał", MenuMaterial);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjście", EXIT);
#else

	glutAddSubMenu("Material", MenuMaterial);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// określenie przycisku myszki obsługującej menu podręczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// wprowadzenie programu do obsługi pętli komunikatów
	glutMainLoop();
	return 0;
}