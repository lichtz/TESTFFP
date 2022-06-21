//
// Created by zhaoy on 2021/9/22.
//

#ifndef LIVIDEO_MACRO_H
#define LIVIDEO_MACRO_H
#include "android/log.h"



#define DELETE(object) if(object){delete object;object = 0;}


#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"LIPLAYER",__VA_ARGS__)


#define  THREAD_MAIN 1
#define THREAD_CHILD 2

#define ERROR_CODE_FFMPEG_PREPARE -1000

#define FFMPEG_CAN_NOT_OPEN_URL (ERROR_CODE_FFMPEG_PREPARE -1)

#endif //LIVIDEO_MACRO_H
