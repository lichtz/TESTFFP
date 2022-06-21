//
// Created by zhaoy on 2021/9/22.
//

#include "JavaCallHelper.h"
#include "macro.h"

JavaCallHelper::JavaCallHelper(JavaVM *javaVm_, JNIEnv *env_, jobject instance_){

    this->javaVm = javaVm_;
    this->env = env_;
    this->instance = env->NewGlobalRef(instance_);
    jclass clazz = env->GetObjectClass(instance_);
    jme_prepared = env->GetMethodID(clazz,"onPrepare","()V");

}

JavaCallHelper::~JavaCallHelper() {
    javaVm = 0;
    env->DeleteGlobalRef(instance);
    instance =0;
}

void JavaCallHelper::onPrepared(int threadMode) {
    if (threadMode == THREAD_MAIN){
        env->CallVoidMethod(instance,jme_prepared);
    } else {
        JNIEnv *env_child;
        javaVm->AttachCurrentThread(&env_child,0);
        env_child->CallVoidMethod(instance,jme_prepared);
        javaVm->DetachCurrentThread();
    }


}

void JavaCallHelper::onError(int i, int error) {

}
