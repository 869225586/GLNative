//
// Created by edz on 2020/8/11.
//

#ifndef NATIVE_OPENGL_H
#define NATIVE_OPENGL_H

#include "../egl/EglThread.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "../filter/FilterOne.h"
#include "../filter/WlFilterTwo.h"
#include "../filter/FilterYuv.h"
#include "../vbo/FilterVboYuv.h"
#include "../vbo/CameraRender.h"
#include "../vbo/MediaCodecRender.h"

#include "../ffmpeg/FFmpeg.h"
class Opengl {
public:
    EglThread *wlEglThread = NULL;
    ANativeWindow *nativeWindow = NULL;
    BaseRender *baseRender = NULL;
    BaseRender *mediaCodecRen = NULL;

    FFmpeg *ffmpeg = NULL;

    int pic_width;
    int pic_height;
    void *pilex = NULL;
    bool fullscreen;

public:
    Opengl(CallJava *callJava);

    ~Opengl();

    void onCreateSurface(JNIEnv *env, jobject surface);

    void onChangeSurface(int width, int height);

    void onDestorySurface();

    void onChangeFilter();

    void setYuvData(void *y, void *u, void *v, int w, int h);

    void setPilex(void *data, int width, int height, int length);

    void preparedFromFFmpeg(PlayStatus *playStatus,CallJava *callJava);

    void setUrl(const char *url);

    void start();

    void pause();

    void resume();

    void changeFullScreen(bool fuscreen);

    long getDuration();

    void seek(int mills);
};


#endif //NATIVE_OPENGL_H
