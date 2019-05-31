package com.qualcomm.qti.avs.companion.util;

import android.os.AsyncTask;

import com.qualcomm.qti.avs.companion.activity.BaseActivity;
import com.qualcomm.qti.avs.companion.fragment.CustomDialogFragment;

import java.lang.ref.WeakReference;


public class RequestAsyncTask extends AsyncTask<Void, Void, Void> {

	public static final String REQUEST_DIALOG_TAG = "RequestDialogTag";

	protected WeakReference<BaseActivity> mActivityRef = null;

	private long mStartTime = 0;
	protected String mMessage = null;
	private boolean mShowDialog;
	private boolean mDismissDialog = true;

	private Object mWaitCondition = new Object();

	public RequestAsyncTask(final BaseActivity baseActivity, final String message, final boolean showDialog) {
		mActivityRef = new WeakReference<>(baseActivity);
		mMessage = message;
		mShowDialog = showDialog;
	}

	protected void dismissDialog(final boolean dismissDialog) {
		mDismissDialog = dismissDialog;
	}

	@Override
	protected void onPreExecute() {
		final BaseActivity baseActivity = mActivityRef.get();
		if (!Utils.isActivityActive(baseActivity)) {
			return;
		}
		if (mShowDialog) {
			CustomDialogFragment customDialogFragment = CustomDialogFragment
					.newProgressDialog(REQUEST_DIALOG_TAG, mMessage);
			baseActivity.getCustomDialogManager().showDialog(customDialogFragment, REQUEST_DIALOG_TAG);
		}
		mStartTime = System.currentTimeMillis();
	}

	@Override
	protected Void doInBackground(Void... params) {
		return null;
	}

	@Override
	protected void onPostExecute(final Void param) {
		final BaseActivity baseActivity = mActivityRef.get();
		if (!Utils.isActivityActive(baseActivity)) {
			return;
		}
		if (mDismissDialog) {
			baseActivity.getCustomDialogManager().dismissDialog(REQUEST_DIALOG_TAG);
		}
	}

	protected void doWait(final int timeout) {
		long currentTime = System.currentTimeMillis();
		if ((currentTime - mStartTime) < timeout) {
			synchronized (mWaitCondition) {
				try {
					mWaitCondition.wait(timeout - (currentTime - mStartTime));
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
	}

	public void waitUntilFinished() {
		synchronized (mWaitCondition) {
			try {
				mWaitCondition.wait();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}

	protected void finished() {
		synchronized (mWaitCondition) {
			mWaitCondition.notifyAll();
		}
	}
}
