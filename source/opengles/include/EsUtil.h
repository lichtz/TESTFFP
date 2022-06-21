//
// Created by zhaoy on 2022/4/16.
//


#ifndef LIVIDEO_ESUTIL_H
#define LIVIDEO_ESUTIL_H
#define ESUTIL_API
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>
#include <string>
#include <EGL/egl.h>
#include <EGL/eglext.h>
typedef struct ESContent ESContent;
struct ESContent{
    GLint width;
    GLint height;
    EGLNativeWindowType eglNativeWindowType;
    EGLNativeDisplayType eglNativeDisplayType;
    EGLDisplay  eglDisplay;
    EGLContext  eglContext;
    EGLSurface  eglSurface;
    void       *userData;
    GLuint textId;
    unsigned int VBO, VAO, EBO ,TEXTURES;
};
GLboolean ESUTIL_API esCreateWindow ( ESContent *esContext,GLint width, GLint height);
EGLint GetContextRenderableType ( EGLDisplay eglDisplay );
#endif //LIVIDEO_ESUTIL_H
