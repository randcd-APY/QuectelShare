/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.util;

import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URISyntaxException;
import java.net.URL;
import java.net.URLEncoder;
import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.util.Map;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

public class DeviceSecureConnection {
	private static final String TAG = DeviceSecureConnection.class.getSimpleName();

	private final String mUrl;
	private Map<String, String> mData;

	public static final int OK = 200;

	public DeviceSecureConnection(final String url) {
		mUrl = url;
	}

	public DeviceSecureConnection(final String url, final Map<String, String> data) {
		mUrl = url;
		mData = data;
	}

	public ConnectionResult doRequest() {
		ConnectionResult result = new ConnectionResult();
		DataOutputStream outputStream = null;

		try {
			URL url = new URL(mUrl);
			HttpURLConnection connection = (HttpURLConnection) url.openConnection();

			Log.d(TAG,"url " + url.toString() + " url " + url.toURI().toString());

			if (connection instanceof HttpsURLConnection) {
				((HttpsURLConnection) connection).setSSLSocketFactory(getSSLSocketFactory());
				((HttpsURLConnection) connection).setHostnameVerifier(new HostnameVerifier() {
					@Override
					public boolean verify(String hostname, SSLSession session) {
						return true;
					}
				});
			}

			if ((mData != null) && !mData.isEmpty()) {
				connection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
				connection.setRequestMethod("POST");

				connection.setDoOutput(true);

				String content = new String();
				for (Map.Entry<String, String> entry : mData.entrySet()) {
					if (!content.isEmpty()) {
						content += "&";
					}
					content += entry.getKey() + "=" + URLEncoder.encode(entry.getValue(), "UTF-8");
				}

				outputStream = new DataOutputStream(connection.getOutputStream());
				outputStream.writeBytes(content);
				outputStream.flush();
				outputStream.close();

			} else {
				connection.setRequestMethod("GET");
			}
			connection.connect();
			int responseCode = connection.getResponseCode();
			result.resultCode = responseCode;
			Log.d(TAG,"response code " + responseCode);
			if (responseCode == OK) {
				result.result = readInputStreamToString(connection);
			} else {
				result.result = null;
			}
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		} catch (ProtocolException e) {
			e.printStackTrace();
		} catch (MalformedURLException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (URISyntaxException e) {
			e.printStackTrace();
		} finally {
			if (outputStream != null) {
				try {
					outputStream.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

		return result;
	}

	private String readInputStreamToString(HttpURLConnection connection) {
		String result = null;
		StringBuffer stringBuffer = new StringBuffer();
		InputStream inputStream = null;

		try {
			inputStream = new BufferedInputStream(connection.getInputStream());
			BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
			String line;
			while ((line = bufferedReader.readLine()) != null) {
				stringBuffer.append(line);
				Log.d(TAG,"read result from string buffer " + line);
			}
			result = stringBuffer.toString();
		} catch (Exception e) {
			result = null;
		} finally {
			if (inputStream != null) {
				try {
					inputStream.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

		return result;
	}

	private SSLSocketFactory getSSLSocketFactory() {
		SSLSocketFactory sslSocketFactory = null;
		try {
			TrustManager[] trustAllCerts = new TrustManager[] { new X509TrustManager() {
				public java.security.cert.X509Certificate[] getAcceptedIssuers() {
					return new java.security.cert.X509Certificate[] {};
				}

				public void checkClientTrusted(X509Certificate[] chain,
											   String authType) throws CertificateException {
				}

				public void checkServerTrusted(X509Certificate[] chain,
											   String authType) throws CertificateException {
				}
			} };

			SSLContext sslContext = SSLContext.getInstance("TLS");
			sslContext.init(null, trustAllCerts, new java.security.SecureRandom());
			sslSocketFactory =  sslContext.getSocketFactory();
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		} catch (KeyManagementException e) {
			e.printStackTrace();
		}
		return sslSocketFactory;
	}

	public class ConnectionResult {
		public String result;
		public int resultCode;
	}
}
