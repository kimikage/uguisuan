/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;

import android.app.Application;
import android.content.Context;
import android.util.Log;

public class UguisuAnne extends Application {
    private static final String TAG = "UguisuAnne";
    private static Context sContext;

    private Wave mSourceWave = null;

    public static Context getContext() {
        return sContext;
    }

    @Override
    public void onCreate() {
        Log.d(TAG, "onCreate");
        super.onCreate();
        sContext = this;
    }

    public Wave getSourceWave() {
        return mSourceWave;
    }

    public void setSourceWave(Wave sourceWave) {
        if (mSourceWave != null) {
            // TBD.
        }
        mSourceWave = sourceWave;
    }

}
