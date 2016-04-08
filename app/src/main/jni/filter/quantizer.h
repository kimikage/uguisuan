/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_QUANTIZER_H
#define UGUISUAN_QUANTIZER_H

#include "filter.h"

namespace uguisuan {
namespace filter {

class Quantizer : public Filter<float, int16_t> {
private:
    static constexpr float SCALE = 32768.0f;
    static const int32_t I16_MAX = 0x7FFF;
    static const int32_t I16_MIN = -0x8000;
public:
    Quantizer();

    ~Quantizer();

    virtual void SetSink(IRx<int16_t> *sink) override;

private:
    virtual inline void Proc(const float *__restrict__ src, int16_t *__restrict__ sink,
                             size_t n) const override;

};

}
}

#endif //UGUISUAN_quantizer_H
