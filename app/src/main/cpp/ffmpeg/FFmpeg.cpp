//
// Created by edz on 2020/8/14.
//

#include "FFmpeg.h"

FFmpeg::FFmpeg(PlayStatus *playStatus, CallJava *callJava, const char *url) {
    this->url = url;
    this->playStatus = playStatus;
    this->callJava = callJava;
    exit = false;
    videoPlayer = new VideoPlayer(playStatus,callJava);
    pthread_mutex_init(&init_mutex, NULL);
    pthread_mutex_init(&seek_mutex, NULL);
}

FFmpeg::~FFmpeg() {
    pthread_mutex_destroy(&init_mutex);
    pthread_mutex_destroy(&seek_mutex);
}

void *decodeFFmpeg(void *data) {
    FFmpeg *fFmpeg = static_cast<FFmpeg *>(data);
    fFmpeg->decodeFFmpegThread();
    pthread_exit(&fFmpeg->decodeThread);
}

void FFmpeg::preapared() {
    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);
    LOGD("decodeThread Create");
}

/**
 * 当播放退出时 防止继续请求网络解码 设置一个错误回调 结束 网络请求
 * @param ctx
 * @return
 */
int avformat_callback(void *ctx) {
    FFmpeg *fFmpeg = (FFmpeg *) ctx;
    if (fFmpeg->playStatus->exit) {
        return AVERROR_EOF;
    }
    return 0;
}

/**
 * 子线程中进行解码
 */
void FFmpeg::decodeFFmpegThread() {
    pthread_mutex_lock(&init_mutex);
    av_register_all(); //注册
    avformat_network_init();//开启网络
    pFormatCtx = avformat_alloc_context();//申请上下文内存空间

    //防止播放退出还在请求网络 设置了一个回调
    pFormatCtx->interrupt_callback.callback = avformat_callback;
    pFormatCtx->interrupt_callback.opaque = this;

    if (avformat_open_input(&pFormatCtx, url, NULL, NULL) != 0) {
        LOGE("can not open url : %s", url);
        //TODO 回调给native 端
        exitByInitError();
        return;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("没有找到 媒体流 ");

        //TODO 回调给native
        exitByInitError();
    }
    //遍历媒体流
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        AVStream *avStream = pFormatCtx->streams[i];
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            LOGD("解析到音频流");
            //获取到音频
            if (audioPlayer == NULL) {
                audioPlayer = new AudioPlayer(playStatus, avStream->codecpar->sample_rate);
                audioPlayer->streamIndex = i;
                audioPlayer->codePar = avStream->codecpar;
                //AV_TIME_BASE 是微妙 1000 000  ，得到几 秒
                audioPlayer->duration = pFormatCtx->duration / AV_TIME_BASE;
                audioPlayer->time_base = avStream->time_base;
                duration = audioPlayer->duration;
            }
        } else if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            LOGD("解析到视频流");
            //得到视频流
            if (!isCreateVideo) {
                videoPlayer->streamIndex = i;
                videoPlayer->codecpar = avStream->codecpar;
                videoPlayer->time_base = avStream->time_base;
                //计算 视频帧率  例如 每秒 25帧
                int num = pFormatCtx->streams[i]->avg_frame_rate.num;
                int den = pFormatCtx->streams[i]->avg_frame_rate.den;
//                LOGD("num%d,den %d",num,den);
                if (num != 0 && den != 0) {
                    int fps = num / den;//[25 / 1]
                    // //默认延时（音视频同步用） 每一帧的播放时间（1/帧率）
                    videoPlayer->defaultDelayTime = 1.0 / fps;
                }
                isCreateVideo = true;
            }

        }
    }

    //获取 解码器的上下文
    if (audioPlayer != NULL) {
        getCodecContext(audioPlayer->codePar, &audioPlayer->avCodecContext);
    }
    if (videoPlayer != NULL) {
        getCodecContext(videoPlayer->codecpar, &videoPlayer->avCodecContext);
    }
    start();
    pthread_mutex_unlock(&init_mutex);

}

