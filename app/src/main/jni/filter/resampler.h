/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_RESAMPLER_H
#define UGUISUAN_RESAMPLER_H

#include "filter.h"

namespace uguisuan {
namespace filter {

class Resampler : public Filter<int16_t, float> {
private:
    static const int OUTPUT_SAMPLE_RATE = 48000; // in Hz
    static constexpr float SCALE = 1.0f / 32768.0f;
private:
    int mNumberOfChannels;
    int mSamplePerSec;
public:
    Resampler();

    ~Resampler();

    inline void SetSourceProfile(int numberOfchannels, int samplesPerSec) {
        mNumberOfChannels = numberOfchannels;
        mSamplePerSec = samplesPerSec;
    }

    virtual size_t GetAvailableSourceSize() const override;

    virtual size_t GetAvailableSinkSize() const override;

    virtual size_t Receive(size_t size) override;

    virtual size_t Transmit(size_t size) override;

    virtual void SetSink(IRx<float> *sink) override;

private:
    size_t Receive48kM(size_t size);

    size_t Transmit48kM(size_t size);

    void Proc48kM(const int16_t *__restrict__ src, float *__restrict__ sink, size_t size) const;
};

}
}

#endif //UGUISUAN_RESAMPLER_H
