/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>
#include "test1.hpp"

Test1::Test1(std::string name, program_args &pargs) : Job(name), _pargs(pargs) {
}

bool Test1::Run(job_context &context) {
    //TestRun0( context );
    TestRun1( context );

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

    cv::setOpenGlContext("Window");

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
        cv::ogl::Texture2D backgroundTex;
        backgroundTex.copyFrom( img_src );
        ogl_draw_data data;
        data.handle = this;
        data.backgroundTex = &backgroundTex;

        setProjectionMatrix();
        setModelViewMatrix(rvecs[i], tvecs[i]);

        cv::setOpenGlDrawCallback("Window", opengl_draw_callback_wrapper, (void*)&data);
    
        /* Display image 
         */
        //cv::imshow("Window", img_src);
        cv::updateWindow("Window");
        cv::waitKey(0);
    }

    cv::setOpenGlDrawCallback("Window", 0, 0);
    cv::destroyWindow("Window");

    return;
}

int rotx = 55, roty = 40;

void Test1::opengl_draw_callback(void *param) {
    glViewport(0.0, 0.0, 1280, 720);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

#if 1
    /* Set texture image as a background of 3D scene.
     */
    cv::ogl::Texture2D *backgroundTex = (cv::ogl::Texture2D*) param;
    glEnable( GL_TEXTURE_2D );
    backgroundTex->bind();
    cv::ogl::render(*backgroundTex);
    glDisable( GL_TEXTURE_2D );
#endif

    /* Draw 3D object on the background.
     */

    glMatrixMode( GL_PROJECTION );
    glLoadMatrixf( projectMatrix );
    //glLoadIdentity();
    //gluPerspective(90.0, 1.0, 1.0, 100.0);

    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixf( modelViewMatrix );
    //glLoadIdentity();
    //gluLookAt(0, 0,  15.5, 0, 0, 0, 0, 0.2, 0.0);

    //?????
    //glEnable( GL_DEPTH_TEST );
    //glDepthFunc( GL_LESS );
    

#if 1
    static const int coords[6][4][3] = {
        { {+1,-1,-1}, {-1,-1,-1}, {-1,+1,-1}, {+1,+1,-1} },
        { {+1,+1,-1}, {-1,+1,-1}, {-1,+1,+1}, {+1,+1,+1} },
        { {+1,-1,+1}, {+1,-1,-1}, {+1,+1,-1}, {+1,+1,+1} },
        { {-1,-1,-1}, {-1,-1,+1}, {-1,+1,+1}, {-1,+1,-1} },
        { {+1,-1,+1}, {-1,-1,+1}, {-1,-1,-1}, {+1,-1,-1} },
        { {-1,-1,+1}, {+1,-1,+1}, {+1,+1,+1}, {-1,+1,+1} }
    };

    for( int i = 0; i < 6; i++) {
        glColor3ub( i*20, 10+i*10, i*42 );
        glBegin( GL_QUADS );
        for( int j = 0; j < 4; j++) {
            glVertex3d(
                 square_size * (coords[i][j][0]+1.0f)/2.0f,
                 square_size * (coords[i][j][1]+1.0f)/2.0f,
                -square_size * (coords[i][j][2]+1.0f)/2.0f
            );
        }
        glEnd();
    }
#else
    static const int coords[1][4][3] = {
        { {+0, 0, 0}, { 0, 1, 0}, { 1, 1, 0}, { 1, 0, 0} }
    };

    //glEnable( GL_TEXTURE_2D );

    for( int i = 0; i < 1; i++) {
        glColor3ub( i*20, 100+i*10, i*42 );
        glBegin( GL_QUADS );
        for( int j = 0; j < 4; j++) {
            glVertex3f(
                (float) (square_size * coords[i][j][0]),
                (float) (square_size * coords[i][j][1]),
                (float) (square_size * coords[i][j][2])
            );
        }
        glEnd();
    }

    //glDisable( GL_TEXTURE_2D );

    glFlush();
#endif
}

