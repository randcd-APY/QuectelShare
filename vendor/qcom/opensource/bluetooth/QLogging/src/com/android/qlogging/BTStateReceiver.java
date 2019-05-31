/*********************************************************************
*
* Copyright (c) 2015, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
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
*
************************************************************************/

package com.android.qlogging;

import android.app.ActivityManager;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.CheckBox;
import android.widget.Space;
import android.widget.Spinner;
import android.widget.Toast;
import android.widget.ToggleButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.view.View;
import android.view.ViewGroup;
import android.os.Handler;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.lang.ProcessBuilder;
import java.lang.Exception;
import java.util.Scanner;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.lang.reflect.Field;
import java.lang.Runnable;
import android.os.SystemProperties;
import android.content.pm.PackageManager;

public class BTStateReceiver extends BroadcastReceiver{
    @Override
    public void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();
        Toast toast;
        String mRome ="rome";
        String bt_soc_type;

        final int btstate = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
        switch (btstate) {
            case BluetoothAdapter.STATE_OFF:
                break;
            case BluetoothAdapter.STATE_TURNING_OFF:
                break;
            case BluetoothAdapter.STATE_ON:
                File file_global  = new File(context.getFilesDir(), "GlobalSettings.xml");
                File file_stack  = new File(context.getFilesDir(), "StackSettings.xml");
                File file_soc  = new File(context.getFilesDir(), "StackSettings.xml");
                Map map;
                int counter = 0;
                String[] profile_tags = context.getResources().getStringArray(R.array.profile_list_tags);
                String[] stack_list_names = context.getResources().getStringArray(R.array.stack_list_names);
                String[] stack_list_tags = context.getResources().getStringArray(R.array.stack_list_tags);
                String[] soc_list_names = context.getResources().getStringArray(R.array.soc_list_names);
                String[] soc_list_tags = context.getResources().getStringArray(R.array.soc_list_tags);
                bt_soc_type = SystemProperties.get("qcom.bluetooth.soc");
                if (file_global.exists()) {
                    Utils.setGlobalSettings(context);
                } else if (file_stack.exists()) {
                    map = Utils.getPreviousSettings(context, Main.STACK_MODULE_ID);
                    if (!map.isEmpty()) {
                        for (String stack : stack_list_names) {
                            Object state_obj = map.get(stack);
                            if (state_obj != null) {
                                String state = state_obj.toString();
                                int presState = Character.getNumericValue(state.charAt(2));
                                sendIntent.transmitIntent(context, presState, stack_list_tags[counter], Main.STACK_MODULE_ID);
                                counter++;
                            }
                        }
                    }
                } else if (file_soc.exists()) {
                    map = Utils.getPreviousSettings(context, Main.SOC_MODULE_ID);
                    if (!Main.soc_log_enabled)
                    {
                        if (bt_soc_type.equals(mRome))
                            Log.d(Main.TAG,"soc type is rome dont enable SOC logging");
                        else
                        {
                            sendIntent.transmitIntent(context, 0, "F", Main.SOC_ALL_MODULE_ID);
                            Log.d(Main.TAG,"soc type is not rome enable SOC logging");
                        }
                    }
                    else
                    {
                        getSecondoryOptions.SOC_levels="";
                        counter = 0;
                        if (!map.isEmpty())
                        {
                            for (String soc : soc_list_names)
                            {
                                Object state_obj = map.get(soc);
                                if (state_obj != null)
                                {
                                    String state = state_obj.toString();
                                    int presState = Character.getNumericValue(state.charAt(2));
                                    getSecondoryOptions.SOC_levels+=String.valueOf(presState);
                                    counter++;
                                }else{
                                    getSecondoryOptions.SOC_levels+=String.valueOf(0);
                                }
                            }
                            if (bt_soc_type.equals(mRome))
                                Log.d(Main.TAG,"soc type is rome dont enable SOC logging");
                            else
                            {
                                Log.d(Main.TAG,"soc type is not rome enable SOC logging");
                                sendIntent.transmitIntent(context, Main.SOC_ALL_MODULE_ID, getSecondoryOptions.SOC_levels, Main.SOC_ALL_MODULE_ID);
                            }
                        }
                        else
                            Log.d(Main.TAG,"Map empty for SOC");
                    }
                } else {
                    Log.d(Main.TAG,"File not yet created.");
                }
                break;
            case BluetoothAdapter.STATE_TURNING_ON:
                break;
        }
    }
}
