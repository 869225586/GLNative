//
// Created by edz on 2020/9/22.
//

#include "CameraRender.h"
#include <GLES2/gl2ext.h>

//
// Created by edz on 2020/8/13.
//


/**
 * 启用vbo 的 filter
 */
CameraRender::CameraRender(CallJava *callJava):BaseRender(){
    this->callJava = callJava;
}

CameraRender::~CameraRender() {

}

void CameraRender::onCreate() {


    vertex = "attribute vec4 v_Position;\n"
             "attribute vec2 f_Position;\n"
             "varying vec2 ft_Position;\n"
             "uniform mat4 u_Matrix;\n"
             "void main() {\n"
             "    ft_Position = f_Position;\n"
             "    gl_Position = v_Position * u_Matrix;\n"
             "}";

    fragment = "#extension GL_OES_EGL_image_external : require\n"
               "precision mediump float;\n"
               "varying vec2 ft_Position;\n"
               "uniform samplerExternalOES sTexture;\n"
               "void main() {\n"
               "  gl_FragColor=texture2D(sTexture, ft_Position);\n"
               "}";

    program = createProgrm(vertex, fragment, &vShader, &fShader);
    LOGD("opengl program is %d %d %d", program, vShader, fShader);
    vPosition = glGetAttribLocation(program, "v_Position");//顶点坐标
    fPosition = glGetAttribLocation(program, "f_Position");//纹理坐标

    sampler = glGetUniformLocation(program, "sTexture"); //拿到 2d纹理

    u_matrix = glGetUniformLocation(program, "u_Matrix");

    glGenBuffers(1, &vboId); //启用vbo获取vboid
    glBindBuffer(GL_ARRAY_BUFFER, vboId); //绑定vbo

    //分配内存空间 为顶点长度和纹理长度
    glBufferData(GL_ARRAY_BUFFER, vertexSize + fragmentSize, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, vertexs);
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize, fragmentSize, fragments);

    glBindBuffer(0, GL_ARRAY_BUFFER);


    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,720,1280,0,
                 GL_RGBA,GL_UNSIGNED_BYTE,NULL);


    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &cameraId);
    LOGD("生成camra纹理id %d", cameraId);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, cameraId);

    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    callJava->onCallCamera(cameraId);

    glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);


}

void CameraRender::onChange(int width, int height) {
    surface_width = width;
    surface_height = height;
    glViewport(0, 0, width, height);
    setMatrix(width, height);
}

void CameraRender::onDraw() {
    callJava->onUpdateTexture();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);

    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, matrix);

    glBindBuffer(vboId, GL_ARRAY_BUFFER);//绑定vbo
    glActiveTexture(GL_TEXTURE5);//激活纹理
    glUniform1i(sampler, 5); //使用纹理
    glBindTexture(GL_TEXTURE_2D, texture);//绑定纹理

    glEnableVertexAttribArray(vPosition);
    //2 代表 每两个数字 作为一个点  8 代表每个点占用的字节数 一个float 4个字节 ，   0 代表从0开始取 偏移量
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, false, 8, 0);

    glEnableVertexAttribArray(fPosition);
    //由于 顶点和纹理坐标都换存在 vbo 中所以 偏移量就是从 顶点的size 开始
    glVertexAttribPointer(fPosition, 2, GL_FLOAT, false, 8,
                          reinterpret_cast<const void *>(vertexSize));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(0,GL_ARRAY_BUFFER);

}

void CameraRender::destroyGl() {
    glDeleteTextures(1, &texture);
    glDetachShader(program, vShader);
    glDetachShader(program, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    glDeleteProgram(program);
}

void CameraRender::destroyData() {
    yuv_wdith = 0;
    yuv_height = 0;

}

void CameraRender::setMatrix(int width, int height) {
    initMatrix(matrix);
    //相机 旋转 怎么算的瞎蒙的
    rotateMatrixZ(-90, matrix);
    rotateMatrixX(90,matrix);
}

void CameraRender::setYuvData(void *y, void *u, void *v, int width, int height) {
}





