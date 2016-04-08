/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_FILTER_H
#define UGUISUAN_FILTER_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wpedantic"

#include <cstdint>
#include <cstddef>
#include <memory>

#pragma clang diagnostic pop

#include "../log.h"

// Inspection of the IDE may not resolve the stdint types
#ifndef _STDINT_H
typedef signed char int8_t;
typedef signed short int16_t;
#endif

#define MEM_ALIGN 64

namespace uguisuan {
namespace filter {

template<typename T>
class ITx;

template<typename T>
class IRx {
public:
    virtual ~IRx() { }

    virtual size_t GetAvailableSinkSize() const = 0;

    virtual size_t GetSinkBufferSize() const = 0;

    virtual T *GetBuffer() const = 0;

    virtual size_t Receive(size_t size) = 0;

    virtual void SetSource(ITx<T> *source) = 0;

};

template<typename T>
class ITx {
public:
    virtual ~ITx() { }

    virtual size_t GetAvailableSourceSize() const = 0;

    virtual size_t GetSourceBufferSize() const = 0;

    virtual size_t Transmit(size_t size) = 0;

    virtual void SetSink(IRx<T> *sink) = 0;
};

template<typename TSrc, typename TSink>
class Filter : public IRx<TSrc>, public ITx<TSink> {
protected:
    static const size_t DEFAULT_BUFFER_SIZE = 64;

private:
    int8_t *mRaw;
protected:
    TSrc *__restrict__ mBuffer;
    size_t mBufferSize;
    size_t mHead;
    ITx<TSrc> *mSrc;
    IRx<TSink> *mSink;
public:
    void SetSource(ITx<TSrc> *source) override {
        mSrc = source;
    }

    void SetSink(IRx<TSink> *sink) override {
        mSink = sink;
    }

protected:
    Filter() :
            mRaw(nullptr) {
    }

    virtual ~Filter() {
        FreeBuffer();
    }

    virtual inline size_t GetAvailableSourceSize() const override {
        return mSrc->GetAvailableSourceSize();
    }

    virtual inline size_t GetAvailableSinkSize() const override {
        return mSink->GetAvailableSinkSize();
    }

    virtual inline TSrc *GetBuffer() const override __attribute__((assume_aligned(MEM_ALIGN))) {
        return mBuffer;
    }

    virtual inline size_t GetSinkBufferSize() const override {
        return mBufferSize - mHead;
    }

    virtual inline size_t GetSourceBufferSize() const override {
        return mHead;
    }

    virtual size_t Receive(size_t size) override {
        mHead += size;
        assert(mHead <= mBufferSize);

        const TSrc *__restrict__ src __attribute__((align_value(MEM_ALIGN))) = mBuffer;

        size_t p = 0; // processed;
        for (; ;) {
            const size_t remain = mHead - p;
            const size_t sinkSize =
                    remain < mSink->GetSinkBufferSize() ? remain : mSink->GetSinkBufferSize();
            if (sinkSize == 0) {
                break;
            }
            TSink *__restrict__ sink = mSink->GetBuffer();
            Proc(&src[p], sink, sinkSize);
            size_t received = mSink->Receive(sinkSize);
            if (received == 0) {
                break;
            }
            p += received;
        }
        Shift(p);
        return p;
    }

    virtual size_t Transmit(size_t size) override {
        assert(size <= mSink->GetSinkBufferSize());

        const TSrc *__restrict__ src __attribute__((align_value(MEM_ALIGN))) = mBuffer;
        TSink *__restrict__ sink = mSink->GetBuffer();

        size_t p = 0; // processed;
        if (mHead > 0) {
            if (size < mHead) {
                Proc(src, sink, size);
                Shift(size);
                return size;
            } else {
                Proc(src, sink, mHead);
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
            Proc(src, &sink[p], transmitted);
            p += transmitted;
        }
        return p;
    }

    virtual void Proc(const TSrc *__restrict__, TSink *__restrict__, size_t) const { }

    inline void AllocBuffer(size_t size) {
        FreeBuffer();

        size_t bytes = size * sizeof(TSrc);
        size_t bytesWithPad = bytes + MEM_ALIGN;
        mRaw = new int8_t[bytesWithPad];
        void *ptr = mRaw;
        mBuffer = static_cast<TSrc *>(std::align(MEM_ALIGN, bytes, ptr, bytesWithPad));
        mBufferSize = (mBuffer == nullptr) ? 0 : size;
        mHead = 0;
#ifndef NDEBUG
        for (size_t i = 0; i < mBufferSize; ++i) {
            mBuffer[i] = -1;
        }
#endif
    }

protected:
    void Shift(size_t processed) {
        if (mHead > processed) {
            size_t remain = mHead - processed;
            for (size_t i = 0; i < remain; ++i) {
                mBuffer[i] = mBuffer[processed + i];
            }
            mHead = remain;
        } else {
            mHead = 0;
        }
    }

private:

    inline void FreeBuffer() {
        delete[] mRaw;
        mRaw = nullptr;
        mBuffer = nullptr;
        mBufferSize = 0;
    }
};

template<typename T>
class SourceFilter : public Filter<int8_t, T> {
public:
    virtual ~SourceFilter() { }

    virtual size_t Push(size_t request) = 0;

private:
    virtual size_t Receive(size_t) override {
        return 0;
    }

    virtual size_t GetSinkBufferSize() const override {
        return 0;
    }

    virtual void SetSource(ITx<int8_t> *) override { }


};

template<typename T>
class SinkFilter : public Filter<T, int8_t> {
public:
    virtual ~SinkFilter() { }

    virtual size_t Pull(size_t request) = 0;

    virtual void Flush() {
        this->mHead = 0;
    }

private:
    virtual size_t Transmit(size_t) override {
        return 0;
    }

    virtual size_t GetSourceBufferSize() const override {
        return 0;
    }

    virtual void SetSink(IRx<int8_t> *) override { }
};

}
}
#endif //UGUISUAN_FILTER_H
