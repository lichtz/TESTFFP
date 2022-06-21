//
// Created by zhaoy on 2021/9/22.
//

#include "VideoChannel.h"
#include "macro.h"

VideoChannel::VideoChannel(int id, AVCodecContext *pContext) : BaseChannel(id, pContext) {}

VideoChannel::~VideoChannel() {

}

void VideoChannel::video_play() {

    uint8_t *dst_data[4];
    int dst_linesize[4];
    SwsContext *swsContext = sws_getContext(avCodecContext->width, avCodecContext->height,
                                            avCodecContext->pix_fmt, avCodecContext->width,
                                            avCodecContext->height, AV_PIX_FMT_RGBA, SWS_BICUBLIN,
                                            NULL, NULL, NULL);
    av_image_alloc(dst_data, dst_linesize, avCodecContext->width, avCodecContext->height,
                   AV_PIX_FMT_RGBA, 1);

    Frame *vp;
    while (1) {
        if (!isPlaying ){
            LOGE("END_____releaseAvFrame_EMPTY_END");
            break;
        }
        if (frame_queue_nb_remaining(&frameQueue)> 0){
            vp = frame_queue_peek(&frameQueue);
            sws_scale(swsContext, vp->frame->data, vp->frame->linesize, 0, avCodecContext->height, dst_data,
                      dst_linesize);

            frame_queue_next(&frameQueue);
            if (!isPlaying) {
                LOGE("END_____releaseAvFrame_END");
                vp = NULL;
                break;
            }
            renderCallback(dst_data[0], dst_linesize[0], avCodecContext->width, avCodecContext->height);
            av_usleep(16*1000);
        }


    }

    av_freep(&dst_data[0]);
    isPlaying = 0;
    LOGE("BUBUBU");
}


void VideoChannel::video_decode() {
    AVPacket *avPacket = av_packet_alloc();
    int ser =1;
    while (1) {
        if (!isPlaying){
            LOGE("video_decode end");
            break;
        }
        int ret =  packet_queue_get(&packetQueue,avPacket,1,&ser);

        if (ret == -2){
            LOGE("video_decode packetQueen end");
            av_packet_unref(avPacket);
            break;
        }
        if (!ret) {
            av_packet_unref(avPacket);
            LOGE("video_decode ret fasle");
            continue;
        }
        int sendPacketRet = avcodec_send_packet(avCodecContext, avPacket);
        av_packet_unref(avPacket);

        if (sendPacketRet) {
            LOGE("sendPacketRet");
            break;
        }
        AVFrame *avFrame = av_frame_alloc();
        Frame *frame=  frame_queue_peek_writable(&frameQueue);
        int receiveFrameRet = avcodec_receive_frame(avCodecContext, avFrame);
        if (receiveFrameRet == AVERROR(EAGAIN)) {
            av_frame_unref(avFrame);
            LOGE("AVERROR");
            continue;
        } else if (receiveFrameRet) {
            av_frame_unref(avFrame);
            LOGE("AVERROR___22");
            break;
        }
        av_frame_move_ref(frame->frame,avFrame);
        frame_queue_push(&frameQueue);
        av_frame_unref(avFrame);
    }

    av_packet_unref(avPacket);
    LOGE("AVERROR___zuzuxia");

}

void *task_video_decode(void *args) {
    VideoChannel *videoChannel = static_cast<VideoChannel *>(args);

    videoChannel->video_decode();

    return 0;
}


void *task_video_play(void *args) {
    VideoChannel *videoChannel = static_cast<VideoChannel *>(args);

    videoChannel->video_play();

    return 0;
}

void VideoChannel::start() {
    isPlaying = 1;

    pthread_create(&pid_video_decode, NULL,task_video_decode , this);
    pthread_setname_np(pid_video_decode,"pid_video_decode");
    pthread_create(&pid_video_play, NULL, task_video_play, this);
    pthread_setname_np(pid_video_play,"pid_video_play");


}

void VideoChannel::stop() {
    LOGE("END__FREE%d",frameQueue.size);

}

void VideoChannel::setRenderCallback(RenderCallback callback) {
    this->renderCallback = callback;
}








