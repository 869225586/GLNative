//
// Created by edz on 2020/8/14.
//

#include "AudioPlayer.h"

void *decodPlay(void *data) {
    AudioPlayer *audioPlayer = (AudioPlayer *) data;

    audioPlayer->initOpenSLEs();

    pthread_exit(&audioPlayer->thread_play);
}

void AudioPlayer::play() {
    //播放开启线程 进行初始化 sles 以及播放 音频
    pthread_create(&thread_play, NULL, decodPlay, this);
}

int AudioPlayer::resampleAudio() {
    data_size = 0;//每次拿到数据重采样 清空size  在解析音频时这个方法可能每秒回调很多次
    while (playStatus != NULL && !playStatus->exit) {
        if (playStatus->seek) {
            av_usleep(1000 * 100);
            continue;
        }
        // 判断是否是正在加载
        if (queue->getQueueSize() == 0) {
            //没有数据 加载
            if (playStatus->load) {
                playStatus->load = true;

            }
            av_usleep(1000 * 100);
            continue;
        } else {
            playStatus->load = false;
        }

        avPacket = av_packet_alloc();
        if (queue->getAvpacket(avPacket) != 0) {
            //没有拿到数据 释放资源
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        pthread_mutex_lock(&codecMutex);
        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret != 0) {
            releaseAvpacket();
            pthread_mutex_unlock(&codecMutex);
            continue;
        }
        avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, avFrame);

        if (ret == 0) {
            //获取声道布局
            if (avFrame->channels && avFrame->channel_layout == 0) {
                avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
            } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
                avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
            }

            SwrContext *swr_ctx;
            //这个函数无法对采样率进行冲采样 如果对采样率重采样 需要avfilter！
            swr_ctx = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO, //输出声道布局
                    AV_SAMPLE_FMT_S16,//输出 采样位数 16
                    avFrame->sample_rate, //输出采样率
                    avFrame->channel_layout,//输入声道布局
                    (AVSampleFormat) avFrame->format,//输入的采样位数
                    avFrame->sample_rate, //输入采样率
                    NULL, NULL
            );

            if (!swr_ctx || swr_init(swr_ctx) < 0) {
                releaseAvpacket();
                releaseAvFrame();
                swr_free(&swr_ctx);
                pthread_mutex_unlock(&codecMutex);
                continue;
            }
            //拿到重采样后的数据  buffer
            /**
             * number of samples output per channel,
             */
            int nb = swr_convert(
                    swr_ctx,
                    &buffer,
                    avFrame->nb_samples,
                    (const uint8_t **) avFrame->data,
                    avFrame->nb_samples);

            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            //计算pcm 大小 nb 位重采样后的 采样率
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

            currentTime = avFrame->pts * av_q2d(time_base);
            if (currentTime < clock) {
                currentTime = clock;
            }
            clock = currentTime;
            releaseAvpacket();
            releaseAvFrame();
            swr_free(&swr_ctx);
//            LOGD("音频重采样");
            pthread_mutex_unlock(&codecMutex);
            break;
        } else {
            //没有拿到avframe 释放资源
            releaseAvpacket();
            releaseAvFrame();
            pthread_mutex_unlock(&codecMutex);
            continue;
        }
    }

    return data_size;
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
//    LOGD("播放声音");
    AudioPlayer *wlAudio = (AudioPlayer *) context;
    if (wlAudio != NULL) {
        int buffersize = wlAudio->resampleAudio();
        if (buffersize > 0) {
            wlAudio->clock += buffersize / ((double) (wlAudio->sample_rate * 2 * 2));
            if (wlAudio->clock - wlAudio->preTime >= 0.5) { //间隔多久回调 0.5秒
                wlAudio->preTime = wlAudio->clock;
                LOGD("curPos %f",wlAudio->clock);
                //回调应用层
                wlAudio->callJava->onCallTimeInfo(wlAudio->clock, wlAudio->duration);
            }
            (*wlAudio->pcmBufferQueue)->Enqueue(wlAudio->pcmBufferQueue, (char *) wlAudio->buffer,
                                                buffersize);
        }
    }
}


/**
 * 初始化 SL ES
 */
void AudioPlayer::initOpenSLEs() {
    SLresult result;
    result = slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    //创建混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    (void) result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);

    (void) result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void) result;
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX,
                                         outputMixObject};
    SLDataSink audioSnk = {&outputMix, 0};

    // 第三步，配置PCM格式信息
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            2,//2个声道（立体声）
            static_cast<SLuint32>(getCurrentSampleRate(sample_rate)),//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };
    SLDataSource slDataSource = {&android_queue, &pcm};


    const SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE, SL_IID_PLAYBACKRATE};
    const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSnk, 2,
                                       ids, req);
    //初始化播放器
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);

//    得到接口后调用  获取Player接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);

//    注册回调缓冲区 获取缓冲队列接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    //缓冲接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);
//    获取播放状态接口
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    pcmBufferCallBack(pcmBufferQueue, this); //回调方法 内 开始播放数据
}

//根据传入的 采样率 获得一个用于计算的 类型 。。。
int AudioPlayer::getCurrentSampleRate(int sample_rate) {
    int rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

void AudioPlayer::pause() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);;
    }
}

void AudioPlayer::resume() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    }
}

void AudioPlayer::stop() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
}

void AudioPlayer::release() {
    if (queue != NULL) {
        delete (queue);
        queue = NULL;
    }

    if (pcmPlayerObject != NULL) {
        (*pcmPlayerObject)->Destroy(pcmPlayerObject);
        pcmPlayerObject = NULL;
        pcmPlayerPlay = NULL;
        pcmBufferQueue = NULL;
    }

    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    if (avCodecContext != NULL) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }

    if (playStatus != NULL) {
        playStatus = NULL;
    }
    if(callJava != NULL)
    {
        callJava = NULL;
    }
}


AudioPlayer::~AudioPlayer() {
    pthread_mutex_destroy(&codecMutex);
}

void AudioPlayer::releaseAvpacket() {
    av_packet_free(&avPacket);
    av_free(avPacket);
    avPacket = NULL;
}

void AudioPlayer::releaseAvFrame() {
    av_frame_free(&avFrame);
    av_free(avFrame);
    avFrame = NULL;
}

AudioPlayer::AudioPlayer(PlayStatus *playStatus, int sample_rate, CallJava *callJava) {
    this->playStatus = playStatus;
    this->sample_rate = sample_rate;
    queue = new StreamQueue(playStatus);
    //码率，是指一个数据流中每秒钟能通过的信息量，单位bps（bit per second）
    //码率 = 采样率 * 采样位数 * 声道数
    //计算出 pcm 数据大小  这里计算的是字节   采样位数 一般有 8bit  16bit 32 bit
    //所以这里计算的buffer 大小 我们 一般是 双声道 2  * 采样位数 16 bit =2 字节
    //buffer = 采样率（sample_rate）*采样位数（16 bit/8bit =2 byte） *声道数（2）
    buffer = static_cast<uint8_t *>(av_malloc(sample_rate * 2 * 2));
    pthread_mutex_init(&codecMutex, NULL);
    this->callJava = callJava;
}