void Test1::TestRun1(job_context &context) {
    int image_num = context.imageDB.getImageNum();

    /* Open image display window.
     */
    cv::namedWindow("Window", CV_WINDOW_OPENGL );
    //cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

    /* Get the image position of found calibration chessboard corners.
     */
    std::vector<cv::Mat> images;
    cv::Size boardSize(9,6);
    cv::Size imageSize;
    std::vector<std::vector<cv::Point2f>> imagePoints;

    for(int i = 0; i < image_num; i++) {
        std::cout << "------- " << i << "'th image processing ------" << std::endl;
        std::shared_ptr<Image> img = context.imageDB.getImage(i);
        cv::Mat img_src( *(img->getDataPtr()) );

        cv::resize( img_src, img_src, cv::Size(img_src.cols/2, img_src.rows/2), 0, 0, CV_INTER_CUBIC);
        imageSize = img_src.size();
        images.push_back( img_src );

        // Calibration initial info settings
        std::vector<cv::Point2f> pointBuf;
        int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;

        bool found = findChessboardCorners( img_src, boardSize, pointBuf, chessBoardFlags );
        std::cout << "pointBuf.size() = " << pointBuf.size() << std::endl;

        if( found ) {
            std::cout << "Found all chess board corner position." << std::endl;
            cv::Mat img_gray;
            cv::cvtColor( img_src, img_gray, cv::COLOR_BGR2GRAY );
            cv::cornerSubPix( img_gray, pointBuf, cv::Size(11,11),
                    cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::COUNT, 30, 0.1 ) );

            imagePoints.push_back( pointBuf );
        }

        cv::drawChessboardCorners( img_src, boardSize, cv::Mat(pointBuf), found );

        /* Display image 
         */
        //cv::imshow("Window", img_src);
        //cv::waitKey(0);
    }

    /* Get the world coordinate position of the chessboard.
     */
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<cv::Point3f> objPoints;
    for(int i = 0; i < boardSize.height; i++)
        for(int j = 0; j < boardSize.width; j++)
            objPoints.push_back( cv::Point3f(j*square_size, i*square_size, 0) );
    objectPoints.resize( imagePoints.size(), objPoints );

    /* Camera calibration
     * Get intrinsic/extrinsic matrix of camera.
     */
    int calibFlags = 0;
    cv::calibrateCamera( objectPoints, imagePoints, imageSize, 
            cameraMatrix, distCoeffs, rvecs, tvecs, calibFlags);

    focal_x  = (float)cameraMatrix.at<double>(0,0);
    focal_y  = (float)cameraMatrix.at<double>(1,1);
    screen_x = (float)imageSize.width;
    screen_y = (float)imageSize.height;
    camera_cx= (float)cameraMatrix.at<double>(0,2);
    camera_cy= (float)cameraMatrix.at<double>(1,2);


    std::cout << "Image size     = (" << imageSize.width << " , " << imageSize.height << ")" << std::endl;
    std::cout << "Camera matrix = " << cameraMatrix << std::endl;
    std::cout << "Focal length X = " << focal_x << std::endl;
    std::cout << "Focal length Y = " << focal_y << std::endl;
    std::cout << "Camera pos X   = " << cameraMatrix.at<double>(0,2) << std::endl;
    std::cout << "Camera pos Y   = " << cameraMatrix.at<double>(1,2) << std::endl;
    std::cout << "Screen size X  = " << screen_x << std::endl;
    std::cout << "Screen size Y  = " << screen_y << std::endl;
    std::cout << "Distor coeffs  = " << distCoeffs << std::endl;
    std::cout << "rvecs.size     = " << rvecs.size() << std::endl;
    std::cout << "tvecs.size     = " << tvecs.size() << std::endl;

    cv::resizeWindow("Window", imageSize.width, imageSize.height);

#if 1
    cv::setOpenGlContext("Window");
    /* Draw 3D scene (with OpenGL) on captured 2D image.
     */
    for(int i = 0; i < images.size() ; i++) {
    //for(int i = 0; i < 1; i++) {
        cv::Mat image = images[i];

        cv::ogl::Texture2D backgroundTex;
        backgroundTex.copyFrom( image );

        /* OpenGL drawing function call.
         */
        ogl_draw_data data;
        data.handle = this;
        data.backgroundTex = &backgroundTex;

        setProjectionMatrix();
        setModelViewMatrix(rvecs[i], tvecs[i]);

        //PrintMatrix(projectMatrix, 4, 4, "ProjetionMatrix");
        //PrintMatrix(modelViewMatrix, 4, 4, "modelViewMatrix");

        cv::setOpenGlDrawCallback("Window", opengl_draw_callback_wrapper, (void*)&data);

        cv::updateWindow("Window");
        cv::waitKey(0);
    }
#endif

#if 0
    OpenGlPipeSim(images);
