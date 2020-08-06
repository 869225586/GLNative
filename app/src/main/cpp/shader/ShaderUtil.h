//
// Created by edz on 2020/8/5.
//

#ifndef NATIVE_SHADERUTIL_H
#define NATIVE_SHADERUTIL_H
#include <GLES2/gl2.h>
#include "../log/AndroidLog.h"
static int loadShader(int shaderType,const char *shaderCode){
    ////根据传入的shader类型（定点 还是 片元） 创建一个shader
    int shader= glCreateShader(shaderType);
    LOGD("shader is %d",shader);
    //// 设置shader code 进去
    glShaderSource(shader,1,&shaderCode,0);
    //编译shader
    glCompileShader(shader);
    return shader ;
}

static int createProgrm(const char *vertex , const char * fragment)
{
    int program = glCreateProgram();  //创建一个program

    int vertexShader = loadShader(GL_VERTEX_SHADER, vertex);//load 一个顶点 shader
    int fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragment); //load 一个片元 shader
    LOGD("vertexShader is %d",vertexShader);
    LOGD("fragmentShader is %d",fragmentShader);

    glAttachShader(program, vertexShader); //绑定顶点shader 到program中
    glAttachShader(program, fragmentShader);//绑定片元shader到program中
    glLinkProgram(program); //链接 program
    return program;
}
#endif //NATIVE_SHADERUTIL_H
