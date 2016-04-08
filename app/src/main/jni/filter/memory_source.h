/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_MEMORY_SOURCE_H
#define UGUISUAN_MEMORY_SOURCE_H

#include "filter.h"

namespace uguisuan {
namespace filter {

class MemorySource : public SourceFilter<int16_t> {
private:
    const int16_t *mMemory;
    size_t mSize;
public:
    virtual inline size_t GetAvailableSourceSize() const override {
        return mSize - mHead;
    }

    virtual size_t Push(size_t request) override;

    virtual size_t Transmit(size_t size) override;

    void SetMemory(const int16_t *memory, size_t size);

private:
    void Copy(const int16_t *__restrict__ src, int16_t *__restrict__ sink, size_t n) const;
};

}
}

#endif //UGUISUAN_MEMORY_SOURCE_H