#elif 0
    OpenCvPipeSim(images);
#elif 0
    OpenGlPipeSim(images);
    OpenCvPipeSim(images);
    DrawPipeSimResult(images);
#endif

#if 0
    /* Project Axis on image.
     */
    // Set axis position on world coordinate.
    std::vector<cv::Point3f> axisPoints;
    axisPoints.emplace_back( cv::Point3f( 0, 0, 0 ) );
    axisPoints.emplace_back( cv::Point3f( -square_size, 0, 0 ) );
    axisPoints.emplace_back( cv::Point3f( 0, -square_size, 0 ) );
    axisPoints.emplace_back( cv::Point3f( 0, 0, -square_size ) );

    std::cout << "== check camera parameter for each scene ==" << std::endl;

    for(int i = 0; i < rvecs.size(); i++) {
        std::vector<cv::Point2f> projPoints;
        cv::projectPoints( axisPoints, rvecs[i], tvecs[i], cameraMatrix, distCoeffs, projPoints );

        
        cv::Mat RotMat;
        cv::Rodrigues(rvecs[i], RotMat);
        std::cout << "rmat = " << RotMat << std::endl;
        std::cout << "tvec = " << tvecs[i] << std::endl;

        /* Draw axis on the image.
         */
        cv::line( images[i], projPoints[0], projPoints[1], cv::Scalar(255, 0, 0), 4 );
        cv::line( images[i], projPoints[0], projPoints[2], cv::Scalar(0, 255, 0), 4 );
        cv::line( images[i], projPoints[0], projPoints[3], cv::Scalar(0, 0, 255), 4 );

        std::cout << "PointProjection--->" << std::endl;
        std::cout << axisPoints[0] << "=>" << projPoints[0] << std::endl;
        std::cout << axisPoints[1] << "=>" << projPoints[1] << std::endl;
        std::cout << axisPoints[2] << "=>" << projPoints[2] << std::endl;
        std::cout << axisPoints[3] << "=>" << projPoints[3] << std::endl;

        /* Display image 
         */
        cv::imshow("Window", images[i]);
        cv::waitKey(0);
    }
#endif

    cv::setOpenGlDrawCallback("Window", 0, 0);
    cv::destroyWindow("Window");

    return;
}

