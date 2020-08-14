//
// Created by edz on 2020/8/14.
//

#include "FFmpeg.h"

FFmpeg::FFmpeg(const char *url) {
    this->url = url;
    exit = false;
    pthread_mutex_init(&init_mutex, NULL);
    pthread_mutex_init(&seek_mutex, NULL);

}

FFmpeg::~FFmpeg() {

}

void * decodeFFmpeg(void * data){
    FFmpeg * fFmpeg = static_cast<FFmpeg *>(data);
    fFmpeg->decodeFFmpegThread();
    pthread_exit(&fFmpeg->decodeThread);
}

void FFmpeg::preapared() {
    pthread_create(&decodeThread,NULL,decodeFFmpeg,this);
}
/**
 * 当播放退出时 防止继续请求网络解码 设置一个错误回调 结束 网络请求
 * @param ctx
 * @return
 */
int avformat_callback(void *ctx)
{
//    FFmpeg *fFmpeg = (FFmpeg *) ctx;
//    if(fFmpeg->playstatus->exit)
//    {
//        return AVERROR_EOF;
//    }
//    return 0;
}

/**
 * 子线程中进行解码
 */
void FFmpeg::decodeFFmpegThread() {
     pthread_mutex_lock(&init_mutex);
     av_register_all(); //注册
     avformat_network_init();//开启网络
     pFormatCtx=avformat_alloc_context();//申请上下文内存空间

     //防止播放退出还在请求网络 设置了一个回调
     pFormatCtx->interrupt_callback.callback = avformat_callback;
     pFormatCtx->interrupt_callback.opaque=this;

     if(avformat_open_input(&pFormatCtx,url,NULL,NULL)!=0){
         LOGE("cant open url : %s",url);
         //TODO 回调给native 端
         exitByInitError();
         return;
     }
     if(avformat_find_stream_info(pFormatCtx,NULL)<0){
         LOGE("没有找到 媒体流 ");

         //TODO 回调给native
         exitByInitError();
     }
     //遍历媒体流
     for(int i=0;i<pFormatCtx->nb_streams;i++){

     }

}

void FFmpeg::start() {

}

void FFmpeg::pause() {

}

void FFmpeg::resume() {

}

void FFmpeg::release() {

}

void FFmpeg::seek(int64_t seconds) {

}

int FFmpeg::getCodecContext(AVCodecParameters *codecpar, AVCodecContext **avCodecContext) {
    return 0;
}

void FFmpeg::exitByInitError() {
    exit = true;
    pthread_mutex_unlock(&init_mutex);
}


