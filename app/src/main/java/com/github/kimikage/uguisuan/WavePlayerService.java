/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;

import android.app.Activity;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

public class WavePlayerService extends Service {
    public static final String TAG = "WavePlayerService";
    private static final String ARG_ACTION = "action";
    private static final String ARG_STATUS = "status";

    static {
        System.loadLibrary("wave");
    }

    /*
     * jni function implementations...
     */
    public static native void createSLEngine();

    public static native void deleteSLEngine();

    public static native void startPlay(byte[] pcm);

    public static native void stopPlay();


    public static void start(Activity activity) {

        UguisuAnne app = (UguisuAnne) (activity.getApplicationContext());
        if (app.getSourceWave() == null || app.getSourceWave().getPcm() == null) {
            update(-1);
            return;
        }

        Intent intent = new Intent(activity, WavePlayerService.class);
        intent.putExtra(ARG_ACTION, Receiver.ACTION_START);
        activity.startService(intent);
    }

    public static void stop(Activity activity) {
        Intent intent = new Intent(activity, WavePlayerService.class);
        intent.putExtra(ARG_ACTION, Receiver.ACTION_STOP);
        activity.startService(intent);
    }

    public static void update(int status) {
        Context context = UguisuAnne.getContext();
        Intent intent = new Intent(context, WavePlayerService.class);
        intent.putExtra(ARG_ACTION, Receiver.ACTION_UPDATE);
        intent.putExtra(ARG_STATUS, status);
        context.startService(intent);
    }


    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "onCreate");
        createSLEngine();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (intent == null) {
            return START_STICKY;
        }

        Intent bi = new Intent();
        String action = intent.getStringExtra(ARG_ACTION);
        bi.setAction(action);
        switch (action) {
            case Receiver.ACTION_START:
                Log.d(TAG, "start");
                UguisuAnne app = (UguisuAnne) getApplicationContext();
                Wave wave = app.getSourceWave();
                if (wave == null || wave.getPcm() == null) {
                    break;
                }
                startPlay(wave.getPcm());
                LocalBroadcastManager.getInstance(this).sendBroadcast(bi);
                break;
            case Receiver.ACTION_STOP:
                Log.d(TAG, "stop");
                stopPlay();
                break;
            case Receiver.ACTION_UPDATE:
                int status = intent.getIntExtra(ARG_STATUS, -1);
                Log.d(TAG, "update:" + status);
                if (status >= 0) {
                    LocalBroadcastManager.getInstance(this).sendBroadcast(bi);
                } else {
                    bi.setAction(Receiver.ACTION_STOP);
                    LocalBroadcastManager.getInstance(this).sendBroadcast(bi);
                    stopSelf();
                }
                break;
        }
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy");
        deleteSLEngine();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    public interface Callbacks {
        void onStartPlay();

        void onProgressUpdate();

        void onStopPlay();
    }

    public static class Receiver extends BroadcastReceiver {
        public static final String ACTION_START = "WavePlayerService.onStartPlay";
        public static final String ACTION_UPDATE = "WavePlayerService.onProgressUpdate";
        public static final String ACTION_STOP = "WaveReaderService.onStopPlay";

        private static IntentFilter sFilter;

        static {
            sFilter = new IntentFilter();
            sFilter.addAction(ACTION_START);
            sFilter.addAction(ACTION_UPDATE);
            sFilter.addAction(ACTION_STOP);
        }

        private Callbacks mCallbacks;

        public Receiver(Callbacks callbacks) {
            mCallbacks = callbacks;
        }

        @Override
        public void onReceive(Context context, Intent intent) {
            if (mCallbacks == null) {
                return;
            }
            switch (intent.getAction()) {
                case ACTION_START:
                    mCallbacks.onStartPlay();
                    return;
                case ACTION_UPDATE:
                    mCallbacks.onProgressUpdate();
                    return;
                case ACTION_STOP:
                    mCallbacks.onStopPlay();
                    return;
                default:
                    throw new IllegalArgumentException("unknown action");
            }
        }

        public void register(Context context) {
            LocalBroadcastManager.getInstance(context).registerReceiver(this, sFilter);
        }

        public void unregister(Context context) {
            LocalBroadcastManager.getInstance(context).unregisterReceiver(this);
        }
    }
}
