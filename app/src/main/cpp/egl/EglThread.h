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

#define RENDER_AUTO 1
#define RENDER_HADNLE 2

class EglThread {
public:
    pthread_t pthread = -1;
    ANativeWindow *nativeWindow = NULL;
    bool isCreate = false;
    bool isChange = false;
    bool isExit = false;
    bool isStart = false;
    bool isChangeFiliter = false;//更新滤镜标识位

    int surfaceWidth = 0;
    int surfaceHeight = 0;

    typedef void (*onCreate)(void *);//定义回调函数 变量名称为onCreate  参数类型为void *  == java 的object
    onCreate create; //声明方法变量用于赋值 方法对象
    void *onCreateCtx; //回调的参数 声明成员变量 用于赋值

    typedef void (*onChange)(int width, int height, void *);

    onChange change;
    void *onChangeCtx;

    typedef void (*onDraw)(void *);

    onDraw draw;
    void *onDrawCtx;

    typedef void (*onChangeFilter)(int width, int height, void *); //更新滤镜 回调方法

    onChangeFilter changeFilter;
    void *onFilterCtx;

    typedef void(*OnDestroy)(void *);

    OnDestroy onDestroy;
    void *onDestroyctx;


    int renderType = RENDER_AUTO;//默认是自动刷新

    pthread_mutex_t pthreadMutex; //线程锁

    pthread_cond_t pthreadCond;  //线程锁条件 用于接收释放信号

public:
    EglThread();

    ~EglThread();

    void onSurfaceCreate(EGLNativeWindowType windowType);

    void onSurfaceChange(int width, int height);

    void setCreateCallBack(onCreate create, void *ctx);

    void setChangeCallBack(onChange change, void *ctx);

    void setDrawCallBack(onDraw draw, void *ctx);

    void setChangeFilterCallBack(onChangeFilter changeFilter, void *ctx);

    void setDestroyCallBack(OnDestroy destory, void *ctx);

    void setRenderType(int renderType);//设置渲染模式

    void notifyRender();//用于手动渲染模式 的 更新功能

    void startChangeFilter();//开始切换滤镜

    void destroy();


};


#endif //NATIVE_EGLTHREAD_H
