/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;

import android.os.Parcel;
import android.os.Parcelable;

public class Wave implements Parcelable {
    public static final byte[] CHUNK_ID_RIFF = {'R', 'I', 'F', 'F'};
    public static final byte[] CHUNK_ID_WAVE = {'W', 'A', 'V', 'E'};
    public static final byte[] CHUNK_ID_FMT = {'f', 'm', 't', ' '};
    public static final byte[] CHUNK_ID_DATA = {'d', 'a', 't', 'a'};

    public static final int WAVE_FORMAT_PCM = 0x0001;
    public static final int WAVE_FORMAT_IEEE_FLOAT = 0x0003;

    public static final int MIN_SIZE_FMT = 16;
    public static final int MIN_SIZE_WAVE = MIN_SIZE_FMT + 8;
    public static final int MIN_SIZE_RIFF = MIN_SIZE_WAVE + 8;
    public static final Creator<Wave> CREATOR = new Creator<Wave>() {
        @Override
        public Wave createFromParcel(Parcel in) {
            return new Wave(in);
        }

        @Override
        public Wave[] newArray(int size) {
            return new Wave[size];
        }
    };
    private int mRiffSize;
    private int mFmtSize;
    private int mDataSize;
    private int mFormatTag;
    private int mNumberOfChannels;
    private int mSamplesPerSec;
    private int mAvgBytesPerSec;
    private int mBlockAlign;
    private int mBitsPerSample;
    private byte[] mPcm = null;

    public Wave() {

    }

    protected Wave(Parcel in) {
        mRiffSize = in.readInt();
        mFmtSize = in.readInt();
        mDataSize = in.readInt();
        mFormatTag = in.readInt();
        mNumberOfChannels = in.readInt();
        mSamplesPerSec = in.readInt();
        mAvgBytesPerSec = in.readInt();
        mBlockAlign = in.readInt();
        mBitsPerSample = in.readInt();
        boolean containsPcm = in.readInt() != 0;
        mPcm = containsPcm ? in.createByteArray() : null;
    }

    public int getRiffSize() {
        return mRiffSize;
    }

    public void setRiffSize(int riffSize) {
        mRiffSize = riffSize;
    }

    public int getFmtSize() {
        return mFmtSize;
    }

    public void setFmtSize(int fmtSize) {
        mFmtSize = fmtSize;
    }

    public int getDataSize() {
        return mDataSize;
    }

    public void setDataSize(int dataSize) {
        mDataSize = dataSize;
    }

    public int getFormatTag() {
        return mFormatTag;
    }

    public void setFormatTag(int formatTag) {
        mFormatTag = formatTag;
    }

    public int getNumberOfChannels() {
        return mNumberOfChannels;
    }

    public void setNumberOfChannels(int numberOfChannels) {
        mNumberOfChannels = numberOfChannels;
    }

    public int getSamplesPerSec() {
        return mSamplesPerSec;
    }

    public void setSamplesPerSec(int samplesPerSec) {
        mSamplesPerSec = samplesPerSec;
    }

    public int getAvgBytesPerSec() {
        return mAvgBytesPerSec;
    }

    public void setAvgBytesPerSec(int avgBytesPerSec) {
        mAvgBytesPerSec = avgBytesPerSec;
    }

    public int getBlockAlign() {
        return mBlockAlign;
    }

    public void setBlockAlign(int blockAlign) {
        mBlockAlign = blockAlign;
    }

    public int getBitsPerSample() {
        return mBitsPerSample;
    }

    public void setBitsPerSample(int bitsPerSample) {
        mBitsPerSample = bitsPerSample;
    }

    public byte[] getPcm() {
        return mPcm;
    }

    public void setPcm(byte[] pcm) {
        mPcm = pcm;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(mRiffSize);
        dest.writeInt(mFmtSize);
        dest.writeInt(mDataSize);
        dest.writeInt(mFormatTag);
        dest.writeInt(mNumberOfChannels);
        dest.writeInt(mSamplesPerSec);
        dest.writeInt(mAvgBytesPerSec);
        dest.writeInt(mBlockAlign);
        dest.writeInt(mBitsPerSample);

        dest.writeInt(mPcm != null ? 1 : 0);
        if (mPcm != null) {
            dest.writeByteArray(mPcm);
        }
    }
}
