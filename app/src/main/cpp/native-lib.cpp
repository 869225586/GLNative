#include <jni.h>
#include <string>
#include "log/AndroidLog.h"

#include "egl/EglHelper.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "GLES2/gl2.h"
#include "egl/EglThread.h"
EglHelper *wlEglHelper = NULL;
ANativeWindow *nativeWindow = NULL;
EglThread *thread=NULL;
extern "C" JNIEXPORT jstring JNICALL
Java_com_kwai_video_uikit_anative_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_kwai_video_uikit_opengl_NativeOpengl_surfaceCreate(JNIEnv *env, jobject thiz,
                                                            jobject surface) {
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    thread= new EglThread();
    thread->onSurfaceCreate(nativeWindow);
/*
    wlEglHelper = new EglHelper();
    wlEglHelper->initEgl(nativeWindow);

    //opnegl
    glViewport(0, 0, 720, 1280);

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    wlEglHelper->swapBuffers();*/
}extern "C"
JNIEXPORT void JNICALL
Java_com_kwai_video_uikit_opengl_NativeOpengl_surfaceChange(JNIEnv *env, jobject thiz, jint i1,
                                                            jint i2) {
   thread->onSurfaceChange(i1,i2);
}extern "C"
JNIEXPORT jint JNICALL
Java_com_kwai_video_uikit_anative_MainActivity_sum(JNIEnv *env, jobject thiz) {
    int   a= 0 ;
    int b =2;
    int sum = a +b ;

    return sum;
}