/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;


import android.os.Bundle;
import android.support.v4.app.Fragment;

public class WavePlayerFragment extends Fragment implements WavePlayerService.Callbacks {
    public static final String TAG = "WaverPlayerFragment";
    private static final String ARG_IS_PLAYING = "isPlaying";

    private boolean mIsPlaying = false;
    private WavePlayerService.Receiver mReceiver = null;


    public WavePlayerFragment() {
    }

    public static WavePlayerFragment newInstance() {
        WavePlayerFragment fragment = new WavePlayerFragment();
        Bundle args = new Bundle();
        args.putBoolean(ARG_IS_PLAYING, false);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putBoolean(ARG_IS_PLAYING, mIsPlaying);
        super.onSaveInstanceState(outState);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Bundle args = savedInstanceState != null ? savedInstanceState : getArguments();
        if (args != null) {
            mIsPlaying = args.getBoolean(ARG_IS_PLAYING, false);
        }

        if (!(getActivity() instanceof WavePlayerCallbacks)) {
            throw new ClassCastException("Activity must implement WavePlayerCallbacks.");
        }

        mReceiver = new WavePlayerService.Receiver(this);
        mReceiver.register(getActivity());
        if (mIsPlaying) {
            return;
        }

        mIsPlaying = true;
        WavePlayerService.start(getActivity());
    }


    public void stopPlay() {
        WavePlayerService.stop(getActivity());
    }

    @Override
    public void onResume() {
        super.onResume();
        mReceiver.register(getActivity());
    }

    @Override
    public void onStop() {
        mReceiver.unregister(getActivity());
        super.onStop();
    }

    @Override
    public void onStartPlay() {
        ((WavePlayerCallbacks) getActivity()).onStartPlay();
    }

    @Override
    public void onProgressUpdate() {
    }

    @Override
    public void onStopPlay() {
        mIsPlaying = false;
        ((WavePlayerCallbacks) getActivity()).onStopPlay();
    }

    public boolean isPlaying() {
        return mIsPlaying;
    }


    public interface WavePlayerCallbacks {
        void onStartPlay();

        void onStopPlay();
    }
}
