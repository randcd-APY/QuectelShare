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

import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.View;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.widget.CompoundButton;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.EditText;
import android.os.CountDownTimer;
import android.widget.Button;
import android.util.Log;

public class VendorOptionView extends RelativeLayout {
/*
    Class to handle the profile View actions
*/
    private EditText edittext;
    private EditText ocftext;
    private Button tButton;
    private Button resetButton;
    private TextView tview_error;
    private MalibuCountDownTimer countDownTimer;
    private static boolean ocf_valid = false;
    private static boolean payload_valid = false;
    //Min OCF :0x000
    private final int OCF_MIN_INT = 0;
    //Max OCF :0x3FF
    private final int OCF_MAX_INT = 1023;
    private final int PAYLOAD_MAX = 255;
    private final long startTime = 10 * 1000;
    private final long interval = 1000;
    private int optionSelected = getSecondoryOptions.selected;

    public VendorOptionView(Context context,AttributeSet attributeSet) {
        super(context, attributeSet);
        Resources res = getResources();
        int value = (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 15, res.getDisplayMetrics());
        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View rootView =  inflater.inflate(R.layout.vendor_log_options, this,true);
        tButton = (Button)rootView.findViewById(R.id.vsc_button);
        resetButton = (Button)rootView.findViewById(R.id.reset_button);
        edittext = (EditText)rootView.findViewById(R.id.editText);
        ocftext = (EditText)rootView.findViewById(R.id.ocfText);
        tview_error = (TextView)rootView.findViewById(R.id.error);
        countDownTimer = new MalibuCountDownTimer(startTime, interval);


        resetButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
            ocftext.setText(null);
            edittext.setText(null);
            tview_error.setVisibility(View.INVISIBLE);
            }
        });

        // OnClick Listener
        tButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                String vsc_edittext = edittext.getText().toString();
                String ocf_edittext = "0x" + ocftext.getText().toString();
                int ocf_int = 0;
                int count = 0;
                int payload = 0;
                if(ocftext.getText().toString().matches("")) {
                    tview_error.setVisibility(View.VISIBLE);
                    tview_error.setText("Enter valid OCF.");
                    ocftext.setText(null);
                    return;
                }
                try {
                    ocf_int = Integer.decode(ocf_edittext);
                } catch (Exception e) {
                    Log.d(Main.TAG,"OCF decode Exception" + e);
                    tview_error.setVisibility(View.VISIBLE);
                    tview_error.setText("Not Valid OCF:" + ocf_edittext);
                    ocftext.setText(null);
                    return;
                }
                // Check OCF value in 0x0000 - 0x03ff otherwise give error
                if (ocf_int >= OCF_MIN_INT && ocf_int <= OCF_MAX_INT) {
                    Log.d(Main.TAG,"Valid OCF command" + ocf_edittext);
                    ocf_valid = true;
                    tview_error.setVisibility(View.INVISIBLE);
                } else {
                    tview_error.setVisibility(View.VISIBLE);
                    tview_error.setText("Not Valid OCF:" + ocf_edittext);
                    ocftext.setText(null);
                    return;
                }
                if (vsc_edittext.matches("") && ocf_valid) {
                    Log.d(Main.TAG,"payload is empty");
                    sendIntent.transmitIntent(getContext(), ocf_int, vsc_edittext, Main.VSC_MODULE_ID);
                    tview_error.setVisibility(View.VISIBLE);
                    tview_error.setText("VSC Sent");
                    tButton.setEnabled(false);
                    countDownTimer.start();
                    return;
                }
                String [] vsc_command = vsc_edittext.split(",");
                for ( int i = 0; i <= vsc_command.length - 1; i++) {
                  String data_edittext = "0x" + vsc_command[i].toString();
                    try {
                        payload = Integer.decode(data_edittext);
                    } catch (Exception e) {
                        Log.d(Main.TAG,"payload decode Exception"+e);
                        payload_valid = false;
                        tview_error.setVisibility(View.VISIBLE);
                        tview_error.setText("Not Valid Data");
                        return;
                    }

                    if (payload <= PAYLOAD_MAX  && payload >=0) {
                        payload_valid = true;
                        count++;
                    } else {
                        payload_valid = false;
                        tview_error.setVisibility(View.VISIBLE);
                        tview_error.setText("Not Valid Data");
                        return;
                    }
                }
                Log.d(Main.TAG,"payload count:" + count);
                if ( payload_valid && ocf_valid && count <= PAYLOAD_MAX ) {
                    sendIntent.transmitIntent(getContext(), ocf_int, vsc_edittext, Main.VSC_MODULE_ID);
                    tview_error.setVisibility(View.VISIBLE);
                    tview_error.setText("VSC sent");
                    tButton.setEnabled(false);
                    countDownTimer.start();
                } else {
                    tview_error.setVisibility(View.VISIBLE);
                    tview_error.setText("Data overflow");
                }
           }
        });
    }



    public VendorOptionView(Context context){
        this(context, null);
    }

    public class MalibuCountDownTimer extends CountDownTimer {
        public  MalibuCountDownTimer(long startTime, long interval)
        {
            super(startTime, interval);
        }
        @Override
        public void onTick(long millisUntilFinished) {
            tButton.setText("Active after: " + (millisUntilFinished/1000));
        }
        @Override
        public void onFinish() {
            tButton.setEnabled(true);
            tButton.setText("SEND VS CMD");
        }
    };
}
