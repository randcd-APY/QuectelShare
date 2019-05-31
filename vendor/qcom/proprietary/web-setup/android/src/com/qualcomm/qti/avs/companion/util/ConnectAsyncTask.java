/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.util;

import android.util.Log;

import com.qualcomm.qti.avs.companion.R;
import com.qualcomm.qti.avs.companion.activity.BaseActivity;
import com.qualcomm.qti.avs.companion.manager.OnboardingManager;

public class ConnectAsyncTask extends RequestAsyncTask implements OnboardingManager.OnWifiOnboardeeConnectionStateChangedListener {

	private static final int CONNECTING_TO_DEVICE_WAIT_TIME = 60000;
	private static final String TAG = ConnectAsyncTask.class.getSimpleName();
	private final WifiOnboardee mWifiOnboardee;
	private final String mPassword;
	private final boolean mHidden;

	private OnConnectPostExecute mOnConnectPostExecute = null;

	public ConnectAsyncTask(final BaseActivity baseActivity, final WifiOnboardee wifiOnboardee,
							final String password, final boolean showDialog, final boolean hidden) {
		super(baseActivity, baseActivity.getString(R.string.connecting), showDialog);
		mWifiOnboardee = wifiOnboardee;
		mPassword = password;
		mHidden = hidden;
	}

	public void setOnConnectPostExecute(final OnConnectPostExecute listener) {
		mOnConnectPostExecute = listener;
	}

	@Override
	protected void onPreExecute() {
		super.onPreExecute();
		final BaseActivity baseActivity = mActivityRef.get();
		if (Utils.isActivityActive(baseActivity)) {
			baseActivity.getApp().getOnboardingManager().addOnWifiOnboardeeConnectionStateChangedListener(this);
		}
	}

	@Override
	protected Void doInBackground(Void... params) {
		final BaseActivity baseActivity = mActivityRef.get();
		if (baseActivity != null) {
			if (baseActivity.getApp().getOnboardingManager().connect(mWifiOnboardee, mPassword, mHidden)) {
				if (mWifiOnboardee.getConnectionState() == WifiOnboardee.ConnectionState.CONNECTING) {
					Log.d(TAG,"wait");
					doWait(CONNECTING_TO_DEVICE_WAIT_TIME);
				}
			}
		}
		return null;
	}

	@Override
	protected void onPostExecute(final Void param) {
		super.onPostExecute(param);

		final BaseActivity baseActivity = mActivityRef.get();
		if (!Utils.isActivityActive(baseActivity)) {
			return;
		}

		baseActivity.getApp().getOnboardingManager().removeOnWifiOnboardeeConnectionStateChangedListener(this);

		if (mOnConnectPostExecute != null) {
			mOnConnectPostExecute.onConnectPostExecute(mWifiOnboardee);
		}
	}

	@Override
	public void onWifiOnboardeeConnectionStateChanged(WifiOnboardee wifiOnboardee, WifiOnboardee.ConnectionState connectionState) {
		if (mWifiOnboardee.equals(wifiOnboardee)) {
			if (!connectionState.equals(WifiOnboardee.ConnectionState.CONNECTING)) {
				Log.d(TAG,"connection state " + connectionState);
				finished();
			}
		}
	}

	public interface OnConnectPostExecute {
		void onConnectPostExecute(final WifiOnboardee wifiOnboardee);
	}
}
