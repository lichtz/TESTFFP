//
// Created by zhaoy on 2021/9/22.
//

#ifndef LIVIDEO_AUDIOCHANNEL_H
#define LIVIDEO_AUDIOCHANNEL_H


#include "BaseChannel.h"
extern  "C"{
#include "libswresample/swresample.h"

};


class AudioChannel : public BaseChannel {
public:
    AudioChannel(int id, AVCodecContext *pContext);

    ~AudioChannel();
    void start();
    void stop();

    void audio_decode();

    void audio_play(void *pVoid);

    int getPcm();
    uint8_t  *bufferx;

private:
    pthread_t pid_audio_decode;
    pthread_t pid_audio_play;

    void initOpenSl();

    SwrContext  *swrContext;


};


#endif //LIVIDEO_AUDIOCHANNEL_H
