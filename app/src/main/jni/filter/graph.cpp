/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#include "graph.h"

namespace uguisuan {
namespace filter {

Graph::Graph() {
    mSink = new MemorySink();
    mQuantizer = new Quantizer();
    mResampler = new Resampler();
    mSrc = new MemorySource();
    Connect(mQuantizer, mSink);
    Connect(mResampler, mQuantizer);
    Connect(mSrc, mResampler);
}

Graph::~Graph() {
    delete mSink;
    delete mQuantizer;
    delete mResampler;
    delete mSrc;
}

void Graph::SetSourceMemory(const int16_t *source, size_t size) {
    mSrc->SetMemory(source, size);
}

void Graph::SetSinkMemory(int16_t *sink, size_t size) {
    mSink->SetMemory(sink, size);
}

size_t Graph::Push(size_t request) {
    return mSrc->Push(request);
}

size_t Graph::Pull(size_t request) {
    return mSink->Pull(request);
}

void Graph::Flush() {
    mSink->Flush();
}

}
}
