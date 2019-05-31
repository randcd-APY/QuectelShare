/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.manager;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.DhcpInfo;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkInfo;
import android.net.NetworkRequest;
import android.net.wifi.ScanResult;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.support.v4.content.ContextCompat;
import android.util.Log;

import com.qualcomm.qti.avs.companion.util.Utils;
import com.qualcomm.qti.avs.companion.util.WifiOnboardee;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class OnboardingManager {
	final private static String TAG = OnboardingManager.class.getSimpleName();

	final static String HEX_PATTERN = "[\\dA-Fa-f]+";

	final private static long WIFI_SCAN_TIMEOUT = 10000;
	final private static long CONNECT_TIMEOUT = 60000;

	private Context mContext = null;
	private WifiManager mWifiManager = null;
	private WifiScanReceiver mWifiScanReceiver = null;
	private WifiReceiver mWifiReceiver = null;

	private Map<String, WifiOnboardee> mOnboardeesMap = null;
	private Handler mHandler = null;

	private List<OnWifiOnboardeesChangedListener> mOnWifiOnboardeesChangedListeners = new ArrayList<>();
	private List<OnWifiOnboardeeConnectionStateChangedListener> mOnWifiOnboardeeConnectionStateChangedListeners = new ArrayList<>();
	private List<OnWifiNetworkChangedListener> mOnWifiNetworkChangedListeners = new ArrayList<>();

	private WifiConfiguration mPreviousWifiConfiguration = null;
	private String mCurrentSSID = null;
	private String mServerIP = null;
	private boolean mWifiOn = false;

	private WifiOnboardee mTargetConnectOnboardee;
	private WifiOnboardee mConnectedOnboardee;

	private boolean mNetworkParsed = false;
	private boolean mNetworkAvailable = false;

	private final Runnable mScanWifiRunnable = new Runnable() {
		@Override
		public void run() {
			synchronized (this) {
				if (mWifiScanReceiver == null) {
					return;
				}
				mWifiManager.startScan();
				mHandler.postDelayed(mScanWifiRunnable, WIFI_SCAN_TIMEOUT);
			}
		}
	};

	private final Runnable mConnectOnboardeeTimeoutRunnable = new Runnable() {
		@Override
		public void run() {
			if (mTargetConnectOnboardee == null) {
				return;
			}
			notifyOnWifiOnboardeeConnectionStateChangedListeners(mTargetConnectOnboardee,
					WifiOnboardee.ConnectionState.CONNECTING_TIMEOUT);
			reconnectToPreviousNetwork(true);

			mTargetConnectOnboardee = null;
		}
	};

	public OnboardingManager(final Context context) {
		mContext = context;
		mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
		mOnboardeesMap = new ConcurrentHashMap<>();
		mHandler = new Handler(Looper.getMainLooper());

		mCurrentSSID = parseWifiName(mContext);

		mWifiReceiver = new WifiReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
		filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
		filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
		mContext.registerReceiver(mWifiReceiver, filter);
	}

	public void addOnWifiOnboardeesChangedListener(final OnWifiOnboardeesChangedListener listener) {
		synchronized (mOnWifiOnboardeesChangedListeners) {
			if ((listener != null) && !mOnWifiOnboardeesChangedListeners.contains(listener)) {
				mOnWifiOnboardeesChangedListeners.add(listener);
			}
		}
	}

	public void removeOnWifiOnboardeesChangedListener(final OnWifiOnboardeesChangedListener listener) {
		synchronized (mOnWifiOnboardeesChangedListeners) {
			if (listener != null) {
				mOnWifiOnboardeesChangedListeners.remove(listener);
			}
		}
	}

	public void addOnWifiOnboardeeConnectionStateChangedListener(final OnWifiOnboardeeConnectionStateChangedListener listener) {
		synchronized (mOnWifiOnboardeeConnectionStateChangedListeners) {
			if ((listener != null) && !mOnWifiOnboardeeConnectionStateChangedListeners.contains(listener)) {
				mOnWifiOnboardeeConnectionStateChangedListeners.add(listener);
			}
		}
	}

	public void removeOnWifiOnboardeeConnectionStateChangedListener(final OnWifiOnboardeeConnectionStateChangedListener listener) {
		synchronized (mOnWifiOnboardeeConnectionStateChangedListeners) {
			if (listener != null) {
				mOnWifiOnboardeeConnectionStateChangedListeners.remove(listener);
			}
		}
	}

	public void addOnWifiNetworkChangedListener(final OnWifiNetworkChangedListener listener) {
		synchronized (mOnWifiNetworkChangedListeners) {
			if ((listener != null) && !mOnWifiNetworkChangedListeners.contains(listener)) {
				mOnWifiNetworkChangedListeners.add(listener);
				if (mNetworkParsed) {
					listener.onWifiNetworkChanged(mWifiOn, mCurrentSSID);
				}
			}
		}
	}

	public void removeOnWifiNetworkChangedListener(final OnWifiNetworkChangedListener listener) {
		synchronized (mOnWifiNetworkChangedListeners) {
			if (listener != null) {
				mOnWifiNetworkChangedListeners.remove(listener);
			}
		}
	}

	public String getCurrentSSID() {
		synchronized (this) {
			return mCurrentSSID;
		}
	}

	public String getServerIP() {
		synchronized (this) {
			return mServerIP;
		}
	}

	public void startScan() {
		synchronized (this) {
			if (mWifiScanReceiver != null) {
				return;
			}
			if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
				if ((ContextCompat.checkSelfPermission(mContext, Manifest.permission.ACCESS_COARSE_LOCATION) !=
						PackageManager.PERMISSION_GRANTED)) {
					return;
				}
			}
			mWifiScanReceiver = new WifiScanReceiver();
			IntentFilter filter = new IntentFilter();
			filter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
			mContext.registerReceiver(mWifiScanReceiver, filter);

			mWifiManager.startScan();
			mHandler.postDelayed(mScanWifiRunnable, WIFI_SCAN_TIMEOUT);
		}
	}

	public void stopScan() {
		synchronized (this) {
			if (mWifiScanReceiver != null) {
				mContext.unregisterReceiver(mWifiScanReceiver);
			}
			mWifiScanReceiver = null;
			mHandler.removeCallbacks(mScanWifiRunnable);
		}
	}

	public List<WifiOnboardee> getWifiOnboardees() {
		List<WifiOnboardee> wifiOnboardees = new ArrayList<>();
		for (WifiOnboardee wifiOnboardee : mOnboardeesMap.values()) {
			wifiOnboardees.add(wifiOnboardee);
		}
		Collections.sort(wifiOnboardees);
		return wifiOnboardees;
	}

	private boolean connect(final String wifiName, final WifiOnboardee.AuthType authType, final String password, final boolean hidden) {
		if (!mWifiManager.isWifiEnabled()) {
			//wifi disabled
			return false;
		}

		if (Utils.isStringEmpty(wifiName)) {
			return false;
		}
		mNetworkAvailable = false;

		final ConnectivityManager connectivityManager = (ConnectivityManager) mContext.getSystemService(Context.CONNECTIVITY_SERVICE);

		if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
			NetworkRequest.Builder builder = new NetworkRequest.Builder();
			builder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI).removeCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET).build();


			connectivityManager.registerNetworkCallback(builder.build(), new ConnectivityManager.NetworkCallback() {
				@Override
				public void onAvailable(Network network) {
					if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
						NetworkInfo networkInfo = connectivityManager.getNetworkInfo(network);
						Log.d(TAG, "Network is Available. Network Info: " + networkInfo + " id " + network.toString() + " wifi ap state - 1 " + mWifiManager.getWifiState());

						if ((networkInfo.getExtraInfo()).contains(wifiName)) {
							connectivityManager.unregisterNetworkCallback(this);

							if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
								connectivityManager.bindProcessToNetwork(network);      // M and above
							} else if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
									connectivityManager.setProcessDefaultNetwork(network);
							}

							if (mTargetConnectOnboardee.getName().contains(wifiName)) {
								mConnectedOnboardee = mTargetConnectOnboardee;
								mTargetConnectOnboardee = null;
								mHandler.removeCallbacks(mConnectOnboardeeTimeoutRunnable);
								Log.d(TAG,"callback for connected - onAvailable");
								notifyOnWifiOnboardeeConnectionStateChangedListeners(mConnectedOnboardee,WifiOnboardee.ConnectionState.CONNECTED);
							}
						}
					}
				}
			});
		} else {
			mNetworkAvailable = true;
		}

		int networkID = -1;
		boolean result = false;
		boolean shouldUpdate = false;

		WifiConfiguration wifiConfiguration = findConfiguration(wifiName);

		if (wifiConfiguration != null) {
			if (android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.LOLLIPOP) {
				result = mWifiManager.removeNetwork(wifiConfiguration.networkId);
				result = mWifiManager.saveConfiguration();
				wifiConfiguration = new WifiConfiguration();
			} else {
				shouldUpdate = true;
			}
		} else {
			Log.d(TAG,"wifi config is null");
			wifiConfiguration = new WifiConfiguration();
		}

		wifiConfiguration.SSID = "\"" + wifiName + "\"";
		wifiConfiguration.hiddenSSID = hidden;

		switch (authType) {
			case OPEN:
				//wifiConfiguration.priority = 40;
				wifiConfiguration.status = WifiConfiguration.Status.ENABLED;
				wifiConfiguration.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
				assignHighestPriority(wifiConfiguration);
				break;
			case WPA2:
				// handle special case when WPA/WPA2 and 64 length password that can
				// be HEX
				if ((password.length() == 64) && password.matches(HEX_PATTERN)) {
					wifiConfiguration.preSharedKey = password;
				} else {
					wifiConfiguration.preSharedKey = "\"" + password + "\"";
				}
				wifiConfiguration.priority = 40;
				wifiConfiguration.status = WifiConfiguration.Status.ENABLED;
				wifiConfiguration.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
				wifiConfiguration.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
				wifiConfiguration.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
				wifiConfiguration.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
				wifiConfiguration.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
				wifiConfiguration.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
				wifiConfiguration.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
				Log.d(TAG,"wpa2");
				break;
			default:
				break;
		}
		if (shouldUpdate) {
			networkID = mWifiManager.updateNetwork(wifiConfiguration);
			Log.d(TAG,"network id update network " + networkID);
			if ((networkID < 0) && (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)) {
				networkID = wifiConfiguration.networkId;
			}
		} else {
			networkID = mWifiManager.addNetwork(wifiConfiguration);
			Log.d(TAG,"network id add network " + networkID);
		}
		if (networkID < 0) {
			Log.d(TAG,"network id less than 0 return false");
			return false;
		}
		mWifiManager.saveConfiguration();

		wifiConfiguration = findConfiguration(wifiName);

		if (wifiConfiguration == null) {
			return false;
		}
		if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
			result = mWifiManager.disconnect();

			try { Thread.sleep(1000); } catch(Exception e) {}

			result = mWifiManager.enableNetwork(networkID, true);
		} else {
			if (mWifiManager.getConnectionInfo().getSupplicantState() == SupplicantState.DISCONNECTED) {
				result = mWifiManager.disconnect();
			}
			result = mWifiManager.enableNetwork(networkID, false);

			result = mWifiManager.disconnect();

			result = mWifiManager.enableNetwork(networkID, true);

			try { Thread.sleep(1000); } catch(Exception e) {}
			result = mWifiManager.enableNetwork(networkID, true);

			result = mWifiManager.reconnect();
		}
		return true;
	}

	//To tell OS to give preference to this network
	private void assignHighestPriority(WifiConfiguration config) {
		List<WifiConfiguration> configuredNetworks = mWifiManager.getConfiguredNetworks();
		if (configuredNetworks != null) {
			for (WifiConfiguration existingConfig : configuredNetworks) {
				if (config.priority <= existingConfig.priority) {
					config.priority = existingConfig.priority + 1;
				}
			}
		}
	}

	public boolean connect(final WifiOnboardee wifiOnboardee, final String password, final boolean hidden) {
		if (wifiOnboardee == null) {
			return false;
		}

		if (mTargetConnectOnboardee != null) {
			if ((mTargetConnectOnboardee != null) && mTargetConnectOnboardee.equals(wifiOnboardee)) {
				return true;
			}
			return true;
		}

		if (mConnectedOnboardee != null) {
			if (mConnectedOnboardee.equals(wifiOnboardee)) {
				notifyOnWifiOnboardeeConnectionStateChangedListeners(wifiOnboardee, WifiOnboardee.ConnectionState.CONNECTED);
				return true;
			}
			mPreviousWifiConfiguration = null;
			disconnect(mConnectedOnboardee);
		} else if (wifiOnboardee.getConnectionState() == WifiOnboardee.ConnectionState.CONNECTED) {
			mConnectedOnboardee = wifiOnboardee;
			notifyOnWifiOnboardeeConnectionStateChangedListeners(wifiOnboardee, WifiOnboardee.ConnectionState.CONNECTED);
			return true;
		}

		String ssid = wifiOnboardee.getName();
		if (Utils.isStringEmpty(ssid)) {
			notifyOnWifiOnboardeeConnectionStateChangedListeners(wifiOnboardee, WifiOnboardee.ConnectionState.CONNECTING_ERROR);
			return false;
		}

		if (!mWifiManager.isWifiEnabled()) {
			notifyOnWifiOnboardeeConnectionStateChangedListeners(wifiOnboardee, WifiOnboardee.ConnectionState.WIFI_OFF);
			return false;
		}

		mPreviousWifiConfiguration = null;
		final String currentWifiName = mCurrentSSID;
		if ((currentWifiName != null) && !wifiOnboardee.getName().equals(currentWifiName)) {
			WifiInfo connectionInfo = mWifiManager.getConnectionInfo();
			int currentNetworkID = connectionInfo.getNetworkId();

			List<WifiConfiguration> configurations = mWifiManager.getConfiguredNetworks();
			for (WifiConfiguration configuration : configurations) {
				if ((currentNetworkID >= 0) && (configuration.networkId == currentNetworkID)) {
					mPreviousWifiConfiguration = configuration;
					break;
				}
			}
		}

		Log.d(TAG,"wifi onboardee " + wifiOnboardee.getName());
		mTargetConnectOnboardee = wifiOnboardee;
		notifyOnWifiOnboardeeConnectionStateChangedListeners(wifiOnboardee, WifiOnboardee.ConnectionState.CONNECTING);

		if (!connect(ssid, wifiOnboardee.getAuthType(), password, hidden)) {
			notifyOnWifiOnboardeeConnectionStateChangedListeners(wifiOnboardee,
					(mWifiManager.isWifiEnabled() ? WifiOnboardee.ConnectionState.CONNECTING_ERROR :
							WifiOnboardee.ConnectionState.WIFI_OFF));
			mTargetConnectOnboardee = null;
			return false;
		}

		mHandler.postDelayed(mConnectOnboardeeTimeoutRunnable, CONNECT_TIMEOUT);

		return true;
	}

	public boolean disconnect(final WifiOnboardee onboardee) {
		if (onboardee.getConnectionState() != WifiOnboardee.ConnectionState.CONNECTED)  {
			return false;
		}

		reconnectToPreviousNetwork(true);
		return true;
	}

	public void reconnectToPreviousNetwork(final boolean reconnect) {
		if (reconnect && mPreviousWifiConfiguration != null) {
			mWifiManager.disconnect();
			boolean result = mWifiManager.enableNetwork(mPreviousWifiConfiguration.networkId, true);
			if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
				//quick work around.  Hopefully fixed in LOLLIPOP_MR1
				try { Thread.sleep(500); } catch(Exception e) {}
				result = mWifiManager.enableNetwork(mPreviousWifiConfiguration.networkId, true);
			} else {
				result = mWifiManager.reconnect();
			}
		}
		mPreviousWifiConfiguration = null;
	}

	private String stripSSIDQuotes(String SSID) {
		if ((SSID != null) && (SSID.length() > 2) &&
				SSID.startsWith("\"") &&
				SSID.endsWith("\"")) {
			SSID = SSID.substring(1, SSID.length() - 1);
		}
		return SSID;
	}

	private WifiConfiguration findConfiguration(String ssid) {
		if ((ssid == null) || (ssid.isEmpty())) {
			return null;
		}
		List<WifiConfiguration> configurations = mWifiManager.getConfiguredNetworks();
		if (configurations == null) {
			return null;
		}
		for (WifiConfiguration configuration : configurations) {
			if (ssid.equals(stripSSIDQuotes(configuration.SSID))) {
				return configuration;
			}
		}

		return null;
	}

	private void notifyOnWifiOnboardeesChangedListeners() {
		synchronized (mOnWifiOnboardeesChangedListeners) {
			for (int i = 0; i < mOnWifiOnboardeesChangedListeners.size(); i++) {
				OnWifiOnboardeesChangedListener listener = mOnWifiOnboardeesChangedListeners.get(i);
				if (listener != null) {
					listener.onWifiOnboardeesChanged();
				}
			}
		}
	}

	private void notifyOnWifiOnboardeeConnectionStateChangedListeners(final WifiOnboardee wifiOnboardee,
																	  final WifiOnboardee.ConnectionState connectionState) {
		wifiOnboardee.setConnectionState(connectionState);
		synchronized (mOnWifiOnboardeeConnectionStateChangedListeners) {
			for (int i = 0; i < mOnWifiOnboardeeConnectionStateChangedListeners.size(); i++) {
				OnWifiOnboardeeConnectionStateChangedListener listener = mOnWifiOnboardeeConnectionStateChangedListeners.get(i);
				if (listener != null) {
					listener.onWifiOnboardeeConnectionStateChanged(wifiOnboardee, connectionState);
				}
			}
		}
		switch (connectionState) {
			case CONNECTED:
			case CONNECTING:
				break;
			default:
				wifiOnboardee.setConnectionState(WifiOnboardee.ConnectionState.DISCONNECTED);
				break;
		}
	}

	private void notifyOnWifiNetworkChangedListeners(final boolean on, final String network) {
		synchronized (mOnWifiNetworkChangedListeners) {
			for (int i = 0; i < mOnWifiNetworkChangedListeners.size(); i++) {
				OnWifiNetworkChangedListener listener = mOnWifiNetworkChangedListeners.get(i);
				if (listener != null) {
					listener.onWifiNetworkChanged(on, network);
				}
			}
		}
	}

	private void setDisconnectedState() {
		final String currentWifiName = mCurrentSSID;
		for (WifiOnboardee wifiOnboardee : mOnboardeesMap.values()) {
			if (wifiOnboardee.getName().equals(currentWifiName)) {
				if (mConnectedOnboardee != null) {
					mConnectedOnboardee = null;
				}
				notifyOnWifiOnboardeeConnectionStateChangedListeners(wifiOnboardee, WifiOnboardee.ConnectionState.DISCONNECTED);
			}
		}
	}

	class WifiReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			// No need to continue if it is anything else
			if (!action.equals(ConnectivityManager.CONNECTIVITY_ACTION) &&
				!action.equals(WifiManager.NETWORK_STATE_CHANGED_ACTION) &&
				!action.equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {

				return;
			}

			// Parse the network info
			parseConnection(context);
		}

		private void parseConnection(final Context context) {
			// Defaults to no connection
			String networkName = parseWifiName(context);
			DhcpInfo dhcpInfo = parseServerIP(context);
			if (mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLED) {
				if (Utils.isStringEmpty(networkName)) {
					setDisconnectedState();
				} else if (mTargetConnectOnboardee != null && networkName != null && mNetworkAvailable && ((dhcpInfo != null) && (intToIp(dhcpInfo.dns1) != null))) {
					if (mTargetConnectOnboardee.getName().equals(networkName)) {
						mConnectedOnboardee = mTargetConnectOnboardee;
						mTargetConnectOnboardee = null;
						mHandler.removeCallbacks(mConnectOnboardeeTimeoutRunnable);
						Log.d(TAG,"callback for connected - parseConnection");
						notifyOnWifiOnboardeeConnectionStateChangedListeners(mConnectedOnboardee,WifiOnboardee.ConnectionState.CONNECTED);
					}
				} else if (mConnectedOnboardee != null) {
					if (!mConnectedOnboardee.getName().equals(networkName)) {
						notifyOnWifiOnboardeeConnectionStateChangedListeners(mConnectedOnboardee,
								WifiOnboardee.ConnectionState.DISCONNECTED);
						mConnectedOnboardee = null;
					}
				}
			} else {
				setDisconnectedState();
				clearOnboardees();
				mTargetConnectOnboardee = null;
				if (mConnectedOnboardee != null) {
					notifyOnWifiOnboardeeConnectionStateChangedListeners(mConnectedOnboardee,
							WifiOnboardee.ConnectionState.DISCONNECTED);
				}
				mConnectedOnboardee = null;
				mHandler.removeCallbacks(mConnectOnboardeeTimeoutRunnable);
			}
			boolean changed = false;
			if (!mNetworkParsed || (!Utils.isStringEmpty(mCurrentSSID) && !mCurrentSSID.equals(networkName)) ||
					!Utils.isStringEmpty(networkName) && !networkName.equals(mCurrentSSID)) {
				changed = true;
			}

			boolean wifiOn = (mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLED);
			if (wifiOn != mWifiOn) {
				changed = true;
			}
			mWifiOn = wifiOn;
			mCurrentSSID = networkName;
			if (dhcpInfo != null) {
				mServerIP = intToIp(dhcpInfo.serverAddress);
			}
			mNetworkParsed = true;
			if (changed) {
				notifyOnWifiNetworkChangedListeners(mWifiOn, mCurrentSSID);
			}
		}
	}

	private DhcpInfo parseServerIP(final Context context) {
		ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
		// We get the default active network info.  Network info can be null if device is not connected to anything
		NetworkInfo info = connectivityManager.getActiveNetworkInfo();

		if ((info != null) && info.isConnected()) {
			Log.d(TAG, "network info type " + info.getType() + " extra info " + info.getExtraInfo());
			// Get network name
			WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
			DhcpInfo dhcpInfo = wifiManager.getDhcpInfo();
			Log.d(TAG,"dns " + intToIp(dhcpInfo.dns1) + " gateway " + intToIp(dhcpInfo.gateway) + "  ip address " + intToIp(dhcpInfo.ipAddress) + " server address " + intToIp(dhcpInfo.serverAddress));
			if (intToIp(dhcpInfo.dns1).equals("0.0.0.0")) {
				return null;
			}
			return dhcpInfo;
		}
		return null;
	}

	private String intToIp(int addr) {
		return  ((addr & 0xFF) + "." +
				((addr >>>= 8) & 0xFF) + "." +
				((addr >>>= 8) & 0xFF) + "." +
				((addr >>>= 8) & 0xFF));
	}

	private String parseWifiName(final Context context) {
		WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
		WifiInfo wifiInfo = wifiManager.getConnectionInfo();
		if (wifiInfo != null) {
			Log.d(TAG,"ssid " + wifiInfo.getSSID() + " supplicant state " + wifiInfo.getSupplicantState().name() + " detailed state " + wifiInfo.getDetailedStateOf(wifiInfo.getSupplicantState()) + " wifi ip address " + intToIp(wifiInfo.getIpAddress()) + " network id " + wifiInfo.getNetworkId());
			if (wifiInfo.getSupplicantState() == SupplicantState.COMPLETED)  {
				return stripSSIDQuotes(wifiInfo.getSSID());
			}
		}
		return null;
	}

	private void clearOnboardees() {
		mOnboardeesMap.clear();
		notifyOnWifiOnboardeesChangedListeners();
	}

	class WifiScanReceiver extends BroadcastReceiver {
		// Constants used for different security types
		private static final String WPA2 = "WPA2";
		private static final String WPA = "WPA";
		private static final String WEP = "WEP";
		/* For EAP Enterprise fields */
		private static final String WPA_EAP = "WPA-EAP";
		private static final String IEEE8021X = "IEEE8021X";

		public void onReceive(Context c, Intent intent) {
			String action = intent.getAction();
			if (action.equals(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION)) {
				if (!mWifiManager.isWifiEnabled()) {
					clearOnboardees();
					return;
				}
				if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
					if ((ContextCompat.checkSelfPermission(mContext, Manifest.permission.ACCESS_COARSE_LOCATION) !=
							PackageManager.PERMISSION_GRANTED)) {
						return;
					}
				}
				List<ScanResult> wifiScanList = mWifiManager.getScanResults();
				if (wifiScanList == null) {
					return;
				}
				final String currentWifiName = mCurrentSSID;

				boolean update = false;
				for (ScanResult result : wifiScanList) {
					String SSID = stripSSIDQuotes(result.SSID);
					if ((SSID == null) || SSID.isEmpty() || Utils.isStringEmpty(SSID)) {
						//empty or null ssid
						continue;
					}

					if (!isOpen(result)) {
						continue;
					}

					if (!mOnboardeesMap.containsKey(SSID)) {
						update = true;
						WifiOnboardee wifiOnboardee = new WifiOnboardee(SSID, WifiOnboardee.AuthType.OPEN);
						if (SSID.equals(currentWifiName)) {
							wifiOnboardee.setConnectionState(WifiOnboardee.ConnectionState.CONNECTED);
						}
						mOnboardeesMap.put(SSID, wifiOnboardee);
					}
				}

				for (String key : mOnboardeesMap.keySet()) {
					boolean found = false;
					for (ScanResult result : wifiScanList) {
						String SSID = stripSSIDQuotes(result.SSID);
						if (SSID.equals(key)) {
							found = true;
							break;
						}
					}
					if (!found) {
						mOnboardeesMap.remove(key);
						update = true;
					}
				}
				if (update) {
					notifyOnWifiOnboardeesChangedListeners();
				}
			}
		}

		public boolean isOpen(ScanResult scanResult) {
			final String cap = scanResult.capabilities;
			final String[] securityModes = { WEP, WPA, WPA2, WPA_EAP, IEEE8021X };
			for (int i = securityModes.length - 1; i >= 0; i--) {
				if (cap.contains(securityModes[i])) {
					return false;
				}
			}

			return true;
		}
	}

	public interface OnWifiOnboardeesChangedListener {
		void onWifiOnboardeesChanged();
	}

	public interface OnWifiOnboardeeConnectionStateChangedListener {
		void onWifiOnboardeeConnectionStateChanged(final WifiOnboardee wifiOnboardee,
												   final WifiOnboardee.ConnectionState connectionState);
	}

	public interface OnWifiNetworkChangedListener {
		void onWifiNetworkChanged(final boolean on, final String network);
	}
}

