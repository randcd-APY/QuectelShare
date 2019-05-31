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

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.SystemProperties;
import android.util.Log;
import android.provider.Settings;
import android.content.ContentResolver;
import android.os.UserHandle;
import com.quicinc.wbc.WbcTypes;
import com.quicinc.wbc.WbcManager;

import java.lang.Object;

public class BTEventHandler extends BroadcastReceiver {
    private static final String TAG = "BTEventHandler-A4WP";
    private static boolean V = false/*Constants.VERBOSE*/;
    private static boolean wait_for_gattdereg = false;
    private int state;
    private BluetoothAdapter mBluetoothAdapter;
    static boolean mPtuPresence = false;
    private WbcManager mWbcManager;


    @Override
    public void onReceive(Context context, Intent intent) {
       String action = intent.getAction();
       mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

       V = SystemProperties.getBoolean("persist.a4wp.logging", false);
       /* 1> if State changes from BT-ON to BLE-ALWAYS when MTP is
       ** still on pad, pad detection will be broadcasted to register
       ** a4wp service
       ** 2> Register A4WP if BT is turned-on.
       */
       if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
           state = intent.getIntExtra
                          (BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
            if (BluetoothAdapter.STATE_OFF == state) {
               wait_for_gattdereg = false;
               if(mBluetoothAdapter.isLeEnabled()) {
                   if (mWbcManager != null) {
                       mPtuPresence = (mWbcManager.getPtuPresence() == 0);
                          if (V) Log.d(TAG, "broadcast pad detection mPtuPresence: " + mPtuPresence);
                          if (mPtuPresence == true)
                              context.sendBroadcastAsUser(new Intent(WbcTypes.ACTION_PTU_PRESENT), UserHandle.ALL);
                   }
               }
            } else if (BluetoothAdapter.STATE_ON == state) {
                if(SystemProperties.get("bluetooth.a4wp").equals("true")) {
                    if (V) Log.d(TAG, "Service Already registered");
                    return;
                } else {
                    if(SystemProperties.getBoolean("persist.bluetooth.a4wp", false) == false) {
                        Log.e(TAG, "A4WP is not supported");
                        return;
                    }
                    ComponentName service = context.startService
                                      (new Intent(context, A4wpService.class));
                    if (service != null) {
                        Log.d(TAG, "A4wp service started successfully");
                        SystemProperties.set("bluetooth.a4wp", "true");
                        return;
                    } else {
                        Log.e(TAG, "Could Not Start A4wp Service");
                        return;
                    }
                }
            }
       }

       /* 1> Enable Wipower-BLEOn when placed on PAD. if other app has already
       **    enabled BLE start a4wp service and enable BLE to hold app count so
       **    that even if other app disbles BLE a4wp will continue to hold BLE
       **    enabled.
       ** 2> Enable BLE_SCAN_ALWAYS_AVAILABLE in UI so that BLE Always on
       **    feature is enabled, App needs special permission and has to be
       **    priv app to do this.
       ** 3> if normal BT was turned on and then placed on pad enable BLE
       **    so that it continues to hold BLE Always on even after BT is
       **    turned off.
       */
       if (action.equals("com.quicinc.wbc.action.ACTION_PTU_PRESENT")) {
            if (mBluetoothAdapter != null) {
                if (mBluetoothAdapter.getState() == BluetoothAdapter.STATE_ON) {
                    if (V) Log.d(TAG, "Enable BLE");
                    Settings.Global.putInt(context.getContentResolver(),Settings.Global.BLE_SCAN_ALWAYS_AVAILABLE, 1);
                    mBluetoothAdapter.enableBLE();
                }
                if(SystemProperties.get("bluetooth.a4wp").equals("true")) {
                    Log.e(TAG, "Service Already registered");
                    return;
                } else {
                    SystemProperties.set("bluetooth.a4wp", "true");
                }
                if ((mBluetoothAdapter.isLeEnabled()) ||
                    (mBluetoothAdapter.getState() == BluetoothAdapter.STATE_ON)) {
                    if(SystemProperties.getBoolean("persist.bluetooth.a4wp", false) == false) {
                        Log.e(TAG, "A4WP is not supported");
                        return;
                    }
                    ComponentName service = context.startService
                                      (new Intent(context, A4wpService.class));
                    if (service != null) {
                        Log.e(TAG, "A4wp service started successfully");
                        wait_for_gattdereg = false;
                    } else {
                        Log.e(TAG, "Could Not Start A4wp Service");
                        return;
                    }
                }
                Settings.Global.putInt(context.getContentResolver(),Settings.Global.BLE_SCAN_ALWAYS_AVAILABLE, 1);
                mBluetoothAdapter.enableBLE();
            }
        }

        /* A4WP service will not be registered if wipower OTP is not programmed */
        if(SystemProperties.getBoolean("persist.bluetooth.a4wp", false) == false) {
            Log.e(TAG, "A4WP is not supported");
            return;
        }

        if(!(SystemProperties.get("bluetooth.a4wp").equals("true"))) {
            Log.d(TAG, "no pad detected");
            return;
        }

        /* Pad detection enables BLE, On success "BLE On intent" will be
        ** broadcasted, based on this intent A4WPService will be registered
        */
        if ((action.equals(BluetoothAdapter.ACTION_BLE_STATE_CHANGED)) ||
            (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)))
        {
            state = intent.getIntExtra
                           (BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
            if (V) Log.d(TAG, "BLE state changed " + state);

            if ((BluetoothAdapter.STATE_BLE_ON == state))
            {
                if (V) Log.v(TAG, "Received BLUETOOTH_BLE_STATE_ON");
                if (wait_for_gattdereg == false) {
                    ComponentName service = context.startService
                                      (new Intent(context, A4wpService.class));
                    if (service != null) {
                        Log.e(TAG, "A4wp service started successfully");
                    } else {
                        Log.e(TAG, "Could Not Start A4wp Service");
                        return;
                    }
                }
            } else if ( BluetoothAdapter.STATE_BLE_TURNING_OFF == state ||
                        BluetoothAdapter.STATE_TURNING_OFF == state ) {
                wait_for_gattdereg = true;
                if (V) Log.d(TAG, "Deregister-A4WPService");
                SystemProperties.set("bluetooth.a4wp", "false");
                context.stopService(new Intent(context, A4wpService.class));
            }
        }
    }
}
