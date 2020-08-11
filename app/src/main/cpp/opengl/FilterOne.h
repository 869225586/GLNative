//
// Created by edz on 2020/8/11.
//

#ifndef NATIVE_FILTERONE_H
#define NATIVE_FILTERONE_H


#include "BaseRender.h"
#include "../shader/ShaderUtil.h"
#include "../matrix/MatrixUtil.h"
#include "../log/AndroidLog.h"
//继承baseOpengl
class FilterOne : public BaseRender {
    //声明所需要的变量
public:
    GLint vPosition;
    GLint fPosition;
    GLint sampler;
    GLuint textureId;
    GLint u_matrix;

    int w;
    int h;
    void *pixels = NULL;

    float matrix[16];

public:
    FilterOne();

    ~FilterOne();


    void onCreate();

    void onChange(int w, int h);

    void onDraw();

    void destroy();

    void setPilex(void *data, int width, int height, int length);

    void setMatrix(int width, int height);

};


#endif //NATIVE_FILTERONE_H
