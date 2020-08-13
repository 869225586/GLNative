//
// Created by edz on 2020/8/13.
//

#ifndef NATIVE_FILTERVBOYUV_H
#define NATIVE_FILTERYUV_H

#include "../opengl/BaseRender.h"
#include "../matrix/MatrixUtil.h"
#include "../shader/ShaderUtil.h"

class FilterYuv : public BaseRender {
public:
    GLuint vPosition;
    GLuint fPosition;
    GLuint u_matrix;

    GLuint sampler_y;//shader y 变量
    GLuint sampler_u;//shader u 变量
    GLuint sampler_v;//shader v 变量

    GLuint textures[3];//上面三个纹理的数组
    GLuint vbos[1]; //vbo
    GLuint vboId; //vbo id

    float matrix[16];
    void *y = NULL; //y 数据
    void *u = NULL; //u 数据
    void *v = NULL; //v 数据
    int yuv_wdith = 0;
    int yuv_height = 0;
public:
    FilterYuv();

    ~FilterYuv();


    void onCreate();

    void onChange(int w, int h);

    void onDraw();

    void destroyGl(); //销毁gl相关 需在egl 线程进行 否则销毁无效

    void destroyData();//销毁 数据 像素点 等可不必在 egl 线程中做销毁

    void setMatrix(int width, int height);

    //设置y u v  数据
    void setYuvData(void *y, void *u, void *v, int width, int height);

};


#endif //NATIVE_FILTERVBOYUV_H
