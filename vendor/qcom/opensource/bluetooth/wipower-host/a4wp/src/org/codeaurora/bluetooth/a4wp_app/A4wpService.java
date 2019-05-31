/*
 * Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package org.codeaurora.bt_wipower.a4wp;

import java.util.UUID;

import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.app.Service;
import android.net.Credentials;
import java.io.OutputStream;
import android.util.Log;
import android.os.IBinder;
import android.content.Intent;
import android.os.Process;
import java.nio.ByteBuffer;
import android.wipower.WipowerManager;
import android.wipower.WipowerManagerCallback;
import android.wipower.WipowerManager.WipowerState;
import android.wipower.WipowerManager.PowerApplyEvent;
import android.wipower.WipowerManager.PowerLevel;
import android.wipower.WipowerDynamicParam;
import com.quicinc.wbc.WbcManager;
import com.quicinc.wbc.WbcTypes;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;

import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseSettings;
import android.bluetooth.le.AdvertiseData;
import android.bluetooth.le.BluetoothLeAdvertiser;
import android.os.ParcelUuid;
import android.os.PowerManager;
import android.os.SystemProperties;

/**
 * Class which executes A4WP service
 */
public class A4wpService extends Service
{
    private static final String LOGTAG = "A4wpService";
    private static OutputStream mOutputStream = null;
    private BluetoothAdapter mBluetoothAdapter = null;
    private BluetoothGattServer mBluetoothGattServer = null;
    private BluetoothGattCharacteristic mPruAlertChar = null;
    private BluetoothDevice mDevice = null;
    private PowerManager.WakeLock mWakeLock = null;

    private static boolean sDebug = false;

    // Advertising variables
    private static final int START_ADVERTISING = 1;
    private static final int STOP_ADVERTISING = 0;

    private static final UUID A4WP_SERVICE_UUID = UUID.fromString("6455fffe-a146-11e2-9e96-0800200c9a67");
    //PRU writes
    private static final UUID A4WP_PRU_CTRL_UUID = UUID.fromString("6455e670-a146-11e2-9e96-0800200c9a67");
    private static final UUID A4WP_PTU_STATIC_UUID = UUID.fromString("6455e671-a146-11e2-9e96-0800200c9a67");
    //PRU reads
    private static final UUID A4WP_PRU_ALERT_UUID = UUID.fromString("6455e672-a146-11e2-9e96-0800200c9a67");
    private static final UUID A4WP_PRU_STATIC_UUID = UUID.fromString("6455e673-a146-11e2-9e96-0800200c9a67");
    private static final UUID A4WP_PRU_DYNAMIC_UUID = UUID.fromString("6455e674-a146-11e2-9e96-0800200c9a67");

    private static final UUID A4WP_PRU_ALERT_DESC_UUID = UUID.fromString("64552902-a146-11e2-9e96-0800200c9a67");

    private static final Object mLock = new Object();
    private int mState = BluetoothProfile.STATE_DISCONNECTED;

    private static final short DEFAULT_FIELDS = 0x0000;
    private static final short DEFAULT_PROTOCOL_REV = 0x0001;
    private static final short DEFAULT_RFU = 0x0000;
    //03 if MTP, Has to be 04 for fluid.
    private static final byte DEFAULT_CATEGORY = 0x0003;
    private static final byte DEFAULT_CAPABILITIES = 0x00;
    private static final byte DEFAULT_HW_VERSION = 0x0007;
    private static final byte DEFAULT_FW_VERSION = 0x0006;
    private static final byte DEFAULT_MAX_POWER_DESIRED = 0x0032;  // 5Watts
    private static final short DEFAULT_VRECT_MIN = 7100;       // 7.1 Volts
    private static final short DEFAULT_VRECT_MAX = 19300;       // 19.3 Volts
    private static final short DEFAULT_VRECT_SET = 7200;       // 7.2 Volts
    private static final short DEFAULT_DELTA_R1 = 0x0001;
    private static final int DEFAULT_RFU_VAL = 0x0000;
    private static final int MSB_MASK = 0xFF00;
    private static final int LSB_MASK= 0x00FF;
    // On charge port disbaled need to be set to 10.2 Volts
    private static final short VRECT_MIN_CHG_DISABLED = 10200;
    // Populate Vrectmin, Vrectmax, Vrectset and temperature in the optional fields
    private static final byte OPTIONAL_FIELD_MASK = 0x3C;

    //Timeout value set to 5Sec which enures we advertise in limited mode
    private static final int WIPOWER_ADV_TIMEOUT = 5000;

    //PRU Write param length for validation
    private static final byte A4WP_PTU_STATIC_LENGTH = 0x11;
    private static final byte A4WP_PRU_CTRL_LENGTH = 0x05;
    private static final byte CCCD_LENGTH = 0x02; // Client Characteristic Configuration Declaration length

    //Advertisement interval values.
    private static final byte A4WP_ADV_MIN_INTERVAL = 0x20;
    private static final byte A4WP_ADV_MAX_INTERVAL = 0x20;

    //mask bits for charge port and irect validations
    private static final int CHARGE_PORT_MASK = 0x02;
    private static final int IRECT_MASK_MSB = 0x00;
    private static final int IRECT_MASK_LSB = 0x15;
    private static final int VRECT_MASK = 0x00;
    private static short VRECT_DYN;

    //Indices definitions
    private static final int OPTIONAL_FIELDS = 0;
    private static final int VRECT_LSB = 1;
    private static final int VRECT_MSB = 2;
    private static final int IRECT_LSB = 3;
    private static final int IRECT_MSB = 4;
    private static final int VRECT_MIN_LSB = 10;
    private static final int VRECT_MIN_MSB = 11;
    private static final int VRECT_SET_LSB = 12;
    private static final int VRECT_SET_MSB = 13;
    private static final int VRECT_MAX_LSB = 14;
    private static final int VRECT_MAX_MSB = 15;
    private static final int PRU_ALERT = 16;

