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
PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;
// sta³e do obs³ugi menu podrêcznego
//driu
enum
{
	TEXTURE_COMPRESSION_FASTEST, // kompresja tekstur - GL_FASTEST
	TEXTURE_COMPRESSION_DONT_CARE, // kompresja tekstur - GL_DONT_CARE
	TEXTURE_COMPRESSION_NICEST, // kompresja tekstur - GL_NICEST
	TEXTURE_LENA, // tekstura lena
	TEXTURE_LENA_UNC, // teksura lena nieskompresowana
	TEXTURE_LENA_GRAY, // tekstura lena_gray
	TEXTURE_LENA_GRAY_UNC, // tekstura lena_gray nieskompresowana
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
// identyfikatory tekstur
GLuint LENA, LENA_UNC, LENA_GRAY, LENA_GRAY_UNC;
// identyfikator bie¿¹cej tekstury;
GLuint texture;
// wskazówki do kompresji tekstur
GLint texture_compression_hint = GL_DONT_CARE;
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
	// czyszczenie bufora koloru
	glClear(GL_COLOR_BUFFER_BIT);
	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);
	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();
	// przesuniêcie uk³adu wspó³rzêdnych obiektów do œrodka bry³y odcinania
	glTranslatef(0.0, 0.0, -(near + far) / 2);
	// przesuniêcie obiektu z tekstur¹ do góry okna
	glTranslatef(0.0, 1.0, 0.0);
	// w³¹czenie teksturowania dwuwymiarowego
	glEnable(GL_TEXTURE_2D);
	// ustawienie parametów œrodowiska tekstur
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, texture);
	// filtr powi?kszaj¹cy
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// filtr pomniejszaj¹cy
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// narysowanie kwadratu z tekstur¹
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-1.5, -1.5);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(-1.5, 1.5);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(1.5, 1.5);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(1.5, -1.5);
	glEnd();
	// wy³¹czenie teksturowania dwuwymiarowego
	glDisable(GL_TEXTURE_2D);
	// wyœwietlenie wybranych informacji
	char string[200];
	GLint var;
	glColor3fv(Black);
	// informacja czy tekstura jest skompresowana
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &var);
	if (var == GL_FALSE)
		sprintf(string, "GL_TEXTURE_COMPRESSED = GL_FALSE");
	else
		sprintf(string, "GL_TEXTURE_COMPRESSED = GL_TRUE");
	DrawString(2, 2, string);
	// rozmiar danych tekstury
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &var);
	sprintf(string, "GL_TEXTURE_COMPRESSED_IMAGE_SIZE = %i", var);
	DrawString(2, 16, string);
	// wewnêtrzny format tekstury
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &var);
	switch (var)
	{
		// formaty rozszerzenia EXT_texture_compression_s3tc
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_COMPRESSED_RGB_S3TC_DXT1_EXT");
		break;
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT");
		break;
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT");
		break;
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT");
		break;
		// formaty rozszerzenia 3DFX_texture_compression_FXT1
	case GL_COMPRESSED_RGB_FXT1_3DFX:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_COMPRESSED_RGB_FXT1_3DFX");
		break;
	case GL_COMPRESSED_RGBA_FXT1_3DFX:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_COMPRESSED_RGBA_FXT1_3DFX");
		break;
		// format rozszerzenia ATI_texture_compression_3dc (nie wystêpuje w pliku glext.h)
	case 0x8837:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_COMPRESSED_RGB_3DC_ATI");
		break;
		// wybrane formaty nieskompresowane
	case GL_RED:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_RED");
		break;
	case GL_GREEN:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_GREEN");
		break;
	case GL_BLUE:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_BLUE");
		break;
	case GL_ALPHA:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_ALPHA");
		break;
	case GL_RGB:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_RGB");
		break;
	case GL_RGBA:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_RGBA");
		break;
	case GL_LUMINANCE:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_LUMINANCE");
		break;
	case GL_LUMINANCE_ALPHA:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = GL_LUMINANCE_ALPHA");
		break;
		// pozosta³e formaty
	default:
		sprintf(string, "GL_TEXTURE_INTERNAL_FORMAT = nieznany");
		break;
	}
	DrawString(2, 30, string);
	// informacja o wskazówkach do kompresji tekstur
	glGetIntegerv(GL_TEXTURE_COMPRESSION_HINT, &var);
	switch (var)
	{
	case GL_FASTEST:
		sprintf(string, "GL_TEXTURE_COMPRESSION_HINT = GL_FASTEST");
		break;
	case GL_DONT_CARE:
		sprintf(string, "GL_TEXTURE_COMPRESSION_HINT = GL_DONT_CARE");
		break;
	case GL_NICEST:
		sprintf(string, "GL_TEXTURE_COMPRESSION_HINT = GL_NICEST");
		break;
	}
	DrawString(2, 44, string);
	// iloœæ obs³ugiwanych formatów kompresji tekstur
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &var);
	sprintf(string, "GL_NUM_COMPRESSED_TEXTURE_FORMATS = %i", var);
	// wykaz obs³ugiwanych formatów kompresji tekstur
	GLint formats[256];
	glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, formats);
	for (int i = 0; i < var; i++)
	{
		switch (formats[i])
		{
			// formaty rozszerzenia EXT_texture_compression_s3tc
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			sprintf(string, "GL_COMPRESSED_RGB_S3TC_DXT1_EXT");
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			sprintf(string, "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT");
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			sprintf(string, "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT");
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			sprintf(string, "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT");
			break;
			// formaty rozszerzenia 3DFX_texture_compression_FXT1
		case GL_COMPRESSED_RGB_FXT1_3DFX:
			sprintf(string, "GL_COMPRESSED_RGB_FXT1_3DFX");
			break;
		case GL_COMPRESSED_RGBA_FXT1_3DFX:
			sprintf(string, "GL_COMPRESSED_RGBA_FXT1_3DFX");
			break;
			// format rozszerzenia ATI_texture_compression_3dc
			// (nie wystêpuje w pliku glext.h)
		case 0x8837:
			sprintf(string, "GL_COMPRESSED_RGB_3DC_ATI");
			break;
			// pozostra³e formaty
		default:
			sprintf(string, "Format nieznany (0x%X)", formats[i]);
			break;
		}
		DrawString(2, 70 + 14 * i, string);
	}
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
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else
			// szerokoœæ okna wiêksza lub równa wysokoœci okna
			if (width >= height && height > 0)
				glFrustum(left*width / height, right*width / height, bottom, top, near,
					far);
	}
	else
		glFrustum(left, right, bottom, top, near, far);
	// generowanie sceny 3D
	DisplayScene();
}
// utworzenie list wyœwietlania
void GenerateTextures()
{
	// zmienne u¿yte przy obs³udze plików TARGA
	GLsizei width, height;
	GLenum format, type;
	GLvoid *pixels;
	// tryb upakowania bajtów danych tekstury
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// wskazówki do kompresji tesktur
	glHint(GL_TEXTURE_COMPRESSION_HINT, texture_compression_hint);
	// wczytanie tekstury lena.tga
	GLboolean error = load_targa("blonde_girl_white_skin.tga", width, height, format, type, pixels);
	// b³¹d odczytu pliku
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku lena.tga");
		exit(0);
	}
	// utworzenie identyfikatora tekstury
	glGenTextures(1, &LENA);
	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, LENA);
	// definiowanie tekstury z kompresj¹
	glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, width, height, 0, format, type, pixels);
	// utworzenie identyfikatora tekstury
	glGenTextures(1, &LENA_UNC);
	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, LENA_UNC);
	// definiowanie tekstury bez kompresji
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixels);
	// porz¹dki
	delete[](unsigned char*)pixels;
	// wczytanie tekstury lena_gray.tga
	error = load_targa("blonde_girl_white_skin.tga", width, height, format, type, pixels);
	// b³¹d odczytu pliku
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku lena_gray.tga");
		exit(0);
	}
	// utworzenie identyfikatora tekstury
	glGenTextures(1, &LENA_GRAY);
	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, LENA_GRAY);
	// definiowanie tekstury z kompresj¹
	glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_LUMINANCE, width, height, 0, format, type,
		pixels);
	// utworzenie identyfikatora tekstury
	glGenTextures(1, &LENA_GRAY_UNC);
	// dowi¹zanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, LENA_GRAY_UNC);
	// definiowanie tekstury bez kompresji
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, format, type, pixels);
	// porz¹dki
	delete[](unsigned char*)pixels;
	// wybór bie¿¹cej tekstury
	texture = LENA;
}
// obs³uga menu podrêcznego
void Menu(int value)
{
	GLint tmp_texture;
	switch (value)
	{
		// kompresja tekstur - GL_FASTEST
	case TEXTURE_COMPRESSION_FASTEST:
	{
		texture_compression_hint = GL_FASTEST;
		tmp_texture = texture;
		GenerateTextures();
		texture = tmp_texture;
		DisplayScene();
	}
	break;
	// kompresja tekstur - GL_DONT_CARE
	case TEXTURE_COMPRESSION_DONT_CARE:
	{
		texture_compression_hint = GL_DONT_CARE;
		tmp_texture = texture;
		GenerateTextures();
		texture = tmp_texture;
		DisplayScene();
	}
	break;
	// kompresja tekstur - GL_NICEST
	case TEXTURE_COMPRESSION_NICEST:
	{
		texture_compression_hint = GL_NICEST;
		tmp_texture = texture;
		GenerateTextures();
		texture = tmp_texture;
		DisplayScene();
	}
	break;
	// tekstura lena
	case TEXTURE_LENA:
	{
		texture = LENA;
		DisplayScene();
	}
	break;
	// teksura lena nieskompresowana
	case TEXTURE_LENA_UNC:
	{
		texture = LENA_UNC;
		DisplayScene();
	}
	break;
	// tekstura lena_gray
	case TEXTURE_LENA_GRAY:
	{
		texture = LENA_GRAY;
		DisplayScene();
	}
	break;
	// tekstura lena_gray nieskompresowana
	case TEXTURE_LENA_GRAY_UNC:
	{
		texture = LENA_GRAY_UNC;
		DisplayScene();
	}
	break;
	// obszar renderingu - ca³e okno
	case FULL_WINDOW:
	{
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	break;
	// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
	{
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
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
	// sprawdzenie czy jest co najmniej wersja 1.3 OpenGL lub
	// czy jest obs³ugiwane rozszerzenie ARB_texture_compression
	if (!(major > 1 || minor >= 3) &&
		!glutExtensionSupported("GL_ARB_texture_compression"))
	{
		printf("Brak rozszerzenia GL_ARB_texture_compression!\n");
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
	glutInitWindowSize(550, 550);
	// utworzenie g³ównego okna programu
	glutCreateWindow("Kompresja tekstur");
	// do³¹czenie funkcji generuj¹cej scenê 3D
	glutDisplayFunc(DisplayScene);
	// do³¹czenie funkcji wywo³ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);
	// utworzenie podmenu - Tekstura
	int MenuTexture = glutCreateMenu(Menu);
	glutAddMenuEntry("lena.tga (kompresja)", TEXTURE_LENA);
	glutAddMenuEntry("lena.tga (bez kompresji)", TEXTURE_LENA_UNC);
	glutAddMenuEntry("lena_gray.tga (kompresja)", TEXTURE_LENA_GRAY);
	glutAddMenuEntry("lena_gray.tga (bez kompresji)", TEXTURE_LENA_GRAY_UNC);
	// utworzenie podmenu - GL_TEXTURE_COMPRESSION_HINT
	int MenuTextureCompressionHint = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_FASTEST", TEXTURE_COMPRESSION_FASTEST);
	glutAddMenuEntry("GL_DONT_CARE", TEXTURE_COMPRESSION_DONT_CARE);
	glutAddMenuEntry("GL_NICEST", TEXTURE_COMPRESSION_NICEST);
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
	glutAddSubMenu("Tekstura", MenuTexture);
	glutAddSubMenu("GL_TEXTURE_COMPRESSION_HINT", MenuTextureCompressionHint);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
#ifdef WIN32
	glutAddMenuEntry("Wyjœcie", EXIT);
#else
	glutAddMenuEntry("Wyjscie", EXIT);
#endif
	// okreœlenie przycisku myszki obs³uguj¹cego menu podrêczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	// utworzenie tekstur
	GenerateTextures();
	// sprawdzenie i przygotowanie obs³ugi wybranych rozszerzeñ
	ExtensionSetup();
	// wprowadzenie programu do obs³ugi pêtli komunikatów
	glutMainLoop();
	return 0;
}
