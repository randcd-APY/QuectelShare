/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.util;

import com.qualcomm.qti.avs.companion.activity.BaseActivity;

public class Utils {
	public static boolean isStringEmpty(final String text) {
		return (text == null) || text.trim().isEmpty();
	}

	public static boolean isActivityActive(final BaseActivity baseActivity) {
		return (baseActivity != null) && !baseActivity.isSaveStateCalled() && !baseActivity.isFinishing();
	}
}
