/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.fragment;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.support.v4.content.res.ResourcesCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import com.qualcomm.qti.avs.companion.R;
import com.qualcomm.qti.avs.companion.util.ConnectAsyncTask;
import com.qualcomm.qti.avs.companion.util.DeviceSecureConnection;
import com.qualcomm.qti.avs.companion.util.RequestAsyncTask;
import com.qualcomm.qti.avs.companion.util.Utils;
import com.qualcomm.qti.avs.companion.util.WifiOnboardee;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static com.qualcomm.qti.avs.companion.util.DeviceSecureConnection.OK;

public class WiFiFragment extends SetupListFragment implements CustomDialogFragment.OnCustomDialogDismissedListener,
		CustomDialogFragment.OnEditTextKeyClickedListener {
	private static final String TAG = WiFiFragment.class.getSimpleName();

	private final String INPUT_PASSWORD_DIALOG_TAG = "InputPasswordDialogTag";
	private final String ERROR_NETWORK_NOT_SUPPORTED_TAG = "ErrorNetworkNotSupportedTag";
	private final String ERROR_ONBOARDING_TAG = "ErrorOnboardingTag";
	private final String ERROR_APP_UNABLE_CONNECT_TAG = "ErrorAppUnableConnectTag";
	private final String ERROR_RETRIEVE_SCAN_LIST_TAG = "ErrorRetrieveScanListTag";
	private final String ADD_WIFI_TAG = "AddWifiTag";

	private final String HTTPS_SCHEMA = "https://mobileap.qualcomm.com";
	private final String REFRESH_SCAN_LIST = "/config/refresh_scan_list";
	private final String VERIFY_CONNECT = "/config/verify_connect";
	private final String VERIFY_CONNECTING = "/config/verify_connecting";
	private final String CHECK_CONNECT_STATUS = "/config/check_connect_status";
	private final String JOIN = "/config/join";

	private final int RETRIEVE_SCAN_LIST_INTERVAL = 30000;
	private final String SCAN_LIST = "scanlist";
	private final String SSID = "ssid";
	private final String ENCRYPTION = "encryption";
	private final String WPA = "wpa";
	private final String WEP = "wep";
	private final String KEY = "key";

	private boolean mFirstTry = true;
	private Handler mHandler = null;
	private RetrieveScanListTask mRetrieveScanListTask = null;
	private SendCredentialTask mSendingCredentialTask = null;
	private ConnectAsyncTask mConnectTask = null;

	private DataAdapter mAdapter = null;

	private OnboardingErrorListener mOnboardingErrorListener = null;

	private CustomDialogFragment mAddWifiFragment = null;
	private String mOnboardeeName = null;
	private static final String ONBOARDEE_NAME = "ONBOARDEE_NAME";

	private final Runnable mRetrieveScanListRunnable = new Runnable() {
		@Override
		public void run() {
			String wifi = mApp.getOnboardingManager().getCurrentSSID();
			if ((mRetrieveScanListTask == null) &&
					!Utils.isStringEmpty(wifi) &&
					mOnboardeeName.equals(wifi)) {
				mRetrieveScanListTask = new RetrieveScanListTask();
				mRetrieveScanListTask.execute();
			} else {
				mHandler.postDelayed(mRetrieveScanListRunnable, RETRIEVE_SCAN_LIST_INTERVAL);
			}
		}
	};

	public static WiFiFragment newInstance(final String tag, final String onboardeeName) {
		WiFiFragment fragment = new WiFiFragment();
		Bundle args = new Bundle();
		args.putString(SETUP_TAG_KEY, tag);
		args.putString(ONBOARDEE_NAME, onboardeeName);
		fragment.setArguments(args);
		return fragment;
	}

	@Override
	public View onCreateView(final LayoutInflater inflater, final ViewGroup container, final Bundle savedInstanceState) {
		mHandler = new Handler(Looper.getMainLooper());
		View view = super.onCreateView(inflater, container, savedInstanceState);

		setNoButtonSetup();

		mInstructionTextView.setVisibility(View.GONE);

		mListView.setVisibility(View.VISIBLE);
		mExpandableListView.setVisibility(View.GONE);

		mAdapter = new WiFiFragment.DataAdapter();
		mListView.setAdapter(mAdapter);
		mListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
				final WifiOnboardee wifiOnboardee = (WifiOnboardee) mAdapter.getItem(position);
				if (wifiOnboardee.getAuthType() == WifiOnboardee.AuthType.NOT_SUPPORTED) {
					showNetworkNotSupported(wifiOnboardee);
					return;
				}
				mHandler.removeCallbacks(mRetrieveScanListRunnable);
				if (wifiOnboardee.getAuthType() == WifiOnboardee.AuthType.OPEN) {
					sendCredential(wifiOnboardee, null, false);
				} else {
					final CustomDialogFragment dialogFragment = CustomDialogFragment.newEditTextDialog(INPUT_PASSWORD_DIALOG_TAG,
							getString(R.string.enter_password_for, wifiOnboardee.getName()), null, null, getString(R.string.connect),
							getString(R.string.cancel), true);

					dialogFragment.setOnCustomDialogDismissedListener(new CustomDialogFragment.OnCustomDialogDismissedListener() {
						@Override
						public void onPositiveButtonClicked(String tag) {
							sendCredential(wifiOnboardee, dialogFragment.getEditText(),
									false);
						}

						@Override
						public void onNegativeButtonClicked(String tag) {}

						@Override
						public void onDismissed(String tag) {
							mHandler.postDelayed(mRetrieveScanListRunnable, RETRIEVE_SCAN_LIST_INTERVAL);
						}
					});
					dialogFragment.setOnEditTextKeyClickedListener(new CustomDialogFragment.OnEditTextKeyClickedListener() {
						@Override
						public void onDoneClicked(String tag, EditText editText) {
							mBaseActivity.getCustomDialogManager().dismissDialog(tag);
							sendCredential(wifiOnboardee, editText.getText().toString(), false);
						}
					});
					mBaseActivity.getCustomDialogManager().showDialog(dialogFragment, INPUT_PASSWORD_DIALOG_TAG);
				}
			}
		});

		return view;
	}

	private void sendCredential(final WifiOnboardee wifiOnboardee, final String password, final boolean hidden) {
		if (mSendingCredentialTask == null) {
			mSendingCredentialTask = new SendCredentialTask(wifiOnboardee, password, hidden);
			mSendingCredentialTask.execute();
		}
	}

	@Override
	public void onAttach (Context context) {
		super.onAttach(context);
		mOnboardingErrorListener = (OnboardingErrorListener) context;
		mOnboardeeName = getArguments().getString(ONBOARDEE_NAME);
	}

	@Override
	public void onResume() {
		super.onResume();
		startScan();
	}

	@Override
	public void onPause() {
		super.onPause();
		mHandler.removeCallbacks(mRetrieveScanListRunnable);
	}

	@Override
	public void onPositiveButtonClicked(String tag) {
		if (ERROR_RETRIEVE_SCAN_LIST_TAG.equals(tag) ||
				ERROR_APP_UNABLE_CONNECT_TAG.equals(tag)) {
			mOnboardingErrorListener.onboardingError();
		} else if (ADD_WIFI_TAG.equals(tag)) {
			sendCredentialFromAddWifi(tag);
		}
	}

	@Override
	public void onNegativeButtonClicked(String tag) {}

	@Override
	public void onDismissed(String tag) {
		if (ADD_WIFI_TAG.equals(tag) && (mAddWifiFragment != null)) {
			// This case is negative button pressed or outside pressed
			mAddWifiFragment = null;
			mHandler.postDelayed(mRetrieveScanListRunnable, RETRIEVE_SCAN_LIST_INTERVAL);
		} else if (ERROR_ONBOARDING_TAG.equals(tag)) {
			mHandler.postDelayed(mRetrieveScanListRunnable, RETRIEVE_SCAN_LIST_INTERVAL);
		}
	}

	@Override
	public void onDoneClicked(String tag, EditText editText) {
		sendCredentialFromAddWifi(tag);
	}

	private void sendCredentialFromAddWifi(final String tag) {
		if (mAddWifiFragment == null) {
			return;
		}
		CustomDialogFragment.WifiCredential wifiCredential = mAddWifiFragment.getWifiCredential();
		List<WifiOnboardee> wifiOnboardees = mAdapter.getWifiOnboardees();
		boolean hidden = true;
		for (WifiOnboardee wifiOnboardee : wifiOnboardees) {
			if (wifiOnboardee.getName().equals(wifiCredential.SSID)) {
				hidden = false;
				break;
			}
		}

		mAddWifiFragment = null;
		mBaseActivity.getCustomDialogManager().dismissDialog(tag);
		sendCredential(new WifiOnboardee(wifiCredential.SSID,
				(wifiCredential.isOpen) ? WifiOnboardee.AuthType.OPEN : WifiOnboardee.AuthType.WPA2), wifiCredential.password, hidden);
	}

	private class DataAdapter extends BaseAdapter {
		private List<WifiOnboardee> mWifiOnboardees = new ArrayList<>();

		void update(final List<WifiOnboardee> wifiOnboardees) {
			synchronized (mWifiOnboardees) {
				mWifiOnboardees.clear();
				mWifiOnboardees.addAll(wifiOnboardees);
				notifyDataSetChanged();
			}
		}

		List<WifiOnboardee> getWifiOnboardees() {
			List<WifiOnboardee> wifiOnboardees = new ArrayList<>();
			synchronized (mWifiOnboardees) {
				wifiOnboardees.addAll(mWifiOnboardees);
			}
			return wifiOnboardees;
		}

		@Override
		public int getCount() {
			return mWifiOnboardees.size();
		}

		@Override
		public Object getItem(int position) {
			return mWifiOnboardees.get(position);
		}

		@Override
		public long getItemId(int position) {
			return position;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			WifiOnboardee wifiOnboardee = (WifiOnboardee) getItem(position);
			if (convertView == null) {
				LayoutInflater layoutInflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				convertView = layoutInflater.inflate(R.layout.list_item, parent, false);
			}

			TextView textView = (TextView) convertView.findViewById(R.id.list_item_text);
			textView.setText(wifiOnboardee.getName());

			ImageView rightImageView = (ImageView) convertView.findViewById(R.id.list_item_right_image);
			rightImageView.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.ic_locked, null));

			rightImageView.setVisibility((wifiOnboardee.getAuthType() == WifiOnboardee.AuthType.OPEN) ? View.GONE : View.VISIBLE);

			return convertView;
		}
	}

	class SendCredentialTask extends RequestAsyncTask {
		private final WifiOnboardee mWifiOnboardee;
		private final String mPassword;
		private static final int TRIES = 60;
		private static final String SUCCESS = "0";
		private static final int SECOND = 1000;
		private boolean mSuccess = false;
		private boolean mHidden = false;

		SendCredentialTask(final WifiOnboardee wifiOnboardee, final String password, final boolean hidden) {
			super(mBaseActivity, mBaseActivity.getString(R.string.connecting), true);
			mWifiOnboardee = wifiOnboardee;
			mPassword = password;
			mHidden = hidden;
		}

		@Override
		protected Void doInBackground(Void... params) {
			Map<String, String> data = new HashMap<>();
			data.put(SSID, mWifiOnboardee.getName());
			data.put(KEY, !Utils.isStringEmpty(mPassword) ? mPassword : new String());

			final String verifyConnectUrl = HTTPS_SCHEMA + VERIFY_CONNECT;
			DeviceSecureConnection deviceSecureConnection = new DeviceSecureConnection(verifyConnectUrl, data);
			DeviceSecureConnection.ConnectionResult result = deviceSecureConnection.doRequest();
			if (result.resultCode == OK) {
				int tried = 0;
				while (tried++ < TRIES) {
					try {
						Thread.sleep(SECOND);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}

					final String verifyConnectingUrl = HTTPS_SCHEMA + VERIFY_CONNECTING;
					deviceSecureConnection = new DeviceSecureConnection(verifyConnectingUrl);
					result = deviceSecureConnection.doRequest();

					if ((result.resultCode == OK) && !Utils.isStringEmpty(result.result) && result.result.equals(SUCCESS)) {
						break;
					}
				}

				if ((result.resultCode == OK) && !Utils.isStringEmpty(result.result) && result.result.equals(SUCCESS)) {
					final String checkUrl = HTTPS_SCHEMA + CHECK_CONNECT_STATUS;
					deviceSecureConnection = new DeviceSecureConnection(checkUrl);
					result = deviceSecureConnection.doRequest();
					if ((result.resultCode == OK) && !Utils.isStringEmpty(result.result) && result.result.equals(SUCCESS)) {
						final String joinUrl = HTTPS_SCHEMA + JOIN;
						deviceSecureConnection = new DeviceSecureConnection(joinUrl, data);
						deviceSecureConnection.doRequest();
						mSuccess = true;
					}
				}
			}

			return null;
		}

		@Override
		protected void onPostExecute(final Void param) {
			dismissDialog(false);
			super.onPostExecute(param);

			if (!mSuccess) {
				showOnboardingError(mWifiOnboardee);
			} else {
				mConnectTask = new ConnectAsyncTask(mBaseActivity, mWifiOnboardee, mPassword, false, mHidden);
				mConnectTask.setOnConnectPostExecute(new ConnectAsyncTask.OnConnectPostExecute() {
					@Override
					public void onConnectPostExecute(WifiOnboardee wifiOnboardee) {
						if (wifiOnboardee.getConnectionState() == WifiOnboardee.ConnectionState.CONNECTED) {
							mSetupListFragmentListener.onItemClicked(mTag, wifiOnboardee);
						} else {
							showAppConnectError(wifiOnboardee);

						}
						mConnectTask = null;
					}
				});
				mConnectTask.execute();
			}
			mSendingCredentialTask = null;
		}
	}

	public boolean isConnecting() {
		return (mConnectTask != null) || (mSendingCredentialTask != null);
	}

	public void startScan() {
		if (mRetrieveScanListTask == null) {
			mHandler.post(mRetrieveScanListRunnable);
		}
	}

	public void showAddWiFi() {
		if (mAddWifiFragment == null) {
			mHandler.removeCallbacks(mRetrieveScanListRunnable);
			mAddWifiFragment = CustomDialogFragment.newAddWifiDialog(ADD_WIFI_TAG);
			mAddWifiFragment.setOnCustomDialogDismissedListener(this);
			mAddWifiFragment.setOnEditTextKeyClickedListener(this);
			mBaseActivity.getCustomDialogManager().showDialog(mAddWifiFragment, ADD_WIFI_TAG);
		}
	}

	private void showNetworkNotSupported(final WifiOnboardee wifiOnboardee) {
		CustomDialogFragment customDialogFragment = CustomDialogFragment
				.newDialog(ERROR_NETWORK_NOT_SUPPORTED_TAG, getString(R.string.error),
						getString(R.string.network_not_supported, wifiOnboardee.getName()),
						getString(R.string.ok), null);
		mBaseActivity.getCustomDialogManager().showDialog(customDialogFragment, ERROR_NETWORK_NOT_SUPPORTED_TAG);
	}

	private void showOnboardingError(final WifiOnboardee wifiOnboardee) {
		CustomDialogFragment customDialogFragment = CustomDialogFragment
				.newDialog(ERROR_ONBOARDING_TAG, getString(R.string.error), getString(R.string.onboarding_error, wifiOnboardee.getName()),
						getString(R.string.ok), null);
		customDialogFragment.setOnCustomDialogDismissedListener(this);
		mBaseActivity.getCustomDialogManager().showDialog(customDialogFragment, ERROR_ONBOARDING_TAG);
	}

	private void showAppConnectError(final WifiOnboardee wifiOnboardee) {
		CustomDialogFragment customDialogFragment = CustomDialogFragment
				.newDialog(ERROR_APP_UNABLE_CONNECT_TAG, getString(R.string.error),
						getString(R.string.app_unable_connect, getString(R.string.app_name), wifiOnboardee.getName()),
						getString(R.string.ok), null);
		customDialogFragment.setOnCustomDialogDismissedListener(this);
		mBaseActivity.getCustomDialogManager().showDialog(customDialogFragment, ERROR_APP_UNABLE_CONNECT_TAG);
	}

	private void showRetrieveScanListError() {
		CustomDialogFragment customDialogFragment = CustomDialogFragment
				.newDialog(ERROR_RETRIEVE_SCAN_LIST_TAG, getString(R.string.error),
						getString(R.string.unable_to_retrieve_scan_list),
						getString(R.string.ok), null);
		customDialogFragment.setOnCustomDialogDismissedListener(this);
		mBaseActivity.getCustomDialogManager().showDialog(customDialogFragment, ERROR_RETRIEVE_SCAN_LIST_TAG);
	}

	class RetrieveScanListTask extends RequestAsyncTask {
		private JSONObject mJSONObject = null;

		public RetrieveScanListTask() {
			super(mBaseActivity, mBaseActivity.getString(R.string.retrieving_scan_list), mFirstTry);
		}

		@Override
		protected Void doInBackground(Void... params) {
			String refreshUrl = HTTPS_SCHEMA + REFRESH_SCAN_LIST;
			DeviceSecureConnection deviceSecureConnection = new DeviceSecureConnection(refreshUrl);
			DeviceSecureConnection.ConnectionResult result = deviceSecureConnection.doRequest();
			if ((result.resultCode == OK) && !Utils.isStringEmpty(result.result)) {
				try {
					mJSONObject = new JSONObject(result.result);
				} catch (JSONException e) {
					e.printStackTrace();
				}
			}
			return null;
		}

		@Override
		protected void onPostExecute(final Void param) {
			super.onPostExecute(param);
			List<WifiOnboardee> wifiOnboardees = new ArrayList<>();
			if (mJSONObject != null) {
				try {
					JSONArray scanList = mJSONObject.getJSONArray(SCAN_LIST);
					Set<WifiOnboardee> nameSet = new HashSet<>();
					for (int i = 0; i < scanList.length(); i++) {
						JSONObject scanInfo = scanList.getJSONObject(i);
						String ssid = scanInfo.getString(SSID);
						if (ssid.equals(mApp.getOnboardingManager().getCurrentSSID()) || ssid.trim().isEmpty()) {
							continue;
						}
						JSONObject encryption = scanInfo.getJSONObject(ENCRYPTION);
						boolean wpa2Supported = encryption.has(WPA) ? (encryption.getInt(WPA) > 1) : false;
						boolean wpaSupported = encryption.has(WPA) ? (encryption.getInt(WPA) >= 1) : false;
						boolean haveWep = encryption.has(WEP) ? encryption.getBoolean(WEP) : false;
						WifiOnboardee wifiOnboardee = new WifiOnboardee(ssid, (wpa2Supported) ? WifiOnboardee.AuthType.WPA2 :
								((!wpaSupported && !haveWep) ? WifiOnboardee.AuthType.OPEN : WifiOnboardee.AuthType.NOT_SUPPORTED));
						if (nameSet.contains(wifiOnboardee)) {
							continue;
						}
						nameSet.add(wifiOnboardee);
						wifiOnboardees.add(wifiOnboardee);
					}
				} catch (JSONException e) {
					e.printStackTrace();
				}
			} else if (mFirstTry) {
				showRetrieveScanListError();
				return;
			}
			mFirstTry = false;
			mAdapter.update(wifiOnboardees);
			mRetrieveScanListTask = null;
			mHandler.postDelayed(mRetrieveScanListRunnable, RETRIEVE_SCAN_LIST_INTERVAL);
		}
	}

	public interface OnboardingErrorListener {
		void onboardingError();
	}
}
