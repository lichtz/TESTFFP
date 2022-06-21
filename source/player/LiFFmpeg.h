//
// Created by zhaoy on 2021/9/22.
//

#ifndef LIVIDEO_LIFFMPEG_H
#define LIVIDEO_LIFFMPEG_H

#include "JavaCallHelper.h"
#include "AudioChannel.h"
#include "VideoChannel.h"

class LiFFmpeg {
public:
    LiFFmpeg(JavaCallHelper *pHelper, char *string);

    ~LiFFmpeg();

    void prepare();

    void _prepare();

    void start();

    void _start();

    void setRenderCallback(RenderCallback r);


private:
    JavaCallHelper *javaCallHelper = 0;
    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    char *dataSource;
    pthread_t pid_prepare;
    pthread_t pid_start;
    bool isPlaying;
    RenderCallback renderCallback;


};


#endif //LIVIDEO_LIFFMPEG_H
