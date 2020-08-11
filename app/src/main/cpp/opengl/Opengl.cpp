//
// Created by edz on 2020/8/11.
//

#include "Opengl.h"

void callback_SurfaceCrete(void *ctx)
{

    Opengl *wlOpengl = static_cast<Opengl *>(ctx);

    if(wlOpengl != NULL)
    {
        if(wlOpengl->baseOpengl != NULL)
        {
            wlOpengl->baseOpengl->onCreate();
        }
    }
}

void callback_SurfacChange(int width, int height, void *ctx)
{
    Opengl *wlOpengl = static_cast<Opengl *>(ctx);
    if(wlOpengl != NULL)
    {
        if(wlOpengl->baseOpengl != NULL)
        {
            wlOpengl->baseOpengl->onChange(width, height);
        }
    }
}

void callback_SurfaceDraw(void *ctx)
{

    Opengl *wlOpengl = static_cast<Opengl *>(ctx);
    if(wlOpengl != NULL)
    {
        if(wlOpengl->baseOpengl != NULL)
        {
            wlOpengl->baseOpengl->onDraw();
        }
    }
}

void Opengl::onCreateSurface(JNIEnv *env, jobject surface) {
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    wlEglThread = new EglThread();
    wlEglThread->setRenderType(RENDER_HADNLE);
    wlEglThread->callBackOnCreate(callback_SurfaceCrete, this);
    wlEglThread->callBackOnChange(callback_SurfacChange, this);
    wlEglThread->callBackOnDraw(callback_SurfaceDraw, this);

    baseOpengl = new FilterOne();
    wlEglThread->onSurfaceCreate(nativeWindow);
}

void Opengl::onChangeSurface(int width, int height) {
    if (wlEglThread != NULL) {
        if (baseOpengl != NULL) {
            baseOpengl->surface_width = width;
            baseOpengl->surface_height = height;
        }
        wlEglThread->onSurfaceChange(width, height);
    }
}

void Opengl::onDestorySurface() {
    if (wlEglThread != NULL) {
        wlEglThread->destroy();
    }
    if (baseOpengl != NULL) {
        baseOpengl->destroy();
        delete baseOpengl;
        baseOpengl = NULL;
    }
    if (nativeWindow != NULL) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = NULL;
    }
}

void Opengl::setPilex(void *data, int width, int height, int length) {
    pic_width = width;
    pic_height = height;
    LOGD("设置图片数据0");
    pilex = malloc(length);
    memcpy(pilex, data, length);
    LOGD("设置图片数据1");
    if (baseOpengl != NULL) {
        LOGD("设置图片数据2");
        baseOpengl->setPilex(pilex, width, height, length);
    }
    if (wlEglThread != NULL) {
        wlEglThread->notifyRender();
    }

}

Opengl::Opengl() {

}

Opengl::~Opengl() {

}
