//
// Created by edz on 2020/8/14.
//

#ifndef NATIVE_AUDIOPLAYER_H
#define NATIVE_AUDIOPLAYER_H

#include "StreamQueue.h"
#include "PlayStatus.h"

extern "C"
{
#include "../include/libavutil/time.h"
#include "../include/libavcodec/avcodec.h"
#include "../include/libswresample/swresample.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
}

class AudioPlayer {
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;//解码器上下文
    AVCodecParameters *codePar = NULL;
    StreamQueue *queue = NULL;
    PlayStatus *playStatus = NULL;

    pthread_t audoThread;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int ret = 0 ;
    uint8_t  *buffer = NULL;
    int data_size= 0;
    int sample_rate=0;

    int duration =0 ;
    AVRational  time_base;
    double  totalTime;//总时长
    double  currentTime;//当前frame 时间
    double  preTime ;//上一次调用时间


    //OPEN SL ES 音频接口
    // 引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    //pcm
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;

    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

    pthread_mutex_t codecMutex;

public:
    AudioPlayer(PlayStatus *playStatus,int sample_rate);
    ~AudioPlayer();

    void play();
    int resampleAudio();//冲采样

    void initOpenSLEs();//初始化ES

    int getCurrentSampleRate(int sample_rate);//当前比特率

    void pause();

    void resume();

    void stop();

    void release();

};


#endif //NATIVE_AUDIOPLAYER_H
