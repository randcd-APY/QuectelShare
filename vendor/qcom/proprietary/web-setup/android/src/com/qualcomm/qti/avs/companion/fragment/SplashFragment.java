/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.qualcomm.qti.avs.companion.R;

public class SplashFragment extends SetupFragment {
	private static final String TAG = SplashFragment.class.getSimpleName();

	public static SplashFragment newInstance(final String tag) {
		SplashFragment fragment = new SplashFragment();
		Bundle args = new Bundle();
		args.putString(SETUP_TAG_KEY, tag);
		fragment.setArguments(args);
		return fragment;
	}

	@Override
	public View onCreateView(final LayoutInflater inflater, final ViewGroup container, final Bundle savedInstanceState) {
		View view = super.onCreateView(inflater, container, savedInstanceState);

		inflater.inflate(R.layout.frame_alexa_intro, mFrameLayout, true);

		setOneButtonSetup();

		mBottomButton.setText(getString(R.string.sign_in_with_amazon));

		return view;
	}
}
