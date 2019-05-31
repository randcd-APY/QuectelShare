/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.manager;

import android.support.v4.app.DialogFragment;

import com.qualcomm.qti.avs.companion.activity.BaseActivity;
import com.qualcomm.qti.avs.companion.util.Utils;

import java.lang.ref.WeakReference;

public class CustomDialogManager {
	private static final String TAG = CustomDialogManager.class.getSimpleName();

	// The holding activity
	private WeakReference<BaseActivity> mActivityRef;

	private String mCurrentTag = null;

	public CustomDialogManager(final BaseActivity activity) {
		mActivityRef = new WeakReference<>(activity);
	}

	public void showDialog(final DialogFragment dialogFragment, final String tag) {
		dismissDialog(mCurrentTag);
		final BaseActivity baseActivity = mActivityRef.get();
		if ((dialogFragment != null) && (tag != null) && Utils.isActivityActive(baseActivity)) {
			mCurrentTag = tag;
			// Show dialog fragment
			dialogFragment.show(baseActivity.getSupportFragmentManager(), tag);
			// This call waits for dialog to be actually shown
			baseActivity.getSupportFragmentManager().executePendingTransactions();
		}
	}

	public boolean isDialogShown(final String tag) {
		final BaseActivity baseActivity = mActivityRef.get();
		if (!Utils.isActivityActive(baseActivity)) {
			return false;
		}
		return (baseActivity.getSupportFragmentManager().findFragmentByTag(tag) != null);
	}

	public void dismiss() {
		dismissDialog(mCurrentTag);
	}

	public void dismissDialog(String tag) {
		if (tag == null) {
			return;
		}
		final BaseActivity baseActivity = mActivityRef.get();
		if (Utils.isActivityActive(baseActivity)) {
			final DialogFragment dialogFragment = (DialogFragment) baseActivity.getSupportFragmentManager().findFragmentByTag(tag);
			if (dialogFragment != null) {
				mCurrentTag = null;
				// Dismiss the dialog fragment
				dialogFragment.dismiss();
				// This call waits for the dialog to be actually dismissed
				baseActivity.getSupportFragmentManager().executePendingTransactions();
			}
		}
	}
}
