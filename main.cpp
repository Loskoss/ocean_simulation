#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glut.h>
#include <GL/gl.h> // Include OpenGL header
#include <fftw3.h>
#include "wave.h"
#include <thread>
#include <mutex>
#include <GL/glext.h>
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

mutex oceanMutex;

GLint shaderProgram;

// Function to load shader source code from file
string loadShaderFromFile(const string& filename) {
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to compile shader and check for errors
GLuint compileShader(GLenum shaderType, const string& source) {
    GLuint shader = glCreateShader(shaderType);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, NULL);
    glCompileShader(shader);

    // Check compilation status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        cerr << "Shader compilation failed: " << infoLog << endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

// Function to create shader program
GLuint createShaderProgram(const string& vertexShaderSource, const string& fragmentShaderSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check linking status
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cerr << "Shader program linking failed: " << infoLog << endl;
        return 0;
    }

    // Delete shaders (no longer needed after linking)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

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
    GLfloat ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat diffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    // Enable light
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Shading model
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

float t = 0.00;

void updateOceanHeights() {
    while (true) {
        {
            lock_guard<mutex> lock(oceanMutex);
            t += 0.015;
            o_object->update_heights(t);
        }
        // Add sleep to control update rate
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

// Function to calculate model-view-projection matrix
glm::mat4 calculateModelViewProjectionMatrix() {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 800.0f / 600.0f, 0.1f, 10000.0f);
    glm::mat4 mvp = projection * view * model;
    return mvp;
}

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

    // Lock mutex before accessing ocean data
    {
        lock_guard<mutex> lock(oceanMutex);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Pass uniform variables to the shader (e.g., time for animation)
        GLint timeLocation = glGetUniformLocation(shaderProgram, "time");
        if (timeLocation != -1) {
            glUniform1f(timeLocation, t); // Assuming 't' is the time variable used for animation
        }

        // Calculate model-view-projection matrix
        glm::mat4 modelViewProjectionMatrix = calculateModelViewProjectionMatrix();

        // Get the location of the uniform variable in the shader program
        GLuint mvpLocation = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");

        // Set the uniform variable in the shader program
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

        // Draw the scene
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        o_object->display();
    }

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
    glutCreateWindow("TP 2 ");

    initGL();

    // Load shader source code
    string vertexShaderSource = loadShaderFromFile("water.vert");
    string fragmentShaderSource = loadShaderFromFile("water.frag");

    // Create shader program
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Start a separate thread for updating ocean heights
    thread oceanThread(updateOceanHeights);

    glutDisplayFunc(&window_display);
    glutReshapeFunc(&window_reshape);
    glutKeyboardFunc(&window_key);
    glutSpecialFunc(&callback_special);
    glutIdleFunc(&window_idle);

    glutMainLoop();

    // Join the ocean thread before exiting
    oceanThread.join();

    return 0;
}
