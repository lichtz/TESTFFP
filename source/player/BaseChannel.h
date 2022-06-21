//
// Created by zhaoy on 2021/9/22.
//

#ifndef LIVIDEO_BASECHANNEL_H
#define LIVIDEO_BASECHANNEL_H

#include "queen.h"

extern "C" {
#include "libavformat/avformat.h"
};

class BaseChannel {

public:
    BaseChannel(int id,AVCodecContext *codecContext):id(id),avCodecContext(codecContext) {
//        packets.setReleaseCallback(releaseAvPacket);
//        frames.setReleaseCallback(releaseAvFrame);
    }

    virtual ~BaseChannel() {
        packet_queue_destroy(&packetQueue);
        frame_queue_destory(&frameQueue);

    }

    static void releaseAvPacket(AVPacket **packet) {
        if (packet) {
            av_packet_free(packet);
            *packet = 0;
        }
    }

    static void releaseAvFrame(AVFrame **frame) {
        if (frame) {
            av_frame_free(frame);
            *frame = 0;
        }
    }

    virtual void start() = 0;

    virtual void stop() = 0;

    PacketQueue packetQueue;
    FrameQueue  frameQueue;
    int id;
    bool isPlaying = 0;
    AVCodecContext  *avCodecContext;
    int eof = 0;

};


#endif //LIVIDEO_BASECHANNEL_H
