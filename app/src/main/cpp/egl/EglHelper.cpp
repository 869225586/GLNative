//
// Created by edz on 2020/7/29.
//

#include <pthread.h>
#include "EglHelper.h"

int EglHelper::initEgl(EGLNativeWindowType windowType) {
    mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if(mEglDisplay == EGL_NO_DISPLAY){
        LOGD("egl Display error");
        return -1;
    }
    EGLint *version = new EGLint[2];
    if (!eglInitialize(mEglDisplay,&version[0],&version[1])){
        LOGD("eglInitialize error");
        return -1;
    }

    const EGLint  attribs[] ={
            EGL_RED_SIZE,8,
            EGL_GREEN_SIZE,8,
            EGL_BLUE_SIZE,8,
            EGL_ALPHA_SIZE,8,
            EGL_DEPTH_SIZE,8,
            EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };
    EGLint  num_config;
    if(!eglChooseConfig(mEglDisplay,attribs,NULL,1,&num_config)){
        return -1;
    }
    //4、
    if(!eglChooseConfig(mEglDisplay, attribs, &mEglConfig, num_config, &num_config))
    {
        LOGD("eglChooseConfig  error 2");
        return -1;
    }
    int attrib_list[] = {
            EGL_CONTEXT_CLIENT_VERSION,2,
            EGL_NONE
    };
    mEglContext =eglCreateContext(mEglDisplay,mEglConfig,EGL_NO_CONTEXT,attrib_list);
    if(mEglContext == EGL_NO_CONTEXT){
        LOGD("eglcreatecontext error");
        return -1;
    }
    mEglSurface = eglCreateWindowSurface(mEglDisplay,mEglConfig,windowType,NULL);
    if(mEglSurface == EGL_NO_SURFACE){
        LOGD("eglcreatewindowsurface error");
        return -1;
    }
    if(!eglMakeCurrent(mEglDisplay,mEglSurface,mEglSurface,mEglContext)){
        LOGD("eglMakecurrent error");
        return -1;
    }
    LOGD("egl init success");

    return 0;
}

int EglHelper::swapBuffers() {
    if(mEglDisplay!=EGL_NO_DISPLAY){
        if(eglSwapBuffers(mEglDisplay,mEglSurface)){
            return 0;
        }
    }
    return -1;
}

void EglHelper::destoryEgl() {
    if(mEglDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    if(mEglDisplay != EGL_NO_DISPLAY && mEglSurface != EGL_NO_SURFACE)
    {
        eglDestroySurface(mEglDisplay, mEglSurface);
        mEglSurface = EGL_NO_SURFACE;
    }
    if(mEglDisplay != EGL_NO_DISPLAY && mEglContext != EGL_NO_CONTEXT){
        eglDestroyContext(mEglDisplay, mEglContext);
        mEglContext = EGL_NO_CONTEXT;
    }
    if(mEglDisplay != EGL_NO_DISPLAY)
    {
        eglTerminate(mEglDisplay);
        mEglDisplay = EGL_NO_DISPLAY;
    }
}

EglHelper::EglHelper() {
   mEglDisplay = EGL_NO_DISPLAY;
   mEglSurface = EGL_NO_SURFACE;
   mEglContext = EGL_NO_CONTEXT;
   mEglConfig = NULL;

}

EglHelper::~EglHelper() {

}
