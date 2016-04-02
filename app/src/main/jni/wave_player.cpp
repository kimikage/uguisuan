/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#include "wave_player.h"

namespace uguisuan {

void WavePlayer::BqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    (static_cast<WavePlayer *>(context))->ProcessSLCallback(bq);
}


void WavePlayer::ProcessSLCallback(SLAndroidSimpleBufferQueueItf bq) {
    assert(bq == mPlayBufferQueueItf);

    SLresult result;
    size_t remain = mContext.base + mContext.size - mContext.current;
    if (remain > 0) {
        size_t size = remain < AUDIO_DATA_BUFFER_SIZE ? remain : AUDIO_DATA_BUFFER_SIZE;
        SLint16 *p = const_cast<SLint16 *>(mContext.current);
        result = (*bq)->Enqueue(bq, static_cast<void *>(p),
                                size * sizeof(SLint16)); // Size given in bytes
        if (result != SL_RESULT_SUCCESS) {
            CheckError(result);
        }
        mContext.current += size;
    }
    if (mJavaCallback != nullptr) {
        if (remain > 0) {
            mJavaCallback->run(mContext.size - remain);
        } else {
            mJavaCallback->run(-1);
        }
    }
}

WavePlayer::WavePlayer(SLmilliHertz sampleRate, SLEngineItf engineItf) :
        mOutputMixObj(nullptr),
        mPlayerObj(nullptr),
        mPlayItf(nullptr),
        mVolumeItf(nullptr),
        mPlayBufferQueueItf(nullptr),
        mJavaCallback(nullptr) {
    SLresult result;

    result = (*engineItf)->CreateOutputMix(engineItf, &mOutputMixObj, 0, nullptr, nullptr);
    CheckError(result);

    // realize the output mix in synchronous mode
    result = (*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE);
    CheckError(result);

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue locatorBq = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            DEVICE_SHADOW_BUFFER_QUEUE_LEN};

    SLAndroidDataFormat_PCM_EX format;
    format.formatType = SL_DATAFORMAT_PCM; //SL_ANDROID_DATAFORMAT_PCM_EX;
    format.numChannels = SAMPLE_CHANNELS;
    format.sampleRate = sampleRate;
    format.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    format.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;;
    format.channelMask = SL_SPEAKER_FRONT_CENTER;
    format.endianness = SL_BYTEORDER_LITTLEENDIAN;
    format.representation = SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT;
    SLDataSource source = {&locatorBq, &format};

    // configure audio sink
    SLDataLocator_OutputMix locatorOut = {
            SL_DATALOCATOR_OUTPUTMIX,
            mOutputMixObj};
    SLDataSink sink = {&locatorOut, nullptr};

    SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
    SLboolean req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineItf)->CreateAudioPlayer(engineItf, &mPlayerObj, &source, &sink,
                                             sizeof(ids) / sizeof(ids[0]), ids, req);
    CheckError(result);

    // realize the player in synchronous mode
    result = (*mPlayerObj)->Realize(mPlayerObj, SL_BOOLEAN_FALSE);
    CheckError(result);

    // get the play interface
    result = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_PLAY, &mPlayItf);
    CheckError(result);

    // get the volume interface
    result = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_VOLUME, &mVolumeItf);
    CheckError(result);

    // get the buffer queue interface
    result = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_BUFFERQUEUE, &mPlayBufferQueueItf);
    CheckError(result);

    // register callback on the buffer queue
    result = (*mPlayBufferQueueItf)->RegisterCallback(mPlayBufferQueueItf, BqPlayerCallback, this);
    CheckError(result);

    result = (*mPlayItf)->SetPlayState(mPlayItf, SL_PLAYSTATE_STOPPED);
    CheckError(result);

}

WavePlayer::~WavePlayer() {

    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (mPlayerObj != nullptr) {
        (*mPlayerObj)->Destroy(mPlayerObj);
        mPlayerObj = nullptr;
        mPlayItf = nullptr;
        mVolumeItf = nullptr;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (mOutputMixObj != nullptr) {
        (*mOutputMixObj)->Destroy(mOutputMixObj);
        mOutputMixObj = nullptr;
    }

    if (mJavaCallback != nullptr) {
        delete mJavaCallback;
    }
}


void WavePlayer::SetContext(const SLint16 *base, const SLint16 *current, size_t size) {
    mContext.base = base;
    mContext.current = current;
    mContext.size = size;
}


void WavePlayer::SetCallback(JavaVM *vm, jclass clazz, std::string methodName) {
    if (mJavaCallback != nullptr) {
        delete mJavaCallback;
    }
    mJavaCallback = new JavaCallback(vm, clazz, methodName);
    mJavaCallback->initialize();
}

SLresult WavePlayer::Start(void) {
    SLuint32 state;
    SLresult result = (*mPlayItf)->GetPlayState(mPlayItf, &state);
    if (result != SL_RESULT_SUCCESS) {
        return SL_BOOLEAN_FALSE;
    }
    if (state == SL_PLAYSTATE_PLAYING) {
        return SL_BOOLEAN_TRUE;
    }

    result = (*mPlayItf)->SetPlayState(mPlayItf, SL_PLAYSTATE_STOPPED);
    CheckError(result);

    result = (*mVolumeItf)->SetVolumeLevel(mVolumeItf, 0);
    CheckError(result);

    // send pre-defined audio buffers to device
    int i = PLAY_KICKSTART_BUFFER_COUNT;
    while (i--) {
        ProcessSLCallback(mPlayBufferQueueItf);
    }

    result = (*mPlayItf)->SetPlayState(mPlayItf, SL_PLAYSTATE_PLAYING);
    CheckError(result);

    return SL_BOOLEAN_TRUE;
}

void WavePlayer::Stop(void) {
    SLuint32 state;

    SLresult result = (*mPlayItf)->GetPlayState(mPlayItf, &state);
    CheckError(result);

    if (state == SL_PLAYSTATE_STOPPED) {
        return;
    }

    result = (*mPlayItf)->SetPlayState(mPlayItf, SL_PLAYSTATE_STOPPED);
    CheckError(result);

    if (mJavaCallback != nullptr) {
        mJavaCallback->run(-1);
    }
}

}
