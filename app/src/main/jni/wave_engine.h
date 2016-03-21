/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#ifndef UGUISUAN_WAVE_ENGINE_H
#define UGUISUAN_WAVE_ENGINE_H

#include <cstdint>
#include <cassert>

#include <SLES/OpenSLES.h>

#include "wave_player.h"

namespace uguisuan {

class WavePlayer;

class WaveEngine {
public:
private:
    SLObjectItf mEngineObj;
    SLEngineItf mEngineItf;

    WavePlayer *mPlayer;
private:
    WaveEngine();

    ~WaveEngine() = default;

public:
    WaveEngine(const WaveEngine &) = delete;

    WaveEngine &operator=(const WaveEngine &) = delete;

    WaveEngine(WaveEngine &&) = delete;

    WaveEngine &operator=(WaveEngine &&) = delete;

    static WaveEngine &GetInstance();

public:
    SLresult CreateEngine();

    SLresult DeleteEngine();

    inline WavePlayer *getPlayer() {
        return mPlayer;
    }

};


inline static void CheckError(SLresult result) {
    assert(SL_RESULT_SUCCESS == result);
}

}

#endif //UGUISUAN_WAVE_ENGINE_H
