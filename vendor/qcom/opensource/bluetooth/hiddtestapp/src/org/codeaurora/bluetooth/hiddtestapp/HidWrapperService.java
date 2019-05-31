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

package org.codeaurora.bluetooth.hiddtestapp;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothHidDevice;
import android.bluetooth.BluetoothHidDeviceAppConfiguration;
import android.bluetooth.BluetoothHidDeviceAppQosSettings;
import android.bluetooth.BluetoothHidDeviceAppSdpSettings;
import android.bluetooth.BluetoothHidDeviceCallback;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothProfile.ServiceListener;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import android.util.SparseArray;

import java.nio.ByteBuffer;
import java.util.Arrays;

public class HidWrapperService extends Service {

    private final static String TAG = HidWrapperService.class.getSimpleName();


    private final static int MESSAGE_REGISTER_APP = 1;

    private final static int MESSAGE_APP_REGISTERED = 2;

    private final static int MESSAGE_DEV_CONNECTED = 3;

    private final static int MESSAGE_INTR_DATA_RECEIVED = 4;

    private final static int MESSAGE_GET_REPORT_RECEIVED = 5;

    private final static int MESSAGE_SET_REPORT_RECEIVED = 6;

    private final IBinder mBinder = new LocalBinder();

    private final BluetoothAdapter mAdapter = BluetoothAdapter.getDefaultAdapter();

    private boolean mAppRegistered = false;

    private BluetoothDevice mPluggedDevice;

    private BluetoothHidDevice mHidDevice;

    private byte mProtocol = BluetoothHidDevice.PROTOCOL_REPORT_MODE;

    private BluetoothHidDeviceAppConfiguration mAppConfig;

    private float mBatteryLevel = 0;

    private float mCurrentBatteryLevel = 0;

    private int mConnectionState = BluetoothHidDevice.STATE_DISCONNECTED;

    private MouseWrapper mMouseWrapper = new MouseWrapper();

    private KeyboardWrapper mKeyboardWrapper = new KeyboardWrapper();

    private BatteryWrapper mBatteryWrapper = new BatteryWrapper();

    private HidEventListener mEventListener;

    private SparseArray<ReportData> mInputReportCache = new SparseArray<ReportData>();

    private SparseArray<ReportData> mOutputReportCache = new SparseArray<ReportData>();

    public interface HidEventListener {
        public void onApplicationState(boolean registered);
        public void onPluggedDeviceChanged(BluetoothDevice device);
        public void onConnectionState(BluetoothDevice device, boolean connected);
        public void onProtocolModeState(boolean bootMode);
        public void onKeyboardLedState(boolean numLock, boolean capsLock, boolean scrollLock);
    };

    private class ReportData {
        public byte[] data;
    }


    private final ServiceListener mServiceListener = new ServiceListener() {

        @Override
        public void onServiceConnected(int profile, BluetoothProfile proxy) {
            if (profile != BluetoothProfile.HID_DEVICE) return;

            Log.i(TAG, "Profile proxy connected");

            mHidDevice = (BluetoothHidDevice) proxy;

            mHandler.obtainMessage(MESSAGE_REGISTER_APP).sendToTarget();
        }

        @Override
        public void onServiceDisconnected(int profile) {
            if (profile != BluetoothProfile.HID_DEVICE) return;

            Log.i(TAG, "Profile proxy disconnected");

            // do not dereference proxy here as we won't be able to close it
        }
    };

