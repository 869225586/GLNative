//
// Created by edz on 2020/8/13.
//

#include "FilterVboYuv.h"

/**
 * 启用vbo 的 filter
 */
FilFilterVboYuv::FilFilterVboYuv() {

}

FilFilterVboYuv::~FilFilterVboYuv() {

}

void FilFilterVboYuv::onCreate() {


    vertex = "attribute vec4 v_Position;\n"
             "attribute vec2 f_Position;\n"
             "varying vec2 ft_Position;\n"
             "uniform mat4 u_Matrix;\n"
             "void main() {\n"
             "    ft_Position = f_Position;\n"
             "    gl_Position = v_Position * u_Matrix;\n"
             "}";

    fragment = "precision mediump float;\n"
               "varying vec2 ft_Position;\n"
               "uniform sampler2D sampler_y;\n"
               "uniform sampler2D sampler_u;\n"
               "uniform sampler2D sampler_v;\n"
               "void main() {\n"
               "   float y,u,v;\n"
               "   y = texture2D(sampler_y,ft_Position).r;\n"
               "   u = texture2D(sampler_u,ft_Position).r - 0.5;\n"
               "   v = texture2D(sampler_v,ft_Position).r - 0.5;\n"
               "\n"
               "   vec3 rgb;\n"
               "   rgb.r = y + 1.403 * v;\n"
               "   rgb.g = y - 0.344 * u - 0.714 * v;\n"
               "   rgb.b = y + 1.770 * u;\n"
               "\n"
               "   gl_FragColor = vec4(rgb,1);\n"
               "}";

    program = createProgrm(vertex, fragment, &vShader, &fShader);
    LOGD("opengl program is %d %d %d", program, vShader, fShader);
    vPosition = glGetAttribLocation(program, "v_Position");//顶点坐标
    fPosition = glGetAttribLocation(program, "f_Position");//纹理坐标

    sampler_y = glGetUniformLocation(program, "sampler_y");
    sampler_u = glGetUniformLocation(program, "sampler_u");
    sampler_v = glGetUniformLocation(program, "sampler_v");

    u_matrix = glGetUniformLocation(program, "u_Matrix");

    glGenBuffers(1, &vboId); //启用vbo获取vboid
    glBindBuffer(GL_ARRAY_BUFFER, vboId); //绑定vbo

    //分配内存空间 为顶点长度和纹理长度
    glBufferData(GL_ARRAY_BUFFER, vertexSize + fragmentSize, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, vertexs);
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize, fragmentSize, fragments);

    glBindBuffer(0, GL_ARRAY_BUFFER);


    glGenTextures(3, textures);

    for (int i = 0; i < 3; i++) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}

void FilFilterVboYuv::onChange(int width, int height) {
    surface_width = width;
    surface_height = height;
    glViewport(0, 0, width, height);
    setMatrix(width, height);
}

void FilFilterVboYuv::onDraw() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);

    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, matrix);

    glBindBuffer(vboId, GL_ARRAY_BUFFER);//绑定vbo

    glEnableVertexAttribArray(vPosition);
    //2 代表 每两个数字 作为一个点  8 代表每个点占用的字节数 一个float 4个字节 ，   0 代表从0开始取 偏移量
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, false, 8, 0);

    glEnableVertexAttribArray(fPosition);
    //由于 顶点和纹理坐标都换存在 vbo 中所以 偏移量就是从 顶点的size 开始
    glVertexAttribPointer(fPosition, 2, GL_FLOAT, false, 8,
                          reinterpret_cast<const void *>(vertexSize));

    if (yuv_wdith > 0 && yuv_height > 0) {
        if (y != NULL) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yuv_wdith, yuv_height, 0, GL_LUMINANCE,
                         GL_UNSIGNED_BYTE, y);
            glUniform1i(sampler_y, 0);
        }
        if (u != NULL) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textures[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yuv_wdith / 2, yuv_height / 2, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, u);
            glUniform1i(sampler_u, 1);
        }

        if (v != NULL) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, textures[2]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yuv_wdith / 2, yuv_height / 2, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, v);
            glUniform1i(sampler_v, 2);
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void FilFilterVboYuv::destroyGl() {
    glDeleteTextures(3, textures);
    glDetachShader(program, vShader);
    glDetachShader(program, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    glDeleteProgram(program);
}

void FilFilterVboYuv::destroyData() {
    yuv_wdith = 0;
    yuv_height = 0;

    if (y != NULL) {
        free(y);
        y = NULL;
    }
    if (u != NULL) {
        free(u);
        u = NULL;
    }
    if (v != NULL) {
        free(v);
        v = NULL;
    }
}
/**
 * 每次 suface 宽高发生变化都会调用这个 函数 来重新计算 适配规则
 * @param width
 * @param height
 */
void FilFilterVboYuv::setMatrix(int width, int height) {
    initMatrix(matrix);
    LOGV("change  width %d,height %d,video_width %d,video_height %d",width,height,yuv_wdith,yuv_height);
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
    if(fullScreen){
        //这个全屏的方式 可以不用了（现在直接用activity 旋转就可以做）
        // 但是也是一种思路。全屏的时候 改变surface 的宽高填充屏幕
        //需要旋转 画面 才行。
        rotateMatrixZ(90,matrix);
    }
}

void FilFilterVboYuv::setYuvData(void *Y, void *U, void *V, int width, int height) {
    if (width > 0 && height > 0) {
        if (yuv_wdith != width || yuv_height != height) {
            yuv_wdith = width;
            yuv_height = height;
            if (y != NULL) {
                free(y);
                y = NULL;
            }
            if (u != NULL) {
                free(u);
                u = NULL;
            }
            if (v != NULL) {
                free(v);
                v = NULL;
            }
            y = malloc(yuv_wdith * yuv_height);
            u = malloc(yuv_wdith * yuv_height / 4);
            v = malloc(yuv_wdith * yuv_height / 4);
            setMatrix(surface_width, surface_height);
        }
        memcpy(y, Y, yuv_wdith * yuv_height);
        memcpy(u, U, yuv_wdith * yuv_height / 4);
        memcpy(v, V, yuv_wdith * yuv_height / 4);
    }
}

void FilFilterVboYuv::changeFullScreen(bool fullScreen) {
    LOGD("change full screen");
    this->fullScreen = fullScreen;
}
