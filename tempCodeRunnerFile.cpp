#include <GL/glut.h>
#include <string.h>

void renderBitmapString(float x, float y, void *font, const char *string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string);
        string++;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Display samples of each GLUT font
    renderBitmapString(-0.9, 0.8, GLUT_BITMAP_8_BY_13, "GLUT_BITMAP_8_BY_13");
    renderBitmapString(-0.9, 0.6, GLUT_BITMAP_9_BY_15, "GLUT_BITMAP_9_BY_15");
    renderBitmapString(-0.9, 0.4, GLUT_BITMAP_TIMES_ROMAN_10, "GLUT_BITMAP_TIMES_ROMAN_10");
    renderBitmapString(-0.9, 0.2, GLUT_BITMAP_TIMES_ROMAN_24, "GLUT_BITMAP_TIMES_ROMAN_24");
    renderBitmapString(-0.9, 0.0, GLUT_BITMAP_HELVETICA_10, "GLUT_BITMAP_HELVETICA_10");
    renderBitmapString(-0.9, -0.2, GLUT_BITMAP_HELVETICA_12, "GLUT_BITMAP_HELVETICA_12");
    renderBitmapString(-0.9, -0.4, GLUT_BITMAP_HELVETICA_18, "GLUT_BITMAP_HELVETICA_18");

    glFlush();
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // Black background
    glColor3f(1.0, 1.0, 1.0);         // White text
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Set coordinate system
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("GLUT Bitmap Fonts");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