    private final BluetoothHidDeviceCallback mCallback = new BluetoothHidDeviceCallback() {

        @Override
        public void onAppStatusChanged(BluetoothDevice pluggedDevice,
                                            BluetoothHidDeviceAppConfiguration config, boolean registered) {
            Log.d(TAG, "onAppStatusChanged: pluggedDevice=" +
                (pluggedDevice == null ? null : pluggedDevice.toString()) + " registered=" + registered);

            mEventListener.onApplicationState(registered);

            if (registered) {
                mPluggedDevice = pluggedDevice;
                mAppConfig = config;
                mHandler.obtainMessage(MESSAGE_APP_REGISTERED).sendToTarget();
            } else {
                mPluggedDevice = null;
                mAppConfig = null;
            }

            mEventListener.onPluggedDeviceChanged(mPluggedDevice);
        }

        @Override
        public void onConnectionStateChanged(BluetoothDevice device, int state) {
            mConnectionState = state;

            if (state == BluetoothHidDevice.STATE_CONNECTED) {
                mEventListener.onConnectionState(device, true);
                mHandler.obtainMessage(MESSAGE_DEV_CONNECTED).sendToTarget();

                if (!device.equals(mPluggedDevice)) {
                    mPluggedDevice = device;
                    mEventListener.onPluggedDeviceChanged(mPluggedDevice);
                }
            } else {
                /* Reset Protocol back to report Protocol Mode */
                mProtocol = BluetoothHidDevice.PROTOCOL_REPORT_MODE;
                mEventListener.onConnectionState(device, false);
            }
        }

        @Override
        public void onIntrData(byte reportId, byte[] data) {
            Log.v(TAG, "intr data: reportId=" + reportId + " data=" + Arrays.toString(data));

            mHandler.obtainMessage(MESSAGE_INTR_DATA_RECEIVED, reportId, 0, ByteBuffer.wrap(data))
                    .sendToTarget();
        }

        @Override
        public void onSetProtocol(byte protocol) {
            Log.d(TAG, "protocol set to " + protocol);

            mProtocol = protocol;

            mEventListener.onProtocolModeState(protocol == BluetoothHidDevice.PROTOCOL_BOOT_MODE);
        }

        @Override
        public void onVirtualCableUnplug() {
            mPluggedDevice = null;
            mEventListener.onPluggedDeviceChanged(mPluggedDevice);
        }

        @Override
        public void onGetReport(byte type, byte id, int bufferSize) {
            if (type == BluetoothHidDevice.REPORT_TYPE_INPUT) {
                mHandler.obtainMessage(MESSAGE_GET_REPORT_RECEIVED, id, bufferSize).sendToTarget();
            } else if (type == BluetoothHidDevice.REPORT_TYPE_OUTPUT) {
                if (id != mKeyboardWrapper.getReportId(BluetoothHidDevice.REPORT_TYPE_OUTPUT)) {
                    Log.v(TAG, "onGetReport(), output report for invalid id = " + id);
                    mHidDevice.reportError(BluetoothHidDevice.ERROR_RSP_INVALID_RPT_ID);
                } else {
                    ReportData rd = mOutputReportCache.get(id);
                    if (rd == null) {
                        /* No output report received with particular report id, report default
                         * values */
                        byte[] mBuffer = new byte[8];
                        Log.v(TAG, "get_report id for keyboard");
                        for (int i = 0; i < 8; i++) {
                            mBuffer[i] = 0x00;
                        }
                        mHidDevice.replyReport(BluetoothHidDevice.REPORT_TYPE_OUTPUT, id, mBuffer);
                    } else {
                        mHidDevice.replyReport(BluetoothHidDevice.REPORT_TYPE_OUTPUT, id, rd.data);
                    }
                }
            } else {
                Log.v(TAG, "onGetReport(), unsupported report type = " + type);
                /* Unsupported report type */
                mHidDevice.reportError(BluetoothHidDevice.ERROR_RSP_UNSUPPORTED_REQ);
            }
        }

        @Override
        public void onSetReport(byte type, byte id, byte[] data) {
            if (type != BluetoothHidDevice.REPORT_TYPE_OUTPUT) {
                /* Unsupported report type */
                Log.v(TAG, "onSetReport(), unsupported report type = " + type);
                mHidDevice.reportError(BluetoothHidDevice.ERROR_RSP_UNSUPPORTED_REQ);
            } else {
                if (id != mKeyboardWrapper.getReportId(BluetoothHidDevice.REPORT_TYPE_OUTPUT)) {
                    Log.v(TAG, "onSetReport(), output report for invalid id = " + id);
                    mHidDevice.reportError(BluetoothHidDevice.ERROR_RSP_INVALID_RPT_ID);
                } else {
                    mHandler.obtainMessage(MESSAGE_SET_REPORT_RECEIVED, id, 0,
                        ByteBuffer.wrap(data)).sendToTarget();
                }
            }
        }
    };

    private final Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_REGISTER_APP:
                    BluetoothHidDeviceAppSdpSettings sdp = new BluetoothHidDeviceAppSdpSettings(
                            HidConsts.NAME, HidConsts.DESCRIPTION, HidConsts.PROVIDER,
                            BluetoothHidDevice.SUBCLASS1_COMBO, HidConsts.DESCRIPTOR);

                    BluetoothHidDeviceAppQosSettings inQos = new BluetoothHidDeviceAppQosSettings(
                        BluetoothHidDeviceAppQosSettings.SERVICE_GUARANTEED, 200, 2, 200,
                        10000 /* 10 ms */, 10000 /* 10 ms */);

                    BluetoothHidDeviceAppQosSettings outQos = new BluetoothHidDeviceAppQosSettings(
                        BluetoothHidDeviceAppQosSettings.SERVICE_GUARANTEED, 900, 9, 900,
                        10000 /* 10 ms */, 10000 /* 10 ms */);

