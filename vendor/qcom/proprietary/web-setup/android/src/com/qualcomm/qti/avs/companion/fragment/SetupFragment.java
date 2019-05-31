/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.fragment;

import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;

import com.qualcomm.qti.avs.companion.R;

public abstract class SetupFragment extends BaseFragment implements View.OnClickListener {
	public static final String SETUP_TAG_KEY = "SETUP_TAG_KEY";

	protected FrameLayout mFrameLayout = null;

	protected Button mTopButton = null;
	protected Button mMiddleButton = null;
	protected Button mBottomButton = null;

	protected String mTag = null;

	protected SetupFragmentListener mSetupFragmentListener = null;

	@Override
	public void onAttach (Context context) {
		super.onAttach(context);
		mTag = getArguments().getString(SETUP_TAG_KEY);
		mSetupFragmentListener = (SetupFragmentListener)context;
	}

	@Override
	public View onCreateView(final LayoutInflater inflater, final ViewGroup container, final Bundle savedInstanceState) {
		super.onCreateView(inflater, container, savedInstanceState);
		View view = inflater.inflate(R.layout.fragment_setup, container, false);

		mFrameLayout = (FrameLayout) view.findViewById(R.id.setup_frame);

		mTopButton = (Button) view.findViewById(R.id.setup_top_button);
		mTopButton.setOnClickListener(this);

		mMiddleButton = (Button) view.findViewById(R.id.setup_middle_button);
		mMiddleButton.setOnClickListener(this);

		mBottomButton = (Button) view.findViewById(R.id.setup_bottom_button);
		mBottomButton.setOnClickListener(this);

		return view;
	}

	@Override
	public void onClick(View view) {
		switch (view.getId()) {
			case R.id.setup_top_button:
				onTopButtonClicked();
				break;
			case R.id.setup_middle_button:
				onMiddleButtonClicked();
				break;
			case R.id.setup_bottom_button:
				onBottomButtonClicked();
				break;
			default:
				break;
		}
	}

	public void onTopButtonClicked() {
		if (mSetupFragmentListener != null) {
			mSetupFragmentListener.onTopButtonClicked(mTag);
		}
	}

	public void onMiddleButtonClicked() {
		if (mSetupFragmentListener != null) {
			mSetupFragmentListener.onMiddleButtonClicked(mTag);
		}
	}

	public void onBottomButtonClicked() {
		if (mSetupFragmentListener != null) {
			mSetupFragmentListener.onBottomButtonClicked(mTag);
		}
	}

	protected void setNoButtonSetup() {
		mTopButton.setVisibility(View.GONE);
		mMiddleButton.setVisibility(View.GONE);
		mBottomButton.setVisibility(View.GONE);
	}

	protected void setOneButtonSetup() {
		mTopButton.setVisibility(View.GONE);
		mMiddleButton.setVisibility(View.GONE);
		mBottomButton.setVisibility(View.VISIBLE);
	}

	protected void setTwoButtonsSetup() {
		mTopButton.setVisibility(View.GONE);
		mMiddleButton.setVisibility(View.VISIBLE);
		mBottomButton.setVisibility(View.VISIBLE);
	}

	public interface SetupFragmentListener {
		void onTopButtonClicked(final String tag);
		void onMiddleButtonClicked(final String tag);
		void onBottomButtonClicked(final String tag);
	}
}
