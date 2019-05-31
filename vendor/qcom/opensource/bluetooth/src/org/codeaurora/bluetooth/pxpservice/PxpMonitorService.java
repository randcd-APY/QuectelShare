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

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.QBluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothRssiMonitorCallback;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.ParcelUuid;
import android.text.format.Time;
import android.util.Log;
import android.util.Pair;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.UUID;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;


/**
 * A Proximity Monitor Service. It interacts with the BLE device via the Android
 * BLE API.
 */

public class PxpMonitorService extends Service {

    private static final String TAG = PxpMonitorService.class.getSimpleName();
    private static final     boolean DBG = true;

    public static final String DEVICE_CONNECTED = "android.bluetooth.action.DEVICE_CONNECTED";

    public static final String DEVICE_DISCONNECTED = "android.bluetooth.action.DEVICE_DISCONNECTED";

    public static final String MISSING_LLS_SERVICE = "android.bluetooth.action.MISSING_LLS_SERVICE";

    public static final String LINKLOSS_ALERT = "android.bluetooth.action.LINKLOSS_ALERT";

    public static final String DEVICE_ALLERT = "action.DEVICE_ALLERT";

    public static final String PATHLOSS_ALLERT = "action.PATHLOSS_ALLERT";

    public static final String DEVICE_ALLERT_FINISH = "action.ALLERT_FINISH";

    public static final String EXTRAS_DEVICE = "BLUETOOTH_DEVICE";

    public static final String EXTRAS_TIME = "ALERT_TIME";

    public static final String EXTRAS_RSSI = "DEVICE_RSSI";

    public static final String EXTRAS_LLS_ALERT_VALUE = "LLS_ALERT_VALUE";

    private final int DISCOVER_SERVICES = 1;

    private final int READ_CHARACTERISTIC = 2;

    public static final int HIGH_ALERT = 2;

    public static final int MILD_ALERT = 1;

    public static final int NO_ALERT = 0;

    private BluetoothManager mManager = null;

    private BluetoothAdapter mAdapter = null;

    private ScheduledExecutorService mPathLossScheduler = null;

    /**
     * A HashMap with BluetoothDevice as a key and DeviceProperties as a value.
     */
    private Map<BluetoothDevice, DeviceProperties> mHashMapDevice = null;

    private Queue<BluetoothGattCharacteristic> mReadQueue = new ArrayDeque<BluetoothGattCharacteristic>();
    private boolean   mAutoConnect = false;
    public boolean isWriteThresholdNeedToBeCalled = false;
    /* Monitor state constants */
    private static final int MONITOR_STATE_IDLE     = 0;
    private static final int MONITOR_STATE_STARTING = 1;
    private static final int MONITOR_STATE_STOPPING = 2;
    private static final int MONITOR_STATE_STARTED  = 3;
    private static final int MONITOR_STATE_CLOSED   = 4;
   /* constants for rssi threshold event */
    /** @hide */
    public static final int RSSI_MONITOR_DISABLED = 0x00;
    /** @hide */
    public static final int RSSI_HIGH_ALERT       = 0x01;
    /** @hide */
    public static final int RSSI_MILD_ALERT       = 0x02;
    /** @hide */
    public static final int RSSI_NO_ALERT         = 0x03;

    /* command status */
    /** @hide */
    public static final int COMMAND_STATUS_SUCCESS = 0x00;
    /** @hide */
    public static final int COMMAND_STATUS_FAILED  = 0x01;


