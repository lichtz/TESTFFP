//
// Created by zhaoy on 2022/4/16.
//
#include <android/native_window.h>
#include "./include/EsUtil.h"

EGLint GetContextRenderableType ( EGLDisplay eglDisplay ){
    const char *extensions = eglQueryString( eglDisplay,EGL_EXTENSIONS);
    if ( extensions != NULL && strstr( extensions, "EGL_KHR_create_context" ) )
    {
        return EGL_OPENGL_ES3_BIT_KHR;
    }
    return EGL_OPENGL_ES2_BIT;
}
GLboolean ESUTIL_API esCreateWindow ( ESContent *esContext,GLint width, GLint height){
    EGLConfig config;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    esContext->eglDisplay = eglGetDisplay(esContext->eglNativeDisplayType);
    if ( esContext->eglDisplay == EGL_NO_DISPLAY )
    {
        return GL_FALSE;
    }
    if ( !eglInitialize ( esContext->eglDisplay, &majorVersion, &minorVersion ) )
    {
        return GL_FALSE;
    }

    {
        EGLint numConfigs = 0;
        EGLint attribList[] =
                {
                        EGL_RED_SIZE,       8,
                        EGL_GREEN_SIZE,     8,
                        EGL_BLUE_SIZE,      8,
                        EGL_ALPHA_SIZE,     8 ,
                        EGL_DEPTH_SIZE,     8 ,
                        EGL_STENCIL_SIZE,   8 ,
                        EGL_RENDERABLE_TYPE, GetContextRenderableType ( esContext->eglDisplay ),
                        EGL_NONE
                };


        if ( !eglChooseConfig ( esContext->eglDisplay, attribList, &config, 1, &numConfigs ) )
        {
            return GL_FALSE;
        }

        if ( numConfigs < 1 )
        {
            return GL_FALSE;
        }
    }

    {
        EGLint format = 0;
        eglGetConfigAttrib(esContext->eglDisplay,config, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry ( esContext->eglNativeWindowType, 0, 0, format );

    }

    {
        esContext->eglSurface = eglCreateWindowSurface(esContext->eglDisplay,config,esContext->eglNativeWindowType,
                                                       nullptr);
        if ( esContext->eglSurface == EGL_NO_SURFACE )
        {
            return GL_FALSE;
        }
    }
    {
        esContext->eglContext = eglCreateContext(esContext->eglDisplay,config,EGL_NO_CONTEXT,contextAttribs);
        if ( esContext->eglContext == EGL_NO_CONTEXT )
        {
            return GL_FALSE;
        }
        if ( !eglMakeCurrent ( esContext->eglDisplay, esContext->eglSurface,
                               esContext->eglSurface, esContext->eglContext ) )
        {
            return GL_FALSE;
        }
    }
    return EGL_TRUE;
}

