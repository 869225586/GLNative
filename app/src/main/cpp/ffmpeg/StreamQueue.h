//
// Created by edz on 2020/8/14.
//数据 队列 解码得到的都放在队列里
//

#ifndef NATIVE_STREAMQUEUE_H
#define NATIVE_STREAMQUEUE_H

#include "queue"
#include "pthread.h"
#include "../log/AndroidLog.h"
#include "PlayStatus.h"
extern "C"
{
#include "../include/libavcodec/avcodec.h"
};

class StreamQueue {
public:
    std::queue<AVPacket *> queuePacket;//存储 解码得到的数据 的队列
    pthread_mutex_t  mutexQueue;//队列锁
    pthread_cond_t  condQueue;
    PlayStatus *playStatus;
public:
    StreamQueue(PlayStatus *playStatus);
    ~StreamQueue();

    int putAvpacket(AVPacket *packet); //加入队列
    int getAvpacket(AVPacket *packet); //将你传入的avpacket 赋值 并不是 int 返回

    int getQueueSize();
    void clearQueue();

};


#endif //NATIVE_STREAMQUEUE_H
