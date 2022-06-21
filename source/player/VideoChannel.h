//
// Created by zhaoy on 2021/9/22.
//

#ifndef LIVIDEO_VIDEOCHANNEL_H
#define LIVIDEO_VIDEOCHANNEL_H


#include "BaseChannel.h"
extern "C"{
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
};
typedef void (*RenderCallback)(uint8_t * ,int ,int ,int );
class VideoChannel : public BaseChannel {
public:
    VideoChannel(int id, AVCodecContext *pContext);

    ~VideoChannel();

    void start();

    void stop();

    void video_decode();

    void video_play();


    void setRenderCallback(RenderCallback renderCallback);

private:
    pthread_t pid_video_decode;
    pthread_t pid_video_play;
    RenderCallback  renderCallback;


};


#endif //LIVIDEO_VIDEOCHANNEL_H
