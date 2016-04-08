/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_WAVE_PLAYER_H
#define UGUISUAN_WAVE_PLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "wave_engine.h"
#include "java_callback.h"
#include "filter/graph.h"

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

    JavaCallback *mJavaCallback;
    filter::Graph *mGraph;
    int16_t mPcm[AUDIO_DATA_BUFFER_SIZE];

public:
    explicit WavePlayer(filter::Graph *graph, SLmilliHertz sampleRate, SLEngineItf engineItf);

    ~WavePlayer();

    void SetCallback(JavaVM *vm, jclass clazz, std::string methodName);

    SLresult Start(void);

    void Stop(void);

    void ProcessSLCallback(SLAndroidSimpleBufferQueueItf bq);

private:
    static void BqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

};

}
#endif //UGUISUAN_WAVE_PLAYER_H
