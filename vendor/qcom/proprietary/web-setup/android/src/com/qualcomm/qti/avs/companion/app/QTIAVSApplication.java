/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.app;

import android.app.Application;

import com.qualcomm.qti.avs.companion.manager.AVSNsdManager;
import com.qualcomm.qti.avs.companion.manager.OnboardingManager;

public class QTIAVSApplication extends Application {
	private static final String TAG = QTIAVSApplication.class.getSimpleName();

	private OnboardingManager mOnboardingManager = null;
	private AVSNsdManager mAVSNsdManager = null;

	@Override
	public void onCreate() {
		super.onCreate();
		mOnboardingManager = new OnboardingManager(this);
		mAVSNsdManager = new AVSNsdManager(this);
	}

	public OnboardingManager getOnboardingManager() {
		return mOnboardingManager;
	}

	public AVSNsdManager getAVSNsdManager() {
		return mAVSNsdManager;
	}
}