    /*
     * PRU ALERT fields in BSS 1.3 are defined as follows
     * Bit 7    6    5    4          3              2    1            0
     *     OVP, OCP, OTP, Self_prot, Charge Complt, USB, Charge Port, PowerSharing Resp
     */
    private static final byte PRU_ALERT_OVER_VOLTAGE_DETECT_BIT = (byte) 0x80;
    private static final byte PRU_ALERT_OVER_CURRENT_DETECT_BIT = 0x40;
    private static final byte PRU_ALERT_OVER_TEMPERATURE_DETECT_BIT = 0x20;
    private static final byte PRU_ALERT_SELF_PROTECT_DETECT_BIT = 0x10;
    private static final byte PRU_ALERT_CHARGE_COMPLETE_BIT = 0x08;
    private static final byte PRU_ALERT_WIRE_CHARGER_DETECT_BIT = 0x04;
    private static final byte PRU_ALERT_CHARGE_PORT_ENABLED_BIT = 0x02;
    private static final byte PRU_ALERT_PWR_SHARE_RESP_BIT = 0x01;

    private static boolean mWipowerBoot = false;
    private static boolean isChargePortSet = false;
    static boolean mChargeComplete = false;
    static boolean mUSBCharging = false;
    static boolean mOverTemperature = false;
    static boolean mOutputControl = false;
    private static boolean mDiscInitiated = false; // If TRUE, means A4WP app has initiated the disconnection with PTU
    private static boolean mEnablePruAlerts = false;  // Are PRU Alerts enabled

    private AdvertiseSettings mAdvertiseSettings;
    private AdvertiseData mAdvertisementData;
    private BluetoothLeAdvertiser mAdvertiser;
    private AdvertiseCallback mAdvertiseCallback = new myAdvertiseCallback(1);
    ParcelUuid uuid1 = ParcelUuid.fromString("6455fffe-a146-11e2-9e96-0800200c9a67");

    /*a> Due to bad coupling irect value drops to zero and vrect remains
      constant would render stark to reset the CHG_OK pin, So as to
      set this pin on coupling being recovered host delivers the charge
      enable command to set the CHG_OK pin.
      b> Charge port needs to be enabled only if the vrect value is greater
      than the Vmin values */
    private void chkDynParamsAndStartCharging(byte[] value)
    {
        VRECT_DYN = 0x00;
        if (!isChargePortSet) {
            VRECT_DYN = (short)toUnsigned(value[VRECT_LSB]);
            VRECT_DYN |= (short)(toUnsigned(value[VRECT_MSB]) << 8);
            if (VRECT_MIN_CHG_DISABLED <= VRECT_DYN && mOutputControl) {
                mWipowerManager.startCharging();
                isChargePortSet = true;
            }
        }
        if (isChargePortSet) {
            if ((byte)(value[PRU_ALERT] & CHARGE_PORT_MASK) == CHARGE_PORT_MASK) {
                if ((value[IRECT_LSB] <= IRECT_MASK_LSB && value[IRECT_MSB] == IRECT_MASK_MSB)
                     && (value[VRECT_LSB] > VRECT_MASK || value[VRECT_MSB] > VRECT_MASK)) {
                     mWipowerManager.startCharging();
                }
            }
        }
    }

    private synchronized void initiateDisconnection() {
        if (sDebug) Log.v(LOGTAG, "initiateDisconnection:" + " mDiscInitiated:" + mDiscInitiated + " mState:" + mState);
        if ((mDiscInitiated == false) && (mState == BluetoothProfile.STATE_CONNECTED))
        {
            if (mBluetoothGattServer != null && mDevice != null) {
                if (sDebug) Log.v(LOGTAG, "initiateDisconnection:" + " dropping Connection");
                mDiscInitiated = true;
                mBluetoothGattServer.cancelConnection(mDevice);
                mWipowerManager.enablePowerApply(false, false, false);
                if ((mChargeComplete == true) || (mUSBCharging == true)) {
                    /*
                     * If charge is complete or USB wire charging is in progress, then MTP
                     * needs to connect after 600 ms of continuous PTU power.
                     */
                    mWipowerManager.enablePowerApply(true, true, true);
                } else {
                    /*
                     * Device needs to be charged, so start advertising after 30 ms detecting PTU power.
                     * Connect on the long beacon.
                     */
                    mWipowerManager.enablePowerApply(true, true, false);
                }
            }
        }
        return;
    }

