/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "test2.hpp"

Test2::Test2(std::string name, program_args &pargs) : Job(name), _pargs(pargs) {
}

bool Test2::Run(job_context &context) {
    TestRun0( context );

    return true;
}

bool Test2::Stop(void) {
    return true;
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static const int coords[6][4][3] = {
        { {+1,-1,-1}, {-1,-1,-1}, {-1,+1,-1}, {+1,+1,-1} },
        { {+1,+1,-1}, {-1,+1,-1}, {-1,+1,+1}, {+1,+1,+1} },
        { {+1,-1,+1}, {+1,-1,-1}, {+1,+1,-1}, {+1,+1,+1} },
        { {-1,-1,-1}, {-1,-1,+1}, {-1,+1,+1}, {-1,+1,-1} },
        { {+1,-1,+1}, {-1,-1,+1}, {-1,-1,-1}, {+1,-1,-1} },
        { {-1,-1,+1}, {+1,-1,+1}, {+1,+1,+1}, {-1,+1,+1} }
    };

    float scale = 1.0;
    for( int i = 0; i < 6; i++) {
        glColor3ub( i*20, 100+i*10, i*42 );
        glBegin( GL_QUADS );
        for( int j = 0; j < 4; j++) {
            glVertex3d(
                scale * coords[i][j][0],
                scale * coords[i][j][1],
                scale * coords[i][j][2]
            );
        }
        glEnd();
    }

    glutSwapBuffers();

    return;
}

void Test2::init(void) {
    glEnable(GL_DEPTH_TEST);

    //glViewport(0.0, 0.0, 1280, 720);
    glutReshapeWindow(1280, 720);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective( 28.0, 4.0/3.0, 0.1, 1.0 );
    gluPerspective( 45.0, 1.0, 1.0, 10.0 );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.5,    // Camera position
              0.0, 0.0, 0.0,    // looking at ...
              0.0, 0.2, 0.0 );  // Up vector

    return;
}

void Test2::TestRun0(job_context &context) {
    int argc = 1;
    char **argv = NULL;

    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutCreateWindow("red 3D cude");
    glutDisplayFunc(display);
    init();

    glutMainLoop();

    return;
}
