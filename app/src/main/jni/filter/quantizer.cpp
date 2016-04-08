/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#include "quantizer.h"

namespace uguisuan {
namespace filter {

Quantizer::Quantizer() {
}

Quantizer::~Quantizer() {

}

inline void Quantizer::Proc(
        const float *__restrict__ src, int16_t *__restrict__ sink, size_t n) const {
    for (size_t i = 0; i < n; ++i) {
        if (src[i] >= 1.0f) {
            sink[i] = 0x7FFF;
        } else if (src[i] < -1.0f) {
            sink[i] = -0x8000;
        } else {
            sink[i] = static_cast<int16_t>(src[i] * SCALE);
        }
    }
}

void Quantizer::SetSink(IRx<int16_t> *sink) {
    mSink = sink;
    size_t size = Filter::DEFAULT_BUFFER_SIZE;
    AllocBuffer(size);
}

}
}
