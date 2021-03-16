//
// Created by edz on 2020/8/14.
//

#ifndef NATIVE_AUDIOPLAYER_H
#define NATIVE_AUDIOPLAYER_H

#include "StreamQueue.h"
#include "PlayStatus.h"
#include "../calljava/CallJava.h"

extern "C"
{
#include "../include/libavutil/time.h"
#include "../include/libavcodec/avcodec.h"
#include "../include/libswresample/swresample.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
}

class AudioPlayer {
public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;//解码器上下文
    AVCodecParameters *codePar = NULL;
    StreamQueue *queue = NULL;
    PlayStatus *playStatus = NULL;
    CallJava *callJava = NULL;
    pthread_t thread_play;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int ret = 0;
    uint8_t *buffer = NULL;
    int data_size = 0;
    int sample_rate = 0;

    long duration = 0;//总时长
    /**
     * typedef struct AVRational{
    int num; ///< Numerator
    int den; ///< Denominator
    } AVRational;
    rational number就是可以表示成p/q两个整数相除的数。其实就是传说中的有理数。
     numberator: 分子 denominator: 分母
     计算当前音频的时间 可以用pts 乘以这个分数 得到时间 。
     */
    AVRational time_base; //流的时间基数
    double clock;//播放进度 时长
    double currentTime;//当前frame 时间
    double preTime;//上一次调用时间


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
    AudioPlayer(PlayStatus *playStatus, int sample_rate,CallJava *callJava);

    ~AudioPlayer();

    void play();

    //冲采样 由于 Open sl es 播放时 需要指定播放格式
    // 但是 原始音频 格式 可能不一样 所以为了统一进行冲采样后播放
    int resampleAudio();


    void initOpenSLEs();//初始化ES

    int getCurrentSampleRate(int sample_rate);//获取当前采样率

    void pause();

    void resume();

    void stop();

    void release();

    void releaseAvpacket();//冲采样失败释放 avpacket

    void releaseAvFrame();//重采样 失败释放 avFrame

};


#endif //NATIVE_AUDIOPLAYER_H
