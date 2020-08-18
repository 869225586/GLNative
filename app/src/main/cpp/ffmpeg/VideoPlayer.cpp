//
// Created by edz on 2020/8/17.
//

#include "VideoPlayer.h"
#include "../opengl/Opengl.h"

VideoPlayer::VideoPlayer(PlayStatus *playStatus) {
    this->playStatus = playStatus;
    queue = new StreamQueue(playStatus);
    pthread_mutex_init(&codecMutex, NULL);
}


VideoPlayer::~VideoPlayer() {

}

//视频 解码
void *playVideo(void *data) {
    VideoPlayer *videoPlayer = static_cast<VideoPlayer *>(data);
    while (videoPlayer->playStatus != NULL) {
        if (videoPlayer->playStatus->seek) {
            av_usleep(1000 * 100);
            continue;
        }
        if (videoPlayer->playStatus->pause) {
            av_usleep(1000 * 100);
            continue;
        }
        if (videoPlayer->queue->getQueueSize() == 0) {
            //队列里没有拿到数据
            if (!videoPlayer->playStatus->load) {
                videoPlayer->playStatus->load = true;
//                video->wlCallJava->onCallLoad(CHILD_THREAD, true);
            }
            av_usleep(1000 * 100);
            continue;
        } else {
            if (videoPlayer->playStatus->load) {
                videoPlayer->playStatus->load = true;
//                video->wlCallJava->onCallLoad(CHILD_THREAD, false);
            }
            AVPacket *avPacket = av_packet_alloc();
            if (videoPlayer->queue->getAvpacket(avPacket) != 0) {
                videoPlayer->releaseAvpacket(avPacket);
                continue;
            }
            if (videoPlayer->codecType == CODEC_MEDIACODEC) {
                //硬解 回调java层
            } else if (videoPlayer->codecType == CODEC_YUV) {
                //软解
                pthread_mutex_lock(&videoPlayer->codecMutex);
                if (avcodec_send_packet(videoPlayer->avCodecContext, avPacket) != 0) {
                    videoPlayer->releaseAvpacket(avPacket);
                    pthread_mutex_unlock(&videoPlayer->codecMutex);
                    continue;
                }
                AVFrame *avFrame = av_frame_alloc();
                if (avcodec_receive_frame(videoPlayer->avCodecContext, avFrame) != 0) {
                    videoPlayer->releaseAvFrame(avFrame);
                    videoPlayer->releaseAvpacket(avPacket);
                    pthread_mutex_unlock(&videoPlayer->codecMutex);
                    continue;
                }
                if (avFrame->format == AV_PIX_FMT_YUV420P) {
                    double diff = videoPlayer->getFrameDiffTime(avFrame,NULL);
                    av_usleep(videoPlayer->getDelayTime(diff)*1000000);
                    if (videoPlayer->callYuv != NULL) {
                        videoPlayer->callYuv(avFrame->data[0],
                                             avFrame->data[1],
                                             avFrame->data[2],
                                             avFrame->linesize[0],//这里没有用avcodecContext 的width 因为
                                //avcodeccontext 获取到的宽与实际不一致 导致了画面都抽象了。。
                                             videoPlayer->avCodecContext->height,
                                             videoPlayer->ctx);
                    }
                }
                videoPlayer->releaseAvpacket(avPacket);
                videoPlayer->releaseAvFrame(avFrame);
                //一次解码完成释放锁不然就阻塞了。。。！！！
                pthread_mutex_unlock(&videoPlayer->codecMutex);

            }
        }
    }
    pthread_exit(&videoPlayer->thread_play);
}

void VideoPlayer::play() {
    pthread_create(&thread_play, NULL, playVideo, this);
}

double VideoPlayer::getFrameDiffTime(AVFrame *avFrame, AVPacket *avPacket) {
    double pts = 0;
    if(avFrame != NULL)
    {
        pts = av_frame_get_best_effort_timestamp(avFrame);
    }
    if(avPacket != NULL)
    {
        pts = avPacket->pts;
    }
    if(pts == AV_NOPTS_VALUE)
    {
        pts = 0;
    }
    //pts 是 当前帧 什么时候显示 。 time_base 是结构体 num  den 代表时间刻度
    //相乘？？？？
    pts *= av_q2d(time_base);

    if(pts > 0)
    {
        clock = pts;
    }

    double diff = audioPlayer->clock - clock;
    return diff;
}

double VideoPlayer::getDelayTime(double diff) {
    if(diff > 0.003)
    {
        delayTime = delayTime * 2 / 3;
        if(delayTime < defaultDelayTime / 2)
        {
            delayTime = defaultDelayTime * 2 / 3;
        }
        else if(delayTime > defaultDelayTime * 2)
        {
            delayTime = defaultDelayTime * 2;
        }
    }
    else if(diff < - 0.003)
    {
        delayTime = delayTime * 3 / 2;
        if(delayTime < defaultDelayTime / 2)
        {
            delayTime = defaultDelayTime * 2 / 3;
        }
        else if(delayTime > defaultDelayTime * 2)
        {
            delayTime = defaultDelayTime * 2;
        }
    }
    else if(diff == 0.003)
    {

    }
    if(diff >= 0.5)
    {
        delayTime = 0;
    }
    else if(diff <= -0.5)
    {
        delayTime = defaultDelayTime * 2;
    }

    if(fabs(diff) >= 10)
    {
        delayTime = defaultDelayTime;
    }
    return delayTime;
}

void VideoPlayer::release() {

}

void VideoPlayer::releaseAvpacket(AVPacket *avPacket) {
    av_packet_free(&avPacket);
    av_free(avPacket);
    avPacket = NULL;
}

void VideoPlayer::releaseAvFrame(AVFrame *avFrame) {
    av_frame_free(&avFrame);
    av_free(avFrame);
    avFrame = NULL;
}

void VideoPlayer::setCallYUV(VideoPlayer::onCallYUV onCallYuv, void *ctx) {
    this->callYuv = onCallYuv;
    this->ctx = ctx;
}
