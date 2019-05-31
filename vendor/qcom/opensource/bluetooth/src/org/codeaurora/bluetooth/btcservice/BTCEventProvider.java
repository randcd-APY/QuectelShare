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

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothHeadset;
import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothInputDevice;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.os.SystemProperties;

import java.lang.Object;
/**
 * <p> {@link com.android.bluetooth.btservice.BTCEventProvider} is a BroadcastReceiver
 * which listens to set of Bluetoth related events which required by BTWlan Coex module
 *  to achieve BT WLAN co-existance
 * </p>
 * <p> BTEventProvider is responsible for starting and stopping of
 * {@link com.android.bluetooth.btservice.BTCService} and
 * also to send the events via {@link com.android.bluetooth.btservice.BTCService#sendEvent}
 */

public class BTCEventProvider extends BroadcastReceiver {
    private static final String TAG = "BTCEventProvider";
    private static final boolean D = /*Constants.DEBUG*/false;
    private static final boolean V = false/*Constants.VERBOSE*/;
    private int state;
    private static String btsoc = "invalid";
    BTCService.BTCEvent event = BTCService.BTCEvent.BLUETOOTH_NONE;

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();

        if(btsoc.equals("invalid"))
        {
            btsoc = SystemProperties.get("qcom.bluetooth.soc");
        }
        /* ignore the events for non atheroes BT SOCs */
        if (!btsoc.equals("ath3k")) {
            return;
        }

