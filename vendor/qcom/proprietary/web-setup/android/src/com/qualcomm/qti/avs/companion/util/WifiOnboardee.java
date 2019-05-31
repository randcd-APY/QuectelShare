/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.util;

import android.support.annotation.NonNull;

public class WifiOnboardee implements Comparable<WifiOnboardee> {

	public enum ConnectionState {
		/**
		 * Not connected to network
		 */
		DISCONNECTED,
		/**
		 * Connecting to network
		 */
		CONNECTING,
		/**
		 * Connected to network
		 */
		CONNECTED,
		/**
		 * Connecting error
		 */
		CONNECTING_ERROR,
		/**
		 * Connecting to network timed out
		 */
		CONNECTING_TIMEOUT,
		/**
		 * Wi-Fi is turned off
		 */
		WIFI_OFF,
	}

	private ConnectionState mConnectionState = ConnectionState.DISCONNECTED;

	public enum AuthType {
		OPEN,
		WPA2,
		NOT_SUPPORTED
	}

	private String mName = null;
	private AuthType mAuthType = AuthType.NOT_SUPPORTED;

	public WifiOnboardee(final String name, final AuthType authType) {
		mName = name;
		mAuthType = authType;
	}

	public String getName() {
		return mName;
	}

	public AuthType getAuthType() {
		return mAuthType;
	}

	public ConnectionState getConnectionState() {
		return mConnectionState;
	}

	public void setConnectionState(final ConnectionState connectionState) {
		mConnectionState = connectionState;
	}

	@Override
	public boolean equals(Object obj) {
		if ((obj == null) || !(obj instanceof WifiOnboardee)) {
			return false;
		}
		return mName.equals(((WifiOnboardee) obj).getName()) && mAuthType.equals(((WifiOnboardee) obj).getAuthType());
	}


	@Override
	public int compareTo(@NonNull WifiOnboardee o) {
		return mName.compareTo(o.getName());
	}
}
