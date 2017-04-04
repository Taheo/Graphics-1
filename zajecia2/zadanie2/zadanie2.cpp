#include "stdafx.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "gltools_extracted.h"
#include "VectorMath.cpp"

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

void ChangeSize(int w, int h)
{
    GLfloat fAspect;
    if (h == 0)
        h = 1;
    glViewport(0, 0, w, h);
    fAspect = (GLfloat)w / (GLfloat)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(35.0f, fAspect, 1.0, 40.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SetupRC()
{
    GLuint texture;
    int x, y;
    char pixels[256 * 256];
    GLint iWidth = 256;
    GLint iHeight = 256;
    GLint iComponents = GL_RGB;
    GLenum eFormat = {
        GL_RGBA
    };
    GLfloat whiteLight[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat lightPos[] = { -10.f, 5.0f, 5.0f, 1.0f };
    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_CULL_FACE); 
    glFrontFace(GL_CCW); 
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);


    for (y = 0; y < 256; y++)
    {
        for (x = 0; x < 256; x++)
            pixels[y * 256 + x] = rand() % 256;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);
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
    xRot = (GLfloat)((const int)xRot % 360);
    yRot = (GLfloat)((const int)yRot % 360);
    glutPostRedisplay();
}
void RenderScene(void)
{
	GLTVector3 vNormal;
	GLTVector3 vCorners[6] =
	{ { 0.0f, 1.0f, 0.0f }, 
	{ -0.5f, 0.0f, -.50f }, 
	{ 0.5f, 0.0f, -0.50f }, 
	{ 0.5f, 0.0f, 1.5f }, 
	{ -0.5f, 0.0f, 0.5f } ,
	{ 1.5f,0.f,0.5f },
	};

	for (GLint i = 1; i <= 5; i++)
	{

		vCorners[i][0] = vCorners[i - 1][0] + ((1.0*cos(i * 2 * 3.14 / 5)) * 1.0f);
		vCorners[i][2] = vCorners[i - 1][2] + ((1.0*sin(i * 2 * 3.14 / 5)) * 1.0f);

	}
	GLfloat sx = 0, sz = 0;

	for (int i = 1; i <= 5; i++)
	{
		sx = sx + vCorners[i][0];
		sz = sz + vCorners[i][2];
	}

	vCorners[0][0] = sx / 5;
	vCorners[0][1] = 1.f;
	vCorners[0][2] = sz / 5;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0.0f, -0.25f, -4.0f);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[1]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[2]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[4]);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[2]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[3]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[5]);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[3]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[4]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[1]);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[4]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[5]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[2]);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[5]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[1]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[3]);

	gltGetNormalVector(vCorners[1], vCorners[2], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[2]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[1]);

	gltGetNormalVector(vCorners[2], vCorners[3], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[3]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[2]);

	gltGetNormalVector(vCorners[3], vCorners[4], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[4]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[3]);

	gltGetNormalVector(vCorners[4], vCorners[5], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[5]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[4]);

	gltGetNormalVector(vCorners[5], vCorners[1], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[1]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[5]);

	glEnd();;
	glPopMatrix();
	glutSwapBuffers();
}
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Textured Pyramid");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    SetupRC();
    glutMainLoop();
    return 0;
}
