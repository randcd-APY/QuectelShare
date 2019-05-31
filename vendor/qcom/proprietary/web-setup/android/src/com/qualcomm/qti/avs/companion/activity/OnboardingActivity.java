/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.activity;

import android.net.nsd.NsdServiceInfo;
import android.os.Bundle;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import com.qualcomm.qti.avs.companion.R;
import com.qualcomm.qti.avs.companion.fragment.CustomDialogFragment;
import com.qualcomm.qti.avs.companion.fragment.SetupCompleteFragment;
import com.qualcomm.qti.avs.companion.fragment.SetupFragment;
import com.qualcomm.qti.avs.companion.fragment.SetupListFragment;
import com.qualcomm.qti.avs.companion.fragment.WiFiFragment;
import com.qualcomm.qti.avs.companion.manager.AVSNsdManager;
import com.qualcomm.qti.avs.companion.util.FragmentManagerUtils;
import com.qualcomm.qti.avs.companion.util.RequestAsyncTask;
import com.qualcomm.qti.avs.companion.util.Utils;

public class OnboardingActivity extends BaseActivity implements SetupFragment.SetupFragmentListener,
		SetupListFragment.SetupListFragmentListener, WiFiFragment.OnboardingErrorListener, AVSNsdManager.AVSServiceMetaDataListener {
	private static final String TAG = OnboardingActivity.class.getSimpleName();

	protected Toolbar mToolbar = null;
	protected MenuItem mMenuItem = null;

	private FragmentManagerUtils mFragmentManagerUtils = null;

	private static final String WIFI_FRAGMENT = "WIFI_FRAGMENT";
	private static final String SETUP_COMPLETE_FRAGMENT = "SETUP_COMPLETE_FRAGMENT";

	private static final String ERROR_WRONG_NETWORK_TAG = "ErrorWrongNetworkTag";
	private static final String ERROR_NO_AVS_TAG = "ErrorNoAVSTag";

	public static final String ONBOARDEE_NAME = "ONBOARDEE_NAME";

	private String mOnboardeeName = null;

	private WaitForAVSMetaData mWaitForAVSMetaData = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_onboarding);

		Bundle bundle = getIntent().getExtras();
		if (bundle != null) {
			mOnboardeeName = bundle.getString(ONBOARDEE_NAME);
		}

		mToolbar = (Toolbar) findViewById(R.id.onboarding_tool_bar);
		mToolbar.setTitle(getString(R.string.choose_wifi_network));
		setSupportActionBar(mToolbar);
		getSupportActionBar().setDisplayShowTitleEnabled(true);
		getSupportActionBar().setDisplayHomeAsUpEnabled(true);

		mToolbar.setNavigationOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				onBackPressed();
			}
		});

		mFragmentManagerUtils = new FragmentManagerUtils(getSupportFragmentManager(), R.id.onboarding_frame_layout);
		mFragmentManagerUtils.startFragment(WiFiFragment.newInstance(WIFI_FRAGMENT, mOnboardeeName), WIFI_FRAGMENT, false);
	}

	@Override
	public void onBackPressed() {
		mApp.getOnboardingManager().reconnectToPreviousNetwork(true);
		super.onBackPressed();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		mMenuItem = menu.getItem(0);
		if (WIFI_FRAGMENT.equals(mFragmentManagerUtils.getCurrentFragmentTag())) {
			mMenuItem.setVisible(true);
			mMenuItem.setIcon(ResourcesCompat.getDrawable(getResources(), R.drawable.ic_add, null));
		} else {
			mMenuItem.setVisible(false);
		}
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if (item.getItemId() == R.id.btn_action_bar) {
			WiFiFragment wifiFragment = (WiFiFragment) mFragmentManagerUtils.getCurrentFragment();
			if (wifiFragment != null) {
				wifiFragment.showAddWiFi();
			}
		}
		return super.onOptionsItemSelected(item);
	}

	@Override
	public void onItemClicked(String tag, Object object) {
		if (WIFI_FRAGMENT.equals(tag)) {
			mToolbar.setTitle(getString(R.string.setup_complete));
			mFragmentManagerUtils.startFragment(SetupCompleteFragment.newInstance(SETUP_COMPLETE_FRAGMENT, null),
					SETUP_COMPLETE_FRAGMENT, true);
			if (mMenuItem != null) {
				mMenuItem.setVisible(false);
			}
			getSupportActionBar().setDisplayHomeAsUpEnabled(false);
		}
	}

	@Override
	protected void onPause() {
		super.onPause();
		mApp.getAVSNsdManager().setAVSServiceMetaDataListener(null);
	}

	@Override
	public void onTopButtonClicked(String tag) {

	}

	@Override
	public void onMiddleButtonClicked(String tag) {

	}

	@Override
	public void onBottomButtonClicked(String tag) {
		if (SETUP_COMPLETE_FRAGMENT.equals(tag)) {
			getMeta();
		}
	}

	private void getMeta() {
		if (mWaitForAVSMetaData == null) {
			mWaitForAVSMetaData = new WaitForAVSMetaData();
			mWaitForAVSMetaData.execute();
		}
	}

	@Override
	public void onboardingError() {
		goBack(true);
	}

	private void goBack(final boolean reconnect) {
		mApp.getOnboardingManager().reconnectToPreviousNetwork(reconnect);
		finish();
	}

	@Override
	public void onWifiNetworkChanged(boolean on, String network) {
		if (WIFI_FRAGMENT.equals(mFragmentManagerUtils.getCurrentFragmentTag())) {
			WiFiFragment wifiFragment = (WiFiFragment) mFragmentManagerUtils.getCurrentFragment();
			if (!on) {
				showNoWifi();
			} else {
				if (Utils.isStringEmpty(network)) {
					if (!wifiFragment.isConnecting()) {
						showNoWifi();
					}
				} else {
					dismissNoWifi();

					if (!wifiFragment.isConnecting()) {
						if (!mOnboardeeName.equals(network)) {
							showWrongNetworkConnection(network);
						} else {
							dismissWrongNetworkConnection();
							wifiFragment.startScan();
						}
					}
				}
			}
		}
	}

	private void showWrongNetworkConnection(final String newtork) {
		CustomDialogFragment customDialogFragment = CustomDialogFragment
				.newDialog(ERROR_WRONG_NETWORK_TAG, getString(R.string.error),
						getString(R.string.wrong_network, mOnboardeeName, newtork),
						getString(R.string.ok), null);
		customDialogFragment.setOnCustomDialogDismissedListener(new CustomDialogFragment.OnCustomDialogDismissedListener() {
			@Override
			public void onPositiveButtonClicked(String tag) {
				goBack(false);
			}

			@Override
			public void onNegativeButtonClicked(String tag) {}

			@Override
			public void onDismissed(String tag) {}
		});
		mCustomDialogManager.showDialog(customDialogFragment, ERROR_WRONG_NETWORK_TAG);
	}

	private void showUnableToFindAVS() {
		CustomDialogFragment customDialogFragment = CustomDialogFragment
				.newDialog(ERROR_NO_AVS_TAG, getString(R.string.error),
						getString(R.string.unable_to_obtain_info, mOnboardeeName),
						getString(R.string.ok), null);
		customDialogFragment.setOnCustomDialogDismissedListener(new CustomDialogFragment.OnCustomDialogDismissedListener() {
			@Override
			public void onPositiveButtonClicked(String tag) {
				finish();
			}

			@Override
			public void onNegativeButtonClicked(String tag) {}

			@Override
			public void onDismissed(String tag) {}
		});
		mCustomDialogManager.showDialog(customDialogFragment, ERROR_NO_AVS_TAG);
	}

	private void dismissWrongNetworkConnection() {
		mCustomDialogManager.dismissDialog(ERROR_WRONG_NETWORK_TAG);
	}

	@Override
	public void onAVSServiceMetaDataSuccess(String name) {
		finish();
	}

	@Override
	public void onAVSServiceMetaDataFailed(String name) {
		showUnableToFindAVS();
	}

	private class WaitForAVSMetaData extends RequestAsyncTask implements AVSNsdManager.AVSDiscoveryListener {

		private static final int WAIT_AVS_TIMEOUT = 30000;

		private NsdServiceInfo mServiceInfo = null;

		public WaitForAVSMetaData() {
			super(OnboardingActivity.this, getString(R.string.searching), true);
		}

		@Override
		protected void onPreExecute() {
			super.onPreExecute();
			mApp.getAVSNsdManager().setAVSDiscoveryListener(this);
			mApp.getAVSNsdManager().setAVSServiceMetaDataListener(OnboardingActivity.this);
		}

		@Override
		protected Void doInBackground(Void... params) {
			mApp.getAVSNsdManager().startDiscovery();
			doWait(WAIT_AVS_TIMEOUT);
			return null;
		}

		@Override
		public void onAVSServiceFound(NsdServiceInfo serviceInfo) {
			if (serviceInfo.getServiceName().equals(mOnboardeeName)) {
				mServiceInfo = serviceInfo;
				finished();
			}
		}

		@Override
		public void onAVSServiceLost(NsdServiceInfo serviceInfo) {}

		@Override
		protected void onPostExecute(final Void param) {
			super.onPostExecute(param);
			mWaitForAVSMetaData = null;

			mApp.getAVSNsdManager().setAVSDiscoveryListener(null);
			mApp.getAVSNsdManager().stopDiscovery();
			if (mServiceInfo == null) {
				showUnableToFindAVS();
			} else {
				mApp.getAVSNsdManager().getAVSServiceInfoMetaData(OnboardingActivity.this, mServiceInfo);
			}
		}
	}
}