    private WbcManager.WbcEventListener mWbcCallback = new WbcManager.WbcEventListener() {

        @Override
        public void onWbcEventUpdate(int what, int arg1, int arg2) {
            Log.v(LOGTAG, "onWbcEventUpdate rcvd: " + what + ", " + arg1 + ", " + arg2);

            switch (what) {
                case WbcTypes.WBC_EVENT_TYPE_CHARGE_COMPLETE:
                    if (arg1 == WbcTypes.WBC_BATTERY_STATUS_CHARGE_COMPLETE) {
                        // this will set charge complete bit in pru alert
                        // eventally leading to a possible disconnect from ptu
                        mChargeComplete = true;
                        if (mPruAlert != null) {
                            byte  alert = 0;
                            alert = (byte) (alert | PRU_ALERT_CHARGE_COMPLETE_BIT);
                            mPruAlert.sendPruAlert(alert);
                        }
                        mWipowerManager.enablePowerApply(false, false, false);
                        mWipowerManager.enablePowerApply(true, true, true);
                    } else {
                        mChargeComplete = false;
                        if ((mUSBCharging == false) &&  (mState == BluetoothProfile.STATE_DISCONNECTED)) {
                            mWipowerManager.enablePowerApply(false, false, false);
                            mWipowerManager.enablePowerApply(true, true, false);
                        }
                    }
                    break;
                case WbcTypes.WBC_EVENT_TYPE_PTU_PRESENCE_STATUS:
                    if (arg1 == WbcTypes.WBC_PTU_STATUS_NOT_PRESENT && mState == BluetoothProfile.STATE_CONNECTED)
                        initiateDisconnection();
                    break;
                case WbcTypes.WBC_EVENT_TYPE_USB_CHARGING_PRESENT:
                    if (arg1 == WbcTypes.WBC_USB_CHARGING_PRESENT) {
                        mUSBCharging = true;
                        // Set the wire charger bit here and send alert.
                        if (mPruAlert != null) {
                            byte alert = 0;
                            alert = (byte) (alert | PRU_ALERT_WIRE_CHARGER_DETECT_BIT);
                            mPruAlert.sendPruAlert(alert);
                        }
                        mWipowerManager.enablePowerApply(false, false, false);
                        mWipowerManager.enablePowerApply(true, true, true);
                    } else {
                        mUSBCharging = false;
                        if ((mChargeComplete == false) &&  (mState == BluetoothProfile.STATE_DISCONNECTED)) {
                            mWipowerManager.enablePowerApply(false, false, false);
                            mWipowerManager.enablePowerApply(true, true, false);
                        }
                    }
                    break;
                case WbcTypes.WBC_EVENT_TYPE_BATTERY_OVERHEAT:
                    mWipowerManager.enablePowerApply(false, false, false);
                    // Set the over-temperature bit here and send alert.
                    if (arg1 == WbcTypes.WBC_BATTERY_OVERHEAT) {
                        mOverTemperature = true;
                        if (mPruAlert != null) {
                            byte  alert = 0;
                            alert = (byte) (alert | PRU_ALERT_OVER_TEMPERATURE_DETECT_BIT);
                            mPruAlert.sendPruAlert(alert);
                        }
                    }
                    else {
                        mOverTemperature = false;
                        if ((mChargeComplete == false) &&  mUSBCharging == false && (mState == BluetoothProfile.STATE_DISCONNECTED)) {
                            mWipowerManager.enablePowerApply(false, false, false);
                            mWipowerManager.enablePowerApply(true, true, false);
                        }
                    }
                    break;
                case WbcTypes.WBC_EVENT_TYPE_WIPOWER_CHARGING_ACTIVE_STATUS:
                    // Do nothing.
                    break;
                default:
                    Log.v(LOGTAG, "onWbcEventUpdate: Unrecognized event received");
                    break;
            }
            if (sDebug) Log.v(LOGTAG, "onWbcEventUpdate: charge complete " +  mChargeComplete);
            if (sDebug) Log.v(LOGTAG, "onWbcEventUpdate: USB charging " +  mUSBCharging);
        }
    };


