//
// Created by yangw on 2019-3-31.
//

#ifndef NATIVEOPENGLDEMO_WLFILTERTWO_H
#define NATIVEOPENGLDEMO_WLFILTERTWO_H

#include "../opengl/BaseRender.h"
#include "../matrix/MatrixUtil.h"
#include "../shader/ShaderUtil.h"

class WlFilterTwo : public BaseRender {

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
    WlFilterTwo();

    ~WlFilterTwo();

    void onCreate();

    void onChange(int w, int h);

    void onDraw();

    void destroyGl();

    void destroyData();

    void setPilex(void *data, int width, int height, int length);

    void setMatrix(int width, int height);

};


#endif //NATIVEOPENGLDEMO_WLFILTERTWO_H