        if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
            state = intent.getIntExtra
                           (BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
            if (BluetoothAdapter.STATE_ON == state) {
                if (V) Log.v(TAG, "Received BLUETOOTH_STATE_ON");
                ComponentName service = context.startService
                                      (new Intent(context, BTCService.class));
                if (service != null) {
                    event =  BTCService.BTCEvent.BLUETOOTH_ON;
                } else {
                    if (D) Log.d(TAG, "Could Not Start BTC Service ");
                    return;
                }
            } else if (BluetoothAdapter.STATE_OFF == state) {
                if (V) Log.v(TAG, "Received BLUETOOTH_STATE_OFF");
                event =  BTCService.BTCEvent.BLUETOOTH_OFF;
            }
        } else if (action.equals(BluetoothAdapter.ACTION_DISCOVERY_STARTED)) {
            if (V) Log.v(TAG, "Received ACTION_DISCOVERY_STARTED");
            event =  BTCService.BTCEvent.BLUETOOTH_DISCOVERY_STARTED;
        } else if (action.equals(BluetoothAdapter.ACTION_DISCOVERY_FINISHED)) {
            if (V) Log.v(TAG, "Received ACTION_DISCOVERY_STOPPED");
            event =  BTCService.BTCEvent.BLUETOOTH_DISCOVERY_FINISHED;
        } else if (action.equals(BluetoothDevice.ACTION_ACL_CONNECTED)) {
            if (V) Log.v(TAG, "Received ACTION_ACL_CONNECTED");
            event =  BTCService.BTCEvent.BLUETOOTH_DEVICE_CONNECTED;
        } else if (action.equals(BluetoothDevice.ACTION_ACL_DISCONNECTED)) {
            if (V) Log.v(TAG, "Received ACTION_ACL_DISCONNECTED");
            event =  BTCService.BTCEvent.BLUETOOTH_DEVICE_DISCONNECTED;
        } else if (action.equals(BluetoothHeadset.ACTION_CONNECTION_STATE_CHANGED)) {
            if (V) Log.v(TAG, "ACTION_CONNECTION_STATE_CHANGED");
            state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE, BluetoothAdapter.ERROR);
            if (BluetoothProfile.STATE_CONNECTED == state) {
                if (V) Log.v(TAG, "BluetoothHeadset.STATE_CONNECTED");
                event =  BTCService.BTCEvent.BLUETOOTH_HEADSET_CONNECTED;
            } else if (BluetoothProfile.STATE_DISCONNECTED == state) {
                if (V) Log.v(TAG, "BluetoothHeadset.STATE_DISCONNECTED");
                event =  BTCService.BTCEvent.BLUETOOTH_HEADSET_DISCONNECTED;
            }
        } else if (action.equals(BluetoothHeadset.ACTION_AUDIO_STATE_CHANGED)) {
            if (V) Log.v(TAG, "BluetoothHeadset.ACTION_AUDIO_STATE_CHANGED");
            state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE, BluetoothAdapter.ERROR);
            if (BluetoothProfile.STATE_CONNECTED == state) {
                if (V) Log.v(TAG, "ACTION_AUDIO_STATE_CHANGED:STATE_CONNECTED");
                event =  BTCService.BTCEvent.BLUETOOTH_HEADSET_AUDIO_STREAM_STARTED;
            } else if (BluetoothProfile.STATE_DISCONNECTED == state) {
                if (V) Log.v(TAG, "ACTION_AUDIO_STATE_CHANGED:STATE_DISCONNECTED");
                event =  BTCService.BTCEvent.BLUETOOTH_HEADSET_AUDIO_STREAM_STOPPED;
            }
        } else if (action.equals(BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED)) {
            if (V) Log.v(TAG, "BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED");
            state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE, BluetoothAdapter.ERROR);
            if (BluetoothProfile.STATE_CONNECTED == state) {
                if (V) Log.v(TAG, "A2DP: STATE_CONNECTED");
                event =  BTCService.BTCEvent.BLUETOOTH_AUDIO_SINK_CONNECTED;
            } else if (BluetoothProfile.STATE_DISCONNECTED == state) {
                if (V) Log.v(TAG, "A2DP: STATE_DISCONNECTED");
                event =  BTCService.BTCEvent.BLUETOOTH_AUDIO_SINK_DISCONNECTED;
            }
        } else if (action.equals(BluetoothA2dp.ACTION_PLAYING_STATE_CHANGED)) {
            if (V) Log.v(TAG, "BluetoothA2dp.ACTION_PLAYING_STATE_CHANGED");
            state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE, BluetoothAdapter.ERROR);
            if (BluetoothA2dp.STATE_PLAYING == state) {
                if (V) Log.v(TAG, "A2DP. PLAYING_STATE_CHANGED: PLAYING");
                event =  BTCService.BTCEvent.BLUETOOTH_SINK_STREAM_STARTED;
            } else if (BluetoothA2dp.STATE_NOT_PLAYING == state) {
                if (V) Log.v(TAG, "A2DP.PLAYING_STATE_CHANGED: PLAYING_STOPPED");
                event =  BTCService.BTCEvent.BLUETOOTH_SINK_STREAM_STOPPED;
            }
        } else if (action.equals(BluetoothInputDevice.ACTION_CONNECTION_STATE_CHANGED)) {
            if (V) Log.v(TAG, "HID.ACTION_CONNECTION_STATE_CHANGED");
            state = intent.getIntExtra
                       (BluetoothProfile.EXTRA_STATE, BluetoothAdapter.ERROR);
            if (BluetoothProfile.STATE_CONNECTED == state) {
                if (V) Log.v(TAG, "HID.CONNECTION_STATE_CHANGED: CONNECTED");
                event =  BTCService.BTCEvent.BLUETOOTH_INPUT_DEVICE_CONNECTED;
            } else if (BluetoothProfile.STATE_DISCONNECTED == state) {
                if (V) Log.v(TAG, "HID.CONNECTION_STATE_CHANGED:DISCONNECTED");
                event =  BTCService.BTCEvent.BLUETOOTH_INPUT_DEVICE_DISCONNECTED;
            }
        }
        //Don;t send if it is not relavant event
        if (event != BTCService.BTCEvent.BLUETOOTH_NONE) {
           int status = BTCService.sendEvent(event);
           if (status == 0) {
              if (V) Log.v(TAG, "Event:" + event.getValue() + "Written Succesfully");
           }
           else {
              if (D) Log.d(TAG, "Error while sending Event:" + event.getValue());
           }
           if (event ==  BTCService.BTCEvent.BLUETOOTH_OFF) {
               if (V) Log.v(TAG, "Stop the BTC Service");
               context.stopService(new Intent(context, BTCService.class));
           }
        }
    }
}
