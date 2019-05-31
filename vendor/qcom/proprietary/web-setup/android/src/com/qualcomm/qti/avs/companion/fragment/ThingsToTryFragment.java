/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.fragment;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.method.LinkMovementMethod;
import android.text.style.ClickableSpan;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.qualcomm.qti.avs.companion.R;

public class ThingsToTryFragment extends SetupFragment {
	private static final String TAG = SplashFragment.class.getSimpleName();
	private static final String ALEXA_PACKAGE = "com.amazon.dee.app";

	private TextView mMoreInfoText = null;

	public static ThingsToTryFragment newInstance(final String tag) {
		ThingsToTryFragment fragment = new ThingsToTryFragment();
		Bundle args = new Bundle();
		args.putString(SETUP_TAG_KEY, tag);
		fragment.setArguments(args);
		return fragment;
	}

	@Override
	public View onCreateView(final LayoutInflater inflater, final ViewGroup container, final Bundle savedInstanceState) {
		View view = super.onCreateView(inflater, container, savedInstanceState);

		setNoButtonSetup();

		View thingsToTryView = inflater.inflate(R.layout.frame_alexa_things_to_try, mFrameLayout, true);

		mMoreInfoText = (TextView) thingsToTryView.findViewById(R.id.alexa_more_info_text);

		String moreInfo = getString(R.string.alexa_more_info);
		String alexaApp = getString(R.string.alexa_app);
		SpannableString spannableString = new SpannableString(moreInfo);
		spannableString.setSpan(new ClickableSpan() {
			@Override
			public void onClick(View widget) {
				Intent intent = mBaseActivity.getPackageManager().getLaunchIntentForPackage(ALEXA_PACKAGE);
				if (intent != null) {
					//we found the activity so lets start it
					intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
					startActivity(intent);
				}
				else {
					//activity not found, redirect user to the app on google play store
					startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=" + ALEXA_PACKAGE)));
				}
			}
		}, moreInfo.indexOf(alexaApp), moreInfo.indexOf(alexaApp) + alexaApp.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);

		mMoreInfoText.setText(spannableString);
		mMoreInfoText.setMovementMethod(LinkMovementMethod.getInstance());

		return view;
	}
}
