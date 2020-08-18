//
// Created by edz on 2020/8/17.
//

#ifndef NATIVE_VIDEOPLAYER_H
#define NATIVE_VIDEOPLAYER_H

#include "StreamQueue.h"

#include "AudioPlayer.h"
/**
 *  由于 ffmpeg 包含 videoplayer ， opengl 又包含 ffmpeg  ，所以 opengl 间接包含 videoplayer
    所以这里不能再包含opengl 存在互相包含问题。 需要定一个class 与opengl 同名 然后再cpp 中包含opengl头文件就可以调用opengl 的方法
    #include "../opengl/Opengl.h"
    video player 暴露一个接口用于 透传 yuv 数据给opengl
 **/

#define  CODEC_YUV 0
#define  CODEC_MEDIACODEC 1

extern "C" {
#include "../include/libswscale/swscale.h"
#include "../include/libavutil/imgutils.h"
#include "../include/libavutil/time.h"
#include "../include/libavcodec/avcodec.h"
};

class VideoPlayer {
public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *codecpar = NULL;
    StreamQueue *queue = NULL;
    PlayStatus *playStatus = NULL;

    AVRational time_base;
    pthread_t thread_play;

    AudioPlayer *audioPlayer = NULL;
    double clock = 0;//当前播放时长
    double delayTime = 0;
    double defaultDelayTime = 0.04;

    pthread_mutex_t codecMutex;

    int codecType = CODEC_YUV;
    AVBSFContext *abs_ctx = NULL;

    typedef void (*onCallYUV)(void *y, void *u, void *v, int width, int height, void *ctx);//定义回调函数
    onCallYUV callYuv;
    void *ctx;
public:
    VideoPlayer(PlayStatus *playStatus);

    ~VideoPlayer();

    void play();

    double getFrameDiffTime(AVFrame *avFrame, AVPacket *avPacket);

    double getDelayTime(double diff);

    void release();

    void releaseAvpacket(AVPacket *avPacket);//冲采样失败释放 avpacket

    void releaseAvFrame(AVFrame *avFrame);//重采样 失败释放 avFrame

    void setCallYUV(onCallYUV onCallYuv, void *ctx);


};


#endif //NATIVE_VIDEOPLAYER_H
