#include <iostream>
#include <GL/glut.h>
#include <fftw3.h>
#include "wave.h"

using namespace std;

#define ECHAP 27

Ocean *o_object = NULL;

GLfloat rotacionX = 0.0f;
GLfloat rotacionY = 0.0f;
GLfloat cameraX = 0.0f;
GLfloat cameraY = 0.0f;
GLfloat cameraZ = 50.0f; // Initial camera distance
GLfloat zoomSpeed = 10.0f;
GLfloat moveSpeed = 1.0f;

GLvoid callback_special(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        rotacionX -= 3.0f;
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:
        rotacionX += 3.0f;
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT:
        rotacionY -= 3.0f;
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        rotacionY += 3.0f;
        glutPostRedisplay();
        break;
    }
}

GLvoid initGL()
{
    GLfloat position[] = {0.0f, 5.0f, 10.0f, 0.0};

    // Enable light
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Shading model
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);

    glClearColor(255, 255, 255, 255);

    // Set polygon mode to fill for both front and back faces
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

float t = 0.0;

GLvoid window_display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 800.0 / 600.0, 0.1, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Define camera position, target position, and up vector
    GLfloat targetX = 0.0f;
    GLfloat targetY = 0.0f;
    GLfloat targetZ = 0.0f;
    GLfloat upX = 0.0f;
    GLfloat upY = 1.0f;
    GLfloat upZ = 0.0f;

    // Apply camera translations and rotations
    gluLookAt(cameraX, cameraY, cameraZ, targetX, targetY, targetZ, upX, upY, upZ);
    glRotatef(rotacionX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotacionY, 0.0f, 1.0f, 0.0f);

    // Apply initial translation for base viewing angle
    glTranslatef(-50, -15, -50);

    // Update heights of ocean points
    t += 0.007;
    o_object->update_heights(t);

    // Set polygon mode to fill
    glPolygonMode(GL_FRONT, GL_POINT);

    // Draw the scene with filled polygons
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < o_object->m_z - 1; ++i) {
        for (int j = 0; j < o_object->n_x - 1; ++j) {
            // Get the height and color of each vertex
            GLfloat h1 = o_object->points[i * o_object->n_x + j].hauteur;
            GLfloat h2 = o_object->points[i * o_object->n_x + (j + 1)].hauteur;
            GLfloat h3 = o_object->points[(i + 1) * o_object->n_x + (j + 1)].hauteur;
            GLfloat h4 = o_object->points[(i + 1) * o_object->n_x + j].hauteur;

            // Set color based on height
            glColor3f(h1 * 0.2f, h1 * 0.4f, h1);  // Blue with green tint for shallows
            glVertex3f(j, h1, i);
            // Set color based on height
            glColor3f(h2 * 0.2f, h2 * 0.4f, h2);  // Blue with green tint for shallows
            glVertex3f(j + 1, h2, i);
            // Set color based on height
            glColor3f(h3 * 0.2f, h3 * 0.4f, h3);  // Blue with green tint for shallows
            glVertex3f(j + 1, h3, i + 1);

            // Set color based on height
            glColor3f(h1 * 0.2f, h1 * 0.4f, h1);  // Blue with green tint for shallows
            glVertex3f(j, h1, i);
            // Set color based on height
            glColor3f(h3 * 0.2f, h3 * 0.4f, h3);  // Blue with green tint for shallows
            glVertex3f(j + 1, h3, i + 1);
            // Set color based on height
            glColor3f(h4 * 0.2f, h4 * 0.4f, h4);  // Blue with green tint for shallows
            glVertex3f(j, h4, i + 1);
        }
    }
    glEnd();

    glutSwapBuffers();
    glFlush();
}


GLvoid window_reshape(GLsizei width, GLsizei height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 800.0 / 600.0, 0.1, 10000);
    glMatrixMode(GL_MODELVIEW);
}

GLvoid window_key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w': // Move camera forward
        cameraZ -= moveSpeed;
        glutPostRedisplay();
        break;
    case 's': // Move camera backward
        cameraZ += moveSpeed;
        glutPostRedisplay();
        break;
    case 'a': // Move camera left
        cameraX -= moveSpeed;
        glutPostRedisplay();
        break;
    case 'd': // Move camera right
        cameraX += moveSpeed;
        glutPostRedisplay();
        break;
    case '+': // Zoom in
        cameraZ -= zoomSpeed;
        glutPostRedisplay();
        break;
    case '-': // Zoom out
        cameraZ += zoomSpeed;
        glutPostRedisplay();
        break;
    case ECHAP: // Exit program
        exit(1);
        break;
    default:
        printf("Key %c is not supported.\n", key);
        break;
    }
}

GLvoid window_idle()
{
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    o_object = new Ocean(100, 100, 1, 1);
    o_object->create_list_vertex();
    o_object->create_index_list();
    o_object->load_waves("spectrum.txt");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Ocean Simulation");

    initGL();

    glutDisplayFunc(&window_display);
    glutReshapeFunc(&window_reshape);
    glutKeyboardFunc(&window_key);
    glutSpecialFunc(&callback_special);
    glutIdleFunc(&window_idle);

    glutMainLoop();
    return 0;
}
