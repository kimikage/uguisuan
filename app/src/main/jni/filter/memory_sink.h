/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_MEMORY_SINK_H
#define UGUISUAN_MEMORY_SINK_H

#include "filter.h"

namespace uguisuan {
namespace filter {

class MemorySink : public SinkFilter<int16_t> {
private:
    int16_t *mMemory;
    size_t mSize;
    size_t mCount;
public:
    MemorySink();

    virtual ~MemorySink() { }

    virtual size_t Pull(size_t request) override;

    virtual inline size_t GetAvailableSinkSize() const override {
        return mSize - mHead;
    }

    virtual inline size_t GetSinkBufferSize() const override {
        return GetAvailableSinkSize();
    }

    virtual inline int16_t *GetBuffer() const override {
        return &mMemory[mHead];
    }

    virtual size_t Receive(size_t size) override;

    void SetMemory(int16_t *memory, size_t size);

    size_t GetSampleCount() const;

    void ResetSampleCount();

};

}
}


#endif //UGUISUAN_MEMORY_SINK_H
