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
void callback_Filter(int width, int height, void *ctx)
{
    Opengl *wlOpengl = static_cast<Opengl *>(ctx);
    if(wlOpengl != NULL)
    {
        //切换滤镜 先删除之前的资源   然后加载第二个shader
        if(wlOpengl->baseOpengl != NULL)
        {
            wlOpengl->baseOpengl->destroy();
            delete wlOpengl->baseOpengl;
            wlOpengl->baseOpengl = NULL;
        }
        wlOpengl->baseOpengl=new WlFilterTwo();
        wlOpengl->baseOpengl->onCreate();
        wlOpengl->baseOpengl->onChange(width,height);
        wlOpengl->baseOpengl->setPilex(wlOpengl->pilex, wlOpengl->pic_width, wlOpengl->pic_height, 0);
        wlOpengl->wlEglThread->notifyRender();

    }
}

void Opengl::onCreateSurface(JNIEnv *env, jobject surface) {
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    wlEglThread = new EglThread();
    wlEglThread->setRenderType(RENDER_HADNLE);
    wlEglThread->setCreateCallBack(callback_SurfaceCrete, this);
    wlEglThread->setChangeCallBack(callback_SurfacChange, this);
    wlEglThread->setDrawCallBack(callback_SurfaceDraw, this);
    wlEglThread->setChangeFilterCallBack(callback_Filter,this);

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
    pilex = malloc(length);
    memcpy(pilex, data, length);
    if (baseOpengl != NULL) {
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

void Opengl::onChangeFilter() {
   if(wlEglThread!=NULL){
       wlEglThread->startChangeFilter();
   }
}
