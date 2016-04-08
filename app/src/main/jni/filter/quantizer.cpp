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
        int32_t s = static_cast<int32_t>(src[i] * SCALE);
        sink[i] = static_cast<int16_t>(s > I16_MAX ? I16_MAX : (s < I16_MIN ? I16_MIN : s));
    }
}

void Quantizer::SetSink(IRx<int16_t> *sink) {
    mSink = sink;
    size_t size = Filter::DEFAULT_BUFFER_SIZE;
    AllocBuffer(size);
}

}
}