                    boolean result = mHidDevice.registerApp(sdp, inQos, outQos, mCallback);
                    Log.v(TAG, "registerApp()=" + result);
                    break;

                case MESSAGE_DEV_CONNECTED:
                    mInputReportCache.clear();
                    mOutputReportCache.clear();
                    mBatteryWrapper.update(mBatteryLevel);
                    mCurrentBatteryLevel = mBatteryLevel;
                    mEventListener.onProtocolModeState(
                        mProtocol == BluetoothHidDevice.PROTOCOL_BOOT_MODE);
                    break;

                case MESSAGE_INTR_DATA_RECEIVED:
                    byte reportId = (byte) msg.arg1;
                    byte[] data = ((ByteBuffer) msg.obj).array();

                    mKeyboardWrapper.parseIncomingReport(reportId, data, mEventListener);
                    break;

                case MESSAGE_GET_REPORT_RECEIVED:
                    byte id = (byte) msg.arg1;
                    int size = msg.arg2;

                    ReportData rd = mInputReportCache.get(id);

                    if (rd != null) {
                        mHidDevice.replyReport(BluetoothHidDevice.REPORT_TYPE_INPUT, id, rd.data);
                    } else {
                        rd = new ReportData();
                        /* Report id not in cache, send default values if id is valid */
                        if (id == mKeyboardWrapper.getReportId(
                            BluetoothHidDevice.REPORT_TYPE_INPUT)) {
                            byte[] mBuffer = new byte[8];
                            Log.v(TAG, "get_report id for keyboard");
                            for (int i = 0; i < 8; i++) {
                                mBuffer[i] = 0x00;
                            }
                            storeReport(id, mBuffer, true);
                            mHidDevice.replyReport(BluetoothHidDevice.REPORT_TYPE_INPUT,
                                id, mBuffer);
                        } else if (id == mMouseWrapper.getReportId()) {
                            byte[] mBuffer = new byte[4];
                            Log.v(TAG, "get_report id for mouse");
                            for (int i = 0; i < 4; i++) {
                                mBuffer[i] = 0x00;
                            }
                            storeReport(id, mBuffer, true);
                            mHidDevice.replyReport(BluetoothHidDevice.REPORT_TYPE_INPUT,
                                id, mBuffer);
                        } else {
                            /* Invalid Report Id */
                            Log.v(TAG, "Get Report for Invalid report id = " + id);
                            mHidDevice.reportError(BluetoothHidDevice.ERROR_RSP_INVALID_RPT_ID);
                        }
                    }
                    break;

                case MESSAGE_SET_REPORT_RECEIVED:
                    byte rptId = (byte) msg.arg1;
                    byte[] setRptData = ((ByteBuffer) msg.obj).array();
                    Log.v(TAG, "MESSAGE_SET_REPORT_RECEIVED for id = " + rptId);

                    if (rptId != mKeyboardWrapper.getReportId
                        (BluetoothHidDevice.REPORT_TYPE_OUTPUT)) {
                        Log.v(TAG, "onSetReport(), set report for invalid id = " + rptId);
                        mHidDevice.reportError(BluetoothHidDevice.ERROR_RSP_INVALID_RPT_ID);
                        break;
                    }

                    if (!mKeyboardWrapper.parseIncomingReport(rptId, setRptData, mEventListener)) {
                        Log.v(TAG, "onSetReport(), parameters invalid");
                        mHidDevice.reportError(BluetoothHidDevice.ERROR_RSP_INVALID_PARAM);
                        break;
                    }

