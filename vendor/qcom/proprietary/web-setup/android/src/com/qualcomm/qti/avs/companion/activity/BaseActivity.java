/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.activity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Looper;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;

import com.qualcomm.qti.avs.companion.app.QTIAVSApplication;
import com.qualcomm.qti.avs.companion.fragment.CustomDialogFragment;
import com.qualcomm.qti.avs.companion.manager.CustomDialogManager;
import com.qualcomm.qti.avs.companion.manager.OnboardingManager;
import com.qualcomm.qti.avs.companion.util.Utils;

public class BaseActivity extends AppCompatActivity implements OnboardingManager.OnWifiNetworkChangedListener {

	protected boolean mIsSaveStateCalled = false;
	protected QTIAVSApplication mApp = null;
	protected CustomDialogManager mCustomDialogManager = null;

	protected final String NO_WIFI_DIALOG = "NoWifiDialog";

	public static final int MY_PERMISSIONS_REQUEST_ACCESS_COARSE_LOCATION = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		mIsSaveStateCalled = false;
		mApp = (QTIAVSApplication) getApplicationContext();
		mCustomDialogManager = new CustomDialogManager(this);
	}

	@Override
	protected void onStart() {
		super.onStart();
		mIsSaveStateCalled = false;
	}

	@Override
	protected void onResume() {
		super.onResume();
		mIsSaveStateCalled = false;

		mApp.getOnboardingManager().addOnWifiNetworkChangedListener(this);
	}

	@Override
	protected void onPause() {
		super.onPause();
		mIsSaveStateCalled = true;

		mApp.getOnboardingManager().removeOnWifiNetworkChangedListener(this);
	}

	public boolean isSaveStateCalled() {
		return mIsSaveStateCalled;
	}

	public CustomDialogManager getCustomDialogManager() {
		return mCustomDialogManager;
	}

	public QTIAVSApplication getApp() {
		return mApp;
	}

	protected void update() {}

	protected void updateInUiThread() {
		if (Looper.myLooper() == Looper.getMainLooper()) {
			update();
		} else {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					update();
				}
			});
		}
	}

	protected boolean hasAskedForAccessCoarseLocationPermission() {
		// We need to do a location permission check for Android M and above
		if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
			if ((ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) !=
					PackageManager.PERMISSION_GRANTED)) {
				if (!ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.ACCESS_COARSE_LOCATION)) {
					ActivityCompat.requestPermissions(this,
							new String[]{ Manifest.permission.ACCESS_COARSE_LOCATION },
							MY_PERMISSIONS_REQUEST_ACCESS_COARSE_LOCATION);
					return false;
				}
			}
		}
		return true;
	}

	@Override
	public void onWifiNetworkChanged(boolean on, String network) {
		if (!Utils.isStringEmpty(network)) {
			dismissNoWifi();
		} else {
			showNoWifi();
		}
	}

	protected void showNoWifi() {
		if (!mCustomDialogManager.isDialogShown(NO_WIFI_DIALOG)) {
			CustomDialogFragment customDialogFragment = CustomDialogFragment.newNoWifiDialog(NO_WIFI_DIALOG);
			mCustomDialogManager.showDialog(customDialogFragment, NO_WIFI_DIALOG);
		}
	}

	protected void dismissNoWifi() {
		if (mCustomDialogManager.isDialogShown(NO_WIFI_DIALOG)) {
			mCustomDialogManager.dismissDialog(NO_WIFI_DIALOG);
		}
	}
}
