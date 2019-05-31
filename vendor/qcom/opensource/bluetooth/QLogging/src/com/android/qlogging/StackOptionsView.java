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
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.util.Log;
import android.os.Bundle;

import java.lang.reflect.Array;

public class StackOptionsView extends LinearLayout {
/*
    Class to handle the Stack and SOC related view actions.
*/
    private CheckBox checkBox;
    public int prevState=0;
    public int state=0;
    public int prevState2=0;
    private Spinner spinner;
    private boolean justCreated=true;
    private boolean justCreated1=true;
    private ArrayAdapter<CharSequence> adapter;
    private int optionSelected = getSecondoryOptions.selected;

    public StackOptionsView(Context context,AttributeSet attributeSet) {
        super(context, attributeSet);

        setOrientation(LinearLayout.HORIZONTAL);
        setGravity(Gravity.CENTER_VERTICAL);

        Resources res = getResources();
        int value = (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 15, res.getDisplayMetrics());

        setPadding(value, value, value, value);

        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        inflater.inflate(R.layout.stack_log_options, this, true);

        checkBox = (CheckBox) getChildAt(0);
        checkBox.setTextColor(Color.BLACK);

        int id = Resources.getSystem().getIdentifier("btn_check_holo_light", "drawable", "android");
        checkBox.setButtonDrawable(id);

        spinner = (Spinner) getChildAt(2);

        if (getSecondoryOptions.selected == Main.STACK_MODULE_ID) {
            adapter = ArrayAdapter.createFromResource(getContext(), R.array.stack_log_options, R.layout.spinner_item);
        } else if (getSecondoryOptions.selected == Main.SOC_MODULE_ID){
            adapter = ArrayAdapter.createFromResource(getContext(), R.array.soc_log_options, R.layout.spinner_item);
        }
        if (adapter != null)
        {
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            spinner.setAdapter(adapter);
        }

        setBackground(getResources().getDrawable(R.drawable.border_off));
        spinner.setEnabled(false);
        spinner.setClickable(false);
        spinner.setSelection(prevState);

        checkBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (!isChecked) {
                    setBackground(getResources().getDrawable(R.drawable.border_off));
                    spinner.setEnabled(false);
                    spinner.setClickable(false);
                } else {
                    setBackground(getResources().getDrawable(R.drawable.border));
                    spinner.setEnabled(true);
                    spinner.setClickable(true);
                }
            }
        });

        checkBox.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                getSecondoryOptions.init_soc = 0;
                if (!checkBox.isChecked()) {
                    prevState = state;
                    state = 0;
                    spinner.setSelection(0);
                } else {
                    //state = spinner.getSelectedItemPosition() + 1;
                    state = prevState;
                    spinner.setSelection(state);
                }
            }
        });

        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (checkBox.isChecked()) {
                    if(position == 0){
                        prevState=state;
                        checkBox.setChecked(false);
                    }
                }
                    state = position;
                    if (getSecondoryOptions.init_soc == 0)
                        sendIntent.transmitIntent(getContext(), state, getTag().toString(), optionSelected);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
    }

    public StackOptionsView(Context context){
        this(context,null);
    }

    public void setState(String state){
        //set the state of the view
        //Input: String with format - ("%s%s%s",prevState,prevState2,state)
        int prevState = Character.getNumericValue(state.charAt(0));
        int prevState2 = Character.getNumericValue(state.charAt(1));
        int presState = Character.getNumericValue(state.charAt(2));
        this.state=presState;
        this.prevState=prevState;
        this.prevState2=prevState2;
        if(presState==0){
            checkBox.setChecked(false);
            spinner.setSelection(0);
        }else{
            checkBox.setChecked(true);
            spinner.setSelection(presState);
        }
    }
}
