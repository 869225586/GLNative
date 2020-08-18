//
// Created by edz on 2020/8/18.
//

#ifndef NATIVE_CALLJAVA_H
#define NATIVE_CALLJAVA_H

#include "jni.h"
#include <linux/stddef.h>
#include <stdint.h>
#include "../log/AndroidLog.h"

#define MAIN_THREAD 0
#define CHILD_THREAD 1

class CallJava {
public:
    _JavaVM *javaVm = NULL;

};


#endif //NATIVE_CALLJAVA_H
