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

import android.app.ActionBar;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.Space;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Switch;
import android.widget.Spinner;
import android.widget.Toast;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.Shader;
import java.util.Map;
import android.os.SystemProperties;

public class getSecondoryOptions extends Activity{
/*
    Class to handle the second activity
*/
    private Menu menu;
    public static int selected=0;
    public static int init_soc = 0;
    public static LinearLayout lv;
    public static RelativeLayout rl;
    public static ScrollView sv;
    public static String SOC_levels="";
    String mRome ="rome";
    String bt_soc_type = SystemProperties.get("qcom.bluetooth.soc");
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();
        String titlebarHeading = "Nothing yet";
        selected = intent.getFlags();

        setContentView(R.layout.activity_get_secondory_options);

        lv = (LinearLayout) findViewById(R.id.outer_box);
        sv = (ScrollView) findViewById(R.id.outer_scroll_box);
        lv.setTag(selected);
        lv.removeAllViews();

        Map<String,String> map;

        switch (selected) {
            case -1:
                Log.e(Main.TAG, "Option not received");
                break;
            case Main.PROFILE_MODULE_ID:
            //Profiles case
                Log.d(Main.TAG, "Profile button clicked");
                map=Utils.getPreviousSettings(getApplicationContext(),1);
                String[] profile_list = getResources().getStringArray(R.array.profile_list_names);
                String[] profile_tags = getResources().getStringArray(R.array.profile_list_tags);
                int counter=0;
                for (String profile:profile_list) {
                    ProfileOptionsView profileOptionsView = new ProfileOptionsView(this);
                   profileOptionsView.setTag(profile_tags[counter]);
                    CheckBox checkBox = (CheckBox) profileOptionsView.getChildAt(0);
                    checkBox.setText(profile);
                    if(!map.isEmpty()) {
                        Object state_obj = map.get(profile_tags[counter]);
                        if (state_obj != null)
                        {
                            String state= state_obj.toString();
                            profileOptionsView.setState(state);
                            Log.d(Main.TAG,state);
                            int presState = Character.getNumericValue(state.charAt(2));
                            sendIntent.transmitIntent(this, presState,profile_tags[counter], selected);
                        }
                    }
                    lv.addView(profileOptionsView);
                    Space space = new Space(this);
                    space.setMinimumHeight(20);
                    lv.addView(space);
                    counter++;
                }
                titlebarHeading = "Profile log levels";
                break;
            case Main.STACK_MODULE_ID:
            case Main.SOC_MODULE_ID:
            //Stack and SOC case
                Log.d(Main.TAG, "Stack button clicked");
                String[] list_names=null,list_tags=null;
                if(selected==Main.STACK_MODULE_ID){
                    list_names = getResources().getStringArray(R.array.stack_list_names);
                    list_tags = getResources().getStringArray(R.array.stack_list_tags);
                }else if(selected==Main.SOC_MODULE_ID){
                    list_names = getResources().getStringArray(R.array.soc_list_names);
                    list_tags = getResources().getStringArray(R.array.soc_list_tags);
                }
                map = Utils.getPreviousSettings(getApplicationContext(),selected);
                counter=0;
                SOC_levels="";
                for (String stack:list_names) {
                    StackOptionsView stackOptionsView = new StackOptionsView(this);
                    stackOptionsView.setTag(list_tags[counter]);
                    CheckBox checkBox = (CheckBox) stackOptionsView.getChildAt(0);
                    Spinner spinner = (Spinner) stackOptionsView.getChildAt(2);
                    checkBox.setText(stack);
                    if(!map.isEmpty()) {
                        Object state_obj = map.get(stack);
                        if (state_obj != null)
                        {
                            String state= state_obj.toString();
                            init_soc = 1;
                            stackOptionsView.setState(state);
                            init_soc = 0;
                            int presState = Character.getNumericValue(state.charAt(2));
                            if(selected==Main.SOC_MODULE_ID){
                                SOC_levels+=String.valueOf(presState);
                            }
                        }
                    }
                    lv.addView(stackOptionsView);
                    Space space = new Space(this);
                    space.setMinimumHeight(15);
                    lv.addView(space);
                    counter++;
                }
                if(selected==Main.STACK_MODULE_ID){
                    titlebarHeading = "Stack log levels";
                }else if(selected==Main.SOC_MODULE_ID){
                    if(Main.soc_log_enabled){
                        lv.setVisibility(View.VISIBLE);
                    }else{
                        lv.setVisibility(View.GONE);
                        if (bt_soc_type.equals(mRome))
                            Log.d(Main.TAG,"soc type is rome dont enable SOC logging");
                        else
                        {
                            sendIntent.transmitIntent(this, 0, "F", Main.SOC_ALL_MODULE_ID);
                            Log.d(Main.TAG,"soc type is not rome enable SOC logging");
                        }
                    }
                    titlebarHeading = "SOC log levels";
                }
                break;

            case Main.VSC_MODULE_ID:
                Log.d(Main.TAG, "VSC button clicked");
                titlebarHeading = "Send VS Command";
                VendorOptionView vendorOptionView = new VendorOptionView (this);
                lv.addView(vendorOptionView);
            break;
        }

