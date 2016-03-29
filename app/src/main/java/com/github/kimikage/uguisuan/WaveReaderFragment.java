/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;


import android.os.Bundle;
import android.support.v4.app.Fragment;

public class WaveReaderFragment extends Fragment implements WaveReaderService.Callbacks {
    public static final String TAG = "WaverReaderFragment";
    private static final String ARG_FILE_PATH = "filePath";
    private static final String ARG_IS_READING = "isReading";
    private static final String DIALOG_TAG = "ProgressDialog";

    private ProgressDialogFragment mDialogFragment = null;
    private String mFilePath = null;
    private boolean mIsReading = false;
    private WaveReaderService.Receiver mReceiver = null;


    public WaveReaderFragment() {
    }

    public static WaveReaderFragment newInstance(String filePath) {
        WaveReaderFragment fragment = new WaveReaderFragment();
        Bundle args = new Bundle();
        args.putString(ARG_FILE_PATH, filePath);
        args.putBoolean(ARG_IS_READING, false);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putString(ARG_FILE_PATH, mFilePath);
        outState.putBoolean(ARG_IS_READING, mIsReading);
        super.onSaveInstanceState(outState);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Bundle args = savedInstanceState != null ? savedInstanceState : getArguments();
        if (args != null) {
            mFilePath = args.getString(ARG_FILE_PATH);
            mIsReading = args.getBoolean(ARG_IS_READING, false);
        }

        if (!(getActivity() instanceof WaveReaderCallbacks)) {
            throw new ClassCastException("Activity must implement WaveReaderCallbacks.");
        }

        mReceiver = new WaveReaderService.Receiver(this);
        mReceiver.register(getActivity());
        if (mIsReading) {
            return;
        }
        mIsReading = true;
        WaveReaderService.start(getActivity(), mFilePath);
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
    public void onPreExecute() {
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mDialogFragment == null) {
                    showDialog();
                }
            }
        });
    }

    @Override
    public void onProgressUpdate(final Wave wave, final int progress) {
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mDialogFragment == null) {
                    showDialog();
                }
                if (wave != null && wave.getDataSize() > 0) {
                    mDialogFragment.setMax(wave.getDataSize());
                }
                mDialogFragment.setProgress(progress);
            }
        });
    }

    @Override
    public void onPostExecute() {
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mDialogFragment == null) {
                    return;
                }
                mDialogFragment.dismiss();
                mDialogFragment = null;
            }
        });
        ((WaveReaderCallbacks) getActivity()).onReadFinished();
    }

    private void showDialog() {
        ProgressDialogFragment f = findFragment();
        if (f == null) {
            mDialogFragment = ProgressDialogFragment.newInstance(
                    "Loading voice data...", 100);
            mDialogFragment.show(getActivity().getSupportFragmentManager(), DIALOG_TAG);
        } else {
            mDialogFragment = f;
        }
    }

    private ProgressDialogFragment findFragment() {
        Fragment f = getActivity().getSupportFragmentManager().findFragmentByTag(DIALOG_TAG);
        return (ProgressDialogFragment) f;
    }

    public interface WaveReaderCallbacks {
        void onReadFinished();
    }
}
