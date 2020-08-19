//
// Created by edz on 2020/8/17.
//

#include "VideoPlayer.h"
#include "../opengl/Opengl.h"

VideoPlayer::VideoPlayer(PlayStatus *playStatus, CallJava *callJava) {
    this->playStatus = playStatus;
    this->callJava = callJava;
    queue = new StreamQueue(playStatus);
    pthread_mutex_init(&codecMutex, NULL);
}


VideoPlayer::~VideoPlayer() {
    pthread_mutex_destroy(&codecMutex);
}

//视频 解码
void *playVideo(void *data) {
    VideoPlayer *videoPlayer = static_cast<VideoPlayer *>(data);
    while (videoPlayer->playStatus != NULL && !videoPlayer->playStatus->exit) {
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
                if(av_bsf_send_packet(videoPlayer->abs_ctx, avPacket) != 0)
                {
                    av_packet_free(&avPacket);
                    av_free(avPacket);
                    avPacket = NULL;
                    continue;
                }
                while(av_bsf_receive_packet(videoPlayer->abs_ctx, avPacket) == 0)
                {
                    LOGE("开始解码");

                    double diff = videoPlayer->getFrameDiffTime(NULL, avPacket);
                    LOGE("diff is %f", diff);

                    av_usleep(videoPlayer->getDelayTime(diff) * 1000000);
                    videoPlayer->callJava->onCallDecodeAVPacket(avPacket->size, avPacket->data);

                    av_packet_free(&avPacket);
                    av_free(avPacket);
                    continue;
                }
                avPacket = NULL;
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
                    double diff = videoPlayer->getFrameDiffTime(avFrame, NULL);
                    av_usleep(videoPlayer->getDelayTime(diff) * 1000000);
                    if (videoPlayer->callYuv != NULL) {
                        videoPlayer->callYuv(avFrame->data[0],
                                             avFrame->data[1],
                                             avFrame->data[2],
                                             avFrame->linesize[0],//这里没有用avcodecContext 的width 因为
                                //avcodeccontext 获取到的宽与实际不一致 导致了画面都抽象了。。
                                             videoPlayer->avCodecContext->height,
                                             videoPlayer->ctx);
                    }
                } else {
                    LOGE("当前视频不是YUV420P格式");
                    //不是yuv 420 进行转 格式转换
                    AVFrame *pFrameYUV420P = av_frame_alloc();
                    int num = av_image_get_buffer_size(
                            AV_PIX_FMT_YUV420P,
                            videoPlayer->avCodecContext->width,
                            videoPlayer->avCodecContext->height,
                            1);
                    uint8_t *buffer = static_cast<uint8_t *>(av_malloc(num * sizeof(uint8_t)));
                    av_image_fill_arrays(
                            pFrameYUV420P->data,
                            pFrameYUV420P->linesize,
                            buffer,
                            AV_PIX_FMT_YUV420P,
                            videoPlayer->avCodecContext->width,
                            videoPlayer->avCodecContext->height,
                            1);
                    SwsContext *sws_ctx = sws_getContext(
                            videoPlayer->avCodecContext->width,
                            videoPlayer->avCodecContext->height,
                            videoPlayer->avCodecContext->pix_fmt,
                            videoPlayer->avCodecContext->width,
                            videoPlayer->avCodecContext->height,
                            AV_PIX_FMT_YUV420P,
                            SWS_BICUBIC, NULL, NULL, NULL);

                    if (!sws_ctx) {
                        av_frame_free(&pFrameYUV420P);
                        av_free(pFrameYUV420P);
                        av_free(buffer);
                        pthread_mutex_unlock(&videoPlayer->codecMutex);
                        continue;
                    }
                    sws_scale(
                            sws_ctx,
                            reinterpret_cast<const uint8_t *const *>(avFrame->data),
                            avFrame->linesize,
                            0,
                            avFrame->height,
                            pFrameYUV420P->data,
                            pFrameYUV420P->linesize);
                    //渲染
                    double diff = videoPlayer->getFrameDiffTime(avFrame, NULL);
                    av_usleep(videoPlayer->getDelayTime(diff) * 1000000);
                    if (videoPlayer->callYuv != NULL) {
                        videoPlayer->callYuv(pFrameYUV420P->data[0],
                                             pFrameYUV420P->data[1],
                                             pFrameYUV420P->data[2],
                                             pFrameYUV420P->linesize[0],
                                             videoPlayer->avCodecContext->height,
                                             videoPlayer->ctx);
                    }
                    av_frame_free(&pFrameYUV420P);
                    av_free(pFrameYUV420P);
                    av_free(buffer);
                    sws_freeContext(sws_ctx);
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
    if (avFrame != NULL) {
        pts = av_frame_get_best_effort_timestamp(avFrame);
    }
    if (avPacket != NULL) {
        pts = avPacket->pts;
    }
    if (pts == AV_NOPTS_VALUE) {
        pts = 0;
    }
    //pts 是 当前帧 什么时候显示 。 time_base 是结构体 num  den 代表时间刻度
    //相乘？？？？
    pts *= av_q2d(time_base);
    LOGD("当前帧 pts %lf,timebase num %d  timebase den  %d", pts, time_base.num, time_base.den);

    if (pts > 0) {
        clock = pts;
    }

    double diff = audioPlayer->clock - clock;
    return diff;
}

double VideoPlayer::getDelayTime(double diff) {
    if (diff > 0.003) {
        delayTime = delayTime * 2 / 3;
        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 2;
        }
    } else if (diff < -0.003) {
        delayTime = delayTime * 3 / 2;
        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 2;
        }
    } else if (diff == 0.003) {

    }
    if (diff >= 0.5) {
        delayTime = 0;
    } else if (diff <= -0.5) {
        delayTime = defaultDelayTime * 2;
    }

    if (fabs(diff) >= 10) {
        delayTime = defaultDelayTime;
    }
    return delayTime;
}

void VideoPlayer::release() {
    if (queue != NULL) {
        delete (queue);
        queue = NULL;
    }
    if (abs_ctx != NULL) {
        av_bsf_free(&abs_ctx);
        abs_ctx = NULL;
    }
    if (avCodecContext != NULL) {
        pthread_mutex_lock(&codecMutex);
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
        pthread_mutex_unlock(&codecMutex);
    }

    if (playStatus != NULL) {
        playStatus = NULL;
    }

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
