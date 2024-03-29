#include <jni.h>
#include <string>
#include "log/AndroidLog.h"

#include "opengl/Opengl.h"
#include "ffmpeg/PlayStatus.h"
#include "calljava/CallJava.h"

Opengl *opengl = NULL;
PlayStatus *playStatus = NULL;
_JavaVM *javaVM = NULL;
CallJava *callJava = NULL;
extern "C" JNIEXPORT jstring JNICALL
Java_com_sunyeyu_video_uikit_anative_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_surfaceCreate(JNIEnv *env, jobject thiz,
                                                            jobject surface) {
    if (callJava == NULL) {
        callJava = new CallJava(javaVM, env, &thiz);
    }
    if (opengl == NULL) {
        opengl = new Opengl(callJava);
    }
    opengl->onCreateSurface(env, surface);

}extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_surfaceChange(JNIEnv *env, jobject thiz, jint i1,
                                                            jint i2) {
    opengl->onChangeSurface(i1, i2);
}extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_imgData(JNIEnv *env, jobject thiz, jint width,
                                                      jint height,
                                                      jint length, jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);

    opengl->setPilex(data, width, height, length);

    env->ReleaseByteArrayElements(data_, data, 0); //释放data 与data_
}extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_surfaceDestroy(JNIEnv *env, jobject thiz) {
    opengl->onDestorySurface();
    delete opengl;
    opengl = NULL;
    //destroy 一定要将 变量的都删除 值为 null 不然 进场不杀死 activity 退出这个变量还存在
    //造成很多问题
    delete playStatus;
    playStatus = NULL;

    delete callJava;
    callJava = NULL;

}extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_changeFilter(JNIEnv *env, jobject thiz) {
    opengl->onChangeFilter();

}extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_setYuvData(JNIEnv *env, jobject thiz, jbyteArray y_,
                                                         jbyteArray u_, jbyteArray v_, jint w,
                                                         jint h) {
    jbyte *y = env->GetByteArrayElements(y_, NULL);
    jbyte *u = env->GetByteArrayElements(u_, NULL);
    jbyte *v = env->GetByteArrayElements(v_, NULL);

    // TODO
    if (opengl != NULL) {
        opengl->setYuvData(y, u, v, w, h);
    }

    env->ReleaseByteArrayElements(y_, y, 0);
    env->ReleaseByteArrayElements(u_, u, 0);
    env->ReleaseByteArrayElements(v_, v, 0);
}
extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {

        return result;
    }
    return JNI_VERSION_1_4;

}
extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_playFromFFmpeg(JNIEnv *env, jobject thiz,
                                                             jstring url) {
    if (playStatus == NULL) {
        playStatus = new PlayStatus();
    }
    if (callJava == NULL) {

        callJava = new CallJava(javaVM, env, &thiz);
    }
    if (opengl == NULL) {
        opengl = new Opengl(callJava);
    }
    const char *source = env->GetStringUTFChars(url, 0);
    opengl->preparedFromFFmpeg(playStatus, callJava);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_pause(JNIEnv *env, jobject thiz) {
    if (opengl == NULL) {
        opengl = new Opengl(callJava);
    }
    opengl->pause();
}extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_resume(JNIEnv *env, jobject thiz) {
    if (opengl == NULL) {
        opengl = new Opengl(callJava);
    }
    opengl->resume();
}extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_changewindow(JNIEnv *env, jobject thiz,
                                                              jboolean fullscreen) {
    if(opengl!=NULL){
        opengl->changeFullScreen(fullscreen);
    }
}extern "C"
JNIEXPORT jlong JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_getDuration(JNIEnv *env, jobject thiz) {
    if(opengl!=NULL){
        return opengl->getDuration();
    }
    return 0;
}extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_seek(JNIEnv *env, jobject thiz, jint second) {
    if(opengl!=NULL){
        opengl->seek(second);
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_setUrl(JNIEnv *env, jobject thiz, jstring url) {
    if(opengl!=NULL){
        const char *source = env->GetStringUTFChars(url, 0);
        opengl->setUrl(source);
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_sunyeyu_video_uikit_opengl_NativeOpengl_start(JNIEnv *env, jobject thiz) {
    if(opengl!=NULL){
        opengl->start();
    }
}