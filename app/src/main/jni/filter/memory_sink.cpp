/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#include "memory_sink.h"

namespace uguisuan {
namespace filter {

MemorySink::MemorySink() {

}

size_t MemorySink::Pull(size_t request) {
    if (request > GetAvailableSinkSize()) {
        LOGD("overflow");
        request = GetAvailableSinkSize();
    }
    size_t remain = request;
    for (; ;) {
        size_t srcSize = remain < GetAvailableSinkSize() ? remain : GetAvailableSinkSize();
        if (srcSize == 0) {
            break;
        }
        size_t transmitted = mSrc->Transmit(srcSize);
        if (transmitted == 0) {
            break;
        }
        mHead += transmitted;
        remain -= transmitted;
    }
    size_t processed = request - remain;
    mCount += processed;
    if (remain > 0) {
        LOGD("underflow");
    }
    return processed;
}

size_t MemorySink::Receive(size_t size) {
    assert(size <= GetSinkBufferSize());
    mHead += size;
    mCount += size;
    return size;
}

void MemorySink::SetMemory(int16_t *memory, size_t size) {
    mMemory = memory;
    mSize = size;
    mHead = 0;
}

size_t MemorySink::GetSampleCount() const {
    return mCount;
}

void MemorySink::ResetSampleCount() {
    mCount = 0;
}

}
}
