/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __TEST1_HPP__
#define __TEST1_HPP__

#include <GL/gl.h>
#include <GL/glu.h>

#include "types.hpp"
#include "job.hpp"

class Test1;

typedef struct _opengl_draw_callback_data {
    Test1 *handle;
    cv::ogl::Texture2D *backgroundTex;
} ogl_draw_data;

class Test1 : public Job {
public:
    Test1(std::string name, program_args &pargs);
    ~Test1(void) {}
    virtual bool Run(job_context &context);
    virtual bool Stop(void);
    void opengl_draw_callback(void *param);
    void setProjectionMatrix(void);
    void setModelViewMatrix(cv::Mat &rvec, cv::Mat &tvec);
    void PrintMatrix(GLfloat *mat, int row, int col, const std::string &msg);
    void OpenGlPipeSim(std::vector<cv::Mat> &images);
    void OpenCvPipeSim(std::vector<cv::Mat> &images);
    void DrawPipeSimResult(std::vector<cv::Mat> images);

    void setCameraIntrinsicMatrix(void);
    void setCameraExtrinsicMatrix(cv::Mat &rvec, cv::Mat &tvec);
private:
    /* Camera Intrinsic parameters
     */
    float focal_x, focal_y;
    float screen_x, screen_y;
    float camera_cx, camera_cy;

    /* OpenGL/OpenCV Pipe simulation output
     */
    float out_gl[5][4][4], out_cv[5][4][4];
    float out_gl_3d[5][4][4], out_cv_3d[5][4][4];

    int   square_size = 100;
    GLfloat projectMatrix[4*4];
    GLfloat modelViewMatrix[4*4];
    float   CameraIntrinsicMatrix[3*3];
    float   CameraExtrinsicMatrix[4*3];
    void TestRun0(job_context &context);
    void TestRun1(job_context &context);
    program_args  &_pargs;
    std::string    _iflist;

    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
};

static void opengl_draw_callback_wrapper(void *_param) {
    ogl_draw_data *param = (ogl_draw_data *) _param;
    param->handle->opengl_draw_callback( (void*)param->backgroundTex );
}

#endif
