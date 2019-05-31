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

package org.codeaurora.bluetooth.hiddtestapp;

import android.app.ActionBar;
import android.app.ActionBar.Tab;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.bluetooth.BluetoothDevice;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

public class MainActivity extends Activity implements HidWrapperService.HidEventListener {

    private final String TAG = MainActivity.class.getSimpleName();

    private HidWrapperService mHidWrapper;

    private boolean mAppReady = false;

    private boolean mConnected = false;

    private boolean mBootMode = false;

    private boolean mNumLockLed = false;

    private boolean mCapsLockLed = false;

    private boolean mScrollLockLed = false;

    public static class TabListener<T extends Fragment> implements ActionBar.TabListener {

        private Fragment mFragment;

        private final Activity mActivity;

        private final String mTag;

        private final Class<T> mClass;

        private final int mParam;

        public TabListener(Activity activity, String tag, Class<T> clz) {
            this(activity, tag, clz, 0);
        }

        public TabListener(Activity activity, String tag, Class<T> clz, int param) {
            mActivity = activity;
            mTag = tag;
            mClass = clz;
            mParam = param;
        }

        @Override
        public void onTabSelected(Tab tab, FragmentTransaction ft) {
            if (mFragment == null) {
                Bundle args = new Bundle();
                args.putInt("param", mParam);

                mFragment = Fragment.instantiate(mActivity, mClass.getName(), args);
                ft.add(R.id.fragment_container, mFragment, mTag);
            } else {
                ft.attach(mFragment);
            }
        }

        @Override
        public void onTabUnselected(Tab tab, FragmentTransaction ft) {
            if (mFragment != null) {
                ft.detach(mFragment);
            }
        }

        @Override
        public void onTabReselected(Tab tab, FragmentTransaction ft) {
            // do nothing
        }
    }

    private ServiceConnection mConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            mHidWrapper = ((HidWrapperService.LocalBinder) service).getService();
            mHidWrapper.setEventListener(MainActivity.this);
        }

        @Override
        public void onServiceDisconnected(ComponentName className) {
            mHidWrapper = null;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.v(TAG, "onCreate");

        setContentView(R.layout.activity_main);

        ActionBar ab = getActionBar();
        ab.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);

        Tab tab;

        tab = ab.newTab();
        tab.setText(R.string.tab_mouse);
        tab.setTabListener(new TabListener<MouseFragment>(this, "mouse", MouseFragment.class));
        ab.addTab(tab);

        tab = ab.newTab();
        tab.setText(R.string.tab_keyboard_qwerty);
        tab.setTabListener(new TabListener<KeyboardFragment>(this, "kbd-qwerty",
                KeyboardFragment.class, KeyboardFragment.KEYBOARD_TYPE_QWERTY));
        ab.addTab(tab);

        tab = ab.newTab();
        tab.setText(R.string.tab_keyboard_numeric);
        tab.setTabListener(new TabListener<KeyboardFragment>(this, "kbd-num",
                KeyboardFragment.class, KeyboardFragment.KEYBOARD_TYPE_NAVIGATION_AND_NUMERIC));
        ab.addTab(tab);

        if (savedInstanceState != null) {
            ab.setSelectedNavigationItem(savedInstanceState.getInt("tab", 0));
        }

        Intent intent = new Intent(this, HidWrapperService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        Log.v(TAG, "onDestroy");

        if (mHidWrapper != null) {
            unbindService(mConnection);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.actions, menu);

        BluetoothDevice plugdev = (mHidWrapper != null ? mHidWrapper.getPluggedDevice() : null);

        MenuItem piph = menu.findItem(R.id.action_pluginfo_placeholder);
        if (plugdev != null) {
            String name = plugdev.getName();

            if (name == null) {
                name = plugdev.getAddress();
            }

            piph.setTitle(getResources().getString(R.string.plugged, name));
        } else {
            piph.setTitle(R.string.unplugged);
        }

        menu.findItem(R.id.action_proto_boot).setVisible(mBootMode);
        menu.findItem(R.id.action_proto_report).setVisible(!mBootMode);

        menu.findItem(R.id.action_connect).setVisible(!mConnected);
        menu.findItem(R.id.action_disconnect).setVisible(mConnected);

        menu.findItem(R.id.action_connect).setEnabled(mAppReady && plugdev != null);
        menu.findItem(R.id.action_disconnect).setEnabled(mAppReady && plugdev != null);

        menu.findItem(R.id.action_unplug).setEnabled(mConnected && mAppReady);

        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_connect:
                mHidWrapper.connect();
                return true;

            case R.id.action_disconnect:
                mHidWrapper.disconnect();
                return true;

            case R.id.action_unplug:
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setTitle(R.string.unplug_title).setMessage(R.string.unplug_message);
                builder.setPositiveButton(android.R.string.yes,
                        new DialogInterface.OnClickListener() {

                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                mHidWrapper.unplug();
                            }
                        });
                builder.setNegativeButton(android.R.string.no, null);
                builder.create().show();
                return true;
        }

        return super.onContextItemSelected(item);
    }

    HidWrapperService getHidWrapper() {
        return mHidWrapper;
    }

    @Override
    public void onKeyboardLedState(boolean numLock, boolean capsLock, boolean scrollLock) {
        Log.v(TAG, "onKeyboardLedState(): numLock=" + numLock + " capsLock=" + capsLock
                + " scrollLock=" + scrollLock);

        mNumLockLed = numLock;
        mCapsLockLed = capsLock;
        mScrollLockLed = scrollLock;

        // TODO: make tags constants or sth
        for (String tag : new String[] {
                "kbd-qwerty", "kbd-num"
        }) {
            KeyboardFragment fr = (KeyboardFragment) getFragmentManager().findFragmentByTag(tag);

            if (fr != null) {
                fr.updateLeds(numLock, capsLock, scrollLock);
            }
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putInt("tab", getActionBar().getSelectedNavigationIndex());
    }

    @Override
    public void onApplicationState(boolean registered) {
        mAppReady = registered;
        invalidateOptionsMenu();
    }

    @Override
    public void onConnectionState(BluetoothDevice device, boolean connected) {
        mConnected = connected;
        invalidateOptionsMenu();
    }

    @Override
    public void onProtocolModeState(boolean bootMode) {
        mBootMode = bootMode;
        invalidateOptionsMenu();
    }

    @Override
    public void onPluggedDeviceChanged(BluetoothDevice device) {
        invalidateOptionsMenu();
    }

    void queryLeds(KeyboardFragment fr) {
        fr.updateLeds(mNumLockLed, mCapsLockLed, mScrollLockLed);
    }
}
