//
// Created by zhaoy on 2021/9/22.
//

#include "AudioChannel.h"
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"
#include "macro.h"

AudioChannel::AudioChannel(int id, AVCodecContext *pContext) : BaseChannel(id, pContext) {
    bufferx = (uint8_t *)malloc(44100*av_get_bytes_per_sample(pContext->sample_fmt)* av_get_channel_layout_nb_channels(pContext->channel_layout));
}

AudioChannel::~AudioChannel() {

}


void *task_audio_decode(void *args) {
    AudioChannel *audioChannel = static_cast<AudioChannel *>(args);

    audioChannel->audio_decode();

    return 0;
}


void *task_audio_play(void *args) {
    AudioChannel *audioChannel = static_cast<AudioChannel *>(args);

    audioChannel->audio_play(args);

    return 0;
}

void AudioChannel::start() {

     swrContext = swr_alloc();
    int  in_sample_rate= avCodecContext->sample_rate;
    AVSampleFormat in_sample = avCodecContext->sample_fmt;
    uint64_t in_channel_layout = avCodecContext->channel_layout;

    AVSampleFormat out_sample = AV_SAMPLE_FMT_S16;
    int out_sample_rate =44100;
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;

    swr_alloc_set_opts(swrContext, out_ch_layout, out_sample, out_sample_rate, in_channel_layout,
                       in_sample, in_sample_rate, 0, NULL);
     swr_init(swrContext);

    isPlaying = 1;
    LOGE("GETPCM  sample_rate %d",avCodecContext->sample_rate);
    LOGE("GETPCM  sample_format %d",avCodecContext->sample_fmt);
    LOGE("GETPCM  sample_channel %d",avCodecContext->channel_layout);
    LOGE("GETPCM  sample_channelsss %d",avCodecContext->channels);
    pthread_create(&pid_audio_decode, 0, task_audio_decode, this);
    pthread_setname_np(pid_audio_decode,"pid_audio_decode");

    pthread_create(&pid_audio_play, 0, task_audio_play, this);
    pthread_setname_np(pid_audio_play,"pid_audio_play");

}

void AudioChannel::stop() {

}

void AudioChannel::audio_decode() {
    AVPacket *avPacket = av_packet_alloc();
    int ser =1;
    while (isPlaying) {
        int ret =  packet_queue_get(&packetQueue,avPacket,1,&ser);
        if (!isPlaying) {
            break;
        }
        if (ret == -2){
           LOGE("audio_decode ret ==2 ");
            frameQueue.endIndex =1;
            break;
        }
        if (!ret) {
            av_packet_unref(avPacket);
            continue;
        }
        int sendPacketRet = avcodec_send_packet(avCodecContext, avPacket);
        av_packet_unref(avPacket);
        if (sendPacketRet) {
            break;
        }
        AVFrame *avFrame = av_frame_alloc();
        Frame *frame=  frame_queue_peek_writable(&frameQueue);
        int receiveFrameRet = avcodec_receive_frame(avCodecContext, avFrame);
        if (receiveFrameRet == AVERROR(EAGAIN)) {
            av_frame_unref(avFrame);
            continue;
        } else if (receiveFrameRet) {
            av_frame_unref(avFrame);
            break;
        }
        av_frame_move_ref(frame->frame,avFrame);
        frame_queue_push(&frameQueue);
        av_frame_unref(avFrame);
    }

    av_packet_unref(avPacket);


}


void AudioChannel::audio_play(void *pVoid) {
    AudioChannel *audioChannel = static_cast<AudioChannel *>(pVoid);
    audioChannel->initOpenSl();

}


void playerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    AudioChannel *audioChannel = static_cast<AudioChannel *>(context);
    int dataSize =   audioChannel->getPcm();
    LOGE("GETPCM dataSize%d",dataSize);
    if (dataSize >0 ){
        (*bufferQueueItf)->Enqueue(bufferQueueItf,audioChannel->bufferx,dataSize);
    }


}

