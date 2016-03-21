/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#include <jni.h>

#include <SLES/OpenSLES.h>

#include "wave_engine.h"
#include "log.h"

namespace uguisuan {

extern "C" {
JNIEXPORT void JNICALL
        Java_com_github_kimikage_uguisuan_MainActivity_createSLEngine(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL
        Java_com_github_kimikage_uguisuan_MainActivity_deleteSLEngine(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL
        Java_com_github_kimikage_uguisuan_MainActivity_startPlay(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL
        Java_com_github_kimikage_uguisuan_MainActivity_stopPlay(JNIEnv *env, jclass type);
}

JNIEXPORT void JNICALL
Java_com_github_kimikage_uguisuan_MainActivity_createSLEngine(JNIEnv *env, jclass type) {
    WaveEngine::GetInstance().CreateEngine();
}

JNIEXPORT void JNICALL
Java_com_github_kimikage_uguisuan_MainActivity_startPlay(JNIEnv *env, jclass type) {
    WavePlayer *player = WaveEngine::GetInstance().getPlayer();
    assert(player);

    if (SL_BOOLEAN_FALSE == player->Start()) {
        LOGE("====%s failed", __FUNCTION__);
        return;
    }
}

JNIEXPORT void JNICALL
Java_com_github_kimikage_uguisuan_MainActivity_stopPlay(JNIEnv *env, jclass type) {
    WavePlayer *player = WaveEngine::GetInstance().getPlayer();
    assert(player);

    player->Stop();
}

JNIEXPORT void JNICALL
Java_com_github_kimikage_uguisuan_MainActivity_deleteSLEngine(JNIEnv *env, jclass type) {
    WaveEngine::GetInstance().DeleteEngine();
}

}
