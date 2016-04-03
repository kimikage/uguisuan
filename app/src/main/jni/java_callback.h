/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_JAVA_CALLBACK_H
#define UGUISUAN_JAVA_CALLBACK_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wpedantic"

#include <string>

#pragma clang diagnostic pop

#include <pthread.h>
#include <semaphore.h>
#include <jni.h>

namespace uguisuan {

class JavaCallback {
private:
private:
    pthread_t mThread;
    sem_t mSem;
    JavaVM *mVm;
    jclass mClass;
    std::string mMethodName;
    int mParam;
    bool mIsInitialized;
    bool mIsFinishing;

    static void *Run(void *args);

public:
    JavaCallback(JavaVM *vm, jclass clazz, const std::string methodName);

    ~JavaCallback();

    void Initialize();

    void Run(int param);
};

}
#endif //UGUISUAN_JAVA_CALLBACK_H