void AudioChannel::initOpenSl() {
    //音频引擎
    SLEngineItf engineItf = NULL;

    SLObjectItf slEngineObject = NULL;
    //混音器
    SLObjectItf outputMixObject = NULL;
    //播放器
    SLObjectItf bqPlayerObject = NULL;

    SLPlayItf bqPlayerInterface = NULL;

    SLAndroidSimpleBufferQueueItf androidSimpleBufferQueueItf = NULL;
    SLresult sLResult;
    sLResult = slCreateEngine(&slEngineObject, 0, NULL, 0, NULL, NULL);
    if (SL_RESULT_SUCCESS != sLResult) {
        return;
    }
    sLResult = (*slEngineObject)->Realize(slEngineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != sLResult) {
        return;
    }
    sLResult = (*slEngineObject)->GetInterface(slEngineObject, SL_IID_ENGINE, &engineItf);

    if (SL_RESULT_SUCCESS != sLResult) {
        return;
    }

    sLResult = (*engineItf)->CreateOutputMix(engineItf, &outputMixObject, 0, 0, 0);

    if (SL_RESULT_SUCCESS != sLResult) {
        return;
    }
    sLResult = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != sLResult) {
        return;
    }
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq  = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2
    };
    SLuint32 channes = avCodecContext->channels;
    LOGE("SLES  channes::%d",channes);
    SLuint32 channelMask = 0;
    if (channes == 2){
       channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    }else{
        channelMask = SL_SPEAKER_FRONT_CENTER;
    }
    SLuint32 sle = avCodecContext->sample_rate*1000;


    SLDataFormat_PCM format_pcm  = {
            SL_DATAFORMAT_PCM,
            channes ,//立体声
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            channelMask, //前左前右
            SL_BYTEORDER_LITTLEENDIAN //小端模式
    };

    SLDataSource audioSrc = {&loc_bufq, &format_pcm};



    //混音器
    SLDataLocator_OutputMix loc_outmix  = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    //操作队列的接口
//    const SLInterfaceID  ids[1] = {SL_IID_BUFFERQUEUE};
//    const SLboolean  req[1] = {SL_BOOLEAN_TRUE};
    const SLboolean req[3] = {SL_BOOLEAN_FALSE,SL_BOOLEAN_FALSE,SL_BOOLEAN_FALSE};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE,SL_IID_EFFECTSEND,SL_IID_VOLUME};



    sLResult =  (*engineItf)->CreateAudioPlayer(engineItf,
                                    &bqPlayerObject, &audioSrc
            , &audioSnk
            , 3
            , ids  //播放队列ID
            , req); //是否使用内部缓存队列

    if (SL_RESULT_SUCCESS != sLResult){
        LOGE("CreateAudioPlayer error");
        return;
    }
    (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    sLResult =  (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerInterface);
    if (SL_RESULT_SUCCESS != sLResult){
        LOGE("CreateAudioPlayer GetInterface error");
        return;
    }

    //设置播放器回调
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                    &androidSimpleBufferQueueItf);


    (*androidSimpleBufferQueueItf)->RegisterCallback(androidSimpleBufferQueueItf, playerCallback,
                                                     this);

    (*bqPlayerInterface)->SetPlayState(bqPlayerInterface, SL_PLAYSTATE_PLAYING);
    playerCallback(androidSimpleBufferQueueItf, this);


}


int AudioChannel::getPcm() {
    int dataSize = 0;
    Frame *sp;
    while (isPlaying) {
        if (frameQueue.endIndex){
            break;
        }
        sp= frame_queue_peek_readable(&frameQueue);
        if (!isPlaying) {
            break;
        }
        int64_t  ss = sp->frame->pkt_duration;
        int out_size = av_samples_get_buffer_size(NULL,avCodecContext->channel_layout,avCodecContext->sample_rate,avCodecContext->sample_fmt,1);
        uint64_t dst_nb_samples = av_rescale_rnd(
                swr_get_delay(swrContext, sp->frame->sample_rate) + sp->frame->nb_samples, 44100,
                sp->frame->sample_rate,
                AV_ROUND_UP);

        int nb = swr_convert(swrContext, &bufferx, out_size, (const uint8_t **) sp->frame->data,
                             sp->frame->nb_samples);
        LOGE("GETPCM  nb %d",nb);
//        int nb = swr_convert(swrContext, &bufferx, 2*44100, (const uint8_t **) sp->frame->data,
//                             sp->frame->nb_samples);
//        dataSize = nb *avCodecContext->channels *av_get_bytes_per_sample(avCodecContext->sample_fmt);
//        dataSize = nb * av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO) * 44100;
        int out_channel_nb = av_get_channel_layout_nb_channels(  AV_CH_LAYOUT_STEREO);
         dataSize = av_samples_get_buffer_size(NULL, out_channel_nb, sp->frame->nb_samples,
                                               AV_SAMPLE_FMT_S16P, 1);
        frame_queue_unref_item(sp);
        frame_queue_next(&frameQueue);
        break;
    }


    return dataSize;
}

