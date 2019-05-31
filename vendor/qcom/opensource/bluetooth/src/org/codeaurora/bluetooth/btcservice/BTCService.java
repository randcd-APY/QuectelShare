/*
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

package org.codeaurora.bluetooth.btcservice;

import android.app.Service;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.net.Credentials;
import java.io.OutputStream;
import android.util.Log;
import android.os.IBinder;
import android.content.Intent;
import android.os.Process;
import java.nio.ByteBuffer;

/**
 * <p> {@link com.android.bluetooth.btservice.BTCService} is a background service which starts
 *  and stops on BLUETOOTH_ON and BLUETOOTH_OFF intents
 *  Ref: {@see android.bluetooth.BluetoothAdapter#ACTION_STATE_CHANGED}
 * </p>
 * <p> {@link com.android.bluetooth.btservice.BTCEventProvider} is the BroadcaseReciever which
 *  listen to set of bluetooth related intents and responsible for starting and stopping of
 *  BTCService
 * </p>
 * <p> {@link com.android.bluetooth.btservice.BTCService} creates the Local Socket server named as
 * {@link com.android.bluetooth.btservice.BTCService.BTC_SERVER} and listen for the
 * incoming connections up on start
 *
 * <p> {@link com.android.bluetooth.btservice.BTCService} can accepts the connection from clients
 * whose uid is either {@link android.os.Process.BLUETOOTH_UID} or ROOT(with UID 0) and discards
 * all other connections and hence provides the events only to the authenticates processes
 * </p>
 *
 * Once the successful connection is established BTCService pushes the events listed in {@link BTCEvent}
 * over the socket, Client has to read these data and interpret based on the values {@link BTCEvent}
 * </p>
 *
 * <p> As LocalSocket implementation doesn't  properly propagate the closing of socket to
 * to the native layets properly, closing the socket at Java layer doesn't invalidate the
 * socket at the native layers
 * *** So, It is client's responsibility to close their socket
 * end just after recieving the {@link BTCEvent.BLUETOOTH_OFF} event
 * </p>
 *
 */
public class BTCService extends Service
{
    public final static String BTC_SERVER = "qcom.btc.server";
    private static LocalServerSocket mSocket =null;
    private static LocalSocket mRemoteSocket = null;
    private static final String LOGTAG = "BTCService";
    private static OutputStream mOutputStream = null;
    private static Thread mSocketAcceptThread;
    private static boolean mLocalConnectInitiated = false;
    private static final boolean D = false/*Constants.DEBUG*/;
    private static final boolean V = false/*Constants.VERBOSE*/;
    private static final Object mLock = new Object();

    public enum BTCEvent {
        BLUETOOTH_NONE(0x00),
        BLUETOOTH_ON(0x20),
        BLUETOOTH_OFF(0x21),
        BLUETOOTH_DISCOVERY_STARTED(0x22),
        BLUETOOTH_DISCOVERY_FINISHED(0x23),
        BLUETOOTH_DEVICE_CONNECTED(0x24),
        BLUETOOTH_DEVICE_DISCONNECTED(0x25),
        BLUETOOTH_HEADSET_CONNECTED(0x40),
        BLUETOOTH_HEADSET_DISCONNECTED(0x41),
        BLUETOOTH_HEADSET_AUDIO_STREAM_STARTED(0x42),
        BLUETOOTH_HEADSET_AUDIO_STREAM_STOPPED(0x43),
        BLUETOOTH_AUDIO_SINK_CONNECTED(0x60),
        BLUETOOTH_AUDIO_SINK_DISCONNECTED(0x61),
        BLUETOOTH_SINK_STREAM_STARTED(0x62),
        BLUETOOTH_SINK_STREAM_STOPPED(0x63),
        BLUETOOTH_INPUT_DEVICE_CONNECTED(0x80),
        BLUETOOTH_INPUT_DEVICE_DISCONNECTED(0x81);

        private int mValue;
        private BTCEvent(int value) {
            mValue = value;
        }

        public int getValue() {
            return mValue;
        }
    }

    public BTCService() {
        Log.v(LOGTAG, "BTCService");
    }

    static private void cleanupService() throws java.io.IOException {
    synchronized (mLock) {
        if (mSocket != null) {
            mSocket.close();
            mSocket = null;
            if (V) Log.v(LOGTAG, "Server Socket closed");
        }
        if (mRemoteSocket != null) {
            mRemoteSocket.close();
            mRemoteSocket = null;
            if (V) Log.v(LOGTAG, "Client Socket closed");
        }
        if( mSocketAcceptThread != null ) {
            mSocketAcceptThread.interrupt();
            if (V) Log.v(LOGTAG, "Acceptor thread stopped");
        }
    }
    }

