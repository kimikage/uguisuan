/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;

import android.app.Activity;
import android.app.IntentService;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.AssetManager;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Arrays;

public class WaveReaderService extends IntentService {
    public static final String TAG = "WaveReaderService";
    private static final String ARG_FILE_PATH = "filePath";

    private LeDataInputStream mStream = null;
    private Wave mWave = new Wave();

    public WaveReaderService() {
        super(TAG);
    }

    public static void start(Activity activity, String filePath) {
        Intent intent = new Intent(activity, WaveReaderService.class);
        intent.putExtra(ARG_FILE_PATH, filePath);
        activity.startService(intent);
    }

    @Override
    protected void onHandleIntent(Intent intent) {
        if (intent == null) {
            return;
        }
        String filePath = intent.getStringExtra(ARG_FILE_PATH);

        Log.d(TAG, "start");

        Intent bi = new Intent();
        bi.setAction(Receiver.ACTION_PRE);
        LocalBroadcastManager.getInstance(this).sendBroadcast(bi);

        try {
            if (filePath.equals("!")) {
                AssetManager am = getResources().getAssets();
                mStream = new LeDataInputStream(am.open("japanese.wav"));
            } else {
                mStream = new LeDataInputStream(new FileInputStream(filePath));
            }

            readHeader();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        bi.setAction(Receiver.ACTION_UPDATE);

        byte[] buffer = new byte[4];
        int remain = mWave.getRiffSize() - mWave.getFmtSize();
        try {
            while (remain >= 8) {
                mStream.read(buffer);
                int size = mStream.readInt();
                remain -= size + 8;
                if (Arrays.equals(buffer, Wave.CHUNK_ID_DATA)) {
                    mWave.setDataSize(size);
                    readDataChunk();
                } else {
                    mStream.skip(size);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        UguisuAnne app = (UguisuAnne) getApplicationContext();
        app.setSourceWave(mWave);

        bi.setAction(Receiver.ACTION_POST);
        LocalBroadcastManager.getInstance(this).sendBroadcast(bi);
    }

    private void readHeader() throws IOException {
        if (mStream == null) {
            return;
        }
        if (mStream.available() < Wave.MIN_SIZE_RIFF) {
            throw new IllegalArgumentException("The file is too short.");
        }
        byte[] buffer = new byte[4];
        mStream.read(buffer);
        if (!Arrays.equals(buffer, Wave.CHUNK_ID_RIFF)) {
            throw new IllegalArgumentException("RIFF chunk is not found.");
        }
        mWave.setRiffSize(mStream.readInt());
        mStream.read(buffer);
        if (!Arrays.equals(buffer, Wave.CHUNK_ID_WAVE)) {
            throw new IllegalArgumentException("WAVE chunk is not found.");
        }
        mStream.read(buffer);
        if (!Arrays.equals(buffer, Wave.CHUNK_ID_FMT)) {
            throw new IllegalArgumentException("fmt chunk is not found.");
        }
        mWave.setFmtSize(mStream.readInt());
        mWave.setFormatTag(mStream.readUnsignedShort());
        switch (mWave.getFormatTag()) {
            case Wave.WAVE_FORMAT_PCM:
                break;
            case Wave.WAVE_FORMAT_IEEE_FLOAT:
                throw new IllegalArgumentException("float format is not supported.");
            default:
                throw new IllegalArgumentException("The format is not supported.");
        }
        mWave.setNumberOfChannels(mStream.readUnsignedShort());
        switch (mWave.getNumberOfChannels()) {
            case 1:
                break;
            case 2:
                throw new IllegalArgumentException("stereo is not supported.");
            default:
                throw new IllegalArgumentException("multi-channel is not supported.");
        }
        mWave.setSamplesPerSec(mStream.readInt());
        switch (mWave.getSamplesPerSec()) {
            case 48000:
                break;
            default:
                throw new IllegalArgumentException("48 kHz is only available.");
        }
        mWave.setAvgBytesPerSec(mStream.readInt());
        mWave.setBlockAlign(mStream.readUnsignedShort());
        mWave.setBitsPerSample(mStream.readUnsignedShort());
        switch (mWave.getBitsPerSample()) {
            case 16:
                break;
            default:
                throw new IllegalArgumentException("16 bit/Sample is only available.");
        }
        mStream.skipBytes(mWave.getFmtSize() - Wave.MIN_SIZE_FMT);
    }

    private void readDataChunk() throws IOException {
        int size = mWave.getDataSize();
        byte[] pcm = new byte[mWave.getDataSize()];
        int read = 0;

        Intent bi = new Intent();
        bi.setAction(Receiver.ACTION_UPDATE);
        bi.putExtra(Receiver.ARG_WAVE, mWave);

        int segmentSize = 4096 * 2;
        while (read <= (size - segmentSize)) {
            Log.d(TAG, "read: " + read + "/" + size);
            bi.putExtra(Receiver.ARG_PROGRESS, read);
            LocalBroadcastManager.getInstance(this).sendBroadcast(bi);

            mStream.read(pcm, read, segmentSize);
            read += segmentSize;

            try {
                Thread.sleep(10); // for debug
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        mStream.read(pcm, read, size - read);
        bi.putExtra(Receiver.ARG_PROGRESS, size);
        LocalBroadcastManager.getInstance(this).sendBroadcast(bi);

        mWave.setPcm(pcm);
    }

    public interface Callbacks {
        void onPreExecute();

        void onProgressUpdate(Wave wave, int progress);

        void onPostExecute();
    }

    public static class Receiver extends BroadcastReceiver {
        public static final String ARG_PROGRESS = "progress";
        public static final String ARG_WAVE = "wave";
        public static final String ACTION_PRE = "WaveReaderService.onPreExecute";
        public static final String ACTION_UPDATE = "WaveReaderService.onProgressUpdate";
        public static final String ACTION_POST = "WaveReaderService.onPostExecute";

        private static IntentFilter sFilter;

        static {
            sFilter = new IntentFilter();
            sFilter.addAction(ACTION_PRE);
            sFilter.addAction(ACTION_UPDATE);
            sFilter.addAction(ACTION_POST);
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
                case ACTION_PRE:
                    mCallbacks.onPreExecute();
                    return;
                case ACTION_UPDATE:
                    Wave wave = intent.getParcelableExtra(ARG_WAVE);
                    int progress = intent.getIntExtra(ARG_PROGRESS, 0);
                    mCallbacks.onProgressUpdate(wave, progress);
                    return;
                case ACTION_POST:
                    mCallbacks.onPostExecute();
                    return;
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
