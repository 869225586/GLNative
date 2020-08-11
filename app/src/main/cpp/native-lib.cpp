#include <jni.h>
#include <string>
#include "log/AndroidLog.h"

#include "opengl/Opengl.h"

Opengl *opengl;
extern "C" JNIEXPORT jstring JNICALL
Java_com_kwai_video_uikit_anative_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_kwai_video_uikit_opengl_NativeOpengl_surfaceCreate(JNIEnv *env, jobject thiz,

                                                            jobject surface) {

    // TODO
    if (opengl == NULL) {
        opengl = new Opengl();
    }
    opengl->onCreateSurface(env, surface);

}extern "C"
JNIEXPORT void JNICALL
Java_com_kwai_video_uikit_opengl_NativeOpengl_surfaceChange(JNIEnv *env, jobject thiz, jint i1,
                                                            jint i2) {
    opengl->onChangeSurface(i1, i2);
}extern "C"
JNIEXPORT void JNICALL
Java_com_kwai_video_uikit_opengl_NativeOpengl_imgData(JNIEnv *env, jobject thiz, jint width,
                                                      jint height,
                                                      jint length, jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);

    opengl->setPilex(data, width, height, length);

    env->ReleaseByteArrayElements(data_, data, 0); //释放data 与data_
}extern "C"
JNIEXPORT void JNICALL
Java_com_kwai_video_uikit_opengl_NativeOpengl_surfaceDestroy(JNIEnv *env, jobject thiz) {
    opengl->onDestorySurface();
    delete opengl;
    opengl = NULL;

}extern "C"
JNIEXPORT void JNICALL
Java_com_kwai_video_uikit_opengl_NativeOpengl_changeFilter(JNIEnv *env, jobject thiz) {
    // TODO: implement changeFilter()
    opengl->onChangeFilter();

}