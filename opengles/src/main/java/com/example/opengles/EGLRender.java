package com.example.opengles;

import android.view.Surface;

public class EGLRender {
    public native int initEGL(Surface surface);
    public native void setViewPort(int w,int h);
    public native void egl_DRAW();
    public native void egl_Destory();

    static {
        System.loadLibrary("openglesExx");
    }
}
