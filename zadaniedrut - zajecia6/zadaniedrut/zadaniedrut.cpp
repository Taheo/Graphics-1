// zadaniedrut.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GL/glut.h>
#include <stdlib.h>

enum

{

	FULL_WINDOW,       // aspekt obrazu - ca³e okno

	ASPECT_1_1,        // aspekt obrazu 1:1

	EXIT               // wyjœcie

};


// aspekt obrazu


int aspect = FULL_WINDOW;


// rozmiary bry³y obcinania


const GLdouble left = -10.0;

const GLdouble right = 10.0;

const GLdouble bottom = -10.0;

const GLdouble top = 10.0;

const GLdouble near_ = 50.0;

const GLdouble far_ = 70.0;


// wspó³czynnik skalowania


GLfloat scale = 1.0;


// k¹ty obrotu


GLfloat rotatex = 0.0;

GLfloat rotatey = 0.0;


// przesuniêcie


GLfloat translatex = 0.0;

GLfloat translatey = 0.0;


// wska?nik naciœniêcia lewego przycisku myszki


int button_state = GLUT_UP;


// po³o?enie kursora myszki


int button_x, button_y;


// funkcja rysuj¹ca blok 3x3


void Cube3x3()

{
	glutWireSphere(1.0, 8.0, 8.0);
	glTranslatef(1.0, 0.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

	glTranslatef(0.0, -1.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

	glTranslatef(-1.0, 0.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

	glTranslatef(-1.0, 0.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

	glTranslatef(0.0, 1.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

	glTranslatef(0.0, 1.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

	glTranslatef(1.0, 0.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

	glTranslatef(1.0, 0.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

}


// funkcja rysuj¹ca blok 2x2


void Cube2x2()

{

	glutWireSphere(1.0, 8.0, 8.0);

	glTranslatef(1.0, 0.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

	glTranslatef(0.0, -1.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

	glTranslatef(-1.0, 0.0, 0.0);

	glutWireSphere(1.0, 8.0, 8.0);

}


// funkcja rysuj¹ca piramidê z szeœcianów


void Pyramid()

{

	// drugi poziom 5x5

	glPushMatrix();

	glTranslatef(0.0, 0.0, 1.0);

	glPushMatrix();

	glTranslatef(1.0, 1.0, 0.0);

	Cube3x3();

	glPopMatrix();

	glPushMatrix();

	glTranslatef(1.0, -1.0, 0.0);

	Cube2x2();

	glPopMatrix();

	glPushMatrix();

	glTranslatef(-2.0, 2.0, 0.0);

	Cube2x2();

	glPopMatrix();

	glPushMatrix();

	glTranslatef(-1.0, -1.0, 0.0);

	Cube3x3();

	glPopMatrix();

	glPopMatrix();


	// trzeci poziom 4x4

	glPushMatrix();

	glTranslatef(0.0, 0.0, 2.0);

	glPushMatrix();

	glTranslatef(0.5, -0.5, 0.0);

	Cube2x2();

	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.5, 1.5, 0.0);

	Cube2x2();

	glPopMatrix();

	glPushMatrix();

	glTranslatef(-1.5, 1.5, 0.0);

	Cube2x2();

	glPopMatrix();

	glPushMatrix();

	glTranslatef(-1.5, -0.5, 0.0);

	Cube2x2();

	glPopMatrix();

	glPopMatrix();


	// czwarty poziom 3x3

	glPushMatrix();

	glTranslatef(0.0, 0.0, 3.0);

	Cube3x3();

	glPopMatrix();


	// pi¹ty poziom 2x2

	glPushMatrix();

	glTranslatef(0.0, 0.0, 4.0);

	glTranslatef(-0.5, 0.5, 0.0);

	Cube2x2();

	glPopMatrix();


	// szósty poziom 1x1

	glPushMatrix();

	glTranslatef(0.0, 0.0, 5.0);

	glutWireSphere(1.0, 8.0, 8.0);

	glPopMatrix();

}


// funkcja generuj¹ca scenê 3D


void Display()

{

	// kolor t³a - zawartoœæ bufora koloru

	glClearColor(1.0, 1.0, 1.0, 1.0);


	// czyszczenie bufora koloru

	glClear(GL_COLOR_BUFFER_BIT);


	// wybór macierzy modelowania

	glMatrixMode(GL_MODELVIEW);


	// macierz modelowania = macierz jednostkowa

	glLoadIdentity();


	// przesuniêcie uk³adu wspó³rzêdnych obiektu do œrodka bry³y odcinania

	glTranslatef(0, 0, -(near_ + far_) / 2);


	// przesuniêcie obiektu - ruch myszk¹

	glTranslatef(translatex, translatey, 0.0);


	// skalowanie obiektu - klawisze "+" i "-"

	glScalef(scale, scale, scale);


	// obroty obiektu - klawisze kursora

	glRotatef(rotatex, 1.0, 0, 0);

	glRotatef(rotatey, 0, 1.0, 0);


	// kolor krawêdzi obiektu

	glColor3f(0.0, 0.0, 0.0);


	// rysowanie piramidy

	Pyramid();


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

			glFrustum(left, right, bottom*height / width, top*height / width, near_, far_);

		else


			// szerokoœæ okna wiêksza lub równa wysokoœci okna

			if (width >= height && height > 0)

				glFrustum(left*width / height, right*width / height, bottom, top, near_, far_);

	}

	else

		glFrustum(left, right, bottom, top, near_, far_);


	// generowanie sceny 3D

	Display();

}


// obs³uga klawiatury


void Keyboard(unsigned char key, int x, int y)

{

	// klawisz +

	if (key == '+')

		scale += 0.1;

	else


		// klawisz -

		if (key == '-' && scale > 0.1)

			scale -= 0.1;


	// odrysowanie okna

	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

}


// obs³uga klawiszy funkcyjnych i klawiszy kursora


void SpecialKeys(int key, int x, int y)

{

	switch (key)

	{

		// kursor w lewo

	case GLUT_KEY_LEFT:

		rotatey -= 1;

		break;


		// kursor w górê

	case GLUT_KEY_UP:

		rotatex -= 1;

		break;


		// kursor w prawo

	case GLUT_KEY_RIGHT:

		rotatey += 1;

		break;


		// kursor w dó³

	case GLUT_KEY_DOWN:

		rotatex += 1;

		break;

	}


	// odrysowanie okna

	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

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

		translatex += 1.1 *(right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);

		button_x = x;

		translatey += 1.1 *(top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);

		button_y = y;

		glutPostRedisplay();

	}

}


// obs³uga menu podrêcznego


void Menu(int value)

{

	switch (value)

	{

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


int main(int argc, char *argv[])

{

	// inicjalizacja biblioteki GLUT

	glutInit(&argc, argv);


	// inicjalizacja bufora ramki

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);


	// rozmiary g³ównego okna programu

	glutInitWindowSize(400, 400);


	// utworzenie g³ównego okna programu

	glutCreateWindow("Stos modelowania");


	// do³¹czenie funkcji generuj¹cej scenê 3D

	glutDisplayFunc(Display);


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


	// utworzenie podmenu - aspekt obrazu

	int MenuAspect = glutCreateMenu(Menu);

#ifdef WIN32


	glutAddMenuEntry("Aspekt obrazu - ca³e okno", FULL_WINDOW);

#else


	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);

#endif


	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);


	// menu g³ówne

	glutCreateMenu(Menu);

	glutAddSubMenu("Aspekt obrazu", MenuAspect);

#ifdef WIN32


	glutAddMenuEntry("Wyjœcie", EXIT);

#else


	glutAddMenuEntry("Wyjscie", EXIT);

#endif


	// okreœlenie przycisku myszki obs³uguj¹cego menu podrêczne

	glutAttachMenu(GLUT_RIGHT_BUTTON);


	// wprowadzenie programu do obs³ugi pêtli komunikatów

	glutMainLoop();

	return 0;
}
