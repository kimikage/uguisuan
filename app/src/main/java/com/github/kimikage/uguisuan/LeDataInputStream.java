/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;

import java.io.BufferedInputStream;
import java.io.DataInput;
import java.io.IOException;
import java.io.InputStream;

public class LeDataInputStream extends InputStream implements DataInput {
    private InputStream in;
    private byte b[] = new byte[8];

    public LeDataInputStream(InputStream in) {
        this.in = new BufferedInputStream(in);
    }

    @Override
    public int available() throws IOException {
        return in.available();
    }

    @Override
    public final short readShort() throws IOException {
        return (short) readUnsignedShort();
    }

    @Override
    public final int readUnsignedShort() throws IOException {
        in.read(b, 0, 2);
        return (b[1] & 0xff) << 8 | (b[0] & 0xff);
    }

    @Override
    public final char readChar() throws IOException {
        return (char) readUnsignedShort();
    }

    @Override
    public final int readInt() throws IOException {
        in.read(b, 0, 4);
        return (b[3]) << 24 | (b[2] & 0xff) << 16 | (b[1] & 0xff) << 8 | (b[0] & 0xff);
    }

    @Override
    @Deprecated
    public String readLine() throws IOException {
        return null;
    }

    @Override
    public final long readLong() throws IOException {
        in.read(b, 0, 8);
        return (long) (b[7] & 0xff) << 56 |
                (long) (b[6] & 0xff) << 48 |
                (long) (b[5] & 0xff) << 40 |
                (long) (b[4] & 0xff) << 32 |
                (long) (b[3] & 0xff) << 24 |
                (long) (b[2] & 0xff) << 16 |
                (long) (b[1] & 0xff) << 8 |
                (long) (b[0] & 0xff);
    }

    @Override
    public final float readFloat() throws IOException {
        return Float.intBitsToFloat(readInt());
    }

    @Override
    public final double readDouble() throws IOException {
        return Double.longBitsToDouble(readLong());
    }

    @Override
    public void readFully(byte[] dst) throws IOException {
        in.read(dst, 0, dst.length);
    }

    @Override
    public void readFully(byte[] dst, int offset, int byteCount) throws IOException {
        in.read(dst, offset, byteCount);
    }


    @Override
    public final int read(byte[] dst, int offset, int byteCount) throws IOException {
        return in.read(dst, offset, byteCount);
    }

    @Override
    public final int read(byte[] dst) throws IOException {
        return in.read(dst, 0, dst.length);
    }

    @Override
    public final int skipBytes(int n) throws IOException {
        return (int) in.skip(n);
    }

    @Override
    public final boolean readBoolean() throws IOException {
        return readByte() != 0;
    }

    @Override
    public final byte readByte() throws IOException {
        in.read(b, 0, 1);
        return b[0];
    }

    @Override
    public final int readUnsignedByte() throws IOException {
        return readByte() & 0xff;
    }

    @Override
    public int read() throws IOException {
        return in.read();
    }

    @Override
    public final String readUTF() throws IOException {
        throw new UnsupportedOperationException();
    }

    @Override
    public final void close() throws IOException {
        in.close();
    }

}
