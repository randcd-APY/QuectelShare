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
import android.view.Gravity;
import android.view.LayoutInflater;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.Switch;
import android.widget.ToggleButton;
import android.widget.TextView;

import android.util.Log;

public class ProfileOptionsView extends LinearLayout {
/*
    Class to handle the profile View actions
*/

    private CheckBox checkBox;
    private ToggleButton tButton;
    public int state;
    public int prevState;
    private final int OFF=0;
    private final int DEBUG=1;
    private final int VERBOSE=2;
    private int optionSelected= getSecondoryOptions.selected;

    public ProfileOptionsView(Context context,AttributeSet attributeSet){
        super(context, attributeSet);

        setOrientation(LinearLayout.HORIZONTAL);
        setGravity(Gravity.CENTER_VERTICAL);

        Resources res = getResources();
        int value = (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 15, res.getDisplayMetrics());

        setPadding(value, value, value, value);

        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        inflater.inflate(R.layout.profile_log_options, this,true);

        checkBox = (CheckBox) getChildAt(0);
        checkBox.setTextColor(Color.BLACK);
        int id = Resources.getSystem().getIdentifier("btn_check_holo_light", "drawable", "android");
        checkBox.setButtonDrawable(id);

        tButton = (ToggleButton) getChildAt(2);

        tButton.setTextOn("V");
        tButton.setTextOff("D");

        checkBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if (!b) {
                    if(tButton.isChecked()){
                        prevState=VERBOSE;
                    }else{
                        prevState=DEBUG;
                    }
                    state = OFF;
                    setBackground(getResources().getDrawable(R.drawable.border_off));
                    tButton.setClickable(false);
                    tButton.setText("Off");
                    sendIntent.transmitIntent(getContext(), state, checkBox.getText().toString(), optionSelected);
                } else {
                    tButton.setTextOn("V");
                    tButton.setTextOff("D");
                    if(prevState==VERBOSE) {
                        tButton.setChecked(true);
                        state = VERBOSE;
                    }else{
                        tButton.setChecked(false);
                        state = DEBUG;
                    }
                    sendIntent.transmitIntent(getContext(),state, checkBox.getText().toString(), optionSelected);
                    setBackground(getResources().getDrawable(R.drawable.border));
                    tButton.setClickable(true);
                }
            }
        });

        if(!checkBox.isChecked()){
            state = OFF;
            setBackground(getResources().getDrawable(R.drawable.border_off));
            tButton.setClickable(false);
            sendIntent.transmitIntent(getContext(), state, checkBox.getText().toString(), optionSelected);
        } else {
            setBackground(getResources().getDrawable(R.drawable.border));
            tButton.setClickable(true);
            tButton.setTextOn("V");
            tButton.setTextOff("D");
            if(tButton.isChecked()){
                state = DEBUG;
            }else{
                state = VERBOSE;
            }
            sendIntent.transmitIntent(getContext(), state, checkBox.getText().toString(), optionSelected);
        }

        tButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                if (isChecked) {
                    state = VERBOSE;
                    prevState = VERBOSE;
                    sendIntent.transmitIntent(getContext(), state, checkBox.getText().toString(), optionSelected);
                } else {
                    state = DEBUG;
                    prevState = DEBUG;
                    sendIntent.transmitIntent(getContext(), state, checkBox.getText().toString(),optionSelected);
                }
            }
        });
    }

    public ProfileOptionsView(Context context){
        this(context, null);
    }

    public void setState(String state){
        CheckBox checkBox = (CheckBox) this.getChildAt(0);
        ToggleButton toggleButton = (ToggleButton) this.getChildAt(2);
        Log.d(Main.TAG, state);
        int prevState = Character.getNumericValue(state.charAt(0));
        int presState = Character.getNumericValue(state.charAt(2));
        this.prevState=prevState;
        this.state=presState;
        Log.d(Main.TAG,"Profile presValue=" + String.valueOf(presState));
        Log.d(Main.TAG,"Profile prevValue=" + String.valueOf(prevState));
        if(presState==OFF){
            checkBox.setChecked(false);
            toggleButton.setText("Off");
        }else if(presState==DEBUG){
            toggleButton.setTextOff("D");
            toggleButton.setTextOn("V");
            checkBox.setChecked(true);
            toggleButton.setChecked(false);
        }else{
            toggleButton.setTextOff("D");
            toggleButton.setTextOn("V");
            checkBox.setChecked(true);
            toggleButton.setChecked(true);
        }
    }
}

