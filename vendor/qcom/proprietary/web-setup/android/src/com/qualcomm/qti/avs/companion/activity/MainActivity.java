/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.activity;

import android.content.Context;
import android.content.Intent;
import android.net.nsd.NsdServiceInfo;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.qualcomm.qti.avs.companion.R;
import com.qualcomm.qti.avs.companion.fragment.CustomDialogFragment;
import com.qualcomm.qti.avs.companion.manager.AVSNsdManager;
import com.qualcomm.qti.avs.companion.manager.OnboardingManager;
import com.qualcomm.qti.avs.companion.util.ConnectAsyncTask;
import com.qualcomm.qti.avs.companion.util.Utils;
import com.qualcomm.qti.avs.companion.util.WifiOnboardee;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class MainActivity extends BaseActivity implements OnboardingManager.OnWifiOnboardeesChangedListener,
		AdapterView.OnItemClickListener, AVSNsdManager.AVSDiscoveryListener, AVSNsdManager.AVSServiceMetaDataListener {
	public static final String TAG = MainActivity.class.getSimpleName();

	private static final String ERROR_UNABLE_TO_CONNECT_TAG = "ErrorUnableToConnectTag";
	private static final String ERROR_UNABLE_TO_OBTAIN_INFO_TAG = "ErrorUnableToObtainInfoTag";

	View mEmptyView = null;
	TextView mEmptyText = null;
	ListView mListView = null;
	DataAdapter mAdapter = null;

	private ConnectAsyncTask mConnectTask = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		mListView = (ListView) findViewById(R.id.main_list_view);
		mListView.setOnItemClickListener(this);

		mEmptyView = findViewById(R.id.main_empty_view);
		mEmptyText = (TextView) findViewById(R.id.main_empty_view_text);

		Toolbar toolbar = (Toolbar) findViewById(R.id.main_tool_bar);
		setSupportActionBar(toolbar);
		getSupportActionBar().setDisplayShowTitleEnabled(true);
	}

	@Override
	protected void onResume() {
		super.onResume();

		if (!hasAskedForAccessCoarseLocationPermission()) {
			return;
		}

		mApp.getOnboardingManager().addOnWifiOnboardeesChangedListener(this);
		mApp.getOnboardingManager().startScan();

		mApp.getAVSNsdManager().setAVSDiscoveryListener(this);
		mApp.getAVSNsdManager().setAVSServiceMetaDataListener(this);
		mApp.getAVSNsdManager().startDiscovery();

		update();
	}

	@Override
	protected void onPause() {
		super.onPause();

		mApp.getOnboardingManager().removeOnWifiOnboardeesChangedListener(this);
		mApp.getOnboardingManager().stopScan();

		mApp.getAVSNsdManager().setAVSDiscoveryListener(null);
		mApp.getAVSNsdManager().setAVSServiceMetaDataListener(null);
		mApp.getAVSNsdManager().stopDiscovery();
	}

	@Override
	protected void update() {
		if (!Utils.isActivityActive(this)) {
			return;
		}

		mListView.setVisibility(View.VISIBLE);
		mEmptyView.setVisibility(View.GONE);

		if (mAdapter == null) {
			mAdapter = new DataAdapter();
			mListView.setAdapter(mAdapter);
		}

		List<SetupInfo> setupInfos = new ArrayList<>();
		List<NsdServiceInfo> serviceInfos = mApp.getAVSNsdManager().getServiceInfos();
		List<WifiOnboardee> wifiOnboardees = mApp.getOnboardingManager().getWifiOnboardees();

		final String currentNetworkName = mApp.getOnboardingManager().getCurrentSSID();
		Set<String> serviceInfoSet = new HashSet<>();
		if (currentNetworkName != null) {
			for (NsdServiceInfo serviceInfo : serviceInfos) {
				if (!serviceInfo.getServiceName().equals(currentNetworkName)) {
					setupInfos.add(new SetupInfo(serviceInfo.getServiceName(), false));
					serviceInfoSet.add(serviceInfo.getServiceName());
				}
			}
		}

		for (WifiOnboardee wifiOnboardee : wifiOnboardees) {
			if (!serviceInfoSet.contains(wifiOnboardee.getName())) {
				setupInfos.add(new SetupInfo(wifiOnboardee.getName(), true));
			}
		}

		mAdapter.update(setupInfos);

		mListView.setVisibility((setupInfos.isEmpty()) ? View.GONE : View.VISIBLE);
		mEmptyView.setVisibility((setupInfos.isEmpty()) ? View.VISIBLE : View.GONE);

		if (setupInfos.isEmpty()) {
			mEmptyText.setText(getString(R.string.searching));
		}
	}

	@Override
	public void onWifiOnboardeesChanged() {
		updateInUiThread();
	}

	@Override
	public void onWifiNetworkChanged(boolean on, String network) {
		if (mConnectTask != null) {
			return;
		}
		if (on) {
			// as long as it is on, we can scan for soft aps
			dismissNoWifi();
		} else {
			showNoWifi();
		}
	}

	private void getMeta(final NsdServiceInfo serviceInfo) {
		mApp.getAVSNsdManager().getAVSServiceInfoMetaData(this, serviceInfo);
	}

	private void connectWifiOnboardee(final WifiOnboardee wifiOnboardee) {
		if (mConnectTask == null) {
			mConnectTask = new ConnectAsyncTask(this, wifiOnboardee, null, true, false);
			mConnectTask.setOnConnectPostExecute(new ConnectAsyncTask.OnConnectPostExecute() {
				@Override
				public void onConnectPostExecute(final WifiOnboardee postWifiOnboardee) {
					if (wifiOnboardee.getName().equals(postWifiOnboardee.getName()) &&
							(postWifiOnboardee.getConnectionState() == WifiOnboardee.ConnectionState.CONNECTED)) {
						Intent intent = new Intent(MainActivity.this, OnboardingActivity.class);
						Bundle bundle = new Bundle();
						bundle.putString(OnboardingActivity.ONBOARDEE_NAME, wifiOnboardee.getName());
						intent.putExtras(bundle);
						startActivity(intent);
					} else {
						showUnableToConnect(wifiOnboardee);
					}
					mConnectTask = null;
				}
			});
			mConnectTask.execute();
		}
	}

	private void showUnableToConnect(final WifiOnboardee wifiOnboardee) {
		CustomDialogFragment customDialogFragment = CustomDialogFragment
				.newDialog(ERROR_UNABLE_TO_CONNECT_TAG, getString(R.string.error),
						getString(R.string.unable_to_connect, getString(R.string.app_name), wifiOnboardee.getName()),
						getString(R.string.ok), null);
		mCustomDialogManager.showDialog(customDialogFragment, ERROR_UNABLE_TO_CONNECT_TAG);
	}

	private void showUnableToObtainInfo(final String serviceName) {
		CustomDialogFragment customDialogFragment = CustomDialogFragment
				.newDialog(ERROR_UNABLE_TO_OBTAIN_INFO_TAG, getString(R.string.error),
						getString(R.string.unable_to_obtain_info, serviceName),
						getString(R.string.ok), null);
		mCustomDialogManager.showDialog(customDialogFragment, ERROR_UNABLE_TO_OBTAIN_INFO_TAG);
	}

	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
		final SetupInfo setupInfo = (SetupInfo) mAdapter.getItem(position);

		if (setupInfo.softAP) {
			List<WifiOnboardee> wifiOnboardees = mApp.getOnboardingManager().getWifiOnboardees();
			for (WifiOnboardee wifiOnboardee : wifiOnboardees) {
				if ((wifiOnboardee != null) && setupInfo.name.equals(wifiOnboardee.getName())) {
					connectWifiOnboardee(wifiOnboardee);
					break;
				}
			}
		} else {
			List<NsdServiceInfo> serviceInfos = mApp.getAVSNsdManager().getServiceInfos();
			for (NsdServiceInfo serviceInfo : serviceInfos) {
				if ((serviceInfo != null) && setupInfo.name.equals(serviceInfo.getServiceName())) {
					getMeta(serviceInfo);
					break;
				}
			}
		}
	}

	@Override
	public void onAVSServiceFound(NsdServiceInfo serviceInfo) {
		updateInUiThread();
	}

	@Override
	public void onAVSServiceLost(NsdServiceInfo serviceInfo) {
		updateInUiThread();
	}

	@Override
	public void onAVSServiceMetaDataSuccess(String name) {}

	@Override
	public void onAVSServiceMetaDataFailed(final String name) {
		showUnableToObtainInfo(name);
	}

	private class SetupInfo {
		public String name;
		public boolean softAP;

		public SetupInfo(final String name, final boolean softAP) {
			this.name = name;
			this.softAP = softAP;
		}
	}

	private class DataAdapter extends BaseAdapter {
		List<SetupInfo> mSetupInfos = new ArrayList<>();

		public void update(final List<SetupInfo> setupInfos) {
			synchronized (mSetupInfos) {
				mSetupInfos.clear();
				if (setupInfos != null) {
					mSetupInfos.addAll(setupInfos);
				}
			}

			notifyDataSetChanged();
		}

		@Override
		public int getCount() {
			return mSetupInfos.size();
		}

		@Override
		public Object getItem(int position) {
			return mSetupInfos.get(position);
		}

		@Override
		public long getItemId(int position) {
			return position;
		}

		@Override
		public boolean hasStableIds() {
			return false;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			if (convertView == null) {
				LayoutInflater layoutInflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				convertView = layoutInflater.inflate(R.layout.list_item, parent, false);
			}

			TextView textView = (TextView) convertView.findViewById(R.id.list_item_text);

			final SetupInfo setupInfo = (SetupInfo) getItem(position);
			textView.setText(setupInfo.name);

			return convertView;
		}
	}
}
