/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.util;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;

import com.qualcomm.qti.avs.companion.R;

import java.util.Stack;

public class FragmentManagerUtils {
	private static final String TAG = FragmentManagerUtils.class.getSimpleName();

	FragmentManager mFragmentManager;
	int mFrameID;
	Fragment mCurrentFragment = null;
	Stack<String> mTagStack = null;

	public FragmentManagerUtils(final FragmentManager manager, final int frameID) {
		mFragmentManager = manager;
		mFrameID = frameID;
		mTagStack = new Stack<>();
	}

	public void push(final Fragment fragment, final String tag) {
		if (tag == null) {
			return;
		}
		if (tag.equals(getCurrentFragmentTag())) {
			return;
		}
		push(fragment, tag, true);
	}

	private void push(final Fragment fragment, final String tag, final boolean animate) {
		FragmentTransaction ft = mFragmentManager.beginTransaction();
		if (animate) {
			ft.setCustomAnimations(R.anim.slide_in_right, R.anim.slide_out_left);
		}

		if (mCurrentFragment != null) {
			ft.hide(mCurrentFragment);
		}
		mCurrentFragment = fragment;

		ft.add(mFrameID, fragment, tag);
		mTagStack.add(tag);
		ft.commit();

		mFragmentManager.executePendingTransactions();
	}

	public void pop() {
		pop(true);
	}

	public Fragment getCurrentFragment() {
		return mCurrentFragment;
	}

	private void pop(final boolean animate) {
		if (mTagStack.size() > 0) {
			FragmentTransaction ft = mFragmentManager.beginTransaction();
			if (mCurrentFragment != null) {
				if (animate) {
					ft.setCustomAnimations(R.anim.slide_in_left, R.anim.slide_out_right);
				}
				ft.remove(mCurrentFragment);
				mTagStack.pop();
			}
			mCurrentFragment = (mTagStack.size() > 0) ?
					mFragmentManager.findFragmentByTag(getCurrentFragmentTag()) :
					null;
			if (animate && (mCurrentFragment != null)) {
				ft.show(mCurrentFragment);
			}
			ft.commit();
			mFragmentManager.executePendingTransactions();
		}
	}
	public int getCount() {
		return mTagStack.size();
	}

	public String getCurrentFragmentTag() {
		if (mTagStack.isEmpty()) {
			return null;
		}
		return mTagStack.peek();
	}

	public void startFragment(final Fragment fragment, final String tag, final boolean animate) {
		if (tag == null) {
			return;
		}
		if (tag.equals(getCurrentFragmentTag())) {
			return;
		}
		clear();
		push(fragment, tag, animate);
	}

	public void clear() {
		while (mTagStack.size() > 0) {
			pop(false);
		}
	}
}
