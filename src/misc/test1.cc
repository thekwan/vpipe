/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include <GL/gl.h>
#include <GL/glu.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>
#include "test1.hpp"

Test1::Test1(std::string name, program_args &pargs) : Job(name), _pargs(pargs) {
}

bool Test1::Run(job_context &context) {
    TestRun0( context );
    //TestRun1( context );

    return true;
}

bool Test1::Stop(void) {
    return true;
}

void Test1::TestRun0(job_context &context) {
    int image_num = context.imageDB.getImageNum();

    /* Open image display window.
     */
    //cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::namedWindow("Window", CV_WINDOW_OPENGL );
    //cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);


    //for(int i = 0; i < image_num; i++) {
    for(int i = 0; i < 1; i++) {
        std::shared_ptr<Image> img = context.imageDB.getImage(i);
        //cv::Mat img_src;
        cv::Mat img_src(*(img->getDataPtr()));

        /* Convert image format (from colored to gray-scale)
         */
        //cv::cvtColor(*(img->getDataPtr()), img_src, CV_BGR2GRAY);


        /* Set OpenGL drawing callback function.
         */
        //cv::ogl::Texture2D backgroundTex(img_src);
        cv::ogl::Texture2D backgroundTex;
        //std::cout << "texture cols = " << backgroundTex.cols() << std::endl;
        backgroundTex.copyFrom( img_src );
        //std::cout << "texture cols = " << backgroundTex.cols() << std::endl;
        cv::setOpenGlDrawCallback("Window", opengl_draw_callback, &backgroundTex );
    
        /* Display image 
         */
        //cv::imshow("Window", img_src);
        cv::updateWindow("Window");
        cv::waitKey(0);
    }

    //cv::setOpenGlDrawCallback("Window", 0, 0);
    cv::destroyWindow("Window");

    return;
}

int rotx = 55, roty = 40;

static void opengl_draw_callback(void *param) {
    cv::ogl::Texture2D *backgroundTex = (cv::ogl::Texture2D*) param;
    glEnable( GL_TEXTURE_2D );
    backgroundTex->bind();
    cv::ogl::render(*backgroundTex);
    glDisable( GL_TEXTURE_2D );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0, 0, -0.5 );
    glRotatef( rotx, 1, 0, 0 );
    glRotatef( roty, 0, 1, 0 );
    glRotatef(    0, 0, 0, 1 );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    static const int coords[6][4][3] = {
        { {+1,-1,-1}, {-1,-1,-1}, {-1,+1,-1}, {+1,+1,-1} },
        { {+1,+1,-1}, {-1,+1,-1}, {-1,+1,+1}, {+1,+1,+1} },
        { {+1,-1,+1}, {+1,-1,-1}, {+1,+1,-1}, {+1,+1,+1} },
        { {-1,-1,-1}, {-1,-1,+1}, {-1,+1,+1}, {-1,+1,-1} },
        { {+1,-1,+1}, {-1,-1,+1}, {-1,-1,-1}, {+1,-1,-1} },
        { {-1,-1,+1}, {+1,-1,+1}, {+1,+1,+1}, {-1,+1,+1} }
    };

    for( int i = 0; i < 6; i++) {
        glColor3ub( i*20, 100+i*10, i*42 );
        glBegin( GL_QUADS );
        for( int j = 0; j < 4; j++) {
            glVertex3d(
                0.2 * coords[i][j][0],
                0.2 * coords[i][j][1],
                0.2 * coords[i][j][2]
            );
        }
        glEnd();
    }
}
