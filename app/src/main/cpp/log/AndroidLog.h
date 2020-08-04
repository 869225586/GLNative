//
// Created by edz on 2020/7/29.
//

#ifndef NATIVE_ANDROIDLOG_H
#define NATIVE_ANDROIDLOG_H

#include "android/log.h"
#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG,FORMAT,"syy",##__VA_ARGS__);
#endif //NATIVE_ANDROIDLOG_H
