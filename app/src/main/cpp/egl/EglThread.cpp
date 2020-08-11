//
// Created by edz on 2020/7/30.
//

#include "EglThread.h"

EglThread::EglThread() {
    ////  使用锁 对象创建 首先初始化这个锁 和 信号
    pthread_mutex_init(&pthreadMutex, NULL);
    pthread_cond_init(&pthreadCond, NULL);
}

EglThread::~EglThread() {
    //// 析构函数  销毁锁和信号
    pthread_mutex_destroy(&pthreadMutex);
    pthread_cond_destroy(&pthreadCond);
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
            if (eglThread->isChange) {
                LOGD("eglthread call surfaceChange");
                eglThread->isChange = false;
                eglThread->isStart = true;
                //调用change 回调函数  参数为获取到的width 和 height
                eglThread->change(eglThread->surfaceWidth, eglThread->surfaceHeight,
                                  eglThread->onChangeCtx);
            }

            if(eglThread->isChangeFiliter){
                LOGD("eglThread call Filter");
                eglThread->isChangeFiliter= false;
                eglThread->changeFilter(eglThread->surfaceWidth,eglThread->surfaceHeight,eglThread->onFilterCtx);
            }

            if (eglThread->isStart) {
                eglThread->draw(eglThread->onDrawCtx);
                eglHelper->swapBuffers();
                LOGD("draw");
            }
            //// 区分自动刷新和手动刷新
            if (eglThread->renderType == RENDER_AUTO) {
                usleep(1000000 / 60);//睡眠一会 一秒 执行60次  60 帧
            } else {
                //// 手动刷新只执行一次 swap  之后就阻塞 只有手动 解锁才能继续执行
                //1 上锁
                // 2 信号等待 （线程会阻塞在这里 直到收到信号 往下执行 第 3 步 ）
                // 3  释放锁
                pthread_mutex_lock(&eglThread->pthreadMutex);
                pthread_cond_wait(&eglThread->pthreadCond, &eglThread->pthreadMutex);
                pthread_mutex_unlock(&eglThread->pthreadMutex);
            }

            if (eglThread->isExit) {
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
    isChange = true;
    surfaceHeight = height;
    surfaceWidth = width;
    LOGD("size width is %d height is %d", surfaceWidth, surfaceHeight);
    if (renderType == RENDER_HADNLE) {
        notifyRender();//如果是手动刷新那么当surface尺寸发生变化时 要重新 唤醒绘制一次 。
    }
}

//将传入的回调函数 设置给 成员变量
void EglThread::setCreateCallBack(EglThread::onCreate create, void *ctx) {
    this->create = create;
    this->onCreateCtx = ctx;
}

void EglThread::setChangeCallBack(EglThread::onChange change, void *ctx) {
    this->change = change;
    this->onChangeCtx = ctx;
}

void EglThread::setDrawCallBack(EglThread::onDraw draw, void *ctx) {
    this->draw = draw;
    this->onDrawCtx = ctx;
}

void EglThread::setRenderType(int renderType) {
    this->renderType = renderType;
}

/**
 * 唤醒锁
 * 1 首先要拿到锁
 * 2 发送信号解锁
 * 3 解锁
 */
void EglThread::notifyRender() {
    /**  方法需要锁的指针 对象
     *   & 表示引用 。引用这个变量的 就是他的指针
     */
    pthread_mutex_lock(&pthreadMutex);

    pthread_cond_signal(&pthreadCond);

    pthread_mutex_unlock(&pthreadMutex);


}

void EglThread::destroy() {

}


void EglThread::setChangeFilterCallBack(EglThread::onChangeFilter changeFilter, void *ctx) {
     this->changeFilter=changeFilter ;
     this->onFilterCtx =ctx;
}

void EglThread::startChangeFilter() {
    isChangeFiliter=true;
    notifyRender();
}
