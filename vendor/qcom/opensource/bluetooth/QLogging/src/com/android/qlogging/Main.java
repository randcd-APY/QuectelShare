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
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;
import android.widget.Switch;
import android.widget.CompoundButton;
import android.content.pm.PackageManager;
import android.content.ComponentName;
import android.widget.TextView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
public class Main extends Activity {
/*
Class to handle the Main activity
*/
    public final static String TAG = "QLoggingDebug";
    public static boolean isLogging=false;
    private Menu menu;
    public static boolean stack_set_all = false;
    public static boolean bt_log_filter = false;
    public static boolean soc_log_enabled = false;
    public final static int PROFILE_MODULE_ID = 1;
    public final static int STACK_MODULE_ID = 2;
    public final static int SOC_MODULE_ID = 3;
    public final static int SOC_ALL_MODULE_ID = 4;
    public final static int VSC_MODULE_ID = 5;
    public static int log_file_size = 100;
    public static int size_pos = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        if(!Utils.isAppRunning(getApplicationContext(),"org.codeaurora.bluetooth.btlogsave")){
            Log.d(TAG,"Opening BtLogSave app");
            Intent nextIntent = new Intent(Intent.ACTION_MAIN);
            nextIntent.setComponent(new ComponentName("org.codeaurora.bluetooth.btlogsave","org.codeaurora.bluetooth.btlogsave.Main"));
            startActivity(nextIntent);
            Intent intent = new Intent();
            intent.setAction("BTLogSaveIntent");
            intent.putExtra(Intent.EXTRA_TEXT, "LOG_FILE_SIZE");
            intent.putExtra("FILE_SIZE", String.valueOf(log_file_size));
            sendBroadcast(intent);
        }
        //If the app isnt already installed, create the default settings files
        //with the default log levels.
        SharedPreferences ratePrefs = getSharedPreferences("First Update", 0);
        if (!ratePrefs.getBoolean("FirstTime", false)) {
            Utils.setDefaultLogLevels(this,PROFILE_MODULE_ID);
            Utils.setDefaultLogLevels(this,STACK_MODULE_ID);
            Utils.setDefaultLogLevels(this,SOC_MODULE_ID);
            SharedPreferences.Editor edit = ratePrefs.edit();
            edit.putBoolean("FirstTime", true);
            edit.commit();
        }
        Utils.setGlobalSettings(this);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);

        MenuItem menuItem_save_start = menu.findItem(R.id.action_save_log_btn_start);
        MenuItem menuItem_save_stop = menu.findItem(R.id.action_save_log_btn_stop);
        //MenuItem menuItem_btlog_filter= menu.findItem(R.id.action_btlog_filter);

        menuItem_save_start.setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
        menuItem_save_stop.setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
        //menuItem_btlog_filter.setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);

        if(isLogging){
            menuItem_save_start.setVisible(false);
        }else{
            menuItem_save_stop.setVisible(false);
        }

        /*Switch log_filter_switch= (Switch) menuItem_btlog_filter.getActionView();
        log_filter_switch.setTextOn("BT");
        log_filter_switch.setTextOff("All");

        log_filter_switch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked){
                    bt_log_filter=true;
                    Utils.saveGlobalState(getApplicationContext());
                    Toast.makeText(getApplicationContext(), "BT Log filter ON", Toast.LENGTH_SHORT).show();
                }else{
                    bt_log_filter=false;
                    Utils.saveGlobalState(getApplicationContext());
                }
            }
        });
        log_filter_switch.setChecked(bt_log_filter);*/

        MenuItem menuItem_log_file_title = menu.findItem(R.id.action_log_file_title);
        menuItem_log_file_title.setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);

        TextView log_file_title = (TextView) menuItem_log_file_title.getActionView();
        log_file_title.setText("Size(MB):");

        MenuItem menuItem_log_file_size = menu.findItem(R.id.action_log_file_size);
        menuItem_log_file_size.setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);

        Spinner spinner = (Spinner) menuItem_log_file_size.getActionView();

        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(getApplicationContext(), R.array.log_file_size_options, R.layout.spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        spinner.setAdapter(adapter);

        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                Intent intent = new Intent();
                intent.setAction("BTLogSaveIntent");
                intent.putExtra(Intent.EXTRA_TEXT, "LOG_FILE_SIZE");
                intent.putExtra("FILE_SIZE", parent.getItemAtPosition(position).toString());
                getApplicationContext().sendBroadcast(intent);
                log_file_size = Integer.valueOf(parent.getItemAtPosition(position).toString());
                size_pos = position;
                Utils.saveGlobalState(getApplicationContext());
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        spinner.setSelection(size_pos);


        this.menu=menu;
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_settings) {
            return true;
        }
        else if (id == R.id.action_save_log_btn_stop) {
            MenuItem newItem = menu.findItem(R.id.action_save_log_btn_start);
            isLogging=false;
            item.setVisible(false);
            newItem.setVisible(true);
            Utils.stopLogging(this);
            Utils.saveGlobalState(getApplicationContext());
        }
        else if (id == R.id.action_save_log_btn_start) {
            MenuItem newItem = menu.findItem(R.id.action_save_log_btn_stop);
            isLogging=true;
            item.setVisible(false);
            newItem.setVisible(true);
            Utils.startLogging(this);
            Utils.saveGlobalState(getApplicationContext());
            Toast toast = Toast.makeText(this,"Started logs", Toast.LENGTH_SHORT);
            toast.show();
        }

        return super.onOptionsItemSelected(item);

    }

    public void getOptions(View view){
    /*
        Onclick handler for the buttons in the main page.
    */
        Intent intent = new Intent(this,getSecondoryOptions.class);
        int selected = -1;
        switch (view.getId()) {

            case R.id.profile_button:
                selected = PROFILE_MODULE_ID;
                Log.v(TAG,"Profile button clicked");
                break;
            case R.id.stack_button:
                selected = STACK_MODULE_ID;
                Log.v(TAG,"Stack button clicked");
                break;
            case R.id.java_button:
                selected = SOC_MODULE_ID;
                Log.v(TAG,"SOC button clicked");
                break;
            case R.id.m_vsc_button:
                selected = VSC_MODULE_ID;
                Log.v(TAG,"VSC button clicked");
                break;
        }
        intent.addFlags(selected);
        startActivity(intent);
    }
}
