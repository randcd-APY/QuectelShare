/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.fragment;

import android.content.Context;
import android.support.v4.app.Fragment;

import com.qualcomm.qti.avs.companion.activity.BaseActivity;
import com.qualcomm.qti.avs.companion.app.QTIAVSApplication;
import com.qualcomm.qti.avs.companion.util.Utils;

public class BaseFragment extends Fragment {
	private static final String TAG = BaseFragment.class.getSimpleName();

	protected BaseActivity mBaseActivity = null;
	protected QTIAVSApplication mApp = null;

	@Override
	public void onAttach (Context context) {
		super.onAttach(context);
		mApp = (QTIAVSApplication) context.getApplicationContext();
		mBaseActivity = (BaseActivity) context;
	}

	protected void update() {}

	public void updateInUiThread() {
		if (Utils.isActivityActive(mBaseActivity)) {
			mBaseActivity.runOnUiThread(new Runnable() {
				@Override
				public void run() {
					if (Utils.isActivityActive(mBaseActivity)) {
						update();
					}
				}
			});
		}
	}
}
