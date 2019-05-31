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
import android.widget.ExpandableListView;
import android.widget.ListView;
import android.widget.TextView;

import com.qualcomm.qti.avs.companion.R;

import java.util.List;

public class SetupListFragment extends SetupFragment {
	protected TextView mInstructionTextView = null;
	protected ExpandableListView mExpandableListView = null;
	protected ListView mListView = null;

	protected SetupListFragmentListener mSetupListFragmentListener = null;

	@Override
	public void onAttach (Context context) {
		super.onAttach(context);
		mSetupListFragmentListener = (SetupListFragmentListener)context;
	}

	@Override
	public View onCreateView(final LayoutInflater inflater, final ViewGroup container, final Bundle savedInstanceState) {
		View view = super.onCreateView(inflater, container, savedInstanceState);

		View frameView = inflater.inflate(R.layout.frame_setup_list, mFrameLayout, true);

		mInstructionTextView = (TextView) frameView.findViewById(R.id.setup_instruction_text);

		mExpandableListView = (ExpandableListView) frameView.findViewById(R.id.setup_expandable_list_view);
		mExpandableListView.setOnGroupClickListener(new ExpandableListView.OnGroupClickListener() {
			@Override
			public boolean onGroupClick(ExpandableListView parent, View v, int groupPosition, long id) {
				return true;
			}
		});

		//mEmptyTextView = (TextView) frameView.findViewById(R.id.setup_empty_text);
		//mListView.setEmptyView(mEmptyTextView);

		mListView = (ListView) frameView.findViewById(R.id.setup_list_view);

		return view;
	}

	public interface SetupListFragmentListener {
		void onItemClicked(final String tag, final Object object);
	}
}