void Test1::PrintMatrix(GLfloat *mat, int row, int col, const std::string &msg) {
    std::cout << "PrintMatrix::" << msg << std::endl;
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            std::cout << mat[j*col+i] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Test1::setProjectionMatrix(void) {
    GLfloat farPlane = 100.0f*square_size;
    GLfloat nearPlane = 1.0f;

    projectMatrix[0]  = (2.0f * focal_x) / screen_x;
    projectMatrix[1]  = 0;
    projectMatrix[2]  = 0;
    projectMatrix[3]  = 0;

    projectMatrix[4]  = 0;
    projectMatrix[5]  = (2.0f * focal_y) / screen_y;
    projectMatrix[6]  = 0;
    projectMatrix[7]  = 0;

    projectMatrix[8]  = (2.0f * camera_cx / screen_x) - 1.0f;
    projectMatrix[9]  = (2.0f * camera_cy / screen_y) - 1.0f;
    projectMatrix[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    projectMatrix[11] = -1.0f;

    projectMatrix[12] = 0;
    projectMatrix[13] = 0;
    projectMatrix[14] = -2.0f * (farPlane * nearPlane) / (farPlane - nearPlane);
    projectMatrix[15] = 0;

    projectMatrix[8] *= -1.0f;
    //projectMatrix[9] *= -1.0f;

    //projectMatrix[5] *= -1.0f;

    return;
}

void Test1::setModelViewMatrix(cv::Mat &rvec, cv::Mat &tvec) {
    cv::Mat RotMat;
    cv::Rodrigues(rvec, RotMat);

    //std::cout << "RotMat = " << RotMat << std::endl;
    //std::cout << "Tvec   = " << tvec << std::endl;

    modelViewMatrix[0]  = (float)RotMat.at<double>(0,0);
    modelViewMatrix[1]  =-(float)RotMat.at<double>(1,0);
    modelViewMatrix[2]  =-(float)RotMat.at<double>(2,0);
    modelViewMatrix[3]  = 0;

    modelViewMatrix[4]  = (float)RotMat.at<double>(0,1);
    modelViewMatrix[5]  =-(float)RotMat.at<double>(1,1);
    modelViewMatrix[6]  =-(float)RotMat.at<double>(2,1);
    modelViewMatrix[7]  = 0;

    modelViewMatrix[8]  = (float)RotMat.at<double>(0,2);
    modelViewMatrix[9]  =-(float)RotMat.at<double>(1,2);
    modelViewMatrix[10] =-(float)RotMat.at<double>(2,2);
    modelViewMatrix[11] = 0;

    modelViewMatrix[12] = (float) tvec.at<double>(0);
    modelViewMatrix[13] =-(float) tvec.at<double>(1);
    modelViewMatrix[14] =-(float) tvec.at<double>(2);
    modelViewMatrix[15] = 1.0f;

    return;
}

void Test1::setCameraIntrinsicMatrix(void) {
    GLfloat farPlane = 100.0f*square_size;
    GLfloat nearPlane = 1.0f;

    CameraIntrinsicMatrix[0]  = (float) cameraMatrix.at<double>(0,0);
    CameraIntrinsicMatrix[1]  = 0;
    CameraIntrinsicMatrix[2]  = (float) cameraMatrix.at<double>(0,2);

    CameraIntrinsicMatrix[3]  = 0;
    CameraIntrinsicMatrix[4]  = (float) cameraMatrix.at<double>(1,1);
    CameraIntrinsicMatrix[5]  = (float) cameraMatrix.at<double>(1,2);

    CameraIntrinsicMatrix[6]  = 0;
    CameraIntrinsicMatrix[7]  = 0;
    CameraIntrinsicMatrix[8]  = 1;

    return;
}

void Test1::setCameraExtrinsicMatrix(cv::Mat &rvec, cv::Mat &tvec) {
    cv::Mat RotMat;
    cv::Rodrigues(rvec, RotMat);

    //std::cout << "RotMat = " << RotMat << std::endl;
    //std::cout << "Tvec   = " << tvec << std::endl;

#if 1
    /* OpenCV camera extrinsic matrix
     */
    CameraExtrinsicMatrix[0]  = (float)RotMat.at<double>(0,0);
    CameraExtrinsicMatrix[1]  = (float)RotMat.at<double>(0,1);
    CameraExtrinsicMatrix[2]  = (float)RotMat.at<double>(0,2);
    CameraExtrinsicMatrix[3]  = (float) tvec.at<double>(0);

    CameraExtrinsicMatrix[4]  = (float)RotMat.at<double>(1,0);
    CameraExtrinsicMatrix[5]  = (float)RotMat.at<double>(1,1);
    CameraExtrinsicMatrix[6]  = (float)RotMat.at<double>(1,2);
    CameraExtrinsicMatrix[7]  = (float) tvec.at<double>(1);

    CameraExtrinsicMatrix[8]  = (float)RotMat.at<double>(2,0);
    CameraExtrinsicMatrix[9]  = (float)RotMat.at<double>(2,1);
    CameraExtrinsicMatrix[10] = (float)RotMat.at<double>(2,2);
    CameraExtrinsicMatrix[11] = (float) tvec.at<double>(2);
#endif

    return;
}

/* matrix multiplication for column order based matrix.
 */
void matrix_vec_mul_opengl(GLfloat mat[4*4], GLfloat in[4], GLfloat out[4]) {
    for(int i = 0; i < 4; i++) {
        out[i] = 0.0f;
        for(int j = 0; j < 4; j++) {
            out[i] += mat[4*j+i] * in[j];
        }
    }
    return;
}

/* matrix multiplication for row order based matrix.
 */
void matrix_vec_mul_opencv(GLfloat *mat, GLfloat *in, GLfloat *out, int matrow, int matcol) {
    for(int i = 0; i < matcol; i++) {
        out[i] = 0.0f;
        for(int j = 0; j < matrow; j++) {
            out[i] += mat[matrow*i+j] * in[j];
        }
    }
    return;
}


void Test1::OpenCvPipeSim(std::vector<cv::Mat> &images) {

    float in[4][4] = {
        {0, 0, 0, 1.0}, { 0, 100, 0, 1.0}, { 100, 100, 0, 1.0}, { 100, 0, 0, 1.0}
    };
    float out[4][3], out2[4][3];

    for(int i = 0; i < images.size() ; i++) {
    //for(int i = 0; i < 1; i++) {
        cv::Mat image = images[i];

        setCameraIntrinsicMatrix();
        setCameraExtrinsicMatrix(rvecs[i], tvecs[i]);


        // Camera extrinsic matrix compute
        for(int j = 0; j < 4; j++)
            matrix_vec_mul_opencv( CameraExtrinsicMatrix, in[j], out[j], 4, 3 );

        for(int j = 0; j < 4; j++) {
        for(int k = 0; k < 4; k++) {
            out_cv_3d[i][j][k] = out[j][k];
        }
        }


        // Perspective division
        for(int j = 0; j < 4; j++)
        for(int k = 0; k < 3; k++)
            out[j][k] /= out[j][2];

        // Perspective Projection matrix compute
        for(int j = 0; j < 4; j++)
            matrix_vec_mul_opencv( CameraIntrinsicMatrix, out[j], out2[j], 3, 3 );


        for(int j = 0; j < 4; j++) {
            std::cout << "@WND[" << j << "] = ";
            std::cout << out2[j][0] << "\t";
            std::cout << out2[j][1] << "\t";
            std::cout << std::endl;
        }
 
        for(int j = 0; j < 4; j++) {
        for(int k = 0; k < 4; k++) {
            out_cv[i][j][k] = out2[j][k];
            //out_cv_3d[i][j][k] = out[j][k];
        }
        }

        //PrintMatrix(projectMatrix, 4, 4, "ProjetionMatrix");
        //PrintMatrix(modelViewMatrix, 4, 4, "modelViewMatrix");

        /* Draw axis on the image.
         */
        //cv::line( image, cv::Point(out2[0][0], out2[0][1]), cv::Point(out2[1][0], out2[1][1]), cv::Scalar(255, 0, 0), 8 );
        //cv::line( image, cv::Point(out2[1][0], out2[1][1]), cv::Point(out2[2][0], out2[2][1]), cv::Scalar(0, 255, 0), 8 );
        //cv::line( image, cv::Point(out2[2][0], out2[2][1]), cv::Point(out2[3][0], out2[3][1]), cv::Scalar(0, 0, 255), 8 );
        //cv::line( image, cv::Point(out2[3][0], out2[3][1]), cv::Point(out2[0][0], out2[0][1]), cv::Scalar(255, 0, 0), 8 );


        /* Display image 
         */
        //cv::imshow("Window", image);
        //cv::waitKey(0);

        //cv::updateWindow("Window");
        //cv::waitKey(0);
    }

}

void Test1::OpenGlPipeSim(std::vector<cv::Mat> &images) {

    GLfloat in[4][4] = {
        {0, 0, 0, 1.0}, { 0, 100, 0, 1.0}, { 100, 100, 0, 1.0}, { 100, 0, 0, 1.0}
    };
    GLfloat out[4][4], out2[4][4];

    for(int i = 0; i < images.size() ; i++) {
    //for(int i = 0; i < 1; i++) {
        cv::Mat image = images[i];

        setProjectionMatrix();
        setModelViewMatrix(rvecs[i], tvecs[i]);


        // model-view matrix compute
        for(int j = 0; j < 4; j++)
            matrix_vec_mul_opengl( modelViewMatrix, in[j], out[j] );

        // Perspective Projection matrix compute
        for(int j = 0; j < 4; j++)
            matrix_vec_mul_opengl( projectMatrix, out[j], out2[j] );

        // Perspective division
        for(int j = 0; j < 4; j++)
        for(int k = 0; k < 4; k++)
            out2[j][k] /= out2[j][3];

        // Normalized device coordinate 
        for(int j = 0; j < 4; j++) {
            std::cout << "@NDC[" << j << "] = ";
            for(int k = 0; k < 4; k++) {
                std::cout << out2[j][k] << "\t";
            }
            std::cout << std::endl;
        }
        
        // Window view coordinate 
        for(int j = 0; j < 4; j++) {
            out2[j][0] = out2[j][0]*(1280/2) + (1280/2);
            out2[j][1] = out2[j][1]*( 720/2) + ( 720/2);
        }
 
        for(int j = 0; j < 4; j++) {
            std::cout << "@WND[" << j << "] = ";
            std::cout << out2[j][0] << "\t";
            std::cout << out2[j][1] << "\t";
            std::cout << std::endl;
        }

        /* Final output window position.
         */
        for(int j = 0; j < 4; j++) {
        for(int k = 0; k < 4; k++) {
            out_gl[i][j][k] = out2[j][k];
            out_gl_3d[i][j][k] = out[j][k];
        }
        }
 

        //PrintMatrix(projectMatrix, 4, 4, "ProjetionMatrix");
        //PrintMatrix(modelViewMatrix, 4, 4, "modelViewMatrix");

        /* Draw axis on the image.
         */
        //cv::line( image, cv::Point(out2[0][0], out2[0][1]), cv::Point(out2[1][0], out2[1][1]), cv::Scalar(255, 0, 0), 8 );
        //cv::line( image, cv::Point(out2[1][0], out2[1][1]), cv::Point(out2[2][0], out2[2][1]), cv::Scalar(0, 255, 0), 8 );
        //cv::line( image, cv::Point(out2[2][0], out2[2][1]), cv::Point(out2[3][0], out2[3][1]), cv::Scalar(0, 0, 255), 8 );
        //cv::line( image, cv::Point(out2[3][0], out2[3][1]), cv::Point(out2[0][0], out2[0][1]), cv::Scalar(255, 0, 0), 8 );


        /* Display image 
         */
        //cv::imshow("Window", image);
        //cv::waitKey(0);

        //cv::updateWindow("Window");
        //cv::waitKey(0);
    }

}

void Test1::DrawPipeSimResult(std::vector<cv::Mat> images) {

    for(int i = 0; i < images.size() ; i++) {
        cv::Mat image = images[i];

        /* Draw axis on the image.
         */
        cv::line( image, cv::Point(out_gl[i][0][0], out_gl[i][0][1]), cv::Point(out_gl[i][1][0], out_gl[i][1][1]), cv::Scalar(255, 0, 0), 8 );
        cv::line( image, cv::Point(out_gl[i][1][0], out_gl[i][1][1]), cv::Point(out_gl[i][2][0], out_gl[i][2][1]), cv::Scalar(0, 255, 0), 8 );
        cv::line( image, cv::Point(out_gl[i][2][0], out_gl[i][2][1]), cv::Point(out_gl[i][3][0], out_gl[i][3][1]), cv::Scalar(0, 0, 255), 8 );
        cv::line( image, cv::Point(out_gl[i][3][0], out_gl[i][3][1]), cv::Point(out_gl[i][0][0], out_gl[i][0][1]), cv::Scalar(255, 0, 0), 8 );

        cv::line( image, cv::Point(out_cv[i][0][0], out_cv[i][0][1]), cv::Point(out_cv[i][1][0], out_cv[i][1][1]), cv::Scalar(255, 255, 0), 3 );
        cv::line( image, cv::Point(out_cv[i][1][0], out_cv[i][1][1]), cv::Point(out_cv[i][2][0], out_cv[i][2][1]), cv::Scalar(0, 255, 255), 3 );
        cv::line( image, cv::Point(out_cv[i][2][0], out_cv[i][2][1]), cv::Point(out_cv[i][3][0], out_cv[i][3][1]), cv::Scalar(255, 0, 255), 3 );
        cv::line( image, cv::Point(out_cv[i][3][0], out_cv[i][3][1]), cv::Point(out_cv[i][0][0], out_cv[i][0][1]), cv::Scalar(255, 255, 0), 3 );


        std::cout << "============ CV vs GL @ transformed-world coordinate system =======\n";
        for(int j = 0; j < 4; j++) {
            std::cout << "@CV [";
            std::cout << out_cv_3d[i][j][0] << ",";
            std::cout << out_cv_3d[i][j][1] << ",";
            std::cout << out_cv_3d[i][j][2] << "]\t<> @GL [";
            std::cout << out_gl_3d[i][j][0] << ",";
            std::cout << out_gl_3d[i][j][1] << ",";
            std::cout << out_gl_3d[i][j][2] << "]\n";
        }
        std::cout << "============ CV vs GL @ Window coordinate system =======\n";
        for(int j = 0; j < 4; j++) {
            std::cout << "@CV [";
            std::cout << out_cv[i][j][0] << ",";
            std::cout << out_cv[i][j][1] << ",";
            std::cout << out_cv[i][j][2] << "]\t<> @GL [";
            std::cout << out_gl[i][j][0] << ",";
            std::cout << out_gl[i][j][1] << ",";
            std::cout << out_gl[i][j][2] << "]\n";
        }
 
        std::cout << std::endl;


        /* Display image 
         */
        cv::imshow("Window", image);
        cv::waitKey(0);
    }


}