void FFmpeg::start() {
//    LOGD("播放%p",audioPlayer);
    if (audioPlayer == NULL) {
        return;
    }
    if (videoPlayer == NULL) {
        return;
    }
    surpportMediaCodec = false;
    videoPlayer->audioPlayer = audioPlayer;
    const char *codecName = ((const AVCodec *) videoPlayer->avCodecContext->codec)->name;
    if (surpportMediaCodec = callJava->onCallIsSupportVideo(codecName)) {
        LOGE("当前设备支持硬解码当前视频");
        if (strcasecmp(codecName, "h264") == 0) {
            bsFilter = av_bsf_get_by_name("h264_mp4toannexb");
        } else if (strcasecmp(codecName, "h265") == 0) {
            bsFilter = av_bsf_get_by_name("hevc_mp4toannexb");
        }
        if (bsFilter == NULL) {
            goto end;
        }
        if (av_bsf_alloc(bsFilter, &videoPlayer->abs_ctx) != 0) {
            surpportMediaCodec = false;
            goto end;
        }
        if (avcodec_parameters_copy(videoPlayer->abs_ctx->par_in, videoPlayer->codecpar) < 0) {
            surpportMediaCodec = false;
            av_bsf_free(&videoPlayer->abs_ctx);
            videoPlayer->abs_ctx = NULL;
            goto end;
        }
        if (av_bsf_init(videoPlayer->abs_ctx) != 0) {
            surpportMediaCodec = false;
            av_bsf_free(&videoPlayer->abs_ctx);
            videoPlayer->abs_ctx = NULL;
            goto end;
        }
        videoPlayer->abs_ctx->time_base_in = videoPlayer->time_base;
    }
    end:
    surpportMediaCodec=false;
    if (surpportMediaCodec) {
        videoPlayer->codecType = CODEC_MEDIACODEC;
        videoPlayer->callJava->
                onCallInitMediacodec(
                codecName,
                videoPlayer->avCodecContext->width,
                videoPlayer->avCodecContext->height,
                videoPlayer->avCodecContext->extradata_size,
                videoPlayer->avCodecContext->extradata_size,
                videoPlayer->avCodecContext->extradata,
                videoPlayer->avCodecContext->extradata);
    }
    audioPlayer->play();
    videoPlayer->play();

    LOGD("当前视频的格式 %s", codecName);
    while (playStatus != NULL && !playStatus->exit) {
        if (playStatus->seek) {
            av_usleep(1000 * 100);
            continue;
        }
        if (audioPlayer->queue->getQueueSize() > 40) {
            av_usleep(1000 * 100);
            continue;
        }
        AVPacket *avPacket = av_packet_alloc();
        if (av_read_frame(pFormatCtx, avPacket) == 0) {
            if (avPacket->stream_index == audioPlayer->streamIndex) {
                audioPlayer->queue->putAvpacket(avPacket);
            } else if (avPacket->stream_index == videoPlayer->streamIndex) {
                videoPlayer->queue->putAvpacket(avPacket);
            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
            }
        } else {
            //解析完毕 释放 资源 如果没有退出 那么判断队列中是否有数据 如果有数据
            //循环等待 没有数据以后 将 播放退出状态置位 true
            av_packet_free(&avPacket);
            av_free(avPacket);
            while (playStatus != NULL && !playStatus->exit) {
                if (audioPlayer->queue->getQueueSize() > 0) {
                    av_usleep(1000 * 100);
                    continue;
                } else {
                    if (!playStatus->seek) {
                        av_usleep(1000 * 100);
                        playStatus->exit = true;
                    }
                    break;
                }
            }
            break;
        }
        exit = true;
    }
}

void FFmpeg::pause() {
    if (playStatus != NULL) {
        playStatus->pause = true;
    }

    if (audioPlayer != NULL) {
        audioPlayer->pause();
    }

}

void FFmpeg::resume() {
    if (playStatus != NULL) {
        playStatus->pause = false;
    }
    if (audioPlayer != NULL) {
        audioPlayer->resume();
    }
}


void FFmpeg::seek(int64_t seconds) {
    LOGE("seek time %d", seconds);
    if (duration <= 0) {
        return;
    }
    if (seconds >= 0 && seconds <= duration) {
        playStatus->seek = true;
        pthread_mutex_lock(&seek_mutex);
        int64_t rel = seconds * AV_TIME_BASE;
        LOGE("rel time %d", seconds);
        avformat_seek_file(pFormatCtx, -1,
                           INT64_MIN, rel, INT64_MAX, 0);
        if (audioPlayer != NULL) {
            audioPlayer->queue->clearQueue();
            audioPlayer->clock = 0;
            audioPlayer->preTime = 0;
            pthread_mutex_lock(&audioPlayer->codecMutex);
            avcodec_flush_buffers(audioPlayer->avCodecContext);
            pthread_mutex_unlock(&audioPlayer->codecMutex);
        }
        if (videoPlayer != NULL) {
            videoPlayer->queue->clearQueue();
            videoPlayer->clock = 0;
            pthread_mutex_lock(&videoPlayer->codecMutex);
            avcodec_flush_buffers(videoPlayer->avCodecContext);
            pthread_mutex_unlock(&videoPlayer->codecMutex);
        }
        pthread_mutex_unlock(&seek_mutex);
        playStatus->seek = false;
    }
}

int FFmpeg::getCodecContext(AVCodecParameters *codecpar, AVCodecContext **avCodecContext) {
    AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
    if (!dec) {
        exitByInitError();
        return -1;
    }
    *avCodecContext = avcodec_alloc_context3(dec);
    if (!audioPlayer->avCodecContext) {
        exitByInitError();
        return -1;
    }

    if (avcodec_parameters_to_context(*avCodecContext, codecpar) < 0) {
        exitByInitError();
        return -1;
    }

    if (avcodec_open2(*avCodecContext, dec, 0) != 0) {
        exitByInitError();
        return -1;
    }
    return 0;
}

void FFmpeg::exitByInitError() {
    exit = true;
    pthread_mutex_unlock(&init_mutex);
}

void FFmpeg::release() {
    LOGE("开始释放Ffmpe");
    playStatus->exit = true;
    pthread_mutex_lock(&init_mutex);
    //如果 没有播放完毕等一秒再退出 防止直接释放资源导致指针问题
    int sleepCount = 0;
    while (!exit) {
        if (sleepCount > 1000) {
            exit = true;
        }
        LOGE("wait ffmpeg  exit %d", sleepCount);
        sleepCount++;
        av_usleep(1000 * 10);//暂停10毫秒
    }
    LOGE("释放 Audio");
    if (audioPlayer != NULL) {
        audioPlayer->release();
        delete (audioPlayer);
        audioPlayer = NULL;
    }

    LOGE("释放 video");
    if (videoPlayer != NULL) {
        videoPlayer->release();
        delete (videoPlayer);
        videoPlayer = NULL;
    }
    LOGE("释放 封装格式上下文");
    if (pFormatCtx != NULL) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = NULL;
    }
    LOGE("释放 callJava");
    /* if(callJava != NULL)
     {
         callJava = NULL;
     }*/
    LOGE("释放 playstatus");
    if (playStatus != NULL) {
        playStatus = NULL;
    }
    pthread_mutex_unlock(&init_mutex);
}

