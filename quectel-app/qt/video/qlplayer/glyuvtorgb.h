#ifndef GLYUVWIDGET_H
#define GLYUVWIDGET_H
 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3extQCOM.h>
#include <GLES/glext.h>
 
class GLYuvToRgb
{
public:
    GLYuvToRgb(int width, int height);
    ~GLYuvToRgb();
    unsigned char* slotYuv(unsigned char *frame, int w, int h);
    void initializeGL();
 
private:
    unsigned int videoW,videoH;
    GLuint pgm;
    GLint vertexIn, textureIn;
    GLuint textureUniformY, textureUniformU, textureUniformV, textureUniformUV;
    GLuint idY,idU, idV,idUV;
    GLuint fbo, rbo;
    EGLDisplay eDisplay;
    EGLSurface eSurface;

    unsigned char *pixels;
};
 
#endif 
