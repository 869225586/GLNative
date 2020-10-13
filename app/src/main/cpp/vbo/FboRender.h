//
// Created by edz on 2020/9/25.
//

#ifndef NATIVE_FBORENDER_H
#define NATIVE_FBORENDER_H


#include "../opengl/BaseRender.h"

class FboRender : BaseRender {
private:
    GLuint vPosition;
    GLuint fPosition;
    GLuint texture;
public:
    FboRender();

    ~FboRender();

    void onCreate();

    void onChange(int w, int h);

    void onDraw();

    void destroyGl(); //销毁gl相关 需在egl 线程进行 否则销毁无效

    void destroyData();//销毁 数据 像素点 等可不必在 egl 线程中做销毁
};


#endif //NATIVE_FBORENDER_H
