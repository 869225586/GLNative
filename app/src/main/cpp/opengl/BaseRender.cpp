//
// Created by edz on 2020/8/11.
//


#include "BaseRender.h"

BaseRender::BaseRender() {
    vertexs = new float[8];
    fragments = new float[8];

    float v[] = {1, -1,
                 1, 1,
                 -1, -1,
                 -1, 1};
    memcpy(vertexs, v, sizeof(v));

    float f[] = {1, 1,
                 1, 0,
                 0, 1,
                 0, 0};
    memcpy(fragments, f, sizeof(f));
}

BaseRender::~BaseRender() {
    delete[]vertexs;
    delete[]fragments;

}

void BaseRender::destroy() {

}

void BaseRender::onCreate() {

}

void BaseRender::onChange(int w, int h) {

}

void BaseRender::onDraw() {

}

void BaseRender::setPilex(void *data, int width, int height, int length) {

}
