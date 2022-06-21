#include <jni.h>
#include "audio/AudioEncoder.h"
//
// Created by zhaoy on 2022/4/18.
//

//#define LOG_TAG "AudioEncoder"
void encodeAACUsingFFmpeg(const char *pcmPath, const char *aacPath, jint channels, jint bitRate,
                          jint sampleRate);

extern "C"
JNIEXPORT void JNICALL
Java_com_example_opengles_AudioToolsHelper_encodePCMToAAC(JNIEnv *env, jobject thiz,
                                                          jstring pcm_path_param, jint audio_channels,
                                                          jint bit_rate, jint sample_rate,
                                                          jstring aac_path_param) {
   const char * pcmPath = env->GetStringUTFChars(pcm_path_param,JNI_FALSE);
   const char * aacPath = env->GetStringUTFChars(aac_path_param,JNI_FALSE);
   encodeAACUsingFFmpeg(pcmPath,aacPath,audio_channels,bit_rate,sample_rate);
    env->ReleaseStringUTFChars(pcm_path_param,pcmPath);
    env->ReleaseStringUTFChars(aac_path_param,aacPath);
}

void encodeAACUsingFFmpeg(const char *pcmPath, const char *aacPath, jint channels, jint bitRate,
                          jint sampleRate) {

//  AudioEncoder *audioEncoder =  new AudioEncoder();
//  audioEncoder->init(1,2,1,1,"a","a");
}
