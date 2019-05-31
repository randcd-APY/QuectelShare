/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.manager;

import android.content.Context;
import android.content.Intent;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.os.Bundle;
import android.util.Log;

import com.qualcomm.qti.avs.companion.R;
import com.qualcomm.qti.avs.companion.activity.AlexaSetupActivity;
import com.qualcomm.qti.avs.companion.activity.BaseActivity;
import com.qualcomm.qti.avs.companion.util.DeviceSecureConnection;
import com.qualcomm.qti.avs.companion.util.RequestAsyncTask;
import com.qualcomm.qti.avs.companion.util.Utils;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class AVSNsdManager {
	private Context mContext = null;
	private NsdManager.DiscoveryListener mDiscoveryListener = null;
	private NsdManager mNsdManager = null;
	private boolean mNsdStarted = false;
	Map<String, NsdServiceInfo> mNsdServiceInfos = new HashMap<>();

	private static final String AVS_SERVICE_TYPE = "_avs-setup._tcp";
	private static final String SERVICE_TYPE = "_avs-setup._tcp.";

	private GetMetaDataTask mGetMetaTask = null;

	AVSDiscoveryListener mAVSDiscoveryListener = null;
	AVSServiceMetaDataListener mAVSServiceMetaDataListener = null;

	public AVSNsdManager(final Context context) {
		mContext = context;

		mNsdManager = (NsdManager) mContext.getSystemService(Context.NSD_SERVICE);

		mDiscoveryListener = new NsdManager.DiscoveryListener() {

			@Override
			public void onStartDiscoveryFailed(String serviceType, int errorCode) {
				mNsdManager.stopServiceDiscovery(this);
			}

			@Override
			public void onStopDiscoveryFailed(String serviceType, int errorCode) {
				mNsdManager.stopServiceDiscovery(this);
			}

			@Override
			public void onDiscoveryStarted(String serviceType) {}

			@Override
			public void onDiscoveryStopped(String serviceType) {}

			@Override
			public void onServiceFound(NsdServiceInfo serviceInfo) {
				if (serviceInfo.getServiceType().equals(SERVICE_TYPE)) {
					synchronized (mNsdServiceInfos) {
						if (!mNsdServiceInfos.containsKey(serviceInfo.getServiceName())) {
							mNsdServiceInfos.put(serviceInfo.getServiceName(), serviceInfo);
						}
					}
					notifyServiceFound(serviceInfo);
				}
			}

			@Override
			public void onServiceLost(NsdServiceInfo serviceInfo) {
				synchronized (mNsdServiceInfos) {
					mNsdServiceInfos.remove(serviceInfo.getServiceName());
				}
				notifyServiceLost(serviceInfo);
			}
		};
	}

	public synchronized void startDiscovery() {
		mNsdManager.discoverServices(AVS_SERVICE_TYPE, NsdManager.PROTOCOL_DNS_SD, mDiscoveryListener);
		mNsdStarted = true;
	}

	public synchronized void stopDiscovery() {
		if (mNsdStarted) {
			mNsdManager.stopServiceDiscovery(mDiscoveryListener);
		}
		synchronized (mNsdServiceInfos) {
			mNsdServiceInfos.clear();
		}
	}

	public List<NsdServiceInfo> getServiceInfos() {
		List<NsdServiceInfo> serviceInfos = new ArrayList<>();
		synchronized (mNsdServiceInfos) {
			serviceInfos.addAll(mNsdServiceInfos.values());
		}
		return serviceInfos;
	}

	public synchronized void setAVSDiscoveryListener(final AVSDiscoveryListener listener) {
		mAVSDiscoveryListener = listener;
	}

	public void setAVSServiceMetaDataListener(final AVSServiceMetaDataListener listener) {
		mAVSServiceMetaDataListener = listener;
	}

	public void getAVSServiceInfoMetaData(final BaseActivity activity, final NsdServiceInfo serviceInfo) {
		if ((activity != null) && (mGetMetaTask == null)) {
			activity.runOnUiThread(new Runnable() {
				@Override
				public void run() {
					if ((activity != null) && (mGetMetaTask == null)) {
						mGetMetaTask = new GetMetaDataTask(activity, serviceInfo);
						mGetMetaTask.execute();
					}
				}
			});
		}
	}

	private void notifyServiceFound(final NsdServiceInfo serviceInfo) {
		if (mAVSDiscoveryListener != null) {
			mAVSDiscoveryListener.onAVSServiceFound(serviceInfo);
		}
	}

	private void notifyServiceLost(final NsdServiceInfo serviceInfo) {
		if (mAVSDiscoveryListener != null) {
			mAVSDiscoveryListener.onAVSServiceLost(serviceInfo);
		}
	}

	private void notifyAVSServiceMetaDataSuccess(final String name) {
		if (mAVSServiceMetaDataListener != null) {
			mAVSServiceMetaDataListener.onAVSServiceMetaDataSuccess(name);
		}
	}

	private void notifyAVSServiceMetaDataFailed(final String name) {
		if (mAVSServiceMetaDataListener != null) {
			mAVSServiceMetaDataListener.onAVSServiceMetaDataFailed(name);
		}
	}

	private class GetMetaDataTask extends RequestAsyncTask {
		private final String GET_META =  "/config/avs/get_meta";
		private static final String DSN = "deviceSn";
		private static final String PRODUCT_ID = "productId";
		private static final String CODE_CHALLENGE = "codeChallenge";
		private static final String METHOD = "method";

		private NsdServiceInfo mServiceInfo = null;
		private NsdServiceInfo mResolvedServiceInfo = null;

		private JSONObject mJSONObject = null;
		private String mHostName = null;

		private static final int GET_META_TIMEOUT = 30000;

		public GetMetaDataTask(BaseActivity baseActivity, NsdServiceInfo serviceInfo) {
			super(baseActivity, mContext.getString(R.string.connecting), true);
			mServiceInfo = serviceInfo;
		}

		@Override
		protected Void doInBackground(Void... params) {
			mNsdManager.resolveService(mServiceInfo, new NsdManager.ResolveListener() {
				@Override
				public void onResolveFailed(NsdServiceInfo serviceInfo, int errorCode) {
					finished();
				}

				@Override
				public void onServiceResolved(NsdServiceInfo serviceInfo) {
					finished();
					mResolvedServiceInfo = serviceInfo;
				}
			});
			if (mResolvedServiceInfo == null) {
				doWait(GET_META_TIMEOUT);
			}
			if (mResolvedServiceInfo != null) {
				mHostName = mResolvedServiceInfo.getHost().getCanonicalHostName();
				String url = "https://" + mHostName + GET_META;
				DeviceSecureConnection deviceSecureConnection = new DeviceSecureConnection(url);
				DeviceSecureConnection.ConnectionResult result = deviceSecureConnection.doRequest();
				if ((result.resultCode == DeviceSecureConnection.OK) && !Utils.isStringEmpty(result.result)) {
					try {
						mJSONObject = new JSONObject(result.result);
					} catch (JSONException e) {
						e.printStackTrace();
					}
				}
			}
			return null;
		}

		@Override
		protected void onPostExecute(final Void param) {
			super.onPostExecute(param);
			mGetMetaTask = null;
			if ((mJSONObject != null) && !Utils.isStringEmpty(mHostName)) {
				try {
					notifyAVSServiceMetaDataSuccess(mServiceInfo.getServiceName());
					String codeChallenge = mJSONObject.getString(CODE_CHALLENGE);
					String method = mJSONObject.getString(METHOD);
					String productID = mJSONObject.getString(PRODUCT_ID);
					String dsn = mJSONObject.getString(DSN);

					Intent intent = new Intent(mActivityRef.get(), AlexaSetupActivity.class);
					Bundle bundle = new Bundle();
					bundle.putString(AlexaSetupActivity.CODE_CHALLENGE, codeChallenge);
					bundle.putString(AlexaSetupActivity.CODE_CHALLENGE_METHOD, method);
					bundle.putString(AlexaSetupActivity.PRODUCT_ID, productID);
					bundle.putString(AlexaSetupActivity.DEVICE_SERIAL_NUMBER, dsn);
					bundle.putString(AlexaSetupActivity.HOST_ADDRESS, mHostName);
					intent.putExtras(bundle);
					mActivityRef.get().startActivity(intent);
				} catch (JSONException e) {
					e.printStackTrace();
				}
			} else {
				notifyAVSServiceMetaDataFailed(mServiceInfo.getServiceName());
			}
		}
	}

	public interface AVSDiscoveryListener {
		void onAVSServiceFound(NsdServiceInfo serviceInfo);
		void onAVSServiceLost(NsdServiceInfo serviceInfo);
	}

	public interface AVSServiceMetaDataListener {
		void onAVSServiceMetaDataSuccess(final String name);
		void onAVSServiceMetaDataFailed(final String name);
	}
}
