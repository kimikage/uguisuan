/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;

import android.app.Dialog;
import android.os.Bundle;
import android.support.v7.app.AppCompatDialog;
import android.support.v7.app.AppCompatDialogFragment;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;


public class ProgressDialogFragment extends AppCompatDialogFragment {
    private static final String ARG_MESSAGE = "message";
    private static final String ARG_MAX = "max";
    private static final String ARG_PROGRESS = "progress";

    private String mMessage = null;
    private int mMax = 100;
    private int mProgress = 0;
    private ProgressBar mProgressBar;
    private TextView mTextView;

    public static ProgressDialogFragment newInstance(final String message, int max) {
        ProgressDialogFragment fragment = new ProgressDialogFragment();
        Bundle args = new Bundle();
        args.putString(ARG_MESSAGE, message);
        args.putInt(ARG_MAX, max);
        args.putInt(ARG_PROGRESS, 0);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putString(ARG_MESSAGE, mMessage);
        outState.putInt(ARG_MAX, mMax);
        outState.putInt(ARG_PROGRESS, mProgress);
        super.onSaveInstanceState(outState);
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Dialog dialog = new AppCompatDialog(getActivity(), R.style.AppTheme_AlertDialog);
        setupDialog(dialog, STYLE_NO_TITLE);
        dialog.setCanceledOnTouchOutside(false);
        dialog.setCancelable(false);
        return dialog;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        Bundle args = savedInstanceState != null ? savedInstanceState : getArguments();
        if (args != null) {
            mMessage = args.getString(ARG_MESSAGE);
            mMax = args.getInt(ARG_MAX);
            mProgress = args.getInt(ARG_PROGRESS);
        }

        View view = inflater.inflate(R.layout.fragment_progress_dialog, container, false);

        mTextView = (TextView) view.findViewById(R.id.message);
        setMessage(mTextView, mMessage);

        mProgressBar = (ProgressBar) view.findViewById(R.id.progressBar);

        final TypedValue v = new TypedValue();
        mProgressBar.getContext().getTheme().resolveAttribute(R.attr.colorAccent, v, true);
        mProgressBar.getProgressDrawable().setColorFilter(
                v.data, android.graphics.PorterDuff.Mode.SRC_IN);

        mProgressBar.setMax(mMax);
        mProgressBar.setProgress(mProgress);

        return view;
    }

    public void setMessage(String message) {
        mMessage = message;
        if (mTextView != null) {
            setMessage(mTextView, message);
        }
    }

    private void setMessage(TextView view, String message) {
        if (message != null) {
            view.setVisibility(View.VISIBLE);
            view.setText(message);
        } else {
            view.setVisibility(View.INVISIBLE);
        }
    }

    public void setMax(int max) {
        mMax = max;
        if (mProgressBar != null) {
            mProgressBar.setMax(max);
        }
    }

    public void setProgress(int progress) {
        mProgress = progress;
        if (mProgressBar != null) {
            mProgressBar.setProgress(progress);
        }
    }
}