                    Log.v(TAG, "onSetReport(), sending successful handshake for set report");
                    mHidDevice.reportError(BluetoothHidDevice.ERROR_RSP_SUCCESS);
                    break;
            }
        }
    };

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
                        BluetoothAdapter.ERROR);

                if (state == BluetoothAdapter.STATE_ON) {
                    registerApp();
                } else {
                    unregisterApp();
                }
            } else if (action.equals(Intent.ACTION_BATTERY_CHANGED)) {
                int level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
                int scale = intent.getIntExtra(BatteryManager.EXTRA_SCALE, 100);

                mBatteryLevel = (float) level / (float) scale;
                if (mCurrentBatteryLevel != mBatteryLevel) {
                    Log.v(TAG, "ACTION_BATTERY_CHANGED, mBatteryLevel = " + mBatteryLevel);
                    mBatteryWrapper.update(mBatteryLevel);
                    mCurrentBatteryLevel = mBatteryLevel;
                }
            }
        }

    };

    private final class MouseWrapper {

        private byte[] mBuffer = new byte[4];

        private byte getReportId() {
            if (mProtocol == BluetoothHidDevice.PROTOCOL_BOOT_MODE) {
                return HidConsts.BOOT_MOUSE_REPORT_ID;
            } else {
                return HidConsts.MOUSE_REPORT_ID;
            }
        }

        public synchronized void move(byte dx, byte dy) {
            // leave buttons state unchanged
            mBuffer[1] = dx;
            mBuffer[2] = dy;

            byte id = getReportId();
            storeReport(id, mBuffer, true);
            mHidDevice.sendReport(id, mBuffer);
        }

        public synchronized void buttonDown(int which) {
            mBuffer[0] |= (1 << which);
            mBuffer[1] = 0;
            mBuffer[2] = 0;

            byte id = getReportId();
            storeReport(id, mBuffer, true);
            mHidDevice.sendReport(id, mBuffer);
        }

        public synchronized void buttonUp(int which) {
            mBuffer[0] &= ~(1 << which);
            mBuffer[1] = 0;
            mBuffer[2] = 0;

            byte id = getReportId();
            storeReport(id, mBuffer, true);
            mHidDevice.sendReport(id, mBuffer);
        }

        public synchronized void scroll(byte delta) {
            mBuffer[3] = delta;

            byte id = getReportId();
            storeReport(id, mBuffer, true);
            mHidDevice.sendReport(id, mBuffer);

            // reset to 0 after sending so it won't self-repeat in subsequent
            // reports
            mBuffer[3] = 0x00;
        }
    };

    private final class KeyboardWrapper {

        private final static byte MODIFIER_BASE = (byte) 0xe0;
        private final static byte MODIFIER_COUNT = 8; /* left ctrl -> right gui */

        private byte[] mBuffer = new byte[8];

        private byte getReportId(int type) {
            if (mProtocol == BluetoothHidDevice.PROTOCOL_BOOT_MODE) {
                return HidConsts.BOOT_KEYBOARD_REPORT_ID;
            } else {
                if (type == BluetoothHidDevice.REPORT_TYPE_INPUT) {
                    return HidConsts.KEYBOARD_INPUT_REPORT_ID;
                } else {
                    return HidConsts.KEYBOARD_OUTPUT_REPORT_ID;
                }
            }
        }

        public synchronized void keyDown(byte key) {
            if (key >= MODIFIER_BASE && key <= MODIFIER_BASE + MODIFIER_COUNT) {
                mBuffer[0] |= (1 << (key - MODIFIER_BASE));
            } else if ((key & 0x80) != 0) {
                mBuffer[1] |= (1 << (key & 0x07));
            } else {
                for (int i = 2; i < 8; i++) {
                    if (mBuffer[i] == 0x00) {
                        mBuffer[i] = key;
                        break;
                    }
                }
            }

            byte id = getReportId(BluetoothHidDevice.REPORT_TYPE_INPUT);
            storeReport(id, mBuffer, true);
            mHidDevice.sendReport(id, mBuffer);
        }

        public synchronized void keyUp(byte key) {
            if (key >= MODIFIER_BASE && key <= MODIFIER_BASE + MODIFIER_COUNT) {
                mBuffer[0] &= ~(1 << (key - MODIFIER_BASE));
            } else if ((key & 0x80) != 0) {
                mBuffer[1] &= ~(1 << (key & 0x07));
            } else {
                for (int i = 2; i < 8; i++) {
                    if (mBuffer[i] == key) {
                        mBuffer[i] = 0x00;
                        break;
                    }
                }
            }

            byte id = getReportId(BluetoothHidDevice.REPORT_TYPE_INPUT);
            storeReport(id, mBuffer, true);
            mHidDevice.sendReport(id, mBuffer);
        }

        public boolean parseIncomingReport(byte reportId, byte[] data, HidEventListener callback) {
            if (reportId != getReportId(BluetoothHidDevice.REPORT_TYPE_OUTPUT) || callback == null)
                return false;

            Log.v(TAG, "parseIncomingReport(): data.length = "
                + data.length + " mProtocol = " + mProtocol);

            /* Output report can only be of 1 byte */
            if (data.length != 1)
                return false;

            byte leds = data[0];
            storeReport(reportId, data, false);

            callback.onKeyboardLedState(
                    (leds & HidConsts.KEYBOARD_LED_NUM_LOCK) != 0,
                    (leds & HidConsts.KEYBOARD_LED_CAPS_LOCK) != 0,
                    (leds & HidConsts.KEYBOARD_LED_SCROLL_LOCK) != 0);
            return true;
        }
    };

    private final class BatteryWrapper {

        private byte[] mBuffer = new byte[1];

        public synchronized void update(float level) {
            Log.v(TAG, "BatteryWrapper: update: level = " + level);
            if (mConnectionState != BluetoothHidDevice.STATE_CONNECTED) {
                // battery update will not trigger reconnection
                return;
            }

            // battery strength not supported in boot mode
            if (mProtocol != BluetoothHidDevice.PROTOCOL_REPORT_MODE)
                return;

            int val = (int) (level * 255);
            Log.v(TAG, "BatteryWrapper: update: val = " + val);

            mBuffer[0] = (byte) (val & 0xff);
            Log.v(TAG, "BatteryWrapper: update: mBuffer[0] = " + mBuffer[0]);

            if (mHidDevice != null) {
                storeReport(HidConsts.BATTERY_REPORT_ID, mBuffer, true);
                mHidDevice.sendReport(HidConsts.BATTERY_REPORT_ID, mBuffer);
            }
        }
    };

    public class LocalBinder extends Binder {
        HidWrapperService getService() {
            return HidWrapperService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public void onCreate() {
        Log.v(TAG, "onCreate()");

        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        filter.addAction(Intent.ACTION_BATTERY_CHANGED);
        registerReceiver(mReceiver, filter);

        registerApp();
    }

    @Override
    public void onDestroy() {
        Log.v(TAG, "onDestroy()");

        unregisterApp();

        unregisterReceiver(mReceiver);
    }

    private void registerApp() {
        Log.v(TAG, "registerApp(): mAppRegistered=" + mAppRegistered);

        if (mAppRegistered || !mAdapter.isEnabled())
            return;

        if (!mAdapter.getProfileProxy(getApplicationContext(), mServiceListener,
                BluetoothProfile.HID_DEVICE)) {
            Log.w(TAG, "Cannot obtain profile proxy");
            return;
        }

        mAppRegistered = true;
    }

    private void unregisterApp() {
        Log.v(TAG, "unregisterApp(): mAppRegistered=" + mAppRegistered);

        if (!mAppRegistered)
            return;

        if (mHidDevice != null) {
            if (mAppConfig != null) {
                mHidDevice.unregisterApp(mAppConfig);
            }
            mAdapter.closeProfileProxy(BluetoothProfile.HID_DEVICE, mHidDevice);
        }

        mAppRegistered = false;
    }

    public void setEventListener(HidEventListener listener) {
        mEventListener = listener;
    }

    public void mouseMove(int dx, int dy) {
        if (mHidDevice == null) return;
        Log.i(TAG, "touch event received");
        mMouseWrapper.move((byte) dx, (byte) dy);
    }

    public void mouseButtonDown(int which) {
        if (mHidDevice == null) return;
        Log.i(TAG, "touch event received");
        mMouseWrapper.buttonDown(which);
    }

    public void mouseButtonUp(int which) {
        if (mHidDevice == null) return;
        Log.i(TAG, "touch event received");
        mMouseWrapper.buttonUp(which);
    }

    public void mouseScroll(byte delta) {
        if (mHidDevice == null) return;
        Log.i(TAG, "touch event received");
        mMouseWrapper.scroll(delta);
    }

    public void keyboardKeyDown(byte key) {
        if (mHidDevice == null) return;
        Log.i(TAG, "touch event received");
        mKeyboardWrapper.keyDown(key);
    }

    public void keyboardKeyUp(byte key) {
        if (mHidDevice == null) return;
        Log.i(TAG, "touch event received");
        mKeyboardWrapper.keyUp(key);
    }

    public void connect() {
        if (mHidDevice == null) return;
        mHidDevice.connect();
    }

    public void disconnect() {
        if (mHidDevice == null) return;
        mHidDevice.disconnect();
    }

    public void unplug() {
        if (mHidDevice == null) return;
        mHidDevice.unplug();
    }

    public BluetoothDevice getPluggedDevice() {
        return mPluggedDevice;
    }

    private void storeReport(byte reportId, byte[] data, boolean inputReport) {
        ReportData rd;

        if (inputReport) {
            rd = mInputReportCache.get(reportId);
        } else {
            rd = mOutputReportCache.get(reportId);
        }

        if (rd == null) {
            rd = new ReportData();
            if (inputReport) {
                mInputReportCache.put(reportId, rd);
            } else {
                mOutputReportCache.put(reportId, rd);
            }
        }

        rd.data = data.clone();
    }
}
