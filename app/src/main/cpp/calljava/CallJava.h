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
    JNIEnv *jniEnv= NULL;
    jobject jobj;

    jmethodID jmid_supportvideo;
    jmethodID jmid_initmediacodec;
    jmethodID jmid_decodeavpacket;
    jmethodID jmid_callCamera;
    jmethodID jmid_update;
public:
    CallJava(_JavaVM *javaVm,JNIEnv *env,jobject *jobject);
    ~CallJava();

    bool onCallIsSupportVideo(const char *ffcodecname);

    void onCallInitMediacodec(const char *mime, int width, int height, int csd0_size, int csd1_size, uint8_t *csd_0, uint8_t *csd_1);

    void onCallDecodeAVPacket(int datasize, uint8_t *data);

    void onCallCamera(int cameraTexture);

    void onUpdateTexture();
};


#endif //NATIVE_CALLJAVA_H
