//
// Created by edz on 2020/8/14.
//

#include "StreamQueue.h"

StreamQueue::StreamQueue(PlayStatus *playStatus) {
    this->playStatus = playStatus;
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
}

StreamQueue::~StreamQueue() {
    clearQueue();
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
}

//加入队列 先上锁 再解锁 避免同步问题
int StreamQueue::putAvpacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexQueue);
    queuePacket.push(packet);
    pthread_cond_signal(&condQueue);
    pthread_mutex_unlock(&mutexQueue);
    return 0;
}

//从队列取都是循环拿队列第0个 然后让第0 个出队列 先进先出
int StreamQueue::getAvpacket(AVPacket *avPacket) {
    pthread_mutex_lock(&mutexQueue);
    while (playStatus != NULL && !playStatus->exit) {
        if (queuePacket.size() > 0) {
            AVPacket *packet = queuePacket.front();

            if (av_packet_ref(avPacket, packet) == 0) {//取出数据出队
                queuePacket.pop();
            }
            av_packet_free(&packet);
            av_free(packet);
            packet = NULL;
            break;
        } else {
            pthread_cond_wait(&condQueue, &mutexQueue);//等待 没有数据
        }
    }
    pthread_mutex_unlock(&mutexQueue);
    return 0;
}

int StreamQueue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mutexQueue);
    size = queuePacket.size();
    pthread_mutex_unlock(&mutexQueue);
    return size;
}

void StreamQueue::clearQueue() {
    pthread_cond_signal(&condQueue);//先发信号 解锁 避免 有锁等待问题
    //然后上锁 进行 队列清空
    pthread_mutex_lock(&mutexQueue);
    while (!queuePacket.empty()) {
        AVPacket *packet = queuePacket.front();
        queuePacket.pop();
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
    pthread_mutex_unlock(&mutexQueue);
}