    private  void acquire_wake_lock(boolean wake) {
        if (wake == true) {
            if (mWakeLock == null) {
                PowerManager pm = (PowerManager)getSystemService(
                Context.POWER_SERVICE);
                if (pm == null) {
                    Log.e(LOGTAG, "failed to get PM");
                    return;
                }
                mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,
                    "StartingWipowerConnection");
                mWakeLock.setReferenceCounted(false);
                mWakeLock.acquire();
                Log.w(LOGTAG, "Acquire Wake Lock");
            } else {
                Log.w(LOGTAG, "Wake Lock already held");
            }
        } else {
            if (mWakeLock != null) {
                mWakeLock.release();
                mWakeLock = null;
                Log.w(LOGTAG, "Release Wake Lock");
            }
       }
    }

    private class PruStaticParam {
        private byte mOptvalidity;
        private byte mProtoRevision;
        private byte  mRfu;
        private byte mCategory;
        private byte mCapabilities;
        private byte mHwRev;
        private byte mFwRev;
        private byte mMaxPowerDesired;
        private short mVrectMinStatic;
        private short mVrectMaxStatic;
        private short mVrectSet;
        private short mDeltaR1;
        private int mRfuVal;

        public PruStaticParam() {
            mOptvalidity = (byte)DEFAULT_FIELDS;
            mProtoRevision = (byte)DEFAULT_PROTOCOL_REV;
            mRfu = (byte)DEFAULT_RFU;
            mCategory = (byte)DEFAULT_CATEGORY;
            mCapabilities = (byte)DEFAULT_CAPABILITIES;
            mHwRev = (byte)DEFAULT_HW_VERSION;
            mFwRev = (byte)DEFAULT_FW_VERSION;
            mMaxPowerDesired = (byte)DEFAULT_MAX_POWER_DESIRED;
            mVrectMinStatic = (short)DEFAULT_VRECT_MIN;
            mVrectMaxStatic = (short)DEFAULT_VRECT_MAX;
            mVrectSet = (short)DEFAULT_VRECT_SET;
            mDeltaR1 = (short)DEFAULT_DELTA_R1;
            mRfuVal = (int)DEFAULT_RFU_VAL;
            Log.v(LOGTAG, "PruStaticParam initialized");
        }

        public byte[] getValue() {
            byte[] res = new byte[20];
            res[0] = mOptvalidity;
            res[1] = mProtoRevision;
            res[2] = mRfu;
            res[3] = mCategory;
            res[4] = mCapabilities;
            res[5] = mHwRev;
            res[6] = mFwRev;
            res[7] = mMaxPowerDesired;
            res[8] =  (byte)(LSB_MASK & mVrectMinStatic);
            res[9] = (byte)((MSB_MASK & mVrectMinStatic) >> 8);
            res[10] =  (byte)(LSB_MASK & mVrectMaxStatic);
            res[11] = (byte)((MSB_MASK & mVrectMaxStatic) >> 8);
            res[12] =  (byte)(LSB_MASK & mVrectSet);
            res[13] = (byte)((MSB_MASK & mVrectSet) >> 8);
            res[14] =  (byte)(LSB_MASK & mDeltaR1);
            res[15] = (byte)((MSB_MASK & mDeltaR1) >> 8);
            res[16] =  (byte)(LSB_MASK & mRfuVal);
            res[17] = (byte)((MSB_MASK & mRfuVal) >> 8);
            res[18] =  (byte)((0xFF0000 & mRfuVal) >> 16);
            res[19] = (byte)((0xFF000000 & mRfuVal) >> 24);

            return res;
        }

        /*This is used to set the charging values*/
        public void setValue(byte[] value) {
            mOptvalidity = value[0];
            mProtoRevision = value[1];
            mRfu = value[2];
            mCategory = value[3] ;
            mCapabilities = value[4];
            mHwRev = value[5];
            mFwRev = value[6];
            mMaxPowerDesired = value[7];
            mVrectMinStatic = value[8];
            mVrectMinStatic |= (short)(value[9] << 8);
            mVrectMinStatic = value[10];
            mVrectMinStatic |= (short)(value[11] << 8);
            mVrectSet = value[12];
            mVrectSet |= (short)(value[13] << 8);
            mDeltaR1 = value[14];
            mDeltaR1 |= (short)(value[15] << 8);
            mRfuVal = value[16];
            mRfuVal |= (int)(value[17] << 8);
            mRfuVal |= (int)(value[18] << 16);
            mRfuVal |= (int)(value[19] << 24);

            return;
        }

    }

    private class PruAlert {
        private final int PRU_ALERT_NOTIFY_BIT = 0x0100; // Notify bit in CCCD
        private byte mAlert;

        public PruAlert(byte value) {
            mAlert = value;
            mEnablePruAlerts = false;
        }

        public void setValue(byte value) {
            mAlert = value;
        }

        public byte[] getValue() {
            byte[] res = new byte[1];
            res[0] = mAlert;
            return res;
        }

        // Handle the CCCD Write for Notifications/Indications from PTU
        private int processPruAlertRequest(byte[] value) {
            int   status  = 0;
            int intValue= 0;
            intValue = ((value[0]<< 8) & 0x0000ff00) | ((value[1] << 0) & 0x000000ff);

            if (sDebug) Log.v(LOGTAG, "processPruAlertRequest. Value: " + intValue);

            if ((intValue & PRU_ALERT_NOTIFY_BIT) == PRU_ALERT_NOTIFY_BIT) {
                Log.v(LOGTAG, "processPruAlertRequest. PRU Alerts Enabled");
                mEnablePruAlerts  = true;
                mWipowerManager.enableAlertNotification(true);
            } else {
                mWipowerManager.enableAlertNotification(false);
                mEnablePruAlerts  = false;
            }

            return status;
        } // end of processPruAlertRequest

        // Send Notification/Indications to PTU
        private int sendPruAlert(byte alertValue) {
            int status  = 0;
            byte[] alertVal = {0};

            Log.v(LOGTAG, "sendPruAlert. Value: " + alertValue);

            if (mEnablePruAlerts == false)
            {
                Log.v(LOGTAG, "sendPruAlert. PRU Alerts are Disabled");
                return status;
            }

            if (mPruAlertChar == null)
            {
                Log.v(LOGTAG, "sendPruAlert. Alert characteristic is NULL");
                return status;
            }

            if (alertValue == 0)
            {
                Log.v(LOGTAG, "sendPruAlert. No alerts to send");
                return status;
            }

            if (mDevice == null)
            {
                Log.v(LOGTAG, "sendPruAlert. mDevice is NULL");
                return status;
            }

            if (mState != BluetoothProfile.STATE_CONNECTED)
            {
                Log.v(LOGTAG, "sendPruAlert. Not CONNECTED");
                return status;
            }


            alertVal[0] = alertValue;
            mPruAlertChar.setValue(alertVal);
            mBluetoothGattServer.notifyCharacteristicChanged(mDevice,
                mPruAlertChar, false);

            return status;
        } // end of sendPruAlert

    }

    private class PtuStaticParam {
        private byte mOptValidity;
        private byte mPower;
        private byte mMaxSrcImpedence;
        private byte mMaxLoadResistance;
        private short mId;
        private byte mClass;
        private byte mHwRev;
        private byte mFwRev;
        private byte mProtocolRev;
        private byte mMaxDevicesSupported;
        private int mReserved1;
        private short mReserved2;

        public PtuStaticParam(byte[] value) {
            mOptValidity = value[0];
            mPower = value[1];
            mMaxSrcImpedence = value[2];
            mMaxLoadResistance = value[3];
            mId = (short)(value[4] & 0xff);
            mId |= (short)((value[5] & 0xff) << 8);
            mClass = value[6];
            mHwRev = value[7];
            mFwRev = value[8];
            mProtocolRev = value[9];
            mMaxDevicesSupported = value[10];
            mReserved1 = (int)(value[11] & 0xff);
            mReserved1 |= (int)((value[12] & 0xff) << 8);
            mReserved1 |= (int)((value[13] & 0xff) << 16);
            mReserved1 |= (int)((value[14] & 0xff) << 16);
            mReserved2 = (short)(value[15] & 0xff);
            mReserved2 |= (short)((value[16] & 0xff) << 8);
        }

        public void print() {
            Log.v(LOGTAG, "mOptValidity" +  toHex(mOptValidity) +  "mPower" +  toHex(mPower) + "mMaxSrcImpedence" +  toHex(mMaxSrcImpedence) + "mMaxLoadResistance" +  toHex(mMaxLoadResistance));
            Log.v(LOGTAG, "mId" +  toHex(mId) + "mClass" +  toHex(mClass) + "mHwRev" +  toHex(mHwRev) +  "mFwRev" +  toHex(mFwRev));
            Log.v(LOGTAG, "mProtocolRev" +  toHex(mProtocolRev) + "mMaxDevicesSupported" +  toHex(mMaxDevicesSupported) + "mReserved1" +  toHex(mReserved1) + "mReserved2" +  toHex(mReserved2));
        }

        public double getPower() {
            double val = ((mPower&0xfc)>>2);
            val = 0.5*(val+1);
            if (sDebug) Log.v(LOGTAG, "getPower<=" + val);
            if (val > 22) val = 22.0;
            return val;
        }

        public double getMaxSrcImpedence() {
            double val = ((mMaxSrcImpedence&0xf8)>>3);
            val = 50 + (val*10);
            if (sDebug) Log.v(LOGTAG, "getSrcImpedence<=" + val);
            if (val > 375) val = 375.0;
            return val;
        }

        public double getMaxLoadResistance() {
            double val = ((mMaxLoadResistance&0xf8)>>3);
            val = 5 * (val+1);
            if (sDebug) Log.v(LOGTAG, "getMaxLoadResistance<=" + val);
            if (val > 55) val = 55.0;
            return val;
        }

        public float getMaxDevicesSupported() {
            int val = mMaxDevicesSupported +1;
            if (sDebug) Log.v(LOGTAG, "getMaxDevicesSupported<=" + val);
            if (val > 8) val = 8;
            return val;
        }

        public short getId() {
            return mId;
        }

        public int getPtuClass() {
            return (mClass > 4) ? 5 : (mClass+1);
        }

        public byte getHwRev () {
            return mHwRev;
        }

        public byte getFwRev () {
            return mFwRev;
        }

        public byte getProtocolRev () {
            return mProtocolRev;
        }
    }

    public static String toHex(int num) {
        return String.format("0x%8s", Integer.toHexString(num)).replace(' ', '0');
    }

    private class PruControl {
         public byte mEnable;
         public byte mPermission;
         public byte mTimeSet;
         public short mReserved;
         public PruControl () {
             mEnable = 0x0;
             mPermission = 0x0;
             mTimeSet = 0x0;
             mReserved = 0x0;
             mReserved = 0x0;
         }

         public void print() {
             Log.v(LOGTAG, "mEnable: " +  toHex(mEnable));
             Log.v(LOGTAG, "mPermission: " +  toHex(mPermission));
             Log.v(LOGTAG, "mTimeSet: " +  toHex(mTimeSet));
             Log.v(LOGTAG, "mReserved: " +  toHex(mReserved));
         }

         public void setValue(byte[] value) {
             mEnable = (byte)value[0];
             mPermission = (byte)value[1];
             mTimeSet = (byte)value[2];
             mReserved = (short)(value[3] & 0xFF);
             mReserved = (short)((value[4] & 0xFF) << 8);
             return;
         }

         public byte[] getValue() {
             byte[] res = new byte[5];
             res[0] = mEnable;
             res[1] = mPermission;
             res[2] = mTimeSet;
             res[3] = (byte)(LSB_MASK & mReserved);
             res[4] = (byte)(MSB_MASK & mReserved);;
             return res;
         }

         public boolean getEnablePruOutput() {
              if ((mEnable&0x80) == 0x80) return true;
              else return false;
         }

         public boolean getEnableCharger() {
              if ((mEnable&0x40) == 0x40) return true;
              else return false;
         }

         /* returns 0 Maximum power
                    1 66%
                    2 33%
          */
         public PowerLevel getReducePower() {
             PowerLevel res = PowerLevel.POWER_LEVEL_MINIMUM;
             int val = ((mEnable & 0x30) >> 4 );
             if (val == 0) {
                 res = PowerLevel.POWER_LEVEL_MAXIMUM;
             } else if (val == 1 && val == 3) {
                 res = PowerLevel.POWER_LEVEL_MEDIUM;
             } else if (val == 2) {
                 res = PowerLevel.POWER_LEVEL_MINIMUM;
             }
             return res;
         }

         /* returns 0x00 permitted without reason
                    0x01 Permitted with waiting time due to limited affordable power
                    0x80 Denied with system error 3
                    0x81 Denied due to limited affordable power
                    0x82 Denied due to limited PTU Number of Devices
                    0x83 Denied due to limited PTU Class support
                    All other values: RFU
          */
         public boolean getPermission() {

             if (sDebug) Log.v(LOGTAG, "getPermission" + mPermission);
             if (mPermission == 0x00) return true;
             else return false;
         }

         /* returns time in ms */
         public int getSetTime() {
             return (mTimeSet*10);
         }
    };

    private PruAlert mPruAlert;
    private PruControl mPruControl;
    private PruStaticParam mPruStaticParam; //20 bytes
    private PtuStaticParam mPtuStaticParam; //20 bytes
    private static WipowerDynamicParam mPruDynamicParam; //20 bytes
    private WipowerManager mWipowerManager;
    private WbcManager mWbcManager;

    public A4wpService() {
        Log.v(LOGTAG, "A4wpService");
    }

    static private void cleanupService() {
        Log.v(LOGTAG, "cleanupService");
    }

    private int processPruControl(byte[] value) {
        int status = 0;

        Log.v(LOGTAG, "processPruControl>");
        if (value != null) {
            mPruControl.setValue(value);
        } else {
            Log.e(LOGTAG, "control value is null");
            return status;
        }
        mPruControl.print();

        if (mWipowerManager == null) {
            Log.e(LOGTAG, "mWipowerManager is null");
            return status;
        }

        if (mPruControl.getEnablePruOutput() && mPruControl.getPermission())  {
            Log.v(LOGTAG, "do Enable PruOutPut");
            /* Wake lock is enabled by default, to disbale need to set property */
            if(SystemProperties.getBoolean("persist.a4wp.skipwakelock", false) == false) {
                /* Hold wake lock during connection */
                acquire_wake_lock(true);
            }
            mOutputControl = true;
        } else {
            Log.v(LOGTAG, "do Disable PruOutPut");
            mWipowerManager.stopCharging();
            isChargePortSet = false;
            mOutputControl = false;
            return status;
        }

        if (mPruControl.getEnableCharger()) {
            Log.v(LOGTAG, "do Enable Charging");
        } else {
            Log.v(LOGTAG, "do Disable Charging");
        }

        PowerLevel val = mPruControl.getReducePower();
        if (val == PowerLevel.POWER_LEVEL_MAXIMUM) {
            Log.v(LOGTAG, "put to Max Power");
        } else if (val == PowerLevel.POWER_LEVEL_MEDIUM){
            Log.v(LOGTAG, "put to Medium Power");
        } else if (val == PowerLevel.POWER_LEVEL_MINIMUM){
            Log.v(LOGTAG, "put to Min Power");
        }

        mWipowerManager.setPowerLevel(val);

        return status;
    }

    private int processPtuStaticParam(byte[] value) {
        int status = 0;
        if (sDebug) Log.v(LOGTAG, "processPtuStaticParam>");
        mPtuStaticParam = new PtuStaticParam(value);
        mPtuStaticParam.print();

        return status;
    }

    /**
     * Wipower callbacks
     */
    private final WipowerManagerCallback mWipowerCallback = new WipowerManagerCallback() {

        @Override
        public void onWipowerReady() {
            Log.v(LOGTAG, "onWipowerReady");
            mWipowerManager.enablePowerApply(false, false, false);
            if ((mChargeComplete == true) || (mUSBCharging == true)) {
                mWipowerManager.enablePowerApply(true, true, true);
            } else {
                mWipowerManager.enablePowerApply(true, true, false);
            }
            mWipowerBoot = true;
        }

        @Override
        public void onWipowerStateChange(WipowerState state) {
            Log.v(LOGTAG, "onWipowerStateChange" + state);
        }

        @Override
        public void onPowerApply(PowerApplyEvent state) {
            Log.v(LOGTAG, "onPowerApply:" + state);

            if (state == PowerApplyEvent.OFF) {
                initiateDisconnection();
            }
        }

        @Override
        public void onWipowerAlert(byte alert) {
            if (sDebug) Log.v(LOGTAG, "onWipowerAlert: " + alert + " alert recieved");
            mPruAlert.sendPruAlert(alert);
        }


        @Override
        public void onWipowerData(WipowerDynamicParam data) {
            if (sDebug) Log.v(LOGTAG, "onWipowerData Alert");
            byte[] value = data.getValue();
            chkDynParamsAndStartCharging(value);
            if (sDebug) Log.v(LOGTAG, "calling SetValue");
            mPruDynamicParam.setValue(value);
        }

    };

    public static short toUnsigned(byte b) {
        return (short)(b & 0xff);
    }

    /**
     * GATT callbacks
     */
    private final BluetoothGattServerCallback mGattCallbacks = new BluetoothGattServerCallback() {
        @Override
        public void onConnectionStateChange(BluetoothDevice device, int status, int newState) {
            WipowerState state = WipowerState.OFF;
            if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                if (mWipowerManager != null  && device.equals(mDevice)) {
                    Log.v(LOGTAG, "onConnectionStateChange:DISCONNECTED PrevState:" + " Device:" + device + " ChargeComplete:" + mChargeComplete);
                    mState = newState;
                    mDiscInitiated = false;
                    mOutputControl = false;
                    mWipowerManager.enableDataNotification(false);
                    mWipowerManager.enableAlertNotification(false);
                    mEnablePruAlerts  = false;
                    mWipowerManager.stopCharging();
                    mWipowerManager.enablePowerApply(false, false, false); // Always reset the FW state machine
                    if ((mChargeComplete == true) || (mUSBCharging == true)) {
                        mWipowerManager.enablePowerApply(true, true, true);

                    } else {
                        mWipowerManager.enablePowerApply(true, true, false);

                    }
                    if(SystemProperties.getBoolean("persist.a4wp.skipwakelock", false) == false) {
                        /* Drop wake lock once the connection is dropped gracefully */
                        acquire_wake_lock(false);
                    }
                    mDevice = null;
                }
                isChargePortSet = false;
                mPruDynamicParam.resetValues();
            } else if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.v(LOGTAG, "onConnectionStateChange:CONNECTED");
            }
        }

        @Override
        public void onCharacteristicWriteRequest(BluetoothDevice device, int requestId,
                BluetoothGattCharacteristic characteristic,
                boolean preparedWrite, boolean responseNeeded,
                int offset, byte[] value) {

                UUID id = characteristic.getUuid();
                int status =0;

                Log.v(LOGTAG, "onCharacteristicWriteRequest:" + id);
                if (id == A4WP_PRU_CTRL_UUID && value.length == A4WP_PRU_CTRL_LENGTH)
                {
                     status = processPruControl(value);
                }
                else if(id == A4WP_PTU_STATIC_UUID && value.length == A4WP_PTU_STATIC_LENGTH)
                {
                     status = processPtuStaticParam(value);
                }

                if (responseNeeded == true) {
                    mBluetoothGattServer.sendResponse(device, requestId, status,
                                       offset, value);
                }
        }

        @Override
        public void onDescriptorReadRequest(BluetoothDevice device, int requestId,
                        int offset, BluetoothGattDescriptor descriptor) {

                UUID id = descriptor.getUuid();
                byte[] value = {0};
                int status = 0;

                Log.v(LOGTAG, "onDescriptorReadRequest() - descriptor" + id);
                if (id == A4WP_PRU_ALERT_DESC_UUID)
                {
                    value = mPruAlert.getValue();
                }


                if (value != null)
                {
                     if (sDebug) Log.v(LOGTAG, "device=" + id + "requestId=" + requestId + "status=" + status + "offset=" + offset + "value=" + value[0]);
                     mBluetoothGattServer.sendResponse(device, requestId, status, offset, value);
                }
        }

        @Override
        public void onDescriptorWriteRequest(BluetoothDevice device, int requestId,
                BluetoothGattDescriptor descriptor, boolean preparedWrite,
                boolean responseNeeded, int offset,  byte[] value) {

                int status = 0;
                UUID id = descriptor.getUuid();
                Log.v(LOGTAG, "onDescriptorWriteRequest() - descriptor" + id);
                if ((id == A4WP_PRU_ALERT_DESC_UUID) && (value.length == CCCD_LENGTH))
                {
                    mDevice = device; // save the device as Notifications may need to be generated anytime now
                    status = mPruAlert.processPruAlertRequest(value);
                } else
                {
                    Log.v(LOGTAG, "onDescriptorWriteRequest() - Invalid descriptor: " + id + " OR length: " + value.length);
                }

                if (responseNeeded == true)
                    mBluetoothGattServer.sendResponse(device, requestId, status,
                                       offset, value);
        }

        @Override
        public void onCharacteristicReadRequest(BluetoothDevice device, int requestId,
                        int offset, BluetoothGattCharacteristic characteristic) {

                UUID id = characteristic.getUuid();
                byte[] value = {0};
                int status = 0;

                Log.v(LOGTAG, "onCharacteristicReadRequest:" + id);
                if(id == A4WP_PRU_STATIC_UUID && mState == BluetoothProfile.STATE_DISCONNECTED)
                {
                    mWipowerManager.enablePowerApply(false, false, false);
                    value = mPruStaticParam.getValue();
                    mDevice = device;
                    mState = BluetoothProfile.STATE_CONNECTED;
                    /* intiate a server connect so that gatt will maintain
                    ** this as a vote in its existing  LE connections.*/
                    mBluetoothGattServer.connect(mDevice, false);
                    mWipowerManager.enableDataNotification(true);
                }
                else if (id == A4WP_PRU_DYNAMIC_UUID) {
                    if (mPruDynamicParam == null) {
                         Log.e(LOGTAG, "mPruDynamicParam is NULL");
                         return;
                    }
                    value = mPruDynamicParam.getValue();

                    /*
                     * TODO: Populate all PRU ALERT fields in the PRU dynamic parameter. Populate
                     * charge complete, Wire charge detect, Over Temp Protection on WBC feedback.
                     */
                    if (mChargeComplete == true) {
                        value[PRU_ALERT] = (byte)(value[PRU_ALERT] | PRU_ALERT_CHARGE_COMPLETE_BIT);
                    } else {
                        value[PRU_ALERT] = (byte)(value[PRU_ALERT] & (~PRU_ALERT_CHARGE_COMPLETE_BIT));
                    }

                    if (mUSBCharging == true) {
                        value[PRU_ALERT] = (byte)(value[PRU_ALERT] | PRU_ALERT_WIRE_CHARGER_DETECT_BIT);
                    } else {
                        value[PRU_ALERT] = (byte)(value[PRU_ALERT] & (~PRU_ALERT_WIRE_CHARGER_DETECT_BIT));
                    }

                    if (mOverTemperature == true) {
                        value[PRU_ALERT] = (byte)(value[PRU_ALERT] | PRU_ALERT_OVER_TEMPERATURE_DETECT_BIT);
                    } else {
                        value[PRU_ALERT] = (byte)(value[PRU_ALERT] & (~PRU_ALERT_OVER_TEMPERATURE_DETECT_BIT));
                    }

                    value[OPTIONAL_FIELDS] = (byte)OPTIONAL_FIELD_MASK;
                    value[VRECT_MAX_LSB] = (byte)(LSB_MASK & DEFAULT_VRECT_MAX);
                    value[VRECT_MAX_MSB] = (byte)((MSB_MASK & DEFAULT_VRECT_MAX) >> 8);
                    if ((byte)(value[PRU_ALERT] & CHARGE_PORT_MASK) == CHARGE_PORT_MASK) {
                        value[VRECT_MIN_LSB] = (byte)(LSB_MASK & DEFAULT_VRECT_MIN);
                        value[VRECT_MIN_MSB] = (byte)((MSB_MASK & DEFAULT_VRECT_MIN) >> 8);
                        value[VRECT_SET_LSB] = (byte)(LSB_MASK & DEFAULT_VRECT_SET);
                        value[VRECT_SET_MSB] = (byte)((MSB_MASK & DEFAULT_VRECT_SET) >> 8);
                    } else {
                        value[VRECT_MIN_LSB] = (byte)(LSB_MASK & VRECT_MIN_CHG_DISABLED);
                        value[VRECT_MIN_MSB] = (byte)((MSB_MASK & VRECT_MIN_CHG_DISABLED) >> 8);
                        value[VRECT_SET_LSB] = (byte)(LSB_MASK & VRECT_MIN_CHG_DISABLED);
                        value[VRECT_SET_MSB] = (byte)((MSB_MASK & VRECT_MIN_CHG_DISABLED) >> 8);
                    }
                }
                else if (id == A4WP_PRU_CTRL_UUID) {
                    if (mPruControl == null) {
                         Log.e(LOGTAG, "mPruControl is NULL");
                         return;
                    }
                   value = mPruControl.getValue();
                }
                if (mBluetoothGattServer != null && mDiscInitiated != true) {
                    mBluetoothGattServer.sendResponse(device, requestId, status, offset, value);
                }
        }

        @Override
        public void onServiceAdded(final int status, BluetoothGattService service) {
                Log.i(LOGTAG, "Service added");
        }
    };

    private void closeServer() {
        if (mBluetoothGattServer != null) {
            if (sDebug) Log.v(LOGTAG, "disconnect and closeServer");
            if (mDevice != null) mBluetoothGattServer.cancelConnection(mDevice);
            mBluetoothGattServer.close();
        }
    }

    private final class myAdvertiseCallback extends AdvertiseCallback {
        private int mIndex;

        myAdvertiseCallback(int index) {
            mIndex = index;
        }

        @Override
        public void onStartSuccess(AdvertiseSettings settingsInEffect) {
            Log.d(LOGTAG, "advertise success " + mIndex);
            if (mWipowerManager != null) {
                mWipowerManager.enablePowerApply(false, false, false);
                if ((mChargeComplete == true) || (mUSBCharging == true)) {
                    mWipowerManager.enablePowerApply(true, true, true);
                } else {
                    mWipowerManager.enablePowerApply(true, true, false);
                }
            }
        }

        @Override
        public void onStartFailure(int errorCode) {
            Log.d(LOGTAG, "advetise failure " + mIndex);
        }
    }


    private void StartAdvertising()
    {
        /* serviceData represnts service data for Wipower that needs
           to be part of advertising,
           0x28 i& 0x00 represents the primary based handle
           0xFF and 0x60 represents:
           ADV Flags are set to:  CAT3 PRU 21, Reboot bit and  OVP indicator
        */
        byte[] serviceData = new byte[] {
                0x28, 0x00, (byte)0xff, 0x60 };

        mAdvertiser = mBluetoothAdapter.getBluetoothLeAdvertiser();
        mAdvertisementData = new AdvertiseData.Builder()
            .addServiceData(uuid1, serviceData).build();

        mAdvertiseSettings = new AdvertiseSettings.Builder()
            .setAdvertiseMode(AdvertiseSettings.ADVERTISE_MODE_LOW_LATENCY)
            .setTxPowerLevel(AdvertiseSettings.ADVERTISE_TX_POWER_HIGH)
            .setConnectable(true).build();

        if (sDebug) Log.d(LOGTAG, " Calling mAdvertiser.startAdvertising");
        if(mAdvertiser != null)
            mAdvertiser.startAdvertising(mAdvertiseSettings, mAdvertisementData, mAdvertiseCallback);
        else
            Log.d(LOGTAG, " mAdvertiser is null");
    }

    private void stopAdvertising()
    {
       /* to be completed */
       if (mAdvertiseCallback != null &&  mAdvertiser != null) {
           if (sDebug) Log.d(LOGTAG, "stop advertising on service destroy");
           mAdvertiser.stopAdvertising(mAdvertiseCallback);
       }
    }

    private boolean startServer() {
        BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        if (bluetoothManager == null) return false;

        mBluetoothGattServer = bluetoothManager.openGattServer(this, mGattCallbacks, BluetoothDevice.TRANSPORT_LE);
        Log.d(LOGTAG,"calling start server......");
        if (mBluetoothGattServer == null) {
            Log.e(LOGTAG,"mBluetoothGattServer is NULL");
            return false;
        }

        BluetoothGattCharacteristic pruControl = new BluetoothGattCharacteristic(
                A4WP_PRU_CTRL_UUID,
                BluetoothGattCharacteristic.PROPERTY_WRITE | BluetoothGattCharacteristic.PROPERTY_READ,
                BluetoothGattCharacteristic.PERMISSION_WRITE |
                BluetoothGattCharacteristic.PERMISSION_READ);

        BluetoothGattCharacteristic ptuStatic = new BluetoothGattCharacteristic(
                A4WP_PTU_STATIC_UUID,
                BluetoothGattCharacteristic.PROPERTY_WRITE | BluetoothGattCharacteristic.PROPERTY_READ,
                BluetoothGattCharacteristic.PERMISSION_WRITE |
                BluetoothGattCharacteristic.PERMISSION_READ);

        mPruAlertChar = new BluetoothGattCharacteristic(
                A4WP_PRU_ALERT_UUID,
                BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_NOTIFY,
                BluetoothGattCharacteristic.PERMISSION_READ );

        BluetoothGattCharacteristic pruStatic = new BluetoothGattCharacteristic(
                A4WP_PRU_STATIC_UUID,
                BluetoothGattCharacteristic.PROPERTY_READ,
                BluetoothGattCharacteristic.PERMISSION_READ);

        BluetoothGattCharacteristic pruDynamic = new BluetoothGattCharacteristic(
                A4WP_PRU_DYNAMIC_UUID,
                BluetoothGattCharacteristic.PROPERTY_READ,
                BluetoothGattCharacteristic.PERMISSION_READ);


        BluetoothGattDescriptor pruAlertDesc = new BluetoothGattDescriptor(
                A4WP_PRU_ALERT_DESC_UUID,
                BluetoothGattCharacteristic.PERMISSION_READ |
                BluetoothGattCharacteristic.PERMISSION_WRITE);

        mPruAlertChar.addDescriptor(pruAlertDesc);

        BluetoothGattService a4wpService = new BluetoothGattService(
                A4WP_SERVICE_UUID, BluetoothGattService.SERVICE_TYPE_PRIMARY);

        a4wpService.addCharacteristic(pruControl);
        a4wpService.addCharacteristic(ptuStatic);
        a4wpService.addCharacteristic(mPruAlertChar);
        a4wpService.addCharacteristic(pruStatic);
        a4wpService.addCharacteristic(pruDynamic);


        mBluetoothGattServer.addService(a4wpService);
        Log.d(LOGTAG, "calling StartAdvertising");
        StartAdvertising();

        return true;
    }

    @Override
    public void onCreate() {
        Log.v(LOGTAG, "onCreate");
        super.onCreate();

        sDebug = SystemProperties.getBoolean("persist.a4wp.logging", false);

        // Ensure Bluetooth is enabled
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null || !mBluetoothAdapter.isLeEnabled()) {
            Log.d(LOGTAG, "Bluetooth is not available or enabled - exiting...");
            return;
        }

        if (sDebug) Log.v(LOGTAG, "calling startService");
        startServer();
        //Initialize PRU Static param
        mPruStaticParam = new PruStaticParam();
        mPruDynamicParam = new WipowerDynamicParam();
        mPruAlert = new PruAlert((byte)0);
        mPruControl = new PruControl();

        if (sDebug) Log.v(LOGTAG, "onCreate: charge complete " + mChargeComplete);

        /*
         * What happens when WiPowerManager or WbcManager is NULL? This is not being checked?
         * This should be checked for NULL to ensure the appropriate classes/services are created.
         * The system properties determine if the wipower is supported or not. If properties are
         * not set, then need to abort all WiPower functionality including this class.
         */
        mWipowerManager = WipowerManager.getWipowerManger(this, mWipowerCallback);
        if (mWipowerManager != null)
             mWipowerManager.registerCallback(mWipowerCallback);
        mWbcManager = WbcManager.getInstance();
        if (mWbcManager != null) {
            mChargeComplete = (mWbcManager.getChargeComplete() == 1);
            mUSBCharging = (mWbcManager.getUSBChargingPresent() == 1);
            Log.v(LOGTAG, "onCreate: charge complete " + mChargeComplete + " mUSBCharging " + mUSBCharging);
            mWbcManager.register(mWbcCallback);
        }
    }

    @Override
    public void onDestroy() {
        Log.v(LOGTAG, "onDestroy");
        stopAdvertising();
        closeServer();
        if (mWipowerManager != null) {
             mWipowerManager.enableDataNotification(false);
             mWipowerManager.enablePowerApply(false, false,false);
             mWipowerManager.unregisterCallback(mWipowerCallback);
        }
        if (mWbcManager != null)
             mWbcManager.unregister(mWbcCallback);
        if(SystemProperties.getBoolean("persist.a4wp.skipwakelock", false) == false) {
            //release wake lock during BT-OFF.
            acquire_wake_lock(false);
        }
        mOutputControl = false;
        isChargePortSet = false;
    }

    @Override
    public IBinder onBind(Intent in) {
        Log.v(LOGTAG, "onBind");
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(LOGTAG, "onStart Command called!!");

        //mWipowerBoot is used to hold power enable command till the service is been registered completely
        if (mWipowerBoot == true && mWipowerManager != null) {
            if ((mChargeComplete == true) || (mUSBCharging == true)) {
                mWipowerManager.enablePowerApply(true, true, true);
            } else {
                mWipowerManager.enablePowerApply(true, true, false);
            }
        }
        if(SystemProperties.getBoolean("persist.a4wp.skipwakelock", false) == false) {
            //release wake lock in case if held during crashes or on BT restart.
            acquire_wake_lock(false);
        }
        return START_STICKY;
   }
}
