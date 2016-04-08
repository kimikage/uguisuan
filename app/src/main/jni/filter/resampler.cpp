/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#include "resampler.h"

namespace uguisuan {
namespace filter {

Resampler::Resampler() :
        mNumberOfChannels(1), mSamplePerSec(OUTPUT_SAMPLE_RATE) {
}

Resampler::~Resampler() {

}

size_t Resampler::GetAvailableSourceSize() const {
    size_t size = mSrc->GetAvailableSourceSize();
    int samples = static_cast<int>(size) / mNumberOfChannels;
    return static_cast<size_t>(samples * mSamplePerSec / OUTPUT_SAMPLE_RATE);
}

size_t Resampler::GetAvailableSinkSize() const {
    size_t size = mSink->GetAvailableSinkSize();
    int samples = static_cast<int>(size) * mNumberOfChannels;
    return static_cast<size_t>(samples * OUTPUT_SAMPLE_RATE / mSamplePerSec);
}

size_t Resampler::Receive(size_t size) {
    return Receive48kM(size);
}

size_t Resampler::Receive48kM(size_t size) {
    mHead += size;
    assert(mHead <= mBufferSize);

    const int16_t *__restrict__ src __attribute__((align_value(MEM_ALIGN))) = mBuffer;

    size_t p = 0; // processed;
    for (; ;) {
        const size_t remain = mHead - p;
        const size_t sinkSize =
                remain < mSink->GetSinkBufferSize() ? remain : mSink->GetSinkBufferSize();
        if (sinkSize == 0) {
            break;
        }
        float *__restrict__ sink = mSink->GetBuffer();
        Proc48kM(&src[p], sink, sinkSize);
        const size_t received = mSink->Receive(sinkSize);
        if (received == 0) {
            break;
        }
        p += received;
    }
    Shift(p);
    return p;
}

void Resampler::SetSink(IRx<float> *sink) {
    mSink = sink;
    size_t size = Filter::DEFAULT_BUFFER_SIZE;
    int samples = static_cast<int>(size) * mNumberOfChannels;
    AllocBuffer(static_cast<size_t>(samples * OUTPUT_SAMPLE_RATE / mSamplePerSec));
}

size_t Resampler::Transmit(size_t size) {
    return Transmit48kM(size);
}

size_t Resampler::Transmit48kM(size_t size) {
    assert(size <= mSink->GetSinkBufferSize());

    const int16_t *__restrict__ src __attribute__((align_value(MEM_ALIGN))) = mBuffer;
    float *__restrict__ sink = mSink->GetBuffer();

    size_t p = 0; // processed
    if (mHead > 0) {
        if (size < mHead) {
            Proc48kM(src, sink, size);
            Shift(size);
            return size;
        } else {
            Proc48kM(src, sink, mHead);
            p += mHead;
            mHead = 0;
        }
    }
    for (; ;) {
        const size_t remain = size - p;
        const size_t srcSize = remain < mBufferSize ? remain : mBufferSize;
        if (srcSize == 0) {
            break;
        }
        const size_t transmitted = mSrc->Transmit(srcSize);
        if (transmitted == 0) {
            break;
        }
        Proc48kM(src, &sink[p], transmitted);
        p += transmitted;
    }
    return p;
}

inline void Resampler::Proc48kM(const int16_t *__restrict__ src, float *__restrict__ sink,
                                size_t size) const {
    for (size_t i = 0; i < size; ++i) {
        sink[i] = src[i] * SCALE;
    }
}

}
}
