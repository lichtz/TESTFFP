//
// Created by zhaoy on 2021/9/22.
//

#ifndef LIVIDEO_JAVACALLHELPER_H
#define LIVIDEO_JAVACALLHELPER_H
#include <jni.h>

class JavaCallHelper {

public:
    JavaCallHelper(JavaVM *javaVm_, JNIEnv *env_, jobject instance_);

     ~JavaCallHelper();

    void onPrepared(int threadMode);

    void onError(int i, int errorCode);

private:
    JavaVM *javaVm;
    JNIEnv *env ;
    jobject instance;
    jmethodID  jme_prepared;

};


#endif //LIVIDEO_JAVACALLHELPER_H
