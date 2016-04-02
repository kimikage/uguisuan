/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#include "java_callback.h"
#include "log.h"

namespace uguisuan {

JavaCallback::JavaCallback(JavaVM *vm, jclass clazz, std::string methodName) :
        mVm(vm), mClass(clazz), mMethodName(methodName),
        mParam(0), mIsInitialized(false), mIsFinishing(false) {
}

void JavaCallback::initialize() {
    sem_init(&mSem, 0, 0); // not shared
    int ret = pthread_create(&mThread, nullptr, &run, this);
    if (ret != 0) {
        LOGD("failed: pthread_create");
    }
    mIsInitialized = true;
}

void *JavaCallback::run(void *args) {
    JavaCallback *data = reinterpret_cast<JavaCallback *>(args);
    JNIEnv *env = nullptr;
    jint ret;
    LOGD("JavaCallback: start");

    ret = data->mVm->AttachCurrentThread(&env, nullptr);

    if (ret != JNI_OK || env == nullptr) {
        LOGD("failed: AttachCurrentThread");
        return nullptr;
    }

    jclass clazz = data->mClass;

    jmethodID methodId = env->GetStaticMethodID(clazz, data->mMethodName.c_str(), u8"(I)V");
    if (methodId == 0) {
        LOGD("failed: GetStaticMethodID");
        data->mVm->DetachCurrentThread();
        return nullptr;
    }


    int last = -1;
    for (; ;) {
        sem_wait(&data->mSem);
        if (data->mIsFinishing) {
            break;
        }
        int current = data->mParam;
        if (last != current) {
            env->CallStaticVoidMethod(clazz, methodId, current);
            last = current;
        }
    }

    data->mVm->DetachCurrentThread();

    LOGD("JavaCallback: end");
    return nullptr;
}

void JavaCallback::run(int param) {
    mParam = param;
    sem_post(&mSem);
}

JavaCallback::~JavaCallback() {
    if (mIsInitialized) {
        mIsFinishing = true;
        sem_post(&mSem);
        pthread_join(mThread, nullptr);
        pthread_detach(mThread);
        sem_destroy(&mSem);
    }
}

}