        try{
            ActionBar actionBar = getActionBar();
            actionBar.setTitle(titlebarHeading);
        }
        catch (NullPointerException e){
            Log.e(Main.TAG,e.toString());
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_get_secondory_options, menu);

        MenuItem menuItem_save_start = menu.findItem(R.id.save_log_btn_start_2);
        MenuItem menuItem_save_stop = menu.findItem(R.id.save_log_btn_stop_2);
        MenuItem menuItem_set_all = menu.findItem(R.id.action_set_all);
        MenuItem menuItem_soc_enable = menu.findItem(R.id.action_soc_enable);

        menuItem_save_start.setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
        menuItem_save_stop.setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);

        if(Main.isLogging){
            menuItem_save_start.setVisible(false);
        }else{
            menuItem_save_stop.setVisible(false);
        }

        if(selected==Main.SOC_MODULE_ID){
            menuItem_save_start.setVisible(false);
            menuItem_save_stop.setVisible(false);
            menuItem_soc_enable.setVisible(true);
            Switch soc_switch = (Switch) menuItem_soc_enable.getActionView();
            soc_switch.setTextOn("On");
            soc_switch.setTextOff("Off");
            final TextView tv = new TextView(getApplicationContext());
            tv.setText("Enable SOC logging for options");
            tv.setTextSize(getResources().getDimension(R.dimen.textsize_large));
            soc_switch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    sv.removeAllViews();
                    if(isChecked){
                        lv.setVisibility(View.VISIBLE);
                        sv.addView(lv);
                        Main.soc_log_enabled=true;
                        Utils.saveGlobalState(getApplicationContext());
                        Toast.makeText(getApplicationContext(), "SOC logging turned on", Toast.LENGTH_SHORT).show();
                        if (bt_soc_type.equals(mRome))
                            Log.d(Main.TAG,"soc type is rome dont enable SOC logging");
                        else
                        {
                            sendIntent.transmitIntent(getApplicationContext(), 0, SOC_levels, Main.SOC_ALL_MODULE_ID);
                            Log.d(Main.TAG,"soc type is not rome enable SOC logging");
                        }
                        init_soc = 1;
                    }else{
                        Utils.saveCurrentState(getApplicationContext());
                        lv.setVisibility(View.GONE);
                        sv.addView(tv);
                        tv.setVisibility(View.VISIBLE);
                        Main.soc_log_enabled=false;
                        Utils.saveGlobalState(getApplicationContext());
                        if (bt_soc_type.equals(mRome))
                            Log.d(Main.TAG,"soc type is rome dont enable SOC logging");
                        else
                        {
                            sendIntent.transmitIntent(getApplicationContext(), 0, "F", Main.SOC_ALL_MODULE_ID);
                            Log.d(Main.TAG,"soc type is not rome enable SOC logging");
                        }
                    }
                }
            });
            soc_switch.setChecked(Main.soc_log_enabled);
            if(!Main.soc_log_enabled){
                sv.removeAllViews();
                sv.addView(tv);
            }
        }else{
            menuItem_soc_enable.setVisible(false);
        }

        if(selected!=Main.STACK_MODULE_ID){
            menuItem_set_all.setVisible(false);
        }else{
            if(Main.stack_set_all){
                menuItem_set_all.setTitle("Revert to previous state");
            }else{
                menuItem_set_all.setTitle("Set all to Verbose");
            }
        }

        this.menu=menu;
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_settings_2) {
            return true;
        }else if (id==android.R.id.home) {
            onBackPressed();
        }else if (id == R.id.save_log_btn_stop_2) {
            Log.d(Main.TAG,"Stop button clicked");
            MenuItem newItem = menu.findItem(R.id.save_log_btn_start_2);
            Main.isLogging=false;
            item.setVisible(false);
            newItem.setVisible(true);
            Utils.stopLogging(this);
            Utils.saveGlobalState(getApplicationContext());
        }else if (id == R.id.save_log_btn_start_2) {
            Log.d(Main.TAG,"Start button clicked");
            MenuItem newItem = menu.findItem(R.id.save_log_btn_stop_2);
            Main.isLogging=true;
            item.setVisible(false);
            newItem.setVisible(true);
            Utils.startLogging(this);
            Utils.saveGlobalState(getApplicationContext());
            Toast toast = Toast.makeText(this,"Started logs", Toast.LENGTH_SHORT);
            toast.show();
        }else if (id==R.id.action_set_all){
            if(!item.getTitle().equals("Set all to Verbose")){
                String[] stack_list_tags = getResources().getStringArray(R.array.stack_list_tags);
                for (String stack : stack_list_tags) {
                    StackOptionsView stackOptionsView = (StackOptionsView) findViewById(R.id.outer_box).findViewWithTag(stack);
                    CheckBox checkBox = (CheckBox) stackOptionsView.getChildAt(0);
                    Spinner spinner = (Spinner) stackOptionsView.getChildAt(2);
                    if(stackOptionsView.prevState2==0){
                        checkBox.setChecked(false);
                        stackOptionsView.state=0;
                        spinner.setSelection(stackOptionsView.state);
                    }else{
                        stackOptionsView.state=stackOptionsView.prevState2;
                        checkBox.setChecked(true);
                        spinner.setSelection(stackOptionsView.state);
                    }
                }
                Main.stack_set_all=false;
                Utils.saveGlobalState(getApplicationContext());
                item.setTitle("Set all to Verbose");
            }else {
                String[] stack_list_tags = getResources().getStringArray(R.array.stack_list_tags);
                for (String stack : stack_list_tags) {
                    StackOptionsView stackOptionsView = (StackOptionsView) findViewById(R.id.outer_box).findViewWithTag(stack);
                    stackOptionsView.prevState2 = stackOptionsView.state;
                    stackOptionsView.state=6;
                    CheckBox checkBox = (CheckBox) stackOptionsView.getChildAt(0);
                    Spinner spinner = (Spinner) stackOptionsView.getChildAt(2);
                    checkBox.setChecked(true);
                    spinner.setSelection(6);
                }
                Main.stack_set_all=true;
                Utils.saveGlobalState(getApplicationContext());
                item.setTitle("Revert to previous state");
            }
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        Utils.saveCurrentState(getApplicationContext());
        selected=0;
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.d(Main.TAG, "In getSecondaryOptions ONPAUSE");
        Utils.saveCurrentState(getApplicationContext());
    }
}


