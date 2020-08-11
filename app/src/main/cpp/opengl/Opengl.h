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

#include "BaseRender.h"

class Opengl {
public:
    EglThread *wlEglThread = NULL;
    ANativeWindow *nativeWindow = NULL;
    BaseRender *baseOpengl = NULL;

    int pic_width;
    int pic_height;
    void *pilex = NULL;

public:
    Opengl();
    ~Opengl();

    void onCreateSurface(JNIEnv *env, jobject surface);

    void onChangeSurface(int width, int height);

    void onDestorySurface();

    void onChangeFilter();

    void setPilex(void *data, int width, int height, int length);
};


#endif //NATIVE_OPENGL_H
