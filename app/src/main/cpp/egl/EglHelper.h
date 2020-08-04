//
// Created by edz on 2020/7/29.
//

#ifndef NATIVE_EGLHELPER_H
#define NATIVE_EGLHELPER_H

#include "../log/AndroidLog.h"
#include "EGL/egl.h"
class EglHelper {
public: EGLDisplay mEglDisplay ;
        EGLSurface mEglSurface;
        EGLConfig  mEglConfig;
        EGLContext  mEglContext;
        bool        isExit=false;

public:
    EglHelper();
    ~EglHelper();

    int initEgl(EGLNativeWindowType windowType);
    int swapBuffers();
    void destoryEgl();

};


#endif //NATIVE_EGLHELPER_H
