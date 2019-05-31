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

package org.codeaurora.bluetooth.sap;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSap;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.IBluetoothSap;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import android.bluetooth.BluetoothAdapter;
import android.os.SystemProperties;
import android.app.Service;
import android.bluetooth.IBluetooth;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import java.io.OutputStream;
import java.io.InputStream;
import android.os.IBinder;
import java.nio.ByteBuffer;
import java.io.IOException;
import android.content.ServiceConnection;
import android.os.ParcelUuid;
import java.util.UUID;
import android.text.TextUtils;
import android.content.ComponentName;
import android.os.RemoteException;
import org.codeaurora.bluetooth.R;

/**
 * Provides Bluetooth Sap profile, as a service in the BluetoothExt APK.
 * @hide
 */

public class BluetoothSapService extends Service {

    private static final String TAG                             = "BluetoothSapService";
    private static final boolean DBG                            = true;
    public  static final boolean VERBOSE                        = true;

    private final static String SAP_SERVER                      = "qcom.sap.server";

    private static final int MESSAGE_START_LISTENER             = 0x01;
    private static final int MESSAGE_SAP_USER_TIMEOUT           = 0x02;

    private static final int USER_CONFIRM_TIMEOUT_VALUE         = 30000; // 30 seconds

    /**
     * IPC message types which can be exchanged between
     * Sap server process and BluetoothSapService
     */
    private static final byte SAP_IPC_MSG_SAP_REQUEST           = 0x00;
    private static final byte SAP_IPC_MSG_SAP_RESPONSE          = 0x01;
    private static final byte SAP_IPC_MSG_CTRL_REQUEST          = 0x02;
    private static final byte SAP_IPC_MSG_CTRL_RESPONSE         = 0x03;

    /**
     * Control message types between Sap server and
     * BluetoothSapService
     */
    private static final byte SAP_CRTL_MSG_DISCONNECT_REQ       = 0x00;
    private static final byte SAP_CRTL_MSG_DISCONNECT_REQ_IMM   = 0x01;
    private static final byte SAP_CRTL_MSG_CONNECTED_RESP       = 0x02;
    private static final byte SAP_CRTL_MSG_DISCONNECTED_RESP    = 0x03;

    /**
     * Offsets of message data with in the IPC message
     */
    private static final byte SAP_IPC_MSG_OFF_MSG_TYPE          = 0x00;
    private static final byte SAP_IPC_MSG_OFF_MSG_LEN           = 0x01;
    private static final byte SAP_IPC_MSG_OFF_MSG               = 0x03;

    /**
     * Offsets of message data with in the profile message
     */
    private static final byte SAP_MSG_OFF_MSG_ID                = 0x00;
    private static final byte SAP_MSG_OFF_NUM_PARAMS            = 0x01;
    private static final byte SAP_MSG_OFF_PARAM_ID              = 0x00;
    private static final byte SAP_MSG_OFF_PARAM_LEN             = 0x02;
    private static final byte SAP_MSG_OFF_PARAM_VAL             = 0x04;

    /**
     * SAP profile message Header size
     */
    private static final byte SAP_HEADER_SIZE                   = 0x04;

    /**
     * Server supported SAP Profile's maximum message size
     */
    private static final int SAP_MAX_MSG_LEN                    = 32764;

    /**
     * IPC message's header size
     */
    private static final int SAP_IPC_HEADER_SIZE                = 0x03;

    /**
     * IPC message's maximum message size (Sapd <---> SAP service)
     */
    private static final int SAP_MAX_IPC_MSG_LEN                = SAP_MAX_MSG_LEN +
                                                                  SAP_IPC_HEADER_SIZE;
    /**
     * IPC message's control message size
     */
    private static final byte SAP_IPC_CTRL_MSG_SIZE             = 0x01;

    /**
     * SAP profile message's param header size
     */
    private static final byte SAP_PARAM_HEADER_SIZE             = 0x04;

    /**
     * Constants used in error Connection response
     */
    private static final byte CONNECTION_STATUS                 = 0x01;

    private static final short CONN_STATUS_PARAM_LEN            = 0x01;

    private static final byte CONNECT_RESP                      = 0x01;

    private static final byte CONNECT_RESP_NUM_PARAMS           = 0x01;

    private static final byte CONN_ERR                          = 0x01;

    private static IBluetooth mAdapterService                   = null;

    private static BluetoothDevice mRemoteDevice                = null;

    private static volatile SocketAcceptThread mAcceptThread    = null;

    /**
     * Thread for reading the requests from SAP client using the socket
     * mRfcommSocket and upload the same requests to SAPd using the
     * socket mSapdSocket
     */
    private static volatile UplinkThread mUplinkThread          = null;

    /**
     * Thtread for reading(download) the responses from SAPd using
     * the socket mSapdSocket and route the same responses to
     * SAP client using the socket mRfcommSocket
     */
    private static volatile DownlinkThread mDownlinkThread      = null;

    /**
     * Listening Rfcomm Socket
     */
    private static volatile BluetoothServerSocket mListenSocket = null;

    /**
     * Connected Rfcomm socket
     */
    private static volatile BluetoothSocket mRfcommSocket       = null;

    /**
     * Socket represents the connection between SAPd and SAP service
     */
    private static volatile LocalSocket mSapdSocket             = null;

    private boolean mIsWaitingAuthorization                     = false;

    private volatile boolean mInterrupted                       = false;

    private boolean mSapEnable                                  = false;

    /**
     * SAP profile's UUID
     */
    private static final String SAP_UUID = "0000112D-0000-1000-8000-00805F9B34FB";

    /**
     * Intent indicating incoming connection request which is sent to
     * BluetoothSapPermissionActivity
     */
    public static final String SAP_ACCESS_REQUEST_ACTION =
                                     "org.codeaurora.bluetooth.sap.accessrequest";

    /**
     * Intent indicating incoming connection request accepted by user which is
     * sent from BluetoothSapPermissionActivity
     */
    public static final String SAP_ACCESS_ALLOWED_ACTION =
                                     "org.codeaurora.bluetooth.sap.accessallowed";

    /**
     * Intent indicating incoming connection request denied by user which is
     * sent from BluetoothSapPermissionActivity
     */
    public static final String SAP_ACCESS_DISALLOWED_ACTION =
                                  "org.codeaurora.bluetooth.sap.accessdisallowed";