    /**
     * Implements callback methods for GATT events defined by the Bluetooth Low
     * Energy Api.
     */
    private final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {

        /**
         * A callback method indicating when proximity monitor(GATT client) has
         * connected/disconnected form proximity reporter (GATT service).
         *
         * @param gatt GATT client
         * @param status Status of the connect or disconnect operation.
         * @param newState Returns the new connection state.
         * @return void
         */
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            Log.v(TAG, "onConnectionStateChange(): gatt=" + gatt + " status=" + status
                    + " newState=" + newState);
            BluetoothDevice leDevice = gatt.getDevice();
            DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.v(TAG, "onConnectionStateChange(): starting discovery");

                if((deviceProp.mState != MONITOR_STATE_STARTING) && (deviceProp.pathLossAlertLevel != PxpConsts.ALERT_LEVEL_NO)) {
                    int rssiMin = deviceProp.txPowerLevel - deviceProp.maxPathLossThreshold;
                    int rssiMax = deviceProp.txPowerLevel - deviceProp.minPathLossThreshold;
                    Log.d(TAG, "rssiMin::"+rssiMin);
                    Log.d(TAG, "rssiMax::"+rssiMax);

                    if (!deviceProp.mQAdapter.writeRssiThreshold(deviceProp.mLPProxymityMonitorCallback, rssiMin, rssiMax)) {
                     // start software method to monitor rssi
                       startPathLossSwMonitor();
                    }
                    deviceProp.mState = MONITOR_STATE_STARTING;
                }

                if (leDevice.getBondState() != BluetoothDevice.BOND_BONDING) {
                    Log.v(TAG, "bondState != BluetoothDevice.BOND_BONDING");
                    deviceProp.startDiscoverServices = true;
                    mGattHandler.obtainMessage(DISCOVER_SERVICES, deviceProp.gatt).sendToTarget();
                }

                deviceProp.disconnect = false;

            } else if (newState == BluetoothProfile.STATE_DISCONNECTED && deviceProp.disconnect) {
                Log.v(TAG, "device.mIsAlerting = false");

                deviceProp.isAlerting = false;
                deviceProp.connectionState = false;

            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.v(TAG, "Connection timeout");
                Log.v(TAG, "device.mIsAlerting = false");

                deviceProp.isAlerting = false;
                deviceProp.connectionState = false;
                deviceProp.hasLlsService = false;
                deviceProp.hasIasService = false;
                deviceProp.hasTxpService = false;
                String address = leDevice.getAddress();

                if (deviceProp.deviceAddress != null && address.equals(deviceProp.deviceAddress)
                    && deviceProp.gatt != null) {

                   deviceProp.mState = MONITOR_STATE_IDLE;
                   Log.e(TAG, "Trying to use an existing mBluetoothGatt for connection.");
                   if(deviceProp.AddedToWhitelist == false) {
                       Log.v(TAG, "PairingFail = " + deviceProp.PairingFail);
                       if((deviceProp.isPairing && !deviceProp.PairingFail) || !deviceProp.isPairing ) {
                           boolean ret_val = deviceProp.gatt.connect();
                           deviceProp.connectionState = true;
                           deviceProp.AddedToWhitelist = true;
                       }
                       else {
                           deviceProp.PairingFail = false;
                           deviceProp.isPairing = false;
                       }
                   }
                   broadcastUpdate(LINKLOSS_ALERT, leDevice);
               }
            }
        }

        /**
         * A callback method invoked when the list of remote services,
         * characteristics and descriptors for the remote device have been
         * updated.
         *
         * @param gatt GATT client
         * @param staus GATT_SUCCESS if the remote device has been explored
         *            successfully.
         * @return void
         */
        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            Log.v(TAG, "onServicesDiscovered(): gatt=" + gatt + " status=" + status);
            BluetoothDevice leDevice = gatt.getDevice();
            DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

            for (BluetoothGattService service : deviceProp.gatt.getServices()) {
                UUID uuid = service.getUuid();

                Log.v(TAG, "onServicesDiscovered(): service=" + uuid.toString());

                if (uuid.equals(PxpConsts.IAS_UUID)) {
                    Log.v(TAG, "IAS found");
                    deviceProp.iasAlertLevelCh = service
                            .getCharacteristic(PxpConsts.ALERT_LEVEL_UUID);

                    deviceProp.iasAlertLevelCh
                            .setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE);

                    deviceProp.hasIasService = true;

                } else if (uuid.equals(PxpConsts.LLS_UUID)) {
                    Log.v(TAG, "LLS found");

                    deviceProp.llsAlertLevelCh = service
                            .getCharacteristic(PxpConsts.ALERT_LEVEL_UUID);

                    sendReadCharacteristicMessage(deviceProp.gatt, deviceProp.llsAlertLevelCh);

                    deviceProp.hasLlsService = true;

                } else if (uuid.equals(PxpConsts.TPS_UUID)) {
                    Log.v(TAG, "TPS found");

                    deviceProp.txPowerLevelCh = service
                            .getCharacteristic(PxpConsts.TX_POWER_LEVEL_UUID);

                    sendReadCharacteristicMessage(deviceProp.gatt, deviceProp.txPowerLevelCh);

                    deviceProp.hasTxpService = true;
                }
            }

            if (!deviceProp.hasLlsService) {
                PxpMonitorService.this.disconnect(leDevice);
                Log.v(TAG, "Disconnected because Linkloss Service = " + deviceProp.hasLlsService);
                broadcastUpdate(MISSING_LLS_SERVICE, leDevice);
            }
        }

        /**
         * A callback method reporting result of a characteristic read
         * operation.
         *
         * @param gatt GATT client
         * @param ch Characteristic that was read from the device.
         * @param status Status of the characteristic read operation.
         * @return void
         */
        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic ch,
                int status) {
            Log.v(TAG, "onCharacteristicRead(): ch=" + ch.getUuid().toString() + " status="
                    + status);

            BluetoothDevice leDevice = gatt.getDevice();
            DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

            deviceProp.isReading = false;
            Log.v(TAG, "device.misReading = false");

            if (status == BluetoothGatt.GATT_SUCCESS) {

                if (ch.getUuid().equals(PxpConsts.ALERT_LEVEL_UUID)) {
                    Log.v(TAG,
                            "LLS Alert Level is "
                                    + ch.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 0));

                    deviceProp.failedReadAlertLevel = false;

                } else if (ch.getUuid().equals(PxpConsts.TX_POWER_LEVEL_UUID)) {
                    deviceProp.txPowerLevel = ch.getIntValue(
                            BluetoothGattCharacteristic.FORMAT_SINT8, 0);
                    Log.v(TAG, "Tx Power Level is " + deviceProp.txPowerLevel + "dBm");

                    deviceProp.failedReadTxPowerLevel = false;
                }

            } else {
                Log.w(TAG, "Reading " + ch.getUuid() + " failed!");

                if (ch.getUuid().equals(PxpConsts.ALERT_LEVEL_UUID)) {
                    Log.d(TAG, "failed to ReadAlertLevel");
                    deviceProp.failedReadAlertLevel = true;
                }

                if (ch.getUuid().equals(PxpConsts.TX_POWER_LEVEL_UUID)) {
                    Log.d(TAG, "failed to ReadTxPowerLevel");
                    deviceProp.failedReadTxPowerLevel = true;
                }
            }

            // read next from queue
            ch = mReadQueue.poll();
            if (ch != null) {
                deviceProp.isReading = deviceProp.gatt.readCharacteristic(ch);
                Log.v(TAG, "device.mIsReading = " + deviceProp.isReading);
            }

            if (!deviceProp.isReading) {
                deviceProp.connectionState = true;
                broadcastUpdate(DEVICE_CONNECTED, leDevice);
            }
        }

        /**
         * A callback reporting the RSSI for a remote device connection.
         *
         * @param gatt GATT client
         * @param rssi The RSSI value for the device.
         * @param status Status of the read RSSI operation.
         * @return void
         */
        @Override
        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
            BluetoothDevice leDevice = gatt.getDevice();
            DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

            if (status != BluetoothGatt.GATT_SUCCESS) {
                return;
            }

            int pathLoss = deviceProp.txPowerLevel - rssi;

            Log.v(TAG, "tpl=" + deviceProp.txPowerLevel + " rssi=" + rssi + " pathloss="
                    + pathLoss);

            if (pathLoss > deviceProp.maxPathLossThreshold && !deviceProp.isAlerting) {
                deviceProp.iasAlertLevelCh.setValue(deviceProp.pathLossAlertLevel,
                        BluetoothGattCharacteristic.FORMAT_UINT8, 0);
                deviceProp.gatt.writeCharacteristic(deviceProp.iasAlertLevelCh);
                deviceProp.isAlerting = true;

                Time today = new Time(Time.getCurrentTimezone());
                today.setToNow();

                String time = today.year + "-" + today.month + "-" + today.monthDay +
                        ", " + today.hour + ":" + today.minute + ":" + today.second;

                final Intent intent = new Intent(DEVICE_ALLERT);
                intent.putExtra(EXTRAS_DEVICE, leDevice);
                intent.putExtra(EXTRAS_TIME, time);
                intent.putExtra(EXTRAS_RSSI, rssi);
                sendBroadcast(intent);

                sendBroadcast(new Intent(PATHLOSS_ALLERT));

            } else if (pathLoss < deviceProp.maxPathLossThreshold && deviceProp.isAlerting) {

                deviceProp.iasAlertLevelCh.setValue(PxpConsts.ALERT_LEVEL_NO,
                        BluetoothGattCharacteristic.FORMAT_UINT8, 0);
                deviceProp.gatt.writeCharacteristic(deviceProp.iasAlertLevelCh);
                deviceProp.isAlerting = false;

                sendBroadcast(new Intent(DEVICE_ALLERT_FINISH));
            }
        }
    };

    /**
     * Send READ_CHARACTERISTIC message to mGattHandler
     *
     * @param gatt GATT client
     * @param ch Characteristic to be read from the device.
     * @return void
     */
    public void sendReadCharacteristicMessage(BluetoothGatt gatt, BluetoothGattCharacteristic ch) {

        Pair<BluetoothGatt, BluetoothGattCharacteristic> pair =
                Pair.create(gatt, ch);

        mGattHandler.obtainMessage(READ_CHARACTERISTIC, pair).sendToTarget();
    }

    /**
     * The BroadcastReceiver that monitor broadcast Bluetooth device action.
     */
    public BroadcastReceiver mBtReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (action.equals(BluetoothDevice.ACTION_ACL_CONNECTED)) {
                Log.d(TAG, "BTReceiver action equals ACTION_ACL_CONNECTED");

            } else if (action.equals(BluetoothDevice.ACTION_ACL_DISCONNECTED)) {
                Log.d(TAG, "BTReceiver action equals ACTION_ACL_DISCONNECTED");

                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                broadcastUpdate(DEVICE_DISCONNECTED, device);

            } else if (action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)) {
                Log.d(TAG, "BTReceiver action equals ACTION_BOND_STATE_CHANGED");

                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                int prevBondState = intent.getIntExtra(BluetoothDevice.
                        EXTRA_PREVIOUS_BOND_STATE, -1);
                int bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, -1);

                Log.d(TAG, "Device address " + device.getAddress() + " Previous bond state = "
                        + prevBondState + " bond state = " + bondState);
                DeviceProperties deviceProp = mHashMapDevice.get(device);

                if (deviceProp == null) {
                    return;
                }
                if(bondState == BluetoothDevice.BOND_BONDING) {
                   deviceProp.isPairing = true;
                }
                if ((bondState == BluetoothDevice.BOND_NONE) && (prevBondState == BluetoothDevice.BOND_BONDING)) {
                    deviceProp.PairingFail = true;
                }
                if (bondState == BluetoothDevice.BOND_BONDED) {

                    if (!deviceProp.startDiscoverServices) {
                        Log.v(TAG, "bondState = BluetoothDevice.BOND_BONDED");
                        mGattHandler.obtainMessage(DISCOVER_SERVICES, deviceProp.gatt)
                                .sendToTarget();
                        deviceProp.startDiscoverServices = true;
                    } else {

                        if (deviceProp.failedReadAlertLevel) {
                            sendReadCharacteristicMessage(deviceProp.gatt,
                                    deviceProp.llsAlertLevelCh);
                        }

                        if (deviceProp.failedReadTxPowerLevel) {
                            sendReadCharacteristicMessage(deviceProp.gatt,
                                    deviceProp.txPowerLevelCh);
                        }
                    }
                }
            }
        }
    };

    /**
     * Broadcast that sends update on connection state to DeviceActivity.
     *
     * @param action Connection action to be performed.
     * @param leDevice Bluetooth remote device.
     * @return void
     */
    private void broadcastUpdate(final String action, BluetoothDevice leDevice) {
        final Intent intent = new Intent(action);
        Log.d(TAG, "Sending broadcast " + action);

        if (action.equals(LINKLOSS_ALERT)) {
            intent.putExtra(EXTRAS_LLS_ALERT_VALUE, getLinkLossAlertLevel(leDevice));
        }

        intent.putExtra(EXTRAS_DEVICE, leDevice);
        sendBroadcast(intent);
    }

    /**
     * Return the communication channel to the service.
     *
     * @param arg0 Intent that was used to bind to this service, as given to
     *            Context.bindService.
     * @return Return IBinder trough client can communicate with the server
     */
    @Override
    public IBinder onBind(Intent arg0) {
        Log.v(TAG, "onBind()");

        return new IPxpService.Stub() {

            public boolean connect(BluetoothDevice leDevice) {
                return PxpMonitorService.this.connect(leDevice);
            }

            public void disconnect(BluetoothDevice leDevice) {
                PxpMonitorService.this.disconnect(leDevice);
            }

            public boolean setLinkLossAlertLevel(BluetoothDevice leDevice, int level) {
                return PxpMonitorService.this.setLinkLossAlertLevel(leDevice, level);
            }

            public int getLinkLossAlertLevel(BluetoothDevice leDevice) {
                return PxpMonitorService.this.getLinkLossAlertLevel(leDevice);
            }

            public void setMaxPathLossThreshold(BluetoothDevice leDevice, int threshold) {
                PxpMonitorService.this.setMaxPathLossThreshold(leDevice, threshold);
            }

            public int getMaxPathLossThreshold(BluetoothDevice leDevice) {
                return PxpMonitorService.this.getMaxPathLossThreshold(leDevice);
            }

            public void setMinPathLossThreshold(BluetoothDevice leDevice, int threshold) {
                PxpMonitorService.this.setMinPathLossThreshold(leDevice, threshold);
            }

            public int getMinPathLossThreshold(BluetoothDevice leDevice) {
                return PxpMonitorService.this.getMinPathLossThreshold(leDevice);
            }

            public boolean setPathLossAlertLevel(BluetoothDevice leDevice, int level) {
                return PxpMonitorService.this.setPathLossAlertLevel(leDevice, level);
            }

            public int getPathLossAlertLevel(BluetoothDevice leDevice) {
                return PxpMonitorService.this.getPathLossAlertLevel(leDevice);
            }

            public boolean getConnectionState(BluetoothDevice leDevice) {
                return PxpMonitorService.this.getConnectionState(leDevice);
            }

            public boolean isPropertiesSet(BluetoothDevice leDevice) {
                return PxpMonitorService.this.isPropertiesSet(leDevice);
            }

            public boolean checkServiceStatus(BluetoothDevice leDevice, int service) {
                return PxpMonitorService.this.checkServiceStatus(leDevice, service);
            }

            public List<BluetoothDevice> getConnectedDevices() {
                return PxpMonitorService.this.getConnectedDevices();
            }

            public boolean checkFailedReadTxPowerLevel(BluetoothDevice leDevice) {
                return PxpMonitorService.this.checkFailedReadTxPowerLevel(leDevice);
            }

            public void disconnectDevices() {
                PxpMonitorService.this.disconnectDevices();
            }
        };
    }

    /**
     * Called when all clients where disconnected from a interface published by
     * the proximity monitor service.
     *
     * @param intent Intent that was used to bind to this service.
     * @return Return true if you would like to have the service's
     *         onRebind(Intent) method later called when new clients bind to it.
     */
    @Override
    public boolean onUnbind(Intent intent) {
        Log.d(TAG, "Unbinding");
        close();
        stopSelf();
        return super.onUnbind(intent);
    }

    @Override
    public void onCreate() {
        Log.v(TAG, "onCreate()");
        super.onCreate();

        mManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mAdapter = mManager.getAdapter();

        if (mAdapter == null) {
            Log.e(TAG, "Unable to obtain a BluetoothAdapter.");
        }

        if(mHashMapDevice == null) {
            mHashMapDevice = new HashMap<BluetoothDevice, DeviceProperties>();
            Log.v(TAG, "mHashMapDevice created");
        }
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        intentFilter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        intentFilter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECT_REQUESTED);
        intentFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);

        this.registerReceiver(mBtReceiver, intentFilter);
    }

    @Override
    public void onDestroy() {
        Log.v(TAG, "onDestroy()");

        super.onDestroy();

        if (mBtReceiver != null) {
            this.unregisterReceiver(mBtReceiver);
        }

        mGattHandler.removeMessages(DISCOVER_SERVICES);
        mGattHandler.removeMessages(READ_CHARACTERISTIC);

        close();
    }

    /**
     * The Handler that triggers GATT operations (discover services and read
     * characteristic).
     */
    private final Handler mGattHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case DISCOVER_SERVICES:
                    Log.d(TAG, "discoverServices();");
                    BluetoothGatt gatt = (BluetoothGatt) msg.obj;
                    gatt.discoverServices();
                    break;

                case READ_CHARACTERISTIC:
                    Pair<BluetoothGatt, BluetoothGattCharacteristic> pair = (Pair<BluetoothGatt, BluetoothGattCharacteristic>) msg.obj;
                    Log.d(TAG, "readCharacteristic()");
                    Log.d(TAG, "" + pair.second.getUuid());

                    readCharacteristic(pair.first, pair.second);
                    break;
            }
        }
    };

    /**
     * Connect to the GATT service hosted on proximity reporter. The result of
     * connect operation is reported asynchronously through the Bluetooth GATT
     * callback.
     *
     * @param leDevice Bluetooth device
     * @return Return true if the connection where initiated successfully.
     */
    public boolean connect(BluetoothDevice leDevice) {
        Log.w(TAG, "BluetoothAdapter connecting");
        DeviceProperties deviceProp;

        String address = null;

        if (leDevice == null) {
            return false;
        }

        address = leDevice.getAddress();

        if (mAdapter == null || address == null) {
            Log.w(TAG, "BluetoothAdapter not initialized or unspecified address.");
            return false;
        }

        if ((deviceProp = mHashMapDevice.get(leDevice)) == null) {
            deviceProp = new DeviceProperties();

            deviceProp.gatt = leDevice.connectGatt(this, false, mGattCallback);

            deviceProp.deviceAddress = address;

            deviceProp.BluetoothLwPwrProximityMonitor(this, leDevice, new QcBluetoothMonitorRssiCallback(leDevice));
            deviceProp.mState = MONITOR_STATE_IDLE;

            mHashMapDevice.put(leDevice, deviceProp);
            Log.d(TAG, "device added");
            Log.d(TAG, "hashmap size" + mHashMapDevice.size());

        }
        else if (deviceProp.deviceAddress != null && address.equals(deviceProp.deviceAddress)) {
                Log.d(TAG, "register lpp ");
                deviceProp.BluetoothLwPwrProximityMonitor(this, leDevice, new QcBluetoothMonitorRssiCallback(leDevice));
                deviceProp.mState = MONITOR_STATE_IDLE;
                deviceProp.gatt = leDevice.connectGatt(this, false, mGattCallback);
        }
        Log.d(TAG, "Trying to create a new connection.");
        return true;
    }

    /**
     * Disconnect an existing connection with proximity reporter. The result of
     * disconnect operation is reported asynchronously through the Bluetooth
     * GATT callback.
     *
     * @param leDevice Bluetooth device
     * @return void
     */
    public void disconnect(BluetoothDevice leDevice) {
        Log.d(TAG, "on Disconnect");
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

        if (mAdapter == null || deviceProp.gatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }

        deviceProp.disconnect = true;
        if(deviceProp.mState == MONITOR_STATE_STARTED) {
            deviceProp.mQAdapter.enableRssiMonitor(deviceProp.mLPProxymityMonitorCallback, false);
            deviceProp.mState = MONITOR_STATE_STOPPING;
        }

        if (deviceProp.gatt != null) {
            deviceProp.gatt.disconnect();
        }
    }

    /**
     * Close GATT
     *
     * @return void
     */
    public void close() {
        Log.d(TAG, "close");

        stopPathLossSwMonitor();

        for (BluetoothDevice device : mHashMapDevice.keySet()) {
            DeviceProperties deviceProp = mHashMapDevice.get(device);
            Log.d(TAG, "close qcproximty");
             if (DBG) Log.d(TAG, "close()");
            if(MONITOR_STATE_CLOSED == deviceProp.mState)
               return;

            if(deviceProp.mState == MONITOR_STATE_STARTING ||
                deviceProp.mState == MONITOR_STATE_STARTED) {
                    deviceProp.mQAdapter.enableRssiMonitor(deviceProp.mLPProxymityMonitorCallback, false);
            }
            if (deviceProp.gatt != null) {
                deviceProp.gatt.disconnect();
                deviceProp.gatt.close();
            }
            if (DBG) Log.d(TAG, "Monitor is closed");
             deviceProp.mState = MONITOR_STATE_CLOSED;
        }
    }

    /**
     * Request a read on a given characteristic. The result of read operation
     *
     * @param leDevice Bluetooth device
     * @return void
     */
    private void readCharacteristic(BluetoothGatt gatt, BluetoothGattCharacteristic ch) {
        Log.v(TAG, "readCharacteristic(): ch=" + ch.getUuid());
        BluetoothDevice leDevice = gatt.getDevice();
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

        if (!deviceProp.isReading) {
            deviceProp.isReading = deviceProp.gatt.readCharacteristic(ch);
            Log.v(TAG, "readCharacteristic(): read immediately (" + deviceProp.isReading + ")");

        } else {
            Log.v(TAG, "readCharacteristic(): queue for reading later");
            mReadQueue.add(ch);
        }
    }

    /**
     * Read link loss alert level from the remote device.
     *
     * @param leDevice Bluetooth remote device
     * @return Return value of link loss alert Level: 0 - off alert level 1 -
     *         mild alert level 2 - high alert level
     */
    public int getLinkLossAlertLevel(BluetoothDevice leDevice) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

        if(deviceProp.isReading) {
            return -1;
        }
        if (deviceProp.failedReadAlertLevel) {
            return -2;
        }
        if (deviceProp.llsAlertLevelCh == null
                || deviceProp.llsAlertLevelCh.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8,
                        0) == null) {
            return -1;
        }

        return deviceProp.llsAlertLevelCh.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 0);
    }

    /**
     * Write link loss alert level to the remote device.
     *
     * @param leDevice Bluetooth remote device
     * @param level Alert level (0, 1 or 2).
     * @return Return true if the write operation was successful.
     */
    public boolean setLinkLossAlertLevel(BluetoothDevice leDevice, int level) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

        if (level < PxpConsts.ALERT_LEVEL_NO || level > PxpConsts.ALERT_LEVEL_HIGH) {
            return false;
        }

        deviceProp.llsAlertLevelCh.setValue(level, BluetoothGattCharacteristic.FORMAT_UINT8, 0);

        return deviceProp.gatt.writeCharacteristic(deviceProp.llsAlertLevelCh);
    }

    /**
     * Read path loss alert level from the remote device.
     *
     * @param leDevice Bluetooth remote device
     * @return Return value path loss alert Level: 0 - off alert level 1 - mild
     *         alert level 2 - high alert level
     */
    public int getPathLossAlertLevel(BluetoothDevice leDevice) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);
        return deviceProp.pathLossAlertLevel;
    }

    /**
     * Write path loss alert level to the remote device.
     *
     * @param leDevice Bluetooth remote device
     * @param level Alert level (0, 1 or 2).
     * @return Return true if the write operation where initiated successfully.
     */
    public boolean setPathLossAlertLevel(BluetoothDevice leDevice, int level) {
        final DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

        if (level < PxpConsts.ALERT_LEVEL_NO || level > PxpConsts.ALERT_LEVEL_HIGH) {
            return false;
        }

        if (level == deviceProp.pathLossAlertLevel) {
            return true;
        }

        deviceProp.pathLossAlertLevel = level;

        if (deviceProp.isAlerting) {
            Log.d(TAG, "deviceProp.isAlerting = " + deviceProp.isAlerting
                    + " deviceProp.pathLossAlertLevel = " + deviceProp.pathLossAlertLevel);

            deviceProp.iasAlertLevelCh.setValue(deviceProp.pathLossAlertLevel,
                    BluetoothGattCharacteristic.FORMAT_UINT8, 0);
            deviceProp.gatt.writeCharacteristic(deviceProp.iasAlertLevelCh);

            if (deviceProp.pathLossAlertLevel == PxpConsts.ALERT_LEVEL_NO) {
                sendBroadcast(new Intent(DEVICE_ALLERT_FINISH));

            } else {
                sendBroadcast(new Intent(PATHLOSS_ALLERT));
            }
        }

        if (deviceProp.pathLossAlertLevel != PxpConsts.ALERT_LEVEL_NO)
        {
            Log.v(TAG, "deviceProp.mPathLossAlertLevel != PxpConsts.ALERT_LEVEL_NO");
            if (deviceProp.mState != MONITOR_STATE_STARTING) {
                int rssiMin = deviceProp.txPowerLevel - deviceProp.maxPathLossThreshold;
                int rssiMax = deviceProp.txPowerLevel - deviceProp.minPathLossThreshold;
                Log.d(TAG, "rssiMin::"+rssiMin);
                Log.d(TAG, "rssiMax::"+rssiMax);
               // start hardware rssi monitor
                Log.d(TAG, "registerLppClient::");
                deviceProp.mQAdapter.registerLppClient(deviceProp.mLPProxymityMonitorCallback, leDevice.getAddress(), true);
                if (!deviceProp.mQAdapter.writeRssiThreshold(deviceProp.mLPProxymityMonitorCallback, rssiMin, rssiMax)) {
                // start software method to monitor rssi
                    startPathLossSwMonitor();
                }
                deviceProp.mState = MONITOR_STATE_STARTING;
            }
        }
        else {
            // stop hardware rssi monitor
               if(deviceProp.mState == MONITOR_STATE_STARTING ||
                   deviceProp.mState == MONITOR_STATE_STARTED) {
                    deviceProp.mQAdapter.enableRssiMonitor(deviceProp.mLPProxymityMonitorCallback, false);
                   deviceProp.mState = MONITOR_STATE_STOPPING;
                   if (DBG) Log.d(TAG, "Monitor is stopping");
                }

            // stop software method to monitor rssi
            stopPathLossSwMonitor();
         }
        return true;
    }

    /**
     * Start monitoring device path loss every 1 second. Start
     * mPathLossScheduler if it's not running already.
     *
     * @return void
     */
    private void startPathLossSwMonitor() {

        if (mPathLossScheduler != null) {
            return;
        }

        Log.v(TAG, "mPathLossScheduler == null");

        mPathLossScheduler = Executors.newScheduledThreadPool(1);
        mPathLossScheduler.scheduleAtFixedRate(new Runnable() {

            @Override
            public void run() {

                for (DeviceProperties deviceProp : mHashMapDevice.values()) {
                    deviceProp.gatt.readRemoteRssi();
                }
            }

        }, 0, 1, TimeUnit.SECONDS);
    }

    /**
     * Stop monitoring device path loss. If all connected devices have path loss
     * alert level set to 0 stop mPathLossScheduler.
     *
     * @return void
     */
    private void stopPathLossSwMonitor() {
        boolean alertLevelNo = false;

        if (mPathLossScheduler == null) {
            return;
        }

        Log.v(TAG, "mPathLossScheduler != null");

        for (DeviceProperties prop : mHashMapDevice.values()) {

            if (prop.pathLossAlertLevel != PxpConsts.ALERT_LEVEL_NO) {
                alertLevelNo = true;
                break;
            }
        }

        if (!alertLevelNo) {
            mPathLossScheduler.shutdownNow();
            mPathLossScheduler = null;
        }
    }

    /**
     * Get path loss threshold value.
     *
     * @param leDevice Bluetooth remote device
     * @return Return threshold value.
     */
    public int getMaxPathLossThreshold(BluetoothDevice leDevice) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);
        return deviceProp.maxPathLossThreshold;
    }

    /**
     * Set value of path loss threshold.
     *
     * @param leDevice Bluetooth remote device
     * @return void
     */
    public void setMaxPathLossThreshold(BluetoothDevice leDevice, int threshold) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);
        deviceProp.maxPathLossThreshold = threshold;
    }

    /**
     * Get path loss threshold value.
     *
     * @param leDevice Bluetooth remote device
     * @return Return threshold value.
     */
    public int getMinPathLossThreshold(BluetoothDevice leDevice) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);
        return deviceProp.minPathLossThreshold;
    }

    /**
     * Set value of path loss threshold.
     *
     * @param leDevice Bluetooth remote device
     * @return void
     */
    public void setMinPathLossThreshold(BluetoothDevice leDevice, int threshold) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);
        deviceProp.minPathLossThreshold = threshold;
    }

    /**
     * Get current connection state for a given device.
     *
     * @param leDevice Bluetooth remote device
     * @return true if device is connected
     */
    public boolean getConnectionState(BluetoothDevice leDevice) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);
        return deviceProp.connectionState;
    }

    /**
     * Check if variable deviceProp for current device is set.
     *
     * @param leDevice Bluetooth remote device
     * @return true if is set
     */
    public boolean isPropertiesSet(BluetoothDevice leDevice) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

        if (deviceProp != null) {
            return true;

        } else {
            return false;
        }
    }

    /**
     * Check if remote service was found on a given device
     *
     * @param leDevice Bluetooth remote device
     * @param service remote service
     * @return true if remote service was found
     */
    public boolean checkServiceStatus(BluetoothDevice leDevice, int service) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

        final int LinkLossService = 0;
        final int ImmediateAlertervice = 1;
        final int TxPowerService = 2;

        if (service == LinkLossService) {
            return deviceProp.hasLlsService;

        } else if (service == ImmediateAlertervice) {
            return deviceProp.hasIasService;

        } else if (service == TxPowerService) {
            return deviceProp.hasTxpService;
        }

        return false;
    }

    /**
     * Check status of reading Tx Power level characteristic operation.
     *
     * @param leDevice Bluetooth remote device
     * @return true if reading operation failed.
     */
    public boolean checkFailedReadTxPowerLevel(BluetoothDevice leDevice) {
        DeviceProperties deviceProp = mHashMapDevice.get(leDevice);

        return deviceProp.failedReadTxPowerLevel;
    }

    /**
     * Disconnect all connected devices.
     */
    public void disconnectDevices() {
        for (BluetoothDevice leDevice : mHashMapDevice.keySet()) {
            if (leDevice != null) {
                Log.d(TAG, "Disconnect");
                PxpMonitorService.this.disconnect(leDevice);
            }
        }
    }

    /**
     * List connected devices to the proximity monitor.
     *
     * @return Return List<BluetoothDevice> of the connected devices.
     */
    public List<BluetoothDevice> getConnectedDevices() {
        List<BluetoothDevice> connectedDevices = mManager.getConnectedDevices(7);
        Iterator<BluetoothDevice> iterator = connectedDevices.iterator();
        List<BluetoothDevice> pxpConnectedDevice = new ArrayList<BluetoothDevice>();


        for (BluetoothDevice device : mHashMapDevice.keySet()) {
            DeviceProperties deviceProp = mHashMapDevice.get(device);

            if ((deviceProp.gatt != null) && (deviceProp.connectionState == true)) {
                  Log.v(TAG, "pxpConnectedDevice add");
                  pxpConnectedDevice.add(device);
            }
        }
        return pxpConnectedDevice;
    }
    /**
     * Provides the Alert level value based on vendor specific event type received
     * from controller.
     *
     * @return alert level value.
     */
    public int getAlertLevelValue(int evtType) {
        int alertLevelValue = 0;
        switch(evtType) {
            case RSSI_NO_ALERT:
                alertLevelValue = NO_ALERT;
                break;
            case RSSI_MILD_ALERT:
                alertLevelValue = MILD_ALERT;
                break;
            case RSSI_HIGH_ALERT:
                alertLevelValue = HIGH_ALERT;
                break;
        }
        return alertLevelValue;
    }

    private final class QcBluetoothMonitorRssiCallback extends
            BluetoothRssiMonitorCallback {
        private BluetoothDevice mDevice;

        QcBluetoothMonitorRssiCallback(BluetoothDevice device) {
            mDevice = device;
        }

        @Override
        public void onStarted() {
            Log.d(TAG, "onStarted in PxpMonitorService");
        }

        // Callback for stop()
        @Override
        public void onStopped() {
        }

        @Override
        public void onReadRssiThreshold(int thresh_min, int thresh_max, int alert, int status) {
        }

        @Override
        public void onAlert(int evtType, int rssi) {
            int alertLevelValue = 0;
            Log.d(TAG, "onAlert in PxpMonitorService:: evtType::"+evtType+"::rssi::"+rssi);

            DeviceProperties deviceProp = mHashMapDevice.get(mDevice);
            if(deviceProp.minPathLossThreshold == deviceProp.maxPathLossThreshold) {
                if(evtType == RSSI_HIGH_ALERT) {
                    alertLevelValue = deviceProp.pathLossAlertLevel;
                }
            }
            else {
                alertLevelValue = getAlertLevelValue(evtType);
            }

            deviceProp.iasAlertLevelCh.setValue(alertLevelValue,
                    BluetoothGattCharacteristic.FORMAT_UINT8, 0);
            deviceProp.gatt.writeCharacteristic(deviceProp.iasAlertLevelCh);

            if (evtType == RSSI_NO_ALERT
                    || evtType == RSSI_MONITOR_DISABLED) {
                deviceProp.isAlerting = false;

            } else {
                deviceProp.isAlerting = true;

                Time today = new Time(Time.getCurrentTimezone());
                today.setToNow();

                String time = today.year + "-" + today.month + "-" + today.monthDay +
                        ", " + today.hour + ":" + today.minute + ":" + today.second;

                final Intent intent = new Intent(DEVICE_ALLERT);
                intent.putExtra(EXTRAS_DEVICE, mDevice);
                intent.putExtra(EXTRAS_TIME, time);
                intent.putExtra(EXTRAS_RSSI, rssi);
                sendBroadcast(intent);
            }
        }
    }
}
