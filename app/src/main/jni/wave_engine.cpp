/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

#include "wave_engine.h"

namespace uguisuan {

WaveEngine::WaveEngine() :
        mEngineObj(nullptr),
        mEngineItf(nullptr),
        mPlayer(nullptr) {

}

WaveEngine &WaveEngine::GetInstance() {
    static WaveEngine engine;
    return engine;
}

SLresult WaveEngine::CreateEngine() {
    DeleteEngine();
    SLresult result;

    result = slCreateEngine(&mEngineObj, 0, nullptr, 0, nullptr, nullptr);
    CheckError(result);

    result = (*mEngineObj)->Realize(mEngineObj, SL_BOOLEAN_FALSE);
    CheckError(result);

    result = (*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE, &mEngineItf);
    CheckError(result);


    mPlayer = new WavePlayer(SL_SAMPLINGRATE_48, mEngineItf);
    return result;
}

SLresult WaveEngine::DeleteEngine() {
    SLresult result = SL_RESULT_SUCCESS;
    if (mPlayer) {
        delete mPlayer;
        mPlayer = nullptr;
    }
    if (mEngineObj != nullptr) {
        (*mEngineObj)->Destroy(mEngineObj);
        mEngineObj = nullptr;
        mEngineItf = nullptr;
    }
    return result;
}

}
