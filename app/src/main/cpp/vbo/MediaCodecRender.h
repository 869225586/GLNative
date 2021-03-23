//
// Created by edz on 2020/8/13.
//

#ifndef NATIVE_MEDIACODECRENDER_H
#define NATIVE_MEDIACODECRENDER_H

#include "../opengl/BaseRender.h"
#include "../matrix/MatrixUtil.h"
#include "../shader/ShaderUtil.h"
#include <GLES2/gl2ext.h>

class MediaCodecRender : public BaseRender {
public:
    GLuint vPosition;
    GLuint fPosition;
    GLuint u_matrix;

    GLint sampler;
    GLuint texture;//上面三个纹理的数组
    GLuint vboId; //vbo id
    GLuint fboId;

    //fbo id
    GLuint cameraId;//创建一个camera 的纹理id

    float matrix[16];
    int yuv_wdith = 0;
    int yuv_height = 0;

    CallJava *callJava;
public:
    MediaCodecRender(CallJava *callJava);

    ~MediaCodecRender();


    void onCreate();

    void onChange(int w, int h);

    void onDraw();

    void destroyGl(); //销毁gl相关 需在egl 线程进行 否则销毁无效

    void destroyData();//销毁 数据 像素点 等可不必在 egl 线程中做销毁

    void setMatrix(int width, int height);

    //设置y u v  数据
    void setYuvData(void *y, void *u, void *v, int width, int height);

};


#endif //NATIVE_MEDIACODECRENDER_H
