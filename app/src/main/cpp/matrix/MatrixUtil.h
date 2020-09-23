//
// Created by edz on 2020/8/7.
//

#ifndef NATIVE_MATRIXUTIL_H
#define NATIVE_MATRIXUTIL_H

#include <math.h>

//初始化一个单位矩阵
/**
 * 1 0 0 0
 * 0 1 0 0
 * 0 0 1 0
 * 0 0 0 1
 * 除以5取余为0的都是1
 * @param matrix
 */
static void initMatrix(float *matrix) {
    for (int i = 0; i < 16; i++) {
        if (i % 5 == 0) {
            matrix[i] = 1;
        } else {
            matrix[i] = 0;
        }
    }
}
/**
 * z 轴旋转
 * @param angle
 * @param matrix
 */
static void rotateMatrixZ(double angle, float *matrix) {
    angle = angle * (M_PI / 180.0);

    matrix[0] = cos(angle);
    matrix[1] = -sin(angle);
    matrix[4] = sin(angle);
    matrix[5] = cos(angle);
}

/**
 * x 轴旋转
 *
 * 1  0    0     0
 * 0  cosQ -sinQ 0
 * 0  sinQ cosQ  0
 * 0  0    0     1
 * @param angle
 * @param matrix
 */
static void rotateMatrixX(double angle, float *matrix) {
    angle = angle * (M_PI / 180.0);

    matrix[5] = cos(angle);
    matrix[6] = -sin(angle);
    matrix[9] = sin(angle);
    matrix[10] = cos(angle);
}
/**
 * Y 轴旋转
 * cosQ  0   sinQ     0
 * 0     1   0        0
 * -sinQ 0   cosQ     0
 * 0     0   0        1
 * @param angle
 * @param matrix
 */
static void rotateMatrixY(double angle, float *matrix) {
    angle = angle * (M_PI / 180.0);

    matrix[0] = cos(angle);
    matrix[2] = sin(angle);
    matrix[8] = -sin(angle);
    matrix[10] = cos(angle);
}

static void scaleMatrix(double scale, float *matrix) {
    matrix[0] = scale;
    matrix[5] = scale;
}

static void transMatrix(double x, double y, float *matrix) {
    matrix[3] = x;
    matrix[7] = y;
}

static void orthoM(float left, float right, float bottom, float top, float *matrix) {
    matrix[0] = 2 / (right - left);
    matrix[3] = (right + left) / (right - left) * -1;
    matrix[5] = 2 / (top - bottom);
    matrix[7] = (top + bottom) / (top - bottom) * -1;
    matrix[10] = 1;
    matrix[11] = 1;
}

#endif //NATIVE_MATRIXUTIL_H
