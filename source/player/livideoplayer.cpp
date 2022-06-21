#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <unistd.h>
#include "android/log.h"
#include "JavaCallHelper.h"
#include "LiFFmpeg.h"
#include "cstring"

extern "C" {
#include "libavcodec/avcodec.h"
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "libswresample/swresample.h"

}


#define TAG "zj"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

char errors[1024];
int err_code;
JavaVM *javaVm = 0;
JavaCallHelper *javaCallHelper;
LiFFmpeg *liFFmpeg;
ANativeWindow *window;


extern "C" JNIEXPORT jstring JNICALL
Java_com_licht_livideoplayer_TestActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(av_version_info());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_licht_livideoplayer_LiPlayer_native_1start(JNIEnv *env, jobject thiz,
                                                    jstring absolute_string, jobject surface) {

    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    const char *path = env->GetStringUTFChars(absolute_string, 0);

    avformat_network_init();
    AVFormatContext *formatContext = avformat_alloc_context();
    AVDictionary *opts = NULL;
    av_dict_set(&opts, "timeout", "3000000", 0);
    int ret = avformat_open_input(&formatContext, path, NULL, &opts);
    if (ret) {
        return;
    }
    avformat_find_stream_info(formatContext, NULL);
    int video_stram_idx = -1;
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stram_idx = i;
            break;
        }
    }

    AVCodecParameters *codecParameters = formatContext->streams[video_stram_idx]->codecpar;

    AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);

    AVCodecContext *avCodecContext = avcodec_alloc_context3(codec);

    avcodec_parameters_to_context(avCodecContext, codecParameters);
    avcodec_open2(avCodecContext, codec, NULL);
    AVPacket *packet = av_packet_alloc();

    SwsContext *swsContext =
            sws_getContext(avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
                           avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA,
                           SWS_BICUBLIN, 0, 0, 0);

    ANativeWindow_setBuffersGeometry(window, avCodecContext->width, avCodecContext->height,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer outNativeWindowBuffer;

    while (av_read_frame(formatContext, packet) >= 0) {
        //发送到队列中
        avcodec_send_packet(avCodecContext, packet);
        AVFrame *avFrame = av_frame_alloc();
        //从队列中取
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret < 0) {
            break;
        }

        uint8_t *dst_data[4];
        int dst_linesize[4];
        av_image_alloc(dst_data, dst_linesize, avCodecContext->width, avCodecContext->height,
                       AV_PIX_FMT_RGBA, 1);
        sws_scale(swsContext, avFrame->data, avFrame->linesize, 0, avFrame->height, dst_data,
                  dst_linesize);

        ANativeWindow_lock(window, &outNativeWindowBuffer, NULL);

        //渲染
        uint8_t *firstWindown = static_cast<uint8_t *>(outNativeWindowBuffer.bits);
        uint8_t *src_data = dst_data[0];
        //一行有多少个RGBA
        int destStride = outNativeWindowBuffer.stride * 4;
        int src_linesize = dst_linesize[0];
        for (int i = 0; i < outNativeWindowBuffer.height; ++i) {
            memcpy(firstWindown + i * destStride, src_data + i * src_linesize, destStride);
        }

        ANativeWindow_unlockAndPost(window);
        usleep(1000 * 16);
        av_frame_free(&avFrame);

    }
    env->ReleaseStringUTFChars(absolute_string, path);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_licht_livideoplayer_LiPlayer_startAudio(JNIEnv *env, jobject thiz, jstring std) {


}
extern "C"
JNIEXPORT void JNICALL
Java_com_licht_livideoplayer_LiPlayer_switchMp3ToPcm(JNIEnv *env, jobject thiz, jstring src,
                                                     jstring dest) {


    const char *input = env->GetStringUTFChars(src, 0);
    const char *outPut = env->GetStringUTFChars(dest, 0);
    avformat_network_init();


    AVFormatContext *formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, input, NULL, NULL) < 0) {
        LOGD("%s", "无法打开音频文件");
        return;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        LOGD("%s", "无法获取输入文件信息");
        return;
    }

    int audio_steam_index = -1;

    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_steam_index = i;
            break;
        }
    }

    AVCodecParameters *codecpar = formatContext->streams[audio_steam_index]->codecpar;
    AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);

    AVCodecContext *avCodecContext = avcodec_alloc_context3(codec);

    avcodec_parameters_to_context(avCodecContext, codecpar);

    int logare = avcodec_open2(avCodecContext, codec, NULL);
    if (logare < 0) {
        LOGE(" 尝试解码失败%d", logare);
        return;
    }


    SwrContext *swrContext = swr_alloc();
    int in_sample_rate = avCodecContext->sample_rate;
    AVSampleFormat in_sample = avCodecContext->sample_fmt;
    uint64_t in_channel_layout = avCodecContext->channel_layout;

    AVSampleFormat out_sample = AV_SAMPLE_FMT_S16;
    int out_sample_rate = 44100;
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;

    swr_alloc_set_opts(swrContext, out_ch_layout, out_sample, out_sample_rate, in_channel_layout,
                       in_sample, in_sample_rate, 0, NULL);

    swr_init(swrContext);


    AVPacket *packet = av_packet_alloc();
    int count = 0;
    uint8_t *out_buffer = (uint8_t *) (av_malloc(2 * 44100));
    FILE *fp_pcm = fopen(outPut, "wb");

    while (av_read_frame(formatContext, packet) >= 0) {
        int ret = avcodec_send_packet(avCodecContext, packet);
        LOGE("%d xx", ret);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret == AVERROR_EOF) {
            LOGE("解码完成");
            break;
        } else if (ret == AVERROR(EINVAL)) {
            LOGE("AVERROR(EINVAL)");
            continue;

        } else if (ret == AVERROR(ENOMEM)) {
            LOGE("AVERROR(ENOMEM)");
            break;

        }


        AVFrame *frame = av_frame_alloc();

        int cc = avcodec_receive_frame(avCodecContext, frame);
        LOGE("正在解码%d  frame%d", count++, cc);
        swr_convert(swrContext, &out_buffer, 2 * 44100, (const uint8_t **) frame->data,
                    frame->nb_samples);


        int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);

        int outBufferSize = av_samples_get_buffer_size(NULL, out_channel_nb, frame->nb_samples,
                                                       out_sample, 1);

        fwrite(out_buffer, 1, outBufferSize, fp_pcm);
    }

    fclose(fp_pcm);
    av_free(out_buffer);
    swr_free(&swrContext);
    avcodec_close(avCodecContext);
    avformat_close_input(&formatContext);
    fclose(fp_pcm);


    env->ReleaseStringUTFChars(src, input);
    env->ReleaseStringUTFChars(dest, outPut);


}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern "C"
JNIEXPORT void JNICALL
Java_com_licht_livideoplayer_LiPlayer_setSufaceNative(JNIEnv *env, jobject thiz, jobject surface) {
    pthread_mutex_lock(&mutex);
    if (window){
        ANativeWindow_release(window);
        window =0;
    }
    window = ANativeWindow_fromSurface(env,surface);
    pthread_mutex_unlock(&mutex);
}