    /**
     * Intent indicating timeout for user confirmation, which is sent to
     * BluetoothSapPermissionActivity
     */
    public static final String USER_CONFIRM_TIMEOUT_ACTION =
                                "org.codeaurora.bluetooth.sap.userconfirmtimeout";

    public static final String THIS_PACKAGE_NAME = "org.codeaurora.bluetooth";

    /**
     * Intent Extra name indicating always allowed which is sent from
     * BluetoothSapPermissionActivity
     */
    public static final String SAP_EXTRA_ALWAYS_ALLOWED =
                                     "org.codeaurora.bluetooth.sap.alwaysallowed";

    // Ensure not conflict with Opp notification ID
    private static final int SAP_NOTIFICATION_ID_ACCESS = -1000009;

    /**
     * Intent Extra name indicating BluetoothDevice which is sent to
     * BluetoothSapPermissionActivity
     */
    public static final String EXTRA_BLUETOOTH_DEVICE =
                                   "org.codeaurora.bluetooth.sap.bluetoothdevice";

    /**
     * String signifies the SAP profile status
     */
    public static final String BLUETOOTH_SAP_PROFILE_STATUS = "bluetooth.sap.status";

    public static final ParcelUuid SAP = ParcelUuid.fromString(SAP_UUID);

    private static final Object mAcceptLock = new Object();

    private static final Object mUplinkLock = new Object();

    private static final Object mDownlinkLock = new Object();

    private static final Object mAuthLock = new Object();

    private HashMap<BluetoothDevice, BluetoothSapDevice> mSapDevices;

    private IBinder mSapBinder;

    private BluetoothAdapter mAdapter;

    private int mStartId = -1;

    /**
     * package and class name to which we send intent to check SAP profile
     * access permission
     */
    private static final String ACCESS_AUTHORITY_PACKAGE = "com.android.settings";
    private static final String ACCESS_AUTHORITY_CLASS =
        "com.android.settings.bluetooth.BluetoothPermissionRequest";

    private static final String BLUETOOTH_ADMIN_PERM =
                    android.Manifest.permission.BLUETOOTH_ADMIN;

    private static final String BLUETOOTH_PERM =
                    android.Manifest.permission.BLUETOOTH;

    @Override
    public void onCreate() {

        super.onCreate();

        if (VERBOSE) Log.v(TAG, "Sap Service onCreate");

        mSapDevices = new HashMap<BluetoothDevice, BluetoothSapDevice>();

        mAdapter = BluetoothAdapter.getDefaultAdapter();
        mSapBinder = initBinder();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        mInterrupted = false;
        mStartId = startId;

        if (mAdapter == null) {
            Log.w(TAG, "Stopping BluetoothSapService: "
                    + "device does not have BT or device is not ready");
            // Release all resources
            closeSapService();
        } else {
            if (intent != null) {
                parseIntent(intent);
            }
        }
        return START_NOT_STICKY;
    }

    @Override
    public void onDestroy() {
        if (VERBOSE) Log.v(TAG, "Sap Service onDestroy" + mSapBinder);

        super.onDestroy();

        if(mSapDevices != null)
            mSapDevices.clear();

        closeSapService();
        if(mSapHandler != null) {
            mSapHandler.removeCallbacksAndMessages(null);
        }
        mSapBinder = null;
    }

    @Override
    public IBinder onBind(Intent intent) {
        if (VERBOSE) Log.v(TAG, "Sap Service onBind: " + mSapBinder);
        return mSapBinder;
    }

    public boolean onUnbind(Intent intent) {
        if (VERBOSE) Log.v(TAG, "onUnbind");
        return super.onUnbind(intent);
    }


    private IBinder initBinder() {
        if (VERBOSE) Log.v(TAG, "initBinder");

        return new BluetoothSapBinder(this);
    }

