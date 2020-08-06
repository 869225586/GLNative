#include <jni.h>
#include <string>
#include "log/AndroidLog.h"

#include "egl/EglHelper.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "GLES2/gl2.h"
#include "egl/EglThread.h"
#include "shader/ShaderUtil.h"

EglHelper *wlEglHelper = NULL;
ANativeWindow *nativeWindow = NULL;
EglThread *thread = NULL;

const char *vertex = "attribute vec4 v_Position;\n"
                     "attribute vec2 f_Position;\n"
                     "varying vec2 ft_Position;\n"
                     "void main() {\n"
                     "    ft_Position = f_Position;\n"
                     "    gl_Position = v_Position;\n"
                     "}";

const char *fragment = "precision mediump float;\n"
                       "varying vec2 ft_Position;\n"
                       "uniform sampler2D sTexture;\n"
                       "void main() {\n"
                       "    gl_FragColor=texture2D(sTexture, ft_Position);\n"
                       "}";


int program;
GLint vPosition;
GLint fPosition;
GLint sampler;
GLuint textureId;

int w;
int h;
void *pixels = NULL;

float vertexs[] = {
        1, -1,
        1, 1,
        -1, -1,
        -1, 1

};
float fragments[] ={
        1,1,
        1,0,
        0,1,
        0,0
};


void onCreate(void *) {
    //创建一个加载shader 的program
    program = createProgrm(vertex, fragment);
    LOGD("program %d", program);
    vPosition = glGetAttribLocation(program, "v_Position");//拿到shader 中的顶点坐标 变量
    LOGD("vposition %d", vPosition);
    fPosition = glGetAttribLocation(program,"f_Position");//拿到shader中 纹理坐标
    sampler = glGetUniformLocation(program,"sTexture"); //拿到 2d纹理

    glGenTextures(1,&textureId);//生成一个纹理id

    glBindTexture(GL_TEXTURE_2D,textureId);//绑定纹理id

    //设置纹理 一些参数 环绕方向
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    if(pixels != NULL)
    {
        LOGD("设置图片数据");
        //设置数据
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }
    glBindTexture(GL_TEXTURE_2D, 0);//解绑纹理

}

void onChange(int width, int height, void *) {
    glViewport(0, 0, width, height);
}

void onDraw(void *) {
    //1 清屏   2 使用程序  3 为顶点赋值  4 绘制图形
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

//    glActiveTexture(GL_TEXTURE5);//激活纹理
//    glUniform1i(sampler, 5); //使用纹理
    glBindTexture(GL_TEXTURE_2D, textureId);//绑定纹理


    glEnableVertexAttribArray(vPosition);
    //参数 说明
    // 1 顶点 变量
    // 2 几个坐标表示一个点
    // 3 坐标数据类型
    // 4 是否让gl 归一化 我们顶点坐标已经是归一化了就不需要gl 归一化 所以是false
    // 5（没个点占据的数据字节数 一个float 4 字节 一个点 2个float 就是 8 ）
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, false, 8, vertexs);

    glEnableVertexAttribArray(fPosition);
    glVertexAttribPointer(fPosition, 2, GL_FLOAT, false, 8, fragments);

    //绘制图形 三角形 带 方式 公用一个边 。我们顶点坐标设置的时候 两个三角形的共用边 要声明在 中间两个点 才行 。
    //因为 他绘制的时候是 从第一个点开始先绘制三个点 ，再从第二点开始绘制第二个三角形 中间两个点是共用边才能组成一个 四边形
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    LOGE("开始绘制");
    glBindTexture(GL_TEXTURE_2D, 0);
}

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
    //通过传过来的 surface 对象创建一个 nativewindow
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    //创建egl 线程 对象
    thread = new EglThread();

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
    thread->callBackOnCreate(onCreate, thread);
    thread->callBackOnChange(onChange, thread);
    thread->callBackOnDraw(onDraw, thread);
}extern "C"
JNIEXPORT void JNICALL
Java_com_kwai_video_uikit_opengl_NativeOpengl_surfaceChange(JNIEnv *env, jobject thiz, jint i1,
                                                            jint i2) {
    thread->onSurfaceChange(i1, i2);
//   usleep(1000000);//睡眠一会
//   //测试手动刷新
//   thread->notifyRender();
}extern "C"
JNIEXPORT void JNICALL
Java_com_kwai_video_uikit_opengl_NativeOpengl_imgData(JNIEnv *env, jobject thiz, jint width,
        jint height,
                                                      jint length, jbyteArray data_) {
    jbyte *data= env->GetByteArrayElements(data_,NULL);
    w = width;
    h = height;
    pixels = malloc(length);//申请 length 长度的内存空间
    memcpy(pixels, data, length);//拷贝data 到piexels 中

    env->ReleaseByteArrayElements(data_, data, 0); //释放data 与data_
}