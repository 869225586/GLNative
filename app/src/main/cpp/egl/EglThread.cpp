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
            }
             if(eglThread->isChange){
                 LOGD("eglthread call surfaceChange");
                 eglThread->isChange= false;
                 glViewport(0,0,720,1280);
                 eglThread->isStart= true;
             }
//            LOGD("draw");
             if(eglThread->isStart){
                 glClearColor(0.0f,1.0f,1.0f,1.0f);
                 glClear(GL_COLOR_BUFFER_BIT);
                 eglHelper->swapBuffers();
             }
             usleep(1000000/60);
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
