//
// Created by edz on 2020/8/11.
//

#ifndef NATIVE_BASERENDER_H
#define NATIVE_BASERENDER_H

#include "../log/AndroidLog.h"
#include <cstring>
#include <GLES2/gl2.h>
#include "../calljava/CallJava.h"
/**
 * 本基类 是进行opengl 的一些操作 创建纹理 设置 矩阵 绘制 等等
 * 1 构造方法进行oengl 的顶点坐标 初始化 子类进行相应shader 加载 赋值等
 * 2 oncreate 子类进行相应的纹理创建 等
 * 3 onchange 子类进行矩阵变换
 * 4 draw 子类进行相应的draw绘制
 */
class BaseRender {
public:
    int surface_width;
    int surface_height;

    const char *vertex; //顶点shader
    const char *fragment; //着色器 shader

    float *vertexs;//顶点坐标
    float *fragments; //着色器坐标

    GLuint program; //创建的opengl 程序
    GLuint vShader; //拿到顶点 shader 用于在render里 进行释放资源
    GLuint fShader;//拿到 纹理shader  用于在render里 进行释放资源

    int vertexSize;//顶点坐标字节数
    int fragmentSize;//纹理坐标字节数

public:
    BaseRender();
//    BaseRender(CallJava *callJava);
    ~BaseRender();

//    virtual //虚函数 类似java 的抽象方法 由 子类实现
    virtual void onCreate();

    virtual void onChange(int w, int h);

    virtual void onDraw();

    virtual void destroyGl(); //销毁gl相关 需在egl 线程进行 否则销毁无效

    virtual void destroyData();//销毁 数据 像素点 等可不必在 egl 线程中做销毁

    //设置图片数据 data 像素
    virtual void setPilex(void *data, int width, int height, int length);

    virtual void setYuvData(void *y, void *u, void *v, int width, int height);

    virtual void changeFullScreen(bool fullScreen);
};


#endif //NATIVE_BASERENDER_H
