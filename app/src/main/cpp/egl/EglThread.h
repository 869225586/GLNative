//
// Created by edz on 2020/7/30.
//

#ifndef NATIVE_EGLTHREAD_H
#define NATIVE_EGLTHREAD_H

#include "pthread.h"
#include <EGL/eglplatform.h>
#include "pthread.h"
#include "android/native_window.h"
#include "EglHelper.h"
#include <unistd.h>
#include "GLES2/gl2.h"

class EglThread {
public: pthread_t pthread=-1;
        ANativeWindow *nativeWindow = NULL;
        bool isCreate = false;
        bool isChange = false;
        bool isExit = false;
        bool isStart = false ;

        bool surfaceWidth = 0;
        bool surfaceHeight =0 ;

        typedef void (*onCreate)(void *);//定义回调函数 变量名称为onCreate  参数类型为void *  == java 的object
        onCreate  create; //成员变量用于赋值
        void * onCreateCtx; //回调的参数 声明成员变量 用于赋值

        typedef void (*onChange)(int width,int height,void *);
        onChange change;
        void * onChangeCtx;

        typedef void (*onDraw)(void *);
        onDraw draw;
        void *onDrawCtx;


public:
    EglThread();
    ~EglThread();

    void onSurfaceCreate(EGLNativeWindowType windowType);

    void onSurfaceChange(int width ,int height);

    void callBackOnCreate(onCreate create,void* ctx);

    void callBackOnChange(onChange change,void * ctx);

    void callBackOnDraw(onDraw draw,void *ctx);

    };


#endif //NATIVE_EGLTHREAD_H
