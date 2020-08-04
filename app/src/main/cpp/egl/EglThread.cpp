//
// Created by edz on 2020/7/30.
//

#include "EglThread.h"

EglThread::EglThread() {

}

EglThread::~EglThread() {

}

void *eglThreadImpl(void *context) {
    EglThread *eglThread = static_cast<EglThread *>(context);
    if (eglThread != NULL) {
        EglHelper *eglHelper = new EglHelper();
        eglHelper->initEgl(eglThread->nativeWindow);
        eglHelper->isExit = false;
        while (true) {
            if (eglThread->isCreate) {
                LOGD("eglthread call surfaceCreate");
                eglThread->isCreate = false;
                eglThread->create(eglThread->onCreateCtx);//调用传入的回调函数 方法
            }
             if(eglThread->isChange){
                 LOGD("eglthread call surfaceChange");
                 eglThread->isChange= false;
                 eglThread->isStart= true;
                 //调用change 回调函数  参数为获取到的width 和 height
                 eglThread->change(eglThread->surfaceWidth,eglThread->surfaceHeight,eglThread->onChangeCtx);
             }
//            LOGD("draw");
             if(eglThread->isStart){
                 eglThread->draw(eglThread->onDrawCtx);
                 eglHelper->swapBuffers();
             }
             usleep(1000000/60);//睡眠一会 一秒 执行60次  60 帧
             if(eglThread->isExit){
                 break;
             }
        }
    }
    return 0;
}

void EglThread::onSurfaceCreate(EGLNativeWindowType windowType) {
    if (pthread == -1) {
        isCreate = true;
        nativeWindow = windowType;
        pthread_create(&pthread, NULL, eglThreadImpl, this);
    }
}

void EglThread::onSurfaceChange(int width, int height) {
     isChange  = true;
     surfaceHeight =height;
     surfaceWidth = width;
}
//将传入的回调函数 设置给 成员变量
void EglThread::callBackOnCreate(EglThread::onCreate create, void *ctx) {
     this->create=create;
     this->onCreateCtx=ctx;
}

void EglThread::callBackOnChange(EglThread::onChange change, void *ctx) {
     this->change=change;
     this->onChangeCtx=ctx;
}

void EglThread::callBackOnDraw(EglThread::onDraw draw, void *ctx) {
     this->draw=draw;
     this->onDrawCtx=ctx;
}
