/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.fragment;

import android.content.Context;
import android.os.Bundle;
import android.text.Spannable;
import android.text.SpannableString;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.qualcomm.qti.avs.companion.R;

public class SetupCompleteFragment extends SetupAddedFragment {
	private static final String TAG = SetupCompleteFragment.class.getSimpleName();

	private static final String DISPLAY_NAME = "DISPLAY_NAME";
	private String mDisplayName;

	public static SetupCompleteFragment newInstance(String tag, String displayName) {
		SetupCompleteFragment fragment = new SetupCompleteFragment();
		Bundle args = new Bundle();
		args.putString(SETUP_TAG_KEY, tag);
		args.putString(DISPLAY_NAME, displayName);
		fragment.setArguments(args);
		return fragment;
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View view = super.onCreateView(inflater, container, savedInstanceState);

		setOneButtonSetup();

		mAddedText.setText(getString(R.string.successfully_onboarded));

		mBottomButton.setText(getString(R.string.cont));

		return view;
	}

	@Override
	public void onAttach(Context context) {
		super.onAttach(context);
		mDisplayName = getArguments().getString(DISPLAY_NAME);
	}
}
