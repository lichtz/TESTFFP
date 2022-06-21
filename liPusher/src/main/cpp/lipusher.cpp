#include <jni.h>
#include <string>
#include "x264.h"
#include "librtmp/rtmp.h"
#include "VideoTrack.h"
#include "pthread.h"
#include "../../../../livideoplayer/src/main/cpp/macro.h"


VideoTrack *videoTrack;

extern "C" JNIEXPORT jstring JNICALL
Java_com_licht_lipusher_NativeLib_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    x264_picture_t  *x264Picture = new x264_picture_t;
    RTMP_Alloc();

    return env->NewStringUTF("xxxx");
}




extern "C"
JNIEXPORT void JNICALL
Java_com_licht_lipusher_LivePusher_pusherInit(JNIEnv *env, jobject thiz) {
   videoTrack =  new VideoTrack;

}
extern "C"
JNIEXPORT void JNICALL
Java_com_licht_lipusher_LivePusher_native_1setVideoEncInfo(JNIEnv *env, jobject thiz, jint width,
                                                           jint height, jint fps, jint bitrate) {
    if (!videoTrack){
        return;
    }
    videoTrack->setVideoEncInfo(width,height,fps,bitrate);

}


pthread_t pid;

uint32_t start_time;
int32_t readyPushing =0;
void *start(void *args){

    char *url = static_cast<char *>(args);
    RTMP *rtmp = 0;
    rtmp = RTMP_Alloc();
    if (!rtmp){
        LOGE("alloc rtmp 失败");
        return NULL;
    }

    RTMP_Init(rtmp);
 int ret = RTMP_SetupURL(rtmp,url);
    if (!ret){
        LOGE("设置地址失败");
    }

    rtmp->Link.timeout = 5;
    RTMP_EnableWrite((rtmp));
    ret = RTMP_Connect(rtmp,0);
    if (!ret){
        LOGE("连接服务器失败：%s",url);
        return  NULL;
    }
    ret =  RTMP_ConnectStream(rtmp,0);
    if (!ret){
        LOGE("连接流失败：%s",url);
        return  NULL;
    }

    start_time  = RTMP_GetTime();

    readyPushing =1;

    RTMPPacket  *packet =0;
    while (readyPushing){
        //队列取数据


    }




}

int32_t  isStart = 0;
extern "C"
JNIEXPORT void JNICALL
Java_com_licht_lipusher_LivePusher_start_1pusher(JNIEnv *env, jobject thiz, jstring path_) {
 const  char *path =   env->GetStringUTFChars(path_,0);
    if (isStart){
        return;
    }
    isStart =1;
    char *url =  new char [strlen(path)+1];
    stpcpy(url,path);
    pthread_create(&pid,0,&start,url);
}