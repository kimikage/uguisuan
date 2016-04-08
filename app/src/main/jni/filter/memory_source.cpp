/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#include "memory_source.h"

namespace uguisuan {
namespace filter {

size_t MemorySource::Push(size_t request) {
    if (request > GetAvailableSourceSize()) {
        LOGD("underflow");
        request = GetAvailableSourceSize();
    }

    const int16_t *__restrict__ src = &mMemory[mHead];

    size_t p = 0; // processed
    for (; ;) {
        size_t remain = request - p;
        size_t sinkSize =
                remain < mSink->GetSinkBufferSize() ? remain : mSink->GetSinkBufferSize();
        if (sinkSize == 0) {
            break;
        }
        int16_t *__restrict__ sink = mSink->GetBuffer();
        Copy(&src[p], sink, sinkSize);
        size_t received = mSink->Receive(sinkSize);
        if (received == 0) {
            break;
        }
        p += received;
    }
    mHead += p;
    if (request > p) {
        LOGD("overflow");
    }
    return p;
}

size_t MemorySource::Transmit(size_t size) {
    assert(size <= mSink->GetSinkBufferSize());

    const int16_t *__restrict__ src = &mMemory[mHead];
    int16_t *__restrict__ sink = mSink->GetBuffer();

    const size_t srcSize = size < GetAvailableSourceSize() ? size : GetAvailableSourceSize();
    if (srcSize == 0) {
        return 0;
    }
    Copy(src, sink, srcSize);
    mHead += srcSize;
    return srcSize;
}

void MemorySource::SetMemory(const int16_t *memory, size_t size) {
    mMemory = memory;
    mSize = size;
    mHead = 0;
}

inline void MemorySource::Copy(
        const int16_t *__restrict__ src, int16_t *__restrict__ sink, size_t n) const {
    for (size_t i = 0; i < n; ++i) {
        sink[i] = src[i];
    }
}

}
}
