/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_WAVE_PLAYER_H
#define UGUISUAN_WAVE_PLAYER_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"

#include <cstdint>
#include <cstddef>

#pragma clang diagnostic pop

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "wave_engine.h"
#include "wave_context.h"
#include "java_callback.h"

namespace uguisuan {

class WaveEngine;

class WavePlayer {

private:
    // Audio data buffer size in 16 bit words.
    static const int AUDIO_DATA_BUFFER_SIZE = 4096 * 2 / 16;

    static const int DEVICE_SHADOW_BUFFER_QUEUE_LEN = 4;

    static const int SAMPLE_CHANNELS = 1;

    static const int PLAY_KICKSTART_BUFFER_COUNT = 2;

    SLObjectItf mOutputMixObj;
    SLObjectItf mPlayerObj;
    SLPlayItf mPlayItf;
    SLVolumeItf mVolumeItf;
    SLAndroidSimpleBufferQueueItf mPlayBufferQueueItf;

    WaveContext mContext;
    JavaCallback *mJavaCallback;

public:
    explicit WavePlayer(SLmilliHertz sampleRate, SLEngineItf engineItf);

    ~WavePlayer();

    void SetContext(const SLint16 *base, const SLint16 *current, size_t size);

    void SetCallback(JavaVM *vm, jclass clazz, std::string methodName);

    SLresult Start(void);

    void Stop(void);

    void ProcessSLCallback(SLAndroidSimpleBufferQueueItf bq);

private:
    static void BqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

};

}
#endif //UGUISUAN_WAVE_PLAYER_H
