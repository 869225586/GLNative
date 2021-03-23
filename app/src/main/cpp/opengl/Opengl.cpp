//
// Created by edz on 2020/8/11.
//

#include "Opengl.h"
#include "../vbo/CameraRender.h"
#include "../vbo/MediaCodecRender.h"


void callback_yuv(void *y, void *u, void *v, int width, int height, void *ctx) {
    Opengl *opengl = static_cast<Opengl *>(ctx);
    opengl->setYuvData(y, u, v, width, height);
}

void callback_SurfaceCrete(void *ctx) {
    Opengl *wlOpengl = static_cast<Opengl *>(ctx);

    if (wlOpengl != NULL) {
        if (wlOpengl->baseRender != NULL) {
            wlOpengl->baseRender->onCreate();
        }
    }
    LOGD("toPlay");
//    if(wlOpengl->ffmpeg!=NULL){
//        wlOpengl->ffmpeg->preapared();
//        wlOpengl->ffmpeg->videoPlayer->setCallYUV(callback_yuv, wlOpengl);
//    }
}

void callback_SurfacChange(int width, int height, void *ctx) {
    Opengl *wlOpengl = static_cast<Opengl *>(ctx);
    if (wlOpengl != NULL) {
        if (wlOpengl->baseRender != NULL) {
            wlOpengl->baseRender->onChange(width, height);
        }
    }
}

void callback_SurfaceDraw(void *ctx) {

    Opengl *wlOpengl = static_cast<Opengl *>(ctx);
    if (wlOpengl != NULL) {
        if (wlOpengl->baseRender != NULL) {
            wlOpengl->baseRender->onDraw();
        }
    }
}

void callback_Filter(int width, int height, void *ctx) {
    Opengl *wlOpengl = static_cast<Opengl *>(ctx);
    if (wlOpengl != NULL) {
        //切换滤镜 先删除之前的资源   然后加载第二个shader
        if (wlOpengl->baseRender != NULL) {
            LOGD("释放上一个filter");
            wlOpengl->baseRender->destroyGl();
            wlOpengl->baseRender->destroyData();
            delete wlOpengl->baseRender;
            wlOpengl->baseRender = NULL;
        }
        wlOpengl->baseRender = new WlFilterTwo();
        wlOpengl->baseRender->onCreate();
        wlOpengl->baseRender->onChange(width, height);
        wlOpengl->baseRender->setPilex(wlOpengl->pilex, wlOpengl->pic_width, wlOpengl->pic_height,
                                       0);
        wlOpengl->wlEglThread->notifyRender();

    }
}

void callback_SurfaceDestroy(void *ctx) {

    Opengl *wlOpengl = static_cast<Opengl *>(ctx);
    if (wlOpengl != NULL) {
        if (wlOpengl->baseRender != NULL) {
            wlOpengl->baseRender->destroyGl();
        }
    }
}

void Opengl::onCreateSurface(JNIEnv *env, jobject surface) {
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    wlEglThread = new EglThread();
    wlEglThread->setRenderType(RENDER_AUTO);
    wlEglThread->setCreateCallBack(callback_SurfaceCrete, this);
    wlEglThread->setChangeCallBack(callback_SurfacChange, this);
    wlEglThread->setDrawCallBack(callback_SurfaceDraw, this);
    wlEglThread->setChangeFilterCallBack(callback_Filter, this);
    wlEglThread->setDestroyCallBack(callback_SurfaceDestroy, this);

    wlEglThread->onSurfaceCreate(nativeWindow);
    LOGD("surfaceCreate ");
}

void Opengl::onChangeSurface(int width, int height) {
    if (wlEglThread != NULL) {
        if (baseRender != NULL) {
            baseRender->surface_width = width;
            baseRender->surface_height = height;
        }
        wlEglThread->onSurfaceChange(width, height);
    }
}

void Opengl::onDestorySurface() {
    if (wlEglThread != NULL) {
        wlEglThread->destroy();
    }

    if (baseRender != NULL) {
        baseRender->destroyData();
        delete baseRender;
        baseRender = NULL;
    }
    if (nativeWindow != NULL) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = NULL;
    }
    if (ffmpeg != NULL) {
        ffmpeg->release();
        ffmpeg = NULL;
    }
}

void Opengl::setPilex(void *data, int width, int height, int length) {
    pic_width = width;
    pic_height = height;
    if (pilex != NULL) {
        free(pilex);
        pilex = NULL;
    }
    pilex = malloc(length);
    memcpy(pilex, data, length);
    if (baseRender != NULL) {
        baseRender->setPilex(pilex, width, height, length);
    }
    if (wlEglThread != NULL) {
        wlEglThread->notifyRender();
    }

}

Opengl::~Opengl() {

}

void Opengl::onChangeFilter() {
    if (wlEglThread != NULL) {
        wlEglThread->startChangeFilter();
    }
}

void Opengl::setYuvData(void *y, void *u, void *v, int w, int h) {
    if (baseRender != NULL) {
        baseRender->setYuvData(y, u, v, w, h);
    }
    if (wlEglThread != NULL) {
        wlEglThread->notifyRender();
    }
}

void Opengl::preparedFromFFmpeg(PlayStatus *playStatus, CallJava *callJava) {
    ffmpeg = new FFmpeg(playStatus, callJava);
    ffmpeg->videoPlayer->setCallYUV(callback_yuv, this);
    ffmpeg->preapared();
    LOGD("ffmpegPrepare");
}

Opengl::Opengl(CallJava *callJava) {
    LOGD("initOpenGl");
//    baseRender = new FilFilterVboYuv();
//    baseRender = new CameraRender(callJava);
     baseRender = new MediaCodecRender(callJava);
}

void Opengl::pause() {
   if(ffmpeg!=NULL){
       ffmpeg->pause();
   }
}

void Opengl::resume() {
    if(ffmpeg!=NULL){
        ffmpeg->resume();
    }

}

void Opengl::changeFullScreen(bool fuscreen) {
     if(baseRender!=NULL){
         baseRender->changeFullScreen(fuscreen);
     }
}

long Opengl::getDuration() {
    return ffmpeg->duration;
}

void Opengl::seek(long mis) {
    if(ffmpeg!=NULL){
        ffmpeg->seek(mis);
    }
}

void Opengl::setUrl(const char *url) {
     if(ffmpeg!=NULL){
         ffmpeg->setUrl(url);
     }
}

void Opengl::start() {
    if(ffmpeg!=NULL){
      ffmpeg->start();
    }
}


