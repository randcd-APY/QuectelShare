/*
 * Copyright (c) 2008-2009, Motorola, Inc.
 * Copyright (c) 2010-2011, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *            notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *            notice, this list of conditions and the following disclaimer in the
 *            documentation and/or other materials provided with the distribution.
 *        * Neither the name of The Linux Foundation nor
 *            the names of its contributors may be used to endorse or promote
 *            products derived from this software without specific prior written
 *            permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.    IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package org.codeaurora.bluetooth.ftp;

import android.bluetooth.BluetoothSocket;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.obex.ObexTransport;

public class BluetoothFtpTransport implements ObexTransport {
    private BluetoothSocket mSocket = null;

    private final int mType;

    public static final int TYPE_RFCOMM = 0;

    public static final int TYPE_L2CAP = 1;

    public BluetoothFtpTransport(BluetoothSocket rfs, int type) {
        super();
        this.mSocket = rfs;
        this.mType = type;
    }

    public void close() throws IOException {
        mSocket.close();
    }

    public DataInputStream openDataInputStream() throws IOException {
        return new DataInputStream(openInputStream());
    }

    public DataOutputStream openDataOutputStream() throws IOException {
        return new DataOutputStream(openOutputStream());
    }

    public InputStream openInputStream() throws IOException {
        return mSocket.getInputStream();
    }

    public OutputStream openOutputStream() throws IOException {
        return mSocket.getOutputStream();
    }

    public void connect() throws IOException {
    }

    public void create() throws IOException {
    }

    public void disconnect() throws IOException {
    }

    public void listen() throws IOException {
    }

    public boolean isConnected() throws IOException {
        return true;
    }

    public String getRemoteAddress() {
        if (mSocket == null)
            return null;
        return mSocket.getRemoteDevice().getAddress();
    }

    public boolean isAmpCapable() {
        return mType == TYPE_L2CAP;
    }

    public boolean isSrmCapable() {
        return mType == TYPE_L2CAP;
    }

    public int getMaxTransmitPacketSize() {
        return -1;
    }

    public int getMaxReceivePacketSize() {
        return -1;
    }

    public boolean isSrmSupported() {
        return false;
    }

/*
    public boolean setDesiredAmpPolicy(int policy) {
        if (mSocket == null || mType != TYPE_L2CAP)
            return false;
        return mSocket.setDesiredAmpPolicy(policy);
    }*/
}
