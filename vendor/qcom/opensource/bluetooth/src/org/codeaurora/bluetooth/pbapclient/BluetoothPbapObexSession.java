/*
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 * Copyright (c) 2008-2009, Motorola, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of the Motorola, Inc. nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

package org.codeaurora.bluetooth.pbapclient;

import android.os.Handler;
import android.util.Log;

import java.io.IOException;

import javax.obex.ClientSession;
import javax.obex.HeaderSet;
import javax.obex.ObexTransport;
import javax.obex.ResponseCodes;

final class BluetoothPbapObexSession {
    private static final String TAG = "BluetoothPbapObexSession";

    private static final byte[] PBAP_TARGET = new byte[] {
            0x79, 0x61, 0x35, (byte) 0xf0, (byte) 0xf0, (byte) 0xc5, 0x11, (byte) 0xd8, 0x09, 0x66,
            0x08, 0x00, 0x20, 0x0c, (byte) 0x9a, 0x66
    };

    final static int OBEX_SESSION_CONNECTED = 100;
    final static int OBEX_SESSION_FAILED = 101;
    final static int OBEX_SESSION_DISCONNECTED = 102;
    final static int OBEX_SESSION_REQUEST_COMPLETED = 103;
    final static int OBEX_SESSION_REQUEST_FAILED = 104;
    final static int OBEX_SESSION_AUTHENTICATION_REQUEST = 105;
    final static int OBEX_SESSION_AUTHENTICATION_TIMEOUT = 106;

    private Handler mSessionHandler;
    private final ObexTransport mTransport;
    private ObexClientThread mObexClientThread;
    private BluetoothPbapObexAuthenticator mAuth = null;

    public BluetoothPbapObexSession(ObexTransport transport) {
        mTransport = transport;
    }

    public void start(Handler handler) {
        Log.d(TAG, "start");
        mSessionHandler = handler;

        mAuth = new BluetoothPbapObexAuthenticator(mSessionHandler);

        mObexClientThread = new ObexClientThread();
        mObexClientThread.start();
    }

    public void stop() {
        Log.d(TAG, "stop");

        if (mObexClientThread != null) {
            try {
                abort();
                mObexClientThread.interrupt();
                mObexClientThread.join();
                mObexClientThread = null;
            } catch (InterruptedException e) {
            }
        }
    }

    public void abort() {
        Log.d(TAG, "abort");

        if (mObexClientThread != null && mObexClientThread.mRequest != null) {
            /*
             * since abort may block until complete GET is processed inside OBEX
             * session, let's run it in separate thread so it won't block UI
             */
            Thread t = new Thread(new Runnable() {
            public void run () {
                if (mObexClientThread != null && mObexClientThread.mRequest != null) {
                    Log.d(TAG, "Spawning a new thread for aborting");
                    mObexClientThread.mRequest.abort();
                }
            }
            });
            t.start();
            Log.d(TAG, "Exiting from the abort thread");
       }
    }

    public boolean schedule(BluetoothPbapRequest request) {
        Log.d(TAG, "schedule: " + request.getClass().getSimpleName());

        if (mObexClientThread == null) {
            Log.e(TAG, "OBEX session not started");
            return false;
        }

        return mObexClientThread.schedule(request);
    }

    public boolean setAuthReply(String key) {
        Log.d(TAG, "setAuthReply key=" + key);

        if (mAuth == null) {
            return false;
        }

        mAuth.setReply(key);

        return true;
    }

    private class ObexClientThread extends Thread {

        private static final String TAG = "ObexClientThread";

        private ClientSession mClientSession;
        private BluetoothPbapRequest mRequest;

        private volatile boolean mRunning = true;

        public ObexClientThread() {

            mClientSession = null;
            mRequest = null;
        }

        @Override
        public void run() {
            super.run();

            if (!connect()) {
                mSessionHandler.obtainMessage(OBEX_SESSION_FAILED).sendToTarget();
                return;
            }

            mSessionHandler.obtainMessage(OBEX_SESSION_CONNECTED).sendToTarget();

            while (mRunning) {
                synchronized (this) {
                    try {
                        if (mRequest == null) {
                            Log.d(TAG, "waiting for request");
                            this.wait();
                        }
                    } catch (InterruptedException e) {
                        Log.d(TAG, "Interrupted");
                        mRunning = false;
                        break;
                    }
                }

                if (mRunning && mRequest != null) {
                    Log.d(TAG, "before executing the request mRunning:" + mRunning);
                    try {
                        mRequest.execute(mClientSession);
                    } catch (IOException e) {
                        // this will "disconnect" for cleanup
                        mRunning = false;
                    }

                    if (mRequest.isSuccess()) {
                        mSessionHandler.obtainMessage(OBEX_SESSION_REQUEST_COMPLETED, mRequest)
                                .sendToTarget();
                    } else {
                        mSessionHandler.obtainMessage(OBEX_SESSION_REQUEST_FAILED, mRequest)
                                .sendToTarget();
                    }
                    Log.d(TAG, "after executing the request mRunning:" + mRunning);
                }

                mRequest = null;
            }

            disconnect();

            mSessionHandler.obtainMessage(OBEX_SESSION_DISCONNECTED).sendToTarget();
        }

        public synchronized boolean schedule(BluetoothPbapRequest request) {
            Log.d(TAG, "schedule: " + request.getClass().getSimpleName());

            if (mRequest != null) {
                return false;
            }

            mRequest = request;
            notify();

            return true;
        }

        private boolean connect() {
            Log.d(TAG, "connect");

            try {
                mClientSession = new ClientSession(mTransport);
                mClientSession.setAuthenticator(mAuth);
            } catch (IOException e) {
                return false;
            }

            HeaderSet hs = new HeaderSet();
            hs.setHeader(HeaderSet.TARGET, PBAP_TARGET);

            try {
                hs = mClientSession.connect(hs);

                if (hs.getResponseCode() != ResponseCodes.OBEX_HTTP_OK) {
                    disconnect();
                    return false;
                }
            } catch (IOException e) {
                return false;
            }

            return true;
        }

        private void disconnect() {
            Log.d(TAG, "disconnect");

            if (mClientSession != null) {
                try {
                    mClientSession.disconnect(null);
                    mClientSession.close();
                } catch (IOException e) {
                }
            }
        }
    }
}
