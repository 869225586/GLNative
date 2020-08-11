//
// Created by edz on 2020/8/11.
//

#include "FilterOne.h"

void FilterOne::onCreate() {
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
               "uniform sampler2D sTexture;\n"
               "void main() {\n"
               "    gl_FragColor=texture2D(sTexture, ft_Position);\n"
               "}";
    //创建一个加载shader 的program
    program = createProgrm(vertex, fragment,&vShader,&fShader);
    LOGD("program %d", program);
    vPosition = glGetAttribLocation(program, "v_Position");//拿到shader 中的顶点坐标 变量
    LOGD("vposition %d", vPosition);
    fPosition = glGetAttribLocation(program, "f_Position");//拿到shader中 纹理坐标
    sampler = glGetUniformLocation(program, "sTexture"); //拿到 2d纹理
    u_matrix = glGetUniformLocation(program, "u_Matrix");

    glGenTextures(1, &textureId);//生成一个纹理id

    glBindTexture(GL_TEXTURE_2D, textureId);//绑定纹理id
    //设置纹理 一些参数 环绕方向
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);//解绑纹理
}

void FilterOne::onChange(int width, int height) {
    glViewport(0, 0, width, height);
    setMatrix(width, height);
}

void FilterOne::onDraw() {
    //1 清屏   2 使用程序  3 为顶点赋值  4 绘制图形
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, matrix);//使用矩阵

    glActiveTexture(GL_TEXTURE5);//激活纹理
    glUniform1i(sampler, 5); //使用纹理
    glBindTexture(GL_TEXTURE_2D, textureId);//绑定纹理
    LOGD("开始绘制");
    //设置图片像素数据
    if (pixels != NULL) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }
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

void FilterOne::setPilex(void *data, int width, int height, int length) {
    LOGD("设置图片数据");
    w = width;
    h = height;
    pixels = data;
    if (surface_height > 0 && surface_width > 0) {
        setMatrix(surface_width, surface_height);
    }
}

FilterOne::FilterOne() {


}

FilterOne::~FilterOne() {

}

void FilterOne::setMatrix(int width, int height) {
    initMatrix(matrix);
    //计算控件的宽高比  以及图片的宽高比
    float screen_r = 1.0 * width / height;
    float picture_r = 1.0 * w / h;
    if (screen_r > picture_r) //图片宽度缩放
    {
        float r = width / (1.0 * height / h * w);
        orthoM(-r, r, -1, 1, matrix);
    } else {//图片高度缩放
        float r = height / (1.0 * width / w * h);
        orthoM(-1, 1, -r, r, matrix);
    }
}

void FilterOne::destroyGl() {
    LOGE("FilterOne::destroy GL")
    glDeleteTextures(1, &textureId);
    glDetachShader(program, vShader);
    glDetachShader(program, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    glDeleteProgram(program);
}

void FilterOne::destroyData() {
    if (pixels != NULL) {
        pixels = NULL;
    }
}
