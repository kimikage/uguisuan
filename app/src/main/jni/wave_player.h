/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_WAVE_PLAYER_H
#define UGUISUAN_WAVE_PLAYER_H

#include <cstdint>
#include <cstddef>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "wave_engine.h"

namespace uguisuan {

class WaveEngine;

class WavePlayer {

private:
    struct CallbackContext {
        SLint16 *pDataBase;    // Base adress of local audio data storage
        SLint16 *pData;        // Current adress of local audio data storage
        size_t size;
    };

private:
    // Local storage for Audio data in 16 bit words
    static const int AUDIO_DATA_STORAGE_SIZE = 4096 * 2;
    // Audio data buffer size in 16 bit words.
    static const int AUDIO_DATA_BUFFER_SIZE = AUDIO_DATA_STORAGE_SIZE / 16;

    static const int DEVICE_SHADOW_BUFFER_QUEUE_LEN = 4;

    static const int SAMPLE_CHANNELS = 1;

    static const int PLAY_KICKSTART_BUFFER_COUNT = 2;

    SLObjectItf mOutputMixObj;
    SLObjectItf mPlayerObj;
    SLPlayItf mPlayItf;
    SLAndroidSimpleBufferQueueItf mPlayBufferQueueItf;

    CallbackContext *mContext;

    SLint16 mPcmData[AUDIO_DATA_STORAGE_SIZE];

public:
    explicit WavePlayer(SLmilliHertz sampleRate, SLEngineItf engineItf);

    ~WavePlayer();

    SLresult Start(void);

    void Stop(void);

    void ProcessSLCallback(SLAndroidSimpleBufferQueueItf bq);

private:
    static void BqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

};

}
#endif //UGUISUAN_WAVE_PLAYER_H
