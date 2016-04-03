/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#include <jni.h>

#include "wave_engine.h"
#include "log.h"

namespace uguisuan {

extern "C" {
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved);

JNIEXPORT void JNICALL
        Java_com_github_kimikage_uguisuan_WavePlayerService_createSLEngine(JNIEnv *env,
                                                                           jclass type);
JNIEXPORT void JNICALL
        Java_com_github_kimikage_uguisuan_WavePlayerService_deleteSLEngine(JNIEnv *env,
                                                                           jclass type);
JNIEXPORT void JNICALL
        Java_com_github_kimikage_uguisuan_WavePlayerService_startPlay(JNIEnv *env, jclass type,
                                                                      jbyteArray pcm);
JNIEXPORT void JNICALL
        Java_com_github_kimikage_uguisuan_WavePlayerService_stopPlay(JNIEnv *env, jclass type);
}

namespace g {
JavaVM *vm;
jclass classWavePlayerService;
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    g::vm = vm;
    JNIEnv *env = nullptr;
    vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    jclass localClass = env->FindClass(u8"com/github/kimikage/uguisuan/WavePlayerService");
    jclass globalClass = static_cast<jclass>(env->NewGlobalRef(localClass));
    g::classWavePlayerService = globalClass;
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_com_github_kimikage_uguisuan_WavePlayerService_createSLEngine(JNIEnv *env, jclass type) {
    WaveEngine &engine = WaveEngine::GetInstance();
    engine.CreateEngine();
    WavePlayer *player = engine.GetPlayer();
    player->SetCallback(g::vm, g::classWavePlayerService, u8"update");
}

JNIEXPORT void JNICALL
Java_com_github_kimikage_uguisuan_WavePlayerService_startPlay(JNIEnv *env, jclass type,
                                                              jbyteArray pcm) {
    WavePlayer *player = WaveEngine::GetInstance().GetPlayer();
    assert(player);

    jboolean isCopy = JNI_FALSE;
    const SLint16 *pPcm = reinterpret_cast<const SLint16 *>(env->GetByteArrayElements(pcm,
                                                                                      &isCopy));
    jsize size = env->GetArrayLength(pcm);
    player->SetContext(pPcm, pPcm, size / sizeof(SLint16));

    if (SL_BOOLEAN_FALSE == player->Start()) {
        LOGE("====%s failed", __FUNCTION__);
        return;
    }
}

JNIEXPORT void JNICALL
Java_com_github_kimikage_uguisuan_WavePlayerService_stopPlay(JNIEnv *env, jclass type) {
    WavePlayer *player = WaveEngine::GetInstance().GetPlayer();
    assert(player);

    player->Stop();
}

JNIEXPORT void JNICALL
Java_com_github_kimikage_uguisuan_WavePlayerService_deleteSLEngine(JNIEnv *env, jclass type) {
    WaveEngine::GetInstance().DeleteEngine();
}

}