    private final Handler mSapHandler = new Handler() {
        @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case MESSAGE_START_LISTENER:
                        if (mAdapter.isEnabled()) {
                            startRfcommListenerThread();
                        }
                        break;
                    case MESSAGE_SAP_USER_TIMEOUT:
                        {
                            synchronized(mAuthLock) {
                                if (!mIsWaitingAuthorization) {
                                    // already handled, ignore it
                                    break;
                                }

                                mIsWaitingAuthorization = false;
                            }
                            Intent intent = new Intent(USER_CONFIRM_TIMEOUT_ACTION);
                            intent.putExtra(BluetoothDevice.EXTRA_DEVICE, mRemoteDevice);
                            sendBroadcast(intent);
                            removeSapNotification(SAP_NOTIFICATION_ID_ACCESS);
                            /* close the rfcomm socket and restart the listener thread */
                            sendErrorConnResp();
                            closeRfcommSocket();
                            startRfcommListenerThread();

                            if (DBG) Log.d(TAG,"SAP user Authorization Timeout");
                        }
                        break;
                }
            }
    };

    // process the intent from SAP receiver
    private void parseIntent(final Intent intent) {

        String action = intent.getStringExtra("action");
        if (VERBOSE) Log.v(TAG, "action: " + action);

        int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
        if (VERBOSE) Log.v(TAG, "state: " + state);

        boolean removeTimeoutMsg = true;
        if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {

            if ((state == BluetoothAdapter.STATE_ON) && (!mSapEnable)) {

                synchronized(mConnection) {
                    try {
                        if (mAdapterService == null) {
                            Intent bindIntent = new Intent(IBluetooth.class.getName());
                            ComponentName comp = bindIntent.resolveSystemService(getPackageManager(), 0);
                            bindIntent.setComponent(comp);
                            if (comp == null || !bindServiceAsUser(bindIntent, mConnection, 0,
                                    android.os.Process.myUserHandle())) {
                                Log.e(TAG, "Could not bind to Bluetooth SAP Service with " + bindIntent);
                                return;
                            }
                        }
                    } catch (Exception e) {
                        Log.e(TAG, "bindService Exception", e);
                        return;
                    }
                }

                Log.v(TAG, "Starting SAP server process");
                try {
                    SystemProperties.set(BLUETOOTH_SAP_PROFILE_STATUS, "running");
                    mSapEnable = true;
                } catch (RuntimeException e) {
                    Log.v(TAG, "Could not start SAP server process: " + e);
                }

                if (mSapEnable) {
                    mSapHandler.sendMessage(mSapHandler
                            .obtainMessage(MESSAGE_START_LISTENER));
                } else {
                    //Sap server process is not started successfully.
                    //So clean up service connection to avoid service connection leak
                    if (mAdapterService != null) {
                        try {
                            mAdapterService = null;
                            unbindService(mConnection);
                        } catch (IllegalArgumentException e) {
                            Log.e(TAG, "could not unbind the adapter Service", e);
                        }
                    }
                    return;
                }
            }
            else if (state == BluetoothAdapter.STATE_TURNING_OFF) {
                // Send any pending timeout now, as this service will be destroyed.
                if (mSapHandler.hasMessages(MESSAGE_SAP_USER_TIMEOUT)) {
                    Intent timeoutIntent = new Intent(BluetoothDevice.ACTION_CONNECTION_ACCESS_CANCEL);
                    timeoutIntent.setClassName(ACCESS_AUTHORITY_PACKAGE, ACCESS_AUTHORITY_CLASS);
                    sendBroadcast(timeoutIntent, BLUETOOTH_ADMIN_PERM);
                }

                if (mSapEnable) {
                    Log.v(TAG, "Stopping SAP server process");

                    try {
                        SystemProperties.set(BLUETOOTH_SAP_PROFILE_STATUS, "stopped");
                    } catch (RuntimeException e) {
                        Log.v(TAG, "Could not stop SAP server process: " + e);
                    }

                    synchronized(mConnection) {
                        try {
                            mAdapterService = null;
                            unbindService(mConnection);
                        } catch (Exception e) {
                            Log.e(TAG, "", e);
                        }
                    }

                    // Release all resources
                    closeSapService();
                    mSapEnable = false;
                    mConnection = null;
                }
                synchronized(mAuthLock) {
                    if (mIsWaitingAuthorization) {
                        removeSapNotification(SAP_NOTIFICATION_ID_ACCESS);
                        mIsWaitingAuthorization = false;
                    }
                }

            } else {
                removeTimeoutMsg = false;
            }
        } else if (action.equals(BluetoothDevice.ACTION_ACL_DISCONNECTED) &&
                                 mIsWaitingAuthorization) {
            if (mRemoteDevice == null) {
               Log.e(TAG, "Unexpected error!");
               return;
            }
            if (mSapHandler != null) {
               /* Let the user timeout handle this case as well */
               mSapHandler.sendMessage(mSapHandler.obtainMessage(MESSAGE_SAP_USER_TIMEOUT));
               removeTimeoutMsg = false;
            }

        }  else if (action.equals(SAP_ACCESS_ALLOWED_ACTION)) {
            if (mRemoteDevice == null) {
               Log.e(TAG, "Unexpected error!");
               return;
            }

            if (VERBOSE) {

                Log.v(TAG, "SapService-Received ACCESS_ALLOWED_ACTION");
            }
            synchronized(mAuthLock) {
                if (!mIsWaitingAuthorization) {
                    // this reply is not for us
                    return;
                }

                mIsWaitingAuthorization = false;
            }

            if (intent.getBooleanExtra(BluetoothSapService.SAP_EXTRA_ALWAYS_ALLOWED, false) == true) {
                  if(mRemoteDevice != null) {
                     mRemoteDevice.setTrust(true);
                     Log.v(TAG, "setTrust() TRUE " + mRemoteDevice.getName());
                  }
            }
            /* start the uplink thread */
            startUplinkThread();

        } else if (action.equals(SAP_ACCESS_DISALLOWED_ACTION)) {
            /* close the rfcomm socket and restart the listener thread */
            Log.d(TAG,"SAP_ACCESS_DISALLOWED_ACTION:" + mIsWaitingAuthorization);

            mIsWaitingAuthorization = false;
            sendErrorConnResp();
            closeRfcommSocket();
            startRfcommListenerThread();
        } else {
            removeTimeoutMsg = false;
        }

        if (removeTimeoutMsg) {
            if (mSapHandler != null)
                mSapHandler.removeMessages(MESSAGE_SAP_USER_TIMEOUT);
        }
    }

    private void startRfcommListenerThread() {
        if (VERBOSE) Log.v(TAG, "SAP Service startRfcommListenerThread");

        synchronized(mAcceptLock) {
            // if it is already running,close it
            if (mAcceptThread != null) {
                try {
                    mAcceptThread.shutdown();
                    mAcceptThread.join();
                    mAcceptThread = null;
                } catch (InterruptedException ex) {
                    Log.w(TAG, "mAcceptThread close error" + ex);
                }
            }
            if (mAcceptThread == null) {
                mAcceptThread = new SocketAcceptThread();
                mAcceptThread.setName("BluetoothSapAcceptThread");
                mAcceptThread.start();
            }
        }
    }

    private void startUplinkThread() {
        if (VERBOSE) Log.v(TAG, "SAP Service startUplinkThread");

        synchronized(mUplinkLock) {
            if (mUplinkThread != null) {
                try {
                    mUplinkThread.shutdown();
                    mUplinkThread.join();
                    mUplinkThread = null;
                } catch (InterruptedException ex) {
                    Log.w(TAG, "mUplinkThread close error" + ex);
                }
            }
            if (mUplinkThread == null) {
                mUplinkThread = new UplinkThread();
                mUplinkThread.setName("BluetoothSapUplinkThread");
                mUplinkThread.start();
            }
        }
    }

    private void startDownlinkThread() {
        if (VERBOSE) Log.v(TAG, "SAP Service startDownlinkThread");

        synchronized(mDownlinkLock) {
            if (mDownlinkThread == null) {
                mDownlinkThread = new DownlinkThread();
                mDownlinkThread.setName("BluetoothSapDownlinkThread");
                mDownlinkThread.start();
            }
        }
    }

    private final boolean initRfcommSocket() {
        if (VERBOSE) Log.v(TAG, "Sap Service initRfcommSocket");

        boolean initRfcommSocketOK = false;
        final int CREATE_RETRY_TIME = 10;

        // It's possible that create will fail in some cases. retry for 10 times
        for (int i = 0; i < CREATE_RETRY_TIME && !mInterrupted; i++) {
            initRfcommSocketOK = true;
            try {
                // start listening for incoming rfcomm channel connection
                mListenSocket = mAdapter.listenUsingRfcommWithServiceRecord("SIM Access Server", SAP.getUuid());

            } catch (IOException e) {
                Log.e(TAG, "Error create RfcommListenSocket " + e.toString());
                initRfcommSocketOK = false;
            }
            if (!initRfcommSocketOK) {
                // Need to break out of this loop if BT is being turned off.
                if (mAdapter == null) break;
                int state = mAdapter.getState();
                if ((state != BluetoothAdapter.STATE_TURNING_ON) && (state != BluetoothAdapter.STATE_ON)) {
                    Log.w(TAG, "initRfcommSocket failed as BT is (being) turned off");
                    break;
                }
                try {
                    if (VERBOSE) Log.v(TAG, "wait 300 ms");
                    Thread.sleep(300);
                } catch (InterruptedException e) {
                    Log.e(TAG, "socketAcceptThread thread was interrupted (3)");
                    break;
                }
            } else {
                break;
            }
        }

        if (mInterrupted) {
            initRfcommSocketOK = false;
            //close the listen socket to avoid resource leakage
            closeListenSocket();
        }

        if (initRfcommSocketOK) {
            if (VERBOSE) Log.v(TAG, "Succeed to create listening socket ");

        } else {
            Log.e(TAG, "Error to create listening socket after " + CREATE_RETRY_TIME + " try");
        }
        return initRfcommSocketOK;
    }

    private final boolean initSapdClientSocket() {
        if (VERBOSE) Log.v(TAG, "SAP initSapdClientSocket");

        boolean initSapdSocketOK = false;

        try {
            LocalSocketAddress locSockAddr = new LocalSocketAddress(SAP_SERVER);
            mSapdSocket = new LocalSocket();
            mSapdSocket.connect(locSockAddr);
            initSapdSocketOK = true;
        } catch (java.io.IOException e) {
            Log.e(TAG, "cant connect: " + e);
        }

        if (initSapdSocketOK) {
            if (VERBOSE) Log.v(TAG, "Succeed to create Sapd socket ");

        } else {
            Log.e(TAG, "Error to create Sapd socket");
        }
        return initSapdSocketOK;
    }

    private final synchronized void closeListenSocket() {
        // exit SocketAcceptThread early
        if (VERBOSE) Log.v(TAG, "Close listen Socket : " );
        if (mListenSocket != null) {
            try {
                // this will cause mListenSocket.accept() return early with
                // IOException
                mListenSocket.close();
                mListenSocket = null;
            } catch (IOException ex) {
                Log.e(TAG, "Close listen Socket error: " + ex);
            }
        }
    }

    private final synchronized void closeRfcommSocket() {
        if (VERBOSE) Log.v(TAG, "Close rfcomm Socket : " );
        if (mRfcommSocket != null) {
            try {
                mRfcommSocket.close();
                mRfcommSocket = null;
            } catch (IOException e) {
                Log.e(TAG, "Close Rfcomm Socket error: " + e.toString());
            }
        }
    }

    private final synchronized void closeSapdSocket() {
        if (VERBOSE) Log.v(TAG, "Close sapd  Socket : " );
        if (mSapdSocket != null) {
            try {
                mSapdSocket.close();
                mSapdSocket = null;
            } catch (IOException e) {
                Log.e(TAG, "Close Sapd Socket error: " + e.toString());
            }
        }
    }

    private final void closeSapService() {
        if (VERBOSE) Log.v(TAG, "Sap Service closeSapService in");

        // exit initRfcommSocket early
        mInterrupted = true;

        closeListenSocket();

        closeRfcommSocket();

        closeSapdSocket();

        synchronized(mDownlinkLock) {
            if (mDownlinkThread != null) {
                try {
                    mDownlinkThread.shutdown();
                    mDownlinkThread.join();
                    mDownlinkThread = null;
                } catch (InterruptedException ex) {
                    Log.w(TAG, "mDownlinkThread close error" + ex);
                }
            }
        }

        synchronized(mUplinkLock) {
            if (mUplinkThread != null) {
                try {
                    mUplinkThread.shutdown();
                    mUplinkThread.join();
                    mUplinkThread = null;
                } catch (InterruptedException ex) {
                    Log.w(TAG, "mUplinkThread close error" + ex);
                }
            }
        }
        /* remove the MESSAGE_START_LISTENER message which might be posted
         * by the uplink thread */
        mSapHandler.removeMessages(MESSAGE_START_LISTENER);

        synchronized(mAcceptLock) {
            if (mAcceptThread != null) {
                try {
                    mAcceptThread.shutdown();
                    mAcceptThread.join();
                    mAcceptThread = null;
                } catch (InterruptedException ex) {
                    Log.w(TAG, "mAcceptThread close error" + ex);
                }
            }
        }

        if (stopSelfResult(mStartId)) {
            if (VERBOSE) Log.v(TAG, "successfully stopped Sap service");
        }

        if (VERBOSE) Log.v(TAG, "Sap Service closeSapService out");
    }

    private void createSapNotification(BluetoothDevice device) {
        if (VERBOSE) Log.v(TAG, "Creating SAP access notification");

        NotificationManager nm = (NotificationManager)
                getSystemService(Context.NOTIFICATION_SERVICE);

        // Create an intent triggered by clicking on the status icon.
        Intent clickIntent = new Intent();
        clickIntent.setClass(this, BluetoothSapPermissionActivity.class);
        clickIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        clickIntent.setAction(SAP_ACCESS_REQUEST_ACTION);
        clickIntent.putExtra(EXTRA_BLUETOOTH_DEVICE, device);

        // Create an intent triggered by clicking on the
        // "Clear All Notifications" button
        Intent deleteIntent = new Intent();
        deleteIntent.setClass(this, BluetoothSapReceiver.class);

        Notification notification = null;
        String name = device.getName();
        if (TextUtils.isEmpty(name)) {
            name = getString(R.string.defaultname);
        }

        deleteIntent.setAction(SAP_ACCESS_DISALLOWED_ACTION);
        notification = new Notification(android.R.drawable.stat_sys_data_bluetooth,
            getString(R.string.sap_notif_ticker), System.currentTimeMillis());
        notification.setLatestEventInfo(this, getString(R.string.sap_notif_ticker),
                getString(R.string.sap_notif_message, name), PendingIntent
                        .getActivity(this, 0, clickIntent, 0));

        notification.flags |= Notification.FLAG_AUTO_CANCEL;
        notification.flags |= Notification.FLAG_ONLY_ALERT_ONCE;
        notification.defaults = Notification.DEFAULT_SOUND;
        notification.deleteIntent = PendingIntent.getBroadcast(this, 0, deleteIntent, 0);
        nm.notify(SAP_NOTIFICATION_ID_ACCESS, notification);


        if (VERBOSE) Log.v(TAG, "Awaiting Authorization : SAP Connection : " + device.getName());

    }

    private void removeSapNotification(int id) {
        Context context = getApplicationContext();
        NotificationManager nm = (NotificationManager) context
                .getSystemService(Context.NOTIFICATION_SERVICE);
        nm.cancel(id);
    }

    /**
     * A thread that runs in the background waiting for remote rfcomm
     * connect.Once a remote socket connected, this thread shall be
     * shutdown.When the remote disconnect,this thread shall run again
     * waiting for next request.
     */
    private class SocketAcceptThread extends Thread {

        private boolean stopped = false;

        @Override
        public void run() {
            BluetoothServerSocket listenSocket;
            if (mListenSocket == null) {
                if (!initRfcommSocket()) {
                    return;
                }
            }

            while (!stopped) {
                try {
                    if (VERBOSE) Log.v(TAG, "Accepting socket connection...");
                    listenSocket = mListenSocket;
                    if (listenSocket == null) {
                        Log.w(TAG, "mListenSocket is null");
                        break;
                    }

                    mRfcommSocket = mListenSocket.accept();

                    synchronized (BluetoothSapService.this) {
                        if (mRfcommSocket == null) {
                            Log.w(TAG, " mRfcommSocket is null");
                            break;
                        }
                        mRemoteDevice = mRfcommSocket.getRemoteDevice();
                    }
                    if (mRemoteDevice == null) {
                        Log.i(TAG, "getRemoteDevice() = null");
                        break;
                    }
                    boolean trust = false;
                    if (mRemoteDevice != null)
                        trust = mRemoteDevice.getTrustState();
                    if (VERBOSE) Log.v(TAG, "GetTrustState() = " + trust);

                    if (trust) {
                        /* start the uplink thread */
                        startUplinkThread();
                    } else {
                        createSapNotification(mRemoteDevice);

                        if (VERBOSE) Log.v(TAG, "incoming connection accepted from: "+ mRemoteDevice);
                        mIsWaitingAuthorization = true;

                        mSapHandler.sendMessageDelayed(mSapHandler.obtainMessage(MESSAGE_SAP_USER_TIMEOUT),
                                 USER_CONFIRM_TIMEOUT_VALUE);
                    }
                    stopped = true; // job done ,close this thread;
                    if (VERBOSE) Log.v(TAG, "SocketAcceptThread stopped ");
                } catch (IOException ex) {
                    stopped=true;
                    if (VERBOSE) Log.v(TAG, "Accept thread exception: " + ex.toString());
                }
            }
        }

        void shutdown() {
            stopped = true;
            interrupt();
        }
    }

    int checkParamPresent(ByteBuffer msgBuf, int offset, int totalRead) {
        int TotalLeft = totalRead;
        int TotalProcessed = 0;
        int ParamLen = 0;

        // check msgBuf has the SAP Param header
        if(TotalLeft > SAP_PARAM_HEADER_SIZE) {
            ParamLen = msgBuf.getShort(offset + SAP_MSG_OFF_PARAM_LEN);
            TotalLeft = TotalLeft - SAP_PARAM_HEADER_SIZE;
            TotalProcessed = SAP_PARAM_HEADER_SIZE;
        }
        else return -1;

        // check msgBuf has the parameter payload
        // calculate the padding bytes only when the parameter length is not a multiple of 4
        if ((ParamLen > 0) && (ParamLen%4 != 0)) ParamLen = ParamLen + (4 - (ParamLen%4));

        if (TotalLeft >= ParamLen) {
            TotalLeft = TotalLeft - SAP_PARAM_HEADER_SIZE;
            TotalProcessed = TotalProcessed + ParamLen;
            if (VERBOSE) Log.v(TAG, "ParamLen" + ParamLen  + " TotalProcessed" + TotalProcessed +
                                                                        "TotalLeft" + TotalLeft);
            return TotalProcessed;
        }
        else return -1;
    }

    boolean checkCompleteRequest(ByteBuffer msgBuf, int offset, int totalRead) {
        int TotalLeft = totalRead;
        int TotalProcessed = 0;
        int NumParameters = 0;

        // check msgBuf has the SAP Message header
        if (TotalLeft >= SAP_HEADER_SIZE) {
            NumParameters = msgBuf.get(offset + SAP_MSG_OFF_NUM_PARAMS);
            TotalLeft = TotalLeft - SAP_HEADER_SIZE;
            TotalProcessed = SAP_HEADER_SIZE;
        }
        else return false;

        // check msgBuf has the all parameters
        while(NumParameters > 0) {
            int TotalParamLen = 0;
            TotalParamLen = checkParamPresent(msgBuf, offset + TotalProcessed, TotalLeft);
            if (TotalParamLen < 0) {
                return false;
            }
            else if (TotalLeft >= TotalParamLen) {
                TotalLeft = TotalLeft - TotalParamLen;
                TotalProcessed = TotalProcessed + TotalParamLen;
                NumParameters = NumParameters - 1;
            }
            if (VERBOSE) Log.v(TAG, "NumParameters "+ NumParameters + " TotalProcessed  " +
                                                TotalProcessed + "TotalLeft " + TotalLeft);
        }
        return true;
    }

    /**
     * A thread that runs in the background waiting for remote SAPd socket
     * connection.Once the connection is established, this thread starts the
     * downlink thread and starts forwarding the SAP profile requests to
     * the SAPd on receiving the requests from rfcomm channel.
     */
    private class UplinkThread extends Thread {

        private boolean stopped = false;
        private boolean IntExit = false;
        private OutputStream mSapdOutputStream = null;
        private InputStream mRfcommInputStream = null;
        ByteBuffer IpcMsgBuffer = ByteBuffer.allocate(SAP_MAX_IPC_MSG_LEN);
        private int NumRead = 0;
        private int TotalRead = 0;
        private byte TempByte = 0;

        @Override
        public void run() {
            if (mSapdSocket == null) {
                if (!initSapdClientSocket()) {
                    /* close the rfcomm socket to avoid resource leakage */
                    closeRfcommSocket();
                    /*restart the listener thread */
                    mSapHandler.sendMessage(mSapHandler.obtainMessage(MESSAGE_START_LISTENER));
                    return;
                }
            }
            if(mSapdSocket != null) {
                if( mSapdSocket.isConnected())
                    try {
                        mSapdOutputStream = mSapdSocket.getOutputStream();
                    } catch (IOException ex) {
                        if (VERBOSE)
                            Log.v(TAG, "mSapdOutputStream exception: " + ex.toString());
                    }
                else return;
            }
            else return;

            if (mRfcommSocket != null) {
                if(mRfcommSocket.isConnected())
                    try {
                        mRfcommInputStream = mRfcommSocket.getInputStream();
                    } catch (IOException ex) {
                        if (VERBOSE)
                            Log.v(TAG, "mRfcommInputStream exception: " + ex.toString());
                    }
                else return;
            }
            else return;

            // create the downlink thread for reading the responses
            //from SAP server process
            startDownlinkThread();

            while (!stopped) {
                try {
                    if (VERBOSE)
                        Log.v(TAG, "Reading the SAP requests from Rfcomm channel");
                    /* Read the SAP request from Rfcomm channel */
                    NumRead = mRfcommInputStream.read(IpcMsgBuffer.array(), SAP_IPC_MSG_OFF_MSG + TotalRead,
                                                                                SAP_MAX_MSG_LEN - TotalRead);
                    if (VERBOSE) Log.v(TAG, "NumRead from rfcomm = " + NumRead);
                    if ( NumRead < 0) {
                        break;
                    }
                    else if ( NumRead != 0) {
                        TotalRead = TotalRead + NumRead;
                        /* check if the packet has the complete SAP request */
                        if (!checkCompleteRequest(IpcMsgBuffer, SAP_IPC_MSG_OFF_MSG, TotalRead)) {
                            continue;
                        }
                        /* Wrtie the same SAP request to the SAP server socket with
                           some additional parameters */
                        IpcMsgBuffer.put(SAP_IPC_MSG_OFF_MSG_TYPE, SAP_IPC_MSG_SAP_REQUEST);
                        IpcMsgBuffer.putShort(SAP_IPC_MSG_OFF_MSG_LEN, (short)TotalRead);
                        /* swap bytes of message len as buffer is Big Endian */
                        TempByte = IpcMsgBuffer.get(SAP_IPC_MSG_OFF_MSG_LEN);

                        IpcMsgBuffer.put( SAP_IPC_MSG_OFF_MSG_LEN, IpcMsgBuffer.get(SAP_IPC_MSG_OFF_MSG_LEN + 1));

                        IpcMsgBuffer.put(SAP_IPC_MSG_OFF_MSG_LEN + 1, TempByte);
                        try {
                            mSapdOutputStream.write(IpcMsgBuffer.array(), 0, TotalRead + SAP_IPC_HEADER_SIZE);
                            mSapdOutputStream.flush();
                        } catch (IOException ex) {
                            IntExit = true;
                            if (VERBOSE) Log.v(TAG, "sapd write exception: " + ex.toString());
                            break;
                        }
                        // reset the TotalRead
                        TotalRead = 0;
                    }
                } catch (IOException ex) {
                    IntExit = true;
                    if (VERBOSE) Log.v(TAG, "Rfcomm channel Read exception: " + ex.toString());
                    break;
                }
            }
            // send the disconneciton request immediate to Sap server
            if (IntExit) {
                disconnectImmediate(mRemoteDevice);
            }

            /* close the sapd socket */
            closeSapdSocket();
            /* close the rfcomm socket */
            closeRfcommSocket();

            /* wait for sap downlink thread to close */
            synchronized (mDownlinkLock) {
                if (mDownlinkThread != null) {
                    try {
                        mDownlinkThread.shutdown();
                        mDownlinkThread.join();
                        mDownlinkThread = null;
                    } catch (InterruptedException ex) {
                        Log.w(TAG, "mDownlinkThread close error" + ex);
                    }
                }
            }

            // Intimate the Settings APP about the disconnection
            handleSapDeviceStateChange(mRemoteDevice, BluetoothProfile.STATE_DISCONNECTED);

            if (VERBOSE) Log.v(TAG, "IntExit = " + IntExit + "stopped = " + stopped);

            if (IntExit && !stopped) {
                if (VERBOSE) Log.v(TAG, "starting the listener thread ");
                /* start the listener thread */
                mSapHandler.sendMessage(mSapHandler.obtainMessage(MESSAGE_START_LISTENER));
            }
            if (VERBOSE) Log.v(TAG, "uplink thread exited ");
        }

        void shutdown() {
            stopped = true;
            interrupt();
        }
    }

    /**
     * A thread that runs in the background and starts forwarding the
     * SAP profile responses received from Sapd to the rfcomm channel.
     */
    private class DownlinkThread extends Thread {

        private boolean stopped = false;
        private OutputStream mRfcommOutputStream = null;
        private InputStream mSapdInputStream = null;
        ByteBuffer IpcMsgBuffer = ByteBuffer.allocate(SAP_MAX_IPC_MSG_LEN);
        private int NumRead = 0;
        private int ReadIndex = 0;
        private byte TempByte = 0;

        @Override
        public void run() {

            if (mSapdSocket != null) {
                if ( mSapdSocket.isConnected())
                    try {
                        mSapdInputStream = mSapdSocket.getInputStream();
                    } catch (IOException ex) {
                        if (VERBOSE) Log.v(TAG, "mSapdInputStream exception: " + ex.toString());
                    }
                else return;
            }
            else return;

            if (mRfcommSocket != null) {
                if (mRfcommSocket.isConnected())
                    try {
                        mRfcommOutputStream = mRfcommSocket.getOutputStream();
                    } catch (IOException ex) {
                        if (VERBOSE) Log.v(TAG, "mRfcommOutputStream exception: " + ex.toString());
                    }
                else return;
            }
            else return;

            while (!stopped) {
                try {
                    if (VERBOSE) Log.v(TAG, "Reading the SAP responses from Sapd");
                    /* Read the SAP responses from SAP server */
                    NumRead = mSapdInputStream.read(IpcMsgBuffer.array(),0, SAP_MAX_IPC_MSG_LEN);
                    if (VERBOSE) Log.v(TAG, "NumRead from sapd = " + NumRead);
                    if ( NumRead < 0) {
                        break;
                    }
                    else if( NumRead != 0) {
                        /* some times read buffer contains multiple responses */
                        do {
                            /* swap bytes of message len as buffer is Big Endian */
                            TempByte = IpcMsgBuffer.get(ReadIndex + SAP_IPC_MSG_OFF_MSG_LEN);

                            IpcMsgBuffer.put(ReadIndex + SAP_IPC_MSG_OFF_MSG_LEN, IpcMsgBuffer.get(ReadIndex +
                                         SAP_IPC_MSG_OFF_MSG_LEN + 1));

                            IpcMsgBuffer.put(ReadIndex + SAP_IPC_MSG_OFF_MSG_LEN + 1, TempByte);

                            if (IpcMsgBuffer.get(ReadIndex + SAP_IPC_MSG_OFF_MSG_TYPE) == SAP_IPC_MSG_SAP_RESPONSE) {
                                try {
                                    mRfcommOutputStream.write(IpcMsgBuffer.array(), ReadIndex + SAP_IPC_MSG_OFF_MSG,
                                           IpcMsgBuffer.getShort(ReadIndex + SAP_IPC_MSG_OFF_MSG_LEN));
                                    mRfcommOutputStream.flush();
                                } catch (IOException ex) {
                                    stopped = true;
                                    break;
                                }
                                if (VERBOSE)
                                    Log.v(TAG, "Length of DownlinkThread Msg written to Rfcomm" +
                                    IpcMsgBuffer.getShort(ReadIndex + SAP_IPC_MSG_OFF_MSG_LEN));
                            }
                            else if (IpcMsgBuffer.get(ReadIndex + SAP_IPC_MSG_OFF_MSG_TYPE) == SAP_IPC_MSG_CTRL_RESPONSE) {

                                if (IpcMsgBuffer.get(ReadIndex + SAP_IPC_MSG_OFF_MSG) == SAP_CRTL_MSG_CONNECTED_RESP) {
                                    handleSapDeviceStateChange(mRemoteDevice, BluetoothProfile.STATE_CONNECTED);
                                }
                                else if (IpcMsgBuffer.get(ReadIndex + SAP_IPC_MSG_OFF_MSG) == SAP_CRTL_MSG_DISCONNECTED_RESP) {
                                    handleSapDeviceStateChange(mRemoteDevice, BluetoothProfile.STATE_DISCONNECTED);
                                }
                                if (VERBOSE)
                                    Log.v(TAG, "DownlinkThread control message received");
                            }
                            ReadIndex = ReadIndex + IpcMsgBuffer.getShort(ReadIndex + SAP_IPC_MSG_OFF_MSG_LEN);
                            ReadIndex = ReadIndex + SAP_IPC_HEADER_SIZE;
                            // give some delay if responses are more than one
                            if (ReadIndex < NumRead) {
                                try {
                                    Thread.sleep(100);
                                } catch (InterruptedException e) {
                                    Log.v(TAG, "DownlinkThread: sleep exception");
                                }
                            }
                        } while(ReadIndex < NumRead);
                    }

                } catch (IOException ex) {
                    stopped = true;
                    if (VERBOSE) Log.v(TAG, "Sapd Read exception: " + ex.toString());
                }
                /* reset the ReadIndex */
                ReadIndex = 0;
            }
            /* close the sapd socket */
            closeSapdSocket();
            /* close the rfcomm socket */
            closeRfcommSocket();

            if (VERBOSE) Log.v(TAG, "downlink thread exited ");
        }

        void shutdown() {
            stopped = true;
            interrupt();
        }
    }


    boolean disconnect(BluetoothDevice device) {
        enforceCallingOrSelfPermission(BLUETOOTH_PERM, "Need BLUETOOTH permission");
        OutputStream mSapdOutputStream = null;
        int WriteLen = SAP_IPC_HEADER_SIZE + SAP_IPC_CTRL_MSG_SIZE;
        ByteBuffer IpcMsgBuffer = ByteBuffer.allocate(WriteLen);

        if (mSapdSocket != null) {
            if ( mSapdSocket.isConnected())
                try {
                    mSapdOutputStream = mSapdSocket.getOutputStream();
                } catch (IOException ex) {
                    if (VERBOSE) Log.v(TAG, "mSapdOutputStream exception: " + ex.toString());
                }
            else return false;
        }
        else return false;

        IpcMsgBuffer.put(SAP_IPC_MSG_OFF_MSG_TYPE, SAP_IPC_MSG_CTRL_REQUEST);
        IpcMsgBuffer.putShort(SAP_IPC_MSG_OFF_MSG_LEN,SAP_IPC_CTRL_MSG_SIZE);
        IpcMsgBuffer.put(SAP_IPC_MSG_OFF_MSG, SAP_CRTL_MSG_DISCONNECT_REQ);
        try {
            if (mSapdOutputStream != null) {
                mSapdOutputStream.write(IpcMsgBuffer.array(), 0, WriteLen);
                mSapdOutputStream.flush();
            }
        } catch (IOException ex) {
            if (VERBOSE) Log.v(TAG, "mSapdOutputStream wrtie exception: " + ex.toString());
        }
        return true;
    }

    private final boolean disconnectImmediate(BluetoothDevice device) {
        enforceCallingOrSelfPermission(BLUETOOTH_PERM, "Need BLUETOOTH permission");
        OutputStream mSapdOutputStream = null;
        int WriteLen = SAP_IPC_HEADER_SIZE + SAP_IPC_CTRL_MSG_SIZE;
        ByteBuffer IpcMsgBuffer = ByteBuffer.allocate(WriteLen);

        if (mSapdSocket != null) {
            if ( mSapdSocket.isConnected())
                try {
                    mSapdOutputStream = mSapdSocket.getOutputStream();
                } catch (IOException ex) {
                    if (VERBOSE) Log.v(TAG, "mSapdOutputStream exception: " + ex.toString());
                }
            else return false;
        }
        else return false;

        IpcMsgBuffer.put(SAP_IPC_MSG_OFF_MSG_TYPE, SAP_IPC_MSG_CTRL_REQUEST);
        IpcMsgBuffer.putShort(SAP_IPC_MSG_OFF_MSG_LEN,SAP_IPC_CTRL_MSG_SIZE);
        IpcMsgBuffer.put(SAP_IPC_MSG_OFF_MSG, SAP_CRTL_MSG_DISCONNECT_REQ_IMM);
        try {
            if (mSapdOutputStream != null) {
                mSapdOutputStream.write(IpcMsgBuffer.array(), 0, WriteLen);
                mSapdOutputStream.flush();
            }
        } catch (IOException ex) {
            if (VERBOSE) Log.v(TAG, "mSapdOutputStream wrtie exception: " + ex.toString());
        }
        return true;
    }

    private final boolean sendErrorConnResp() {
        enforceCallingOrSelfPermission(BLUETOOTH_PERM, "Need BLUETOOTH permission");
        OutputStream mRfcommOutputStream = null;
        int WriteLen = SAP_HEADER_SIZE + SAP_PARAM_HEADER_SIZE +
                       CONN_STATUS_PARAM_LEN + (4-((CONN_STATUS_PARAM_LEN)%4)); // 4 byte padding

        ByteBuffer IpcMsgBuffer = ByteBuffer.allocate(WriteLen);

        if (mRfcommSocket != null) {
            if ( mRfcommSocket.isConnected())
                try {
                    mRfcommOutputStream = mRfcommSocket.getOutputStream();
                } catch (IOException ex) {
                    if (VERBOSE) Log.v(TAG, "mRfcommOutputStream exception: " + ex.toString());
                }
            else return false;
        }
        else return false;

        IpcMsgBuffer.put(SAP_MSG_OFF_MSG_ID, CONNECT_RESP);
        IpcMsgBuffer.put(SAP_MSG_OFF_NUM_PARAMS, CONNECT_RESP_NUM_PARAMS);
        IpcMsgBuffer.put(SAP_HEADER_SIZE + SAP_MSG_OFF_PARAM_ID,CONNECTION_STATUS);
        IpcMsgBuffer.putShort(SAP_HEADER_SIZE + SAP_MSG_OFF_PARAM_LEN, CONN_STATUS_PARAM_LEN);
        IpcMsgBuffer.put(SAP_HEADER_SIZE + SAP_MSG_OFF_PARAM_VAL, CONN_ERR);

        try {
            if (mRfcommOutputStream != null) {
                mRfcommOutputStream.write(IpcMsgBuffer.array(), 0, WriteLen);
                mRfcommOutputStream.flush();
            }
        } catch (IOException ex) {
            if (VERBOSE) Log.v(TAG, "mRfcommOutputStream  wrtie exception: " + ex.toString());
        }
        return true;
    }


    int getConnectionState(BluetoothDevice device) {
        BluetoothSapDevice sapDevice = mSapDevices.get(device);
        if (sapDevice == null) {
            return BluetoothSap.STATE_DISCONNECTED;
        }
        return sapDevice.mState;
    }

    public void notifyProfileConnectionStateChanged(BluetoothDevice device,
            int profileId, int newState, int prevState) {
        if (mAdapterService != null) {
            try {
                mAdapterService.sendConnectionStateChange(device, profileId, newState, prevState);
            }catch (RemoteException re) {
                Log.e(TAG, "",re);
            }
        }
    }


    private final void handleSapDeviceStateChange(BluetoothDevice device, int state) {
        if (DBG) Log.d(TAG, "handleSapDeviceStateChange: device: " + device + " state: " + state);
        int prevState;
        BluetoothSapDevice sapDevice = mSapDevices.get(device);
        if (sapDevice == null) {
            prevState = BluetoothProfile.STATE_DISCONNECTED;
        } else {
            prevState = sapDevice.mState;
        }
        Log.d(TAG, "handleSapDeviceStateChange preState: " + prevState + " state: " + state);

        if (prevState != state) {
            if (sapDevice == null) {
                sapDevice = new BluetoothSapDevice(state);
                mSapDevices.put(device, sapDevice);
            } else {
            sapDevice.mState = state;
            }
       }

        /* Notifying the connection state change of the profile before sending
           the intent for connection state change, as it was causing a race
           condition, with the UI not being updated with the correct connection
           state. */
        Log.d(TAG, "Sap Device state : device: " + device + " State:" +
                       prevState + "->" + state);
        notifyProfileConnectionStateChanged(device, BluetoothProfile.SAP, state, prevState);
        Intent intent = new Intent(BluetoothSap.ACTION_CONNECTION_STATE_CHANGED);
        intent.putExtra(BluetoothDevice.EXTRA_DEVICE, device);
        intent.putExtra(BluetoothSap.EXTRA_PREVIOUS_STATE, prevState);
        intent.putExtra(BluetoothSap.EXTRA_STATE, state);
        sendBroadcast(intent, BLUETOOTH_PERM);

    }

    List<BluetoothDevice> getConnectedDevices() {
        enforceCallingOrSelfPermission(BLUETOOTH_PERM, "Need BLUETOOTH permission");
        List<BluetoothDevice> devices = getDevicesMatchingConnectionStates(
                new int[] {BluetoothProfile.STATE_CONNECTED});
        return devices;
    }

    List<BluetoothDevice> getDevicesMatchingConnectionStates(int[] states) {
         enforceCallingOrSelfPermission(BLUETOOTH_PERM, "Need BLUETOOTH permission");
        List<BluetoothDevice> sapDevices = new ArrayList<BluetoothDevice>();

        for (BluetoothDevice device: mSapDevices.keySet()) {
            int sapDeviceState = getConnectionState(device);
            for (int state : states) {
                if (state == sapDeviceState) {
                    sapDevices.add(device);
                    break;
                }
            }
        }
        return sapDevices;
    }

    private ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            mAdapterService = IBluetooth.Stub.asInterface(service);
        }

        public void onServiceDisconnected(ComponentName className) {
            mAdapterService = null;
        }
    };

    /**
     * Handlers for incoming service calls
     */
    private static class BluetoothSapBinder extends IBluetoothSap.Stub {

        private BluetoothSapService mService;

        public BluetoothSapBinder(BluetoothSapService svc) {
            mService = svc;
            if (VERBOSE) Log.v(TAG, "BluetoothSapBinder: mService: " + mService);

        }

        private BluetoothSapService getService() {
            if (VERBOSE) Log.v(TAG, "getService: mService: " + mService);

            if (mService != null)
                return mService;
            return null;
        }
        public boolean cleanup()  {
            if (VERBOSE) Log.v(TAG, "cleanup: mService: " + mService);
            mService = null;
            return true;
        }


        public boolean disconnect(BluetoothDevice device) {
            BluetoothSapService service = getService();
            if (service == null) return false;
            return service.disconnect(device);
        }

        public int getConnectionState(BluetoothDevice device) {
            BluetoothSapService service = getService();
            if (service == null) return BluetoothSap.STATE_DISCONNECTED;
            return service.getConnectionState(device);
        }
        public List<BluetoothDevice> getConnectedDevices() {
            BluetoothSapService service = getService();
            if (service == null) return new ArrayList<BluetoothDevice>(0);
            return service.getConnectedDevices();
        }

        public List<BluetoothDevice> getDevicesMatchingConnectionStates(int[] states) {
            BluetoothSapService service = getService();
            if (service == null) return new ArrayList<BluetoothDevice>(0);
            return service.getDevicesMatchingConnectionStates(states);
        }
    };

    private class BluetoothSapDevice {
        private int mState;

        BluetoothSapDevice(int state) {
            mState = state;
        }
    }
}
