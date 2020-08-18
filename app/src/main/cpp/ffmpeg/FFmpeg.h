//
// Created by edz on 2020/8/14.
//

#ifndef NATIVE_FFMPEG_H
#define NATIVE_FFMPEG_H

#include "pthread.h"
#include "../log/AndroidLog.h"
#include "AudioPlayer.h"
#include "VideoPlayer.h"

extern "C"
{
#include "../include/libavformat/avformat.h"
#include <../include/libavutil/time.h>
};

class FFmpeg {
public:
    const char *url = NULL;
    pthread_t decodeThread;
    AVFormatContext *pFormatCtx = NULL; //编解码 上下文 。。入口
    pthread_mutex_t init_mutex; //初始化用的锁
    bool exit = false;
    int duration = 0;
    pthread_mutex_t seek_mutex;//seek 用的锁
    bool surpportMediaCodec = false; //是否支持硬解码

    const AVBitStreamFilter *bsFilter = NULL; //用于获取视频一些 格式等 过滤器

    AudioPlayer *audioPlayer = NULL;

    VideoPlayer *videoPlayer = NULL;

    PlayStatus *playStatus = NULL;

    bool isCreateVideo = false;
public:
    FFmpeg(PlayStatus *playStatus, const char *url);

    ~FFmpeg();

    void preapared();

    void decodeFFmpegThread();

    void start();

    void pause();

    void resume();

    void release();

    void seek(int64_t seconds);

    int getCodecContext(AVCodecParameters *codecpar, AVCodecContext **avCodecContext);

    void exitByInitError(); //初始化出错 退出

};


#endif NATIVE_FFMPEG_H
