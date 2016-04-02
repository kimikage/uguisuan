/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_WAVE_CONTEXT_H
#define UGUISUAN_WAVE_CONTEXT_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"

#include <cstddef>

#pragma clang diagnostic pop

#include <SLES/OpenSLES.h>

namespace uguisuan {

struct WaveContext {
    const SLint16 *base;    // Base adress of local audio data storage
    const SLint16 *current;        // Current adress of local audio data storage
    size_t size;
};

}

#endif //UGUISUAN_WAVE_CONTEXT_H