    /**
     * <p> Sends the BTCEvent over the valid socket connection
     * </p>
     * @param event {@link BTCEvent} object
     * @return 0 on success,
     *        -1 when there is no client connection available,
     *        -2 when there is issue while writing on client socket.
     */
    static public int sendEvent(BTCEvent event) {
        if (V) Log.v(LOGTAG, "sendEvent: " + event.getValue());
        if (mRemoteSocket == null) {
            if(D) Log.d(LOGTAG, "No Valid Connection" );
            return -1;
        }

        ByteBuffer buffer = ByteBuffer.allocate(4);
        buffer.putInt(event.getValue());

        try {
            mOutputStream.write(buffer.array());
        }
        catch (java.io.IOException e) {
            if (D) Log.d(LOGTAG, "Error while posting the event: " + event + "Error:" + e);
            if (D) Log.d(LOGTAG, "connectoin is closed for Unknown reason, restart the acceptor thread ");
            startListener();
            return -2;
        }
        return 0;
    }

    static private void startListener() {
        try  {
            cleanupService();
            mSocket = new LocalServerSocket(BTC_SERVER);
        } catch (java.io.IOException e) {
            Log.e(LOGTAG, "Error While cleanupService:"+ e);
            return;
        }
        mSocketAcceptThread = new Thread() {
        @Override
        public void run() {
        do {
           try {
               if (V) Log.v(LOGTAG, "Waiting for connection...");
               try {
                   mRemoteSocket = mSocket.accept();
               } catch (java.io.IOException e) {
                   Log.e(LOGTAG, "Error While accepting the connection: " + e);
                   //Keep back in accpetor loop
                   mRemoteSocket = null;
               }
               if (mLocalConnectInitiated) {
                   //This is just a local connection
                   //to unblock the accept
                   if (D) Log.d(LOGTAG, "Terminator in action: ");
                   cleanupService();
                   mLocalConnectInitiated = false;
                   return;
               }
               if (mRemoteSocket != null) {
                   Log.v(LOGTAG, "Got socket: " + mRemoteSocket);
                   Credentials cred = null;
                   try {
                       cred = mRemoteSocket.getPeerCredentials();
                   } catch (java.io.IOException e) {
                       Log.e(LOGTAG, "Getting peer credential" + "Error:" + e);
                       cred = null;//Pas through, so that it fails and back in loop
                   }
                   if (cred != null) {
                       if (cred.getUid() != 0 && cred.getUid() != Process.BLUETOOTH_UID) {
                           Log.e(LOGTAG, "Peer with UID: " + cred.getUid() +" is not authenticated");
                           Log.e(LOGTAG, "Close the connection and back in acceptor loop");
                           mRemoteSocket.close();
                           mRemoteSocket = null;
                       } else {
                           //Only ROOT and BLUETOOTH connections are
                           //are accepted
                           Log.v(LOGTAG, "Authenticated socket: " + mRemoteSocket);
                           mOutputStream = mRemoteSocket.getOutputStream();
                           //Close the  Server Socket
                           mSocket.close();
                           mSocket = null;
                           return;
                       }
                  } else {
                      Log.e(LOGTAG, "Not able to get the credentais, discard connection");
                      mRemoteSocket.close();
                      mRemoteSocket = null;
                  }

               } else {
                   Log.e(LOGTAG, "Remote socket unavaialble");
                   //Keep back in accpetor loop
                   mRemoteSocket = null;
               }
           } catch (Exception e) {
                Log.d(LOGTAG,  "RunningThread InterruptedException");
                e.printStackTrace();
                Thread.currentThread().interrupt();
           }
       } while ((!Thread.currentThread().isInterrupted()));
       }
       };
       mSocketAcceptThread.start();
    }

    private void closeServerSocket() {
        Thread t = new Thread() {
        @Override
        public void run() {
            try {
                LocalSocketAddress locSockAddr = new LocalSocketAddress(BTC_SERVER);
                LocalSocket terminator = new LocalSocket();
                Log.e(LOGTAG, "Trying to unblock by connecting");
                mLocalConnectInitiated = true;
                terminator.connect(locSockAddr);
                terminator.close();
                Log.e(LOGTAG, "Terminator closed");
            } catch (java.io.IOException e) {
                Log.e(LOGTAG, "cant connect: " + e);
            }
        }
        };
        t.start();
    }

    @Override
    public void onCreate() {
        Log.v(LOGTAG, "onCreate");
        super.onCreate();

        Log.v(LOGTAG, "calling startListener");
        startListener();
    }

    @Override
    public void onDestroy() {
        Log.v(LOGTAG, "onDestroy");
        try {

            if (mSocket != null) {
                //This is the case where
                //server socket is still stuck in accept()
                closeServerSocket();
            }
            cleanupService();
        }
        catch (java.io.IOException e) {
            Log.e(LOGTAG, "in onDestroy: " + e);
        }
    }

    @Override
    public IBinder onBind(Intent in) {
        Log.v(LOGTAG, "onBind");
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(LOGTAG, "onStart Command called!!");
        //Make this restarable service by
        //Android app manager
        return START_STICKY;
   }
}
