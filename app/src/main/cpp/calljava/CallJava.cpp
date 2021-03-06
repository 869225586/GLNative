//
// Created by edz on 2020/8/18.
//

#include "CallJava.h"

CallJava::~CallJava() {

}

CallJava::CallJava(_JavaVM *javaVm, JNIEnv *env, jobject *jobject) {
    this->javaVm = javaVm;
    this->jniEnv = env;
    this->jobj = *jobject;
    this->jobj = env->NewGlobalRef(jobj);//全局引用 不然 线程间直接使用就会报错

    jclass jlz = jniEnv->GetObjectClass(jobj);
    jmid_supportvideo = jniEnv->GetMethodID(jlz, "onCallIsSupportMediaCodec",
                                            "(Ljava/lang/String;)Z");
    jmid_initmediacodec = jniEnv->GetMethodID(jlz, "initMediaCodec", "(Ljava/lang/String;II[B[B)V");
    jmid_decodeavpacket = jniEnv->GetMethodID(jlz, "decodeAVPacket", "(I[B)V");
    jmid_callCamera = jniEnv->GetMethodID(jlz, "initSurfaceTextture", "(I)V");
    jmid_update = jniEnv->GetMethodID(jlz,"updateSurfaceTextture","()V");
    jmid_callPrepare = jniEnv->GetMethodID(jlz,"callPrepared","()V");
    jmid_callTimeProgress = jniEnv->GetMethodID(jlz,"nativeCallTimeProgress","(DJ)V");
}

bool CallJava::onCallIsSupportVideo(const char *ffcodecname) {
    bool support = false;
    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
        return support;
    }
    jstring type = jniEnv->NewStringUTF(ffcodecname);

    support = jniEnv->CallBooleanMethod(jobj, jmid_supportvideo, type);
    LOGD("calljava8");
    jniEnv->DeleteLocalRef(type);

    javaVm->DetachCurrentThread();
    return support;
}

void CallJava::onCallInitMediacodec(const char *mime, int width, int height, int csd0_size,
                                    int csd1_size, uint8_t *csd_0, uint8_t *csd_1) {
    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
    }
    jstring type = jniEnv->NewStringUTF(mime);
    jbyteArray csd0 = jniEnv->NewByteArray(csd0_size);
    jniEnv->SetByteArrayRegion(csd0, 0, csd0_size, reinterpret_cast<const jbyte *>(csd_0));
    jbyteArray csd1 = jniEnv->NewByteArray(csd1_size);
    jniEnv->SetByteArrayRegion(csd1, 0, csd1_size, reinterpret_cast<const jbyte *>(csd_1));

    jniEnv->CallVoidMethod(jobj, jmid_initmediacodec, type, width, height, csd0, csd1);

    jniEnv->DeleteLocalRef(csd0);
    jniEnv->DeleteLocalRef(csd1);
    jniEnv->DeleteLocalRef(type);
    javaVm->DetachCurrentThread();
}

void CallJava::onCallDecodeAVPacket(int datasize, uint8_t *data_) {
    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
    }
    jbyteArray data = jniEnv->NewByteArray(datasize);
    //将native 数组拷贝到java 数组中传递给java端
    jniEnv->SetByteArrayRegion(data, 0, datasize, reinterpret_cast<const jbyte *>(data_));
    jniEnv->CallVoidMethod(jobj, jmid_decodeavpacket, datasize, data);
    jniEnv->DeleteLocalRef(data);
    javaVm->DetachCurrentThread();
}

void CallJava::onCallCamera(int cameraTexture) {
    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
    }
    jniEnv->CallVoidMethod(jobj, jmid_callCamera, cameraTexture);
    javaVm->DetachCurrentThread();

}

void CallJava::onUpdateTexture() {
    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
    }
    jniEnv->CallVoidMethod(jobj, jmid_update);
    javaVm->DetachCurrentThread();
}

void CallJava::onCallPrepare() {
    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
    }
    jniEnv->CallVoidMethod(jobj, jmid_callPrepare);
    javaVm->DetachCurrentThread();
}

void CallJava::onCallTimeInfo(double currentPos, long duration) {
    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
    }
    jniEnv->CallVoidMethod(jobj, jmid_callTimeProgress,currentPos,duration);
    javaVm->DetachCurrentThread();
}

