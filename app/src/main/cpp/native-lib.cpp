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

void onCreate(void*){

}
void onChange(int width,int height,void *){
    glViewport(0, 0, width, height);
}
void onDraw(void *){
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
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
    //通过传过来的 surface 对象创建一个 nativewindow
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    //创建egl 线程 对象
    thread= new EglThread();

    /**调用线程的create
     * 开始创建线程 内部
     * 1 创建 egl 环境
     * 2 将这里获取的surface转化成 native window 传给 egl 进行 后期的渲染 交换数据
     * 3 在线程里面进行 oncreate onchange  ondestroy 这三个方法的回调
     */
    thread->onSurfaceCreate(nativeWindow);
    //设置刷新模式
    thread->setRenderType(RENDER_HADNLE);
    //设置几个方法的回调函数 在回调函数里做 opengl 的绘制
    thread->callBackOnCreate(onCreate,thread);
    thread->callBackOnChange(onChange,thread);
    thread->callBackOnDraw(onDraw,thread);
}extern "C"
JNIEXPORT void JNICALL
Java_com_kwai_video_uikit_opengl_NativeOpengl_surfaceChange(JNIEnv *env, jobject thiz, jint i1,
                                                            jint i2) {
   thread->onSurfaceChange(i1,i2);
//   usleep(1000000);//睡眠一会
//   //测试手动刷新
//   thread->notifyRender();
}