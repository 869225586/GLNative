//
// Created by edz on 2020/9/22.
//

#include "MediaCodecRender.h"

//
// Created by edz on 2020/8/13.

//


/**
 * 硬解码shader
 * TODO 硬解码一个是要用特有的shader 一个是可能要用GLSurfaceView 来做
 */
MediaCodecRender::MediaCodecRender(CallJava *callJava) {
    this->callJava = callJava;
}

MediaCodecRender::~MediaCodecRender() {

}

void MediaCodecRender::onCreate() {


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

    glGenTextures(1, &cameraId);
    LOGD("生成camra纹理id %d", cameraId);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, cameraId);

    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    callJava->onCallCamera(cameraId);//打开相机



}

void MediaCodecRender::onChange(int width, int height) {
    surface_width = width;
    surface_height = height;
    glViewport(0, 0, width, height);
    setMatrix(width, height);
}

void MediaCodecRender::onDraw() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    callJava->onUpdateTexture();
    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, matrix);
    glEnableVertexAttribArray(vPosition);
    //2 代表 每两个数字 作为一个点  8 代表每个点占用的字节数 一个float 4个字节 ，   0 代表从0开始取 偏移量
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, false, 8, 0);

    glEnableVertexAttribArray(fPosition);
    //由于 顶点和纹理坐标都换存在 vbo 中所以 偏移量就是从 顶点的size 开始
    glVertexAttribPointer(fPosition, 2, GL_FLOAT, false, 8,
                          reinterpret_cast<const void *>(vertexSize));
    glActiveTexture(GL_TEXTURE0);//激活纹理
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, cameraId);//绑定纹理
    glUniform1i(sampler, 0); //使用纹理

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void MediaCodecRender::destroyGl() {
    glDeleteTextures(1, &texture);
    glDetachShader(program, vShader);
    glDetachShader(program, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    glDeleteProgram(program);
}

void MediaCodecRender::destroyData() {
    yuv_wdith = 0;
    yuv_height = 0;

}

void MediaCodecRender::setMatrix(int width, int height) {
    initMatrix(matrix);
    if (yuv_wdith > 0 && yuv_height > 0) {
        float screen_r = 1.0 * width / height; //计算surface 的宽高比
        float picture_r = 1.0 * yuv_wdith / yuv_height; //计算视频的宽高比
        if (screen_r > picture_r) //图片宽度缩放
        {
            float r = width / (1.0 * height / yuv_height * yuv_wdith);
            orthoM(-r, r, -1, 1, matrix);
        } else {//图片高度缩放
            float r = height / (1.0 * width / yuv_wdith * yuv_height);
            orthoM(-1, 1, -r, r, matrix);
        }
    }
    //相机 旋转 怎么算的瞎蒙的
//    rotateMatrixZ(-90, matrix);
//    rotateMatrixX(90, matrix);
}

void MediaCodecRender::setYuvData(void *y, void *u, void *v, int width, int height) {
    if (width > 0 && height > 0) {
        if (yuv_wdith != width || yuv_height != height) {
            yuv_wdith = width;
            yuv_height = height;
            setMatrix(surface_width, surface_height);
        }
    }
}





