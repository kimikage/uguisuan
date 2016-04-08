/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_FILTER_GRAPH_H
#define UGUISUAN_FILTER_GRAPH_H

#include "memory_source.h"
#include "resampler.h"
#include "quantizer.h"
#include "memory_sink.h"

namespace uguisuan {
namespace filter {

class Graph {
private:
    MemorySource *mSrc;
    Resampler *mResampler;
    Quantizer *mQuantizer;
    MemorySink *mSink;
public:
    Graph();

    ~Graph();

    void SetSourceMemory(const int16_t *source, size_t size);

    void SetSinkMemory(int16_t *sink, size_t size);

    size_t Push(size_t request);

    size_t Pull(size_t request);

    inline MemorySink *GetSink() const {
        return mSink;
    }

    void Flush();

    template<typename TSrc, typename TSink>
    void Connect(ITx<TSrc> *src, IRx<TSink> *sink) {
        src->SetSink(sink);
        sink->SetSource(src);
    }
};

}
}

#endif //UGUISUAN_FILTER_GRAPH_H