void renderFrame(uint8_t *data,int lineSize,int width,int height){
    pthread_mutex_lock(&mutex);
    if (!window){
        pthread_mutex_unlock(&mutex);
        return;
    }
    ANativeWindow_setBuffersGeometry(window,width,height,WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;
    if (ANativeWindow_lock(window,&windowBuffer,0)){
        ANativeWindow_release(window);
        window =0;
        pthread_mutex_unlock(&mutex);
        return;
    }
    uint8_t *dst_data = static_cast<uint8_t *>(windowBuffer.bits);

    int dst_lineSize =   windowBuffer.stride*4;

    for (int i = 0; i < windowBuffer.height; ++i) {
        memcpy(dst_data+i*dst_lineSize,data+i*lineSize,dst_lineSize);
    }

    ANativeWindow_unlockAndPost(window);
    pthread_mutex_unlock(&mutex);


}


extern "C"
JNIEXPORT void JNICALL
Java_com_licht_livideoplayer_LiPlayer_prepareNative(JNIEnv *env, jobject thiz,
                                                    jstring data_source_) {
    const char *dataSource = env->GetStringUTFChars(data_source_, nullptr);
    javaCallHelper = new JavaCallHelper(javaVm, env, thiz);
    liFFmpeg = new LiFFmpeg(javaCallHelper, const_cast<char *>(dataSource));
    liFFmpeg->setRenderCallback(renderFrame);
    liFFmpeg->prepare();
    env->ReleaseStringUTFChars(data_source_, dataSource);

}


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVm = vm;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_licht_livideoplayer_LiPlayer_startNative(JNIEnv *env, jobject thiz) {
    if (liFFmpeg){
        liFFmpeg->start();
    }
}



