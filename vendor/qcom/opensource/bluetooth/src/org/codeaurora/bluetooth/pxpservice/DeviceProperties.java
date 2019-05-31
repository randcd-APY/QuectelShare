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


package org.codeaurora.bluetooth.pxpservice;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.QBluetoothAdapter;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.IBluetoothManager;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.bluetooth.BluetoothRssiMonitorCallback;

import android.bluetooth.BluetoothGattCharacteristic;
import android.util.Log;
import java.util.Timer;
import java.util.TimerTask;

/**
 * A Device Properties class. It contains information about bluetooth device
 * which PxpMonitor tried to connect.
 */
public class DeviceProperties {

    private static final boolean DBG = true;

    private static final String TAG = "PxpdeviceProp";

    public BluetoothGatt gatt = null;

    public String deviceAddress = null;

    public BluetoothManager mManager = null;

    public BluetoothAdapter mAdapter = null;

    public QBluetoothAdapter  mQAdapter = null;

    public BluetoothDevice  mDevice = null;

    private Context          mContext = null;

    public int     mState = 0;

    private BluetoothRssiMonitorCallback mMonitorCbk;

    public BluetoothGattCharacteristic iasAlertLevelCh = null;

    public BluetoothGattCharacteristic llsAlertLevelCh = null;

    public BluetoothGattCharacteristic txPowerLevelCh = null;

    public int pathLossAlertLevel = PxpConsts.ALERT_LEVEL_NO;

    public int maxPathLossThreshold = 147;

    public int minPathLossThreshold = 100;

    public int txPowerLevel = 0;

    public boolean isReading = false;

    public boolean isAlerting = false;

    public boolean isPairing = false;

    public boolean PairingFail = false;

    public boolean connectionState = false;

    public boolean AddedToWhitelist = false;

    public boolean hasIasService = false;

    public boolean hasLlsService = false;

    public boolean hasTxpService = false;

    public boolean disconnect = false;

    public boolean startDiscoverServices = false;

    public boolean failedReadAlertLevel = false;

    public boolean failedReadTxPowerLevel = false;
    /* command status */
    /** @hide */
    public static final int COMMAND_STATUS_SUCCESS = 0x00;
    /** @hide */
    public static final int COMMAND_STATUS_FAILED  = 0x01;

    private final Object mStateLock = new Object();
     /* Monitor state constants */
    private static final int MONITOR_STATE_IDLE     = 0;
    private static final int MONITOR_STATE_STARTING = 1;
    private static final int MONITOR_STATE_STOPPING = 2;
    private static final int MONITOR_STATE_STARTED  = 3;
    private static final int MONITOR_STATE_CLOSED   = 4;


    public void reset() {
        iasAlertLevelCh = null;
        llsAlertLevelCh = null;
        txPowerLevelCh = null;
        pathLossAlertLevel = PxpConsts.ALERT_LEVEL_NO;
        maxPathLossThreshold = 147;
        minPathLossThreshold = 100;
        txPowerLevel = 0;
        isReading = false;
        isAlerting = false;
        isPairing = false;
        PairingFail = false;
        connectionState = false;
        AddedToWhitelist = false;
        hasIasService = false;
        hasLlsService = false;
        hasTxpService = false;
        startDiscoverServices = false;
        failedReadAlertLevel = false;
        failedReadTxPowerLevel = false;
        mState = MONITOR_STATE_CLOSED;
    }

 public void BluetoothLwPwrProximityMonitor (Context cxt, BluetoothDevice device, BluetoothRssiMonitorCallback cbk) {
        mContext    = cxt;
        mState      = MONITOR_STATE_CLOSED;
        mMonitorCbk = cbk;

        try {
            mDevice     = device;
        } catch (IllegalArgumentException e) {
            mDevice = null;
            if (DBG) Log.e(TAG, "", e);
        }

        mQAdapter  = QBluetoothAdapter.getDefaultAdapter();
        if (mDevice != null && mQAdapter != null) {
            mState = MONITOR_STATE_IDLE;
           mQAdapter.registerLppClient(mLPProxymityMonitorCallback, mDevice.getAddress(), true);
        } else {
            mDevice = null;
            mQAdapter = null;
        }
    }

  public final QBluetoothAdapter.LeLppCallback mLPProxymityMonitorCallback = new QBluetoothAdapter.LeLppCallback() {

   /* @hide */
    public void onWriteRssiThreshold(int status) {
        if (DBG) Log.d(TAG, "onWriteRssiThreshold() status=" + status);
        synchronized (mStateLock) {
            if (mState == MONITOR_STATE_STARTING){
                if (status == BluetoothGatt.GATT_SUCCESS){
                    if (mQAdapter != null) {
                        mQAdapter.enableRssiMonitor(this, true);
                    }
                } else {
                        mState = MONITOR_STATE_STOPPING;
                }
            }
        }
    }
 /** @hide */
    public void onReadRssiThreshold(int low, int upper, int alert, int status) {
        if (DBG) Log.d(TAG, "onReadRssiThreshold() LowerLimit=" + low +
                       ", UpperLimit=" + upper + ", Alert=" + alert + ", status=" + status);
        if (mMonitorCbk != null) {
            mMonitorCbk.onReadRssiThreshold(low, upper, alert, (status == 0)?COMMAND_STATUS_SUCCESS:COMMAND_STATUS_FAILED);
        }
    }

 /** @hide */
    public void onEnableRssiMonitor(int enable, int status) {
        if (DBG) Log.d(TAG, "onEnableRssiMonitor() enable=" + enable + ", status=" + status);
        synchronized(mStateLock) {
            if (mState == MONITOR_STATE_STARTING) {
                if (status == BluetoothGatt.GATT_SUCCESS && (enable != 0)) {
                    mState = MONITOR_STATE_STARTED;
                    if (DBG) Log.d(TAG, "Monitor is started successfully");
                }
                if (enable == 0) {
                   mQAdapter.registerLppClient(mLPProxymityMonitorCallback, mDevice.getAddress(),false);
                   mState = MONITOR_STATE_CLOSED;
                }
            }
        }
        if (mState == MONITOR_STATE_STARTED && mMonitorCbk != null) {
            if (DBG) Log.d(TAG, "Notify users that monitor has been started successfully");
            mMonitorCbk.onStarted();
        }

    }
    /** @hide */
    public void onRssiThresholdEvent(int evtType, int rssi) {
        if (DBG) Log.d(TAG, "onRssiThresholdEvent() event=" + evtType + ", rssi=" + rssi);
        if (mMonitorCbk != null) mMonitorCbk.onAlert(evtType, rssi);
    }

    /** @hide */
    public boolean onUpdateLease() {
        if (DBG) Log.d(TAG, "onUpdateLease()");
        synchronized(mStateLock) {
            return (mState != MONITOR_STATE_IDLE && mState != MONITOR_STATE_CLOSED);
        }
    }
  };
}
