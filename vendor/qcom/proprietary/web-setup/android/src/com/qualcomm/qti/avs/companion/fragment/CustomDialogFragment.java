/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.fragment;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.text.Editable;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.TextWatcher;
import android.text.method.HideReturnsTransformationMethod;
import android.text.method.PasswordTransformationMethod;
import android.text.style.TextAppearanceSpan;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

import com.qualcomm.qti.avs.companion.R;
import com.qualcomm.qti.avs.companion.activity.BaseActivity;
import com.qualcomm.qti.avs.companion.util.Utils;

import org.w3c.dom.Text;

import java.util.Iterator;

public class CustomDialogFragment extends DialogFragment {
	private static final String TAG = CustomDialogFragment.class.getSimpleName();

	// Internal dialog type IDs
	private static final int CUSTOM_DIALOG = 0;
	private static final int CUSTOM_PROGRESS_DIALOG = 1;
	private static final int CUSTOM_EDIT_TEXT_DIALOG = 2;
	private static final int CUSTOM_ADD_WIFI_DIALOG = 3;
	private static final int CUSTOM_NO_WIFI_DIALOG = 4;

	// Internal dialog keys
	private static final String KEY_TYPE = "keyType";
	private static final String KEY_TAG = "keyTag";
	private static final String KEY_TITLE = "keyTitle";
	private static final String KEY_MESSAGE = "keyMessage";
	private static final String KEY_POSITIVE_BUTTON = "keyPositiveButton";
	private static final String KEY_NEGATIVE_BUTTON = "keyNegativeButton";
	private static final String KEY_HINT = "keyHint";
	private static final String KEY_TEXT = "keyText";
	private static final String KEY_PASSWORD = "keyPassword";

	protected int mTypeDialog;
	protected String mTitle = null;
	protected String mMessage = null;
	protected String mTag = null;
	protected String mPositiveTitle = null;
	protected String mNegativeTitle = null;
	protected String mHint = null;
	protected String mText = null;
	protected boolean mPassword = false;

	protected EditText mEditText = null;
	protected EditText mSSIDEditText = null;
	protected Spinner mSecuritySpinner = null;

	protected BaseActivity mBaseActivity = null;

	private OnCustomDialogDismissedListener mOnCustomDialogDismissedListener = null;
	private OnEditTextKeyClickedListener mOnEditTextKeyClickedListener = null;

	public static CustomDialogFragment newProgressDialog(final String tag, final String message) {
		CustomDialogFragment fragment = new CustomDialogFragment();
		Bundle args = new Bundle();
		args.putInt(KEY_TYPE, CUSTOM_PROGRESS_DIALOG);
		args.putString(KEY_TAG, tag);

		if (!Utils.isStringEmpty(message)) {
			args.putString(KEY_MESSAGE, message.trim());
		}

		fragment.setArguments(args);
		return fragment;
	}

	public static CustomDialogFragment newNoWifiDialog(final String tag) {
		CustomDialogFragment fragment = new CustomDialogFragment();
		Bundle args = new Bundle();
		args.putInt(KEY_TYPE, CUSTOM_NO_WIFI_DIALOG);
		args.putString(KEY_TAG, tag);

		fragment.setArguments(args);
		return fragment;
	}

	public static CustomDialogFragment newDialog(final String tag,
												 final String title, final String message,
												 final String positiveButton, final String negativeButton) {
		CustomDialogFragment fragment = new CustomDialogFragment();
		Bundle args = new Bundle();
		args.putInt(KEY_TYPE, CUSTOM_DIALOG);
		args.putString(KEY_TAG, tag);
		args.putString(KEY_TITLE, title);
		if (!Utils.isStringEmpty(title)) {
			args.putString(KEY_TITLE, title.trim());
		}
		if (!Utils.isStringEmpty(message)) {
			args.putString(KEY_MESSAGE, message.trim());
		}
		if (!Utils.isStringEmpty(positiveButton)) {
			args.putString(KEY_POSITIVE_BUTTON, positiveButton);
		}
		if (!Utils.isStringEmpty(negativeButton)) {
			args.putString(KEY_NEGATIVE_BUTTON, negativeButton);
		}
		fragment.setArguments(args);
		return fragment;
	}

	public static CustomDialogFragment newAddWifiDialog(final String tag) {
		CustomDialogFragment fragment = new CustomDialogFragment();
		Bundle args = new Bundle();
		args.putInt(KEY_TYPE, CUSTOM_ADD_WIFI_DIALOG);
		args.putString(KEY_TAG, tag);
		fragment.setArguments(args);
		return fragment;
	}

	public static CustomDialogFragment newEditTextDialog(final String tag,
														 final String title, final String hint, final String text,
														 final String positiveButton, final String negativeButton,
														 final boolean password) {
		CustomDialogFragment fragment = new CustomDialogFragment();
		Bundle args = new Bundle();
		args.putInt(KEY_TYPE, CUSTOM_EDIT_TEXT_DIALOG);
		args.putString(KEY_TAG, tag);
		args.putString(KEY_TITLE, title);
		if (!Utils.isStringEmpty(title)) {
			args.putString(KEY_TITLE, title.trim());
		}
		if (!Utils.isStringEmpty(text)) {
			args.putString(KEY_TEXT, text);
		}
		if (!Utils.isStringEmpty(hint)) {
			args.putString(KEY_HINT, hint);
		}
		if (!Utils.isStringEmpty(positiveButton)) {
			args.putString(KEY_POSITIVE_BUTTON, positiveButton);
		}
		if (!Utils.isStringEmpty(negativeButton)) {
			args.putString(KEY_NEGATIVE_BUTTON, negativeButton);
		}
		args.putBoolean(KEY_PASSWORD, password);
		fragment.setArguments(args);
		return fragment;
	}

	@Override
	public void onAttach (Context context) {
		super.onAttach(context);
		mBaseActivity = (BaseActivity) context;
	}

	public void setOnCustomDialogDismissedListener(final OnCustomDialogDismissedListener listener) {
		mOnCustomDialogDismissedListener = listener;
	}

	public void setOnEditTextKeyClickedListener(final OnEditTextKeyClickedListener listener) {
		mOnEditTextKeyClickedListener = listener;
	}

	public String getEditText() {
		if (mEditText != null) {
			return mEditText.getText().toString();
		}
		return null;
	}

	@Override
	public void onDismiss(DialogInterface dialog) {
		EditText editText = mSSIDEditText;
		if ((editText == null) || !editText.isFocused()) {
			editText = mEditText;
		}
		if ((editText != null) && (mBaseActivity != null)) {
			InputMethodManager imm = (InputMethodManager)mBaseActivity.getSystemService(Context.INPUT_METHOD_SERVICE);
			imm.hideSoftInputFromInputMethod(editText.getWindowToken(), 0);
		}
		if (mOnCustomDialogDismissedListener != null) {
			mOnCustomDialogDismissedListener.onDismissed(mTag);
		}
	}

	@Override
	public Dialog onCreateDialog(final Bundle savedInstanceState) {
		// Go through all of the arguments and set internal values
		if (getArguments() != null) {
			Iterator<String> it = getArguments().keySet().iterator();
			while (it.hasNext()) {
				String key = it.next();
				if (key.equals(KEY_TYPE)) {
					mTypeDialog = getArguments().getInt(key);
				} else if (key.equals(KEY_TITLE)) {
					mTitle = getArguments().getString(key);
				} else if (key.equals(KEY_MESSAGE)) {
					mMessage = getArguments().getString(key);
				} else if (key.equals(KEY_TAG)) {
					mTag = getArguments().getString(key);
				} else if (key.equals(KEY_POSITIVE_BUTTON)) {
					mPositiveTitle = getArguments().getString(key);
				} else if (key.equals(KEY_NEGATIVE_BUTTON)) {
					mNegativeTitle = getArguments().getString(key);
				} else if (key.equals(KEY_HINT)) {
					mHint = getArguments().getString(key);
				} else if (key.equals(KEY_TEXT)) {
					mText = getArguments().getString(key);
				} else if (key.equals(KEY_PASSWORD)) {
					mPassword = getArguments().getBoolean(key);
				}
			}
		}

		Dialog dialog = createDialogFromType();

		if (dialog != null) {
			dialog.setCancelable(false);
			dialog.setCanceledOnTouchOutside(false);
		}

		return dialog;
	}

	protected Dialog createDialogFromType() {
		Dialog dialog = null;
		switch (mTypeDialog) {
			case CUSTOM_DIALOG:
				dialog = createDialog();
				break;
			case CUSTOM_PROGRESS_DIALOG:
				// Progress dialog
				dialog = createCustomProgresstDialog();
				break;
			case CUSTOM_ADD_WIFI_DIALOG:
				dialog = createCustomAddWifiDialog();
				break;
			case CUSTOM_EDIT_TEXT_DIALOG:
				dialog = createCustomEditTextDialog();
				break;
			case CUSTOM_NO_WIFI_DIALOG:
				dialog = createCustomNoWifiDialog();
				break;
			default:
				break;
		}

		return dialog;
	}

	private Dialog createDialog() {
		AlertDialog.Builder dialog = new AlertDialog.Builder(getActivity());

		SpannableString title = new SpannableString(mTitle);
		title.setSpan(new TextAppearanceSpan(getContext(), R.style.CustomDialogTitle), 0,
				mTitle.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
		dialog.setTitle(title);

		SpannableString message = new SpannableString(mMessage);
		message.setSpan(new TextAppearanceSpan(getContext(), R.style.CustomDialogMessage), 0,
				mMessage.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
		dialog.setMessage(message);

		if (!Utils.isStringEmpty(mPositiveTitle)) {
			dialog.setPositiveButton(mPositiveTitle, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					if (mOnCustomDialogDismissedListener != null) {
						mOnCustomDialogDismissedListener.onPositiveButtonClicked(mTag);
					}
				}
			});
		}
		if (!Utils.isStringEmpty(mNegativeTitle)) {
			dialog.setNegativeButton(mNegativeTitle, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					if (mOnCustomDialogDismissedListener != null) {
						mOnCustomDialogDismissedListener.onNegativeButtonClicked(mTag);
					}
				}
			});
		}

		return dialog.create();
	}

	private Dialog createCustomNoWifiDialog() {
		final ProgressDialog dialog = new ProgressDialog(getActivity());
		dialog.setIndeterminate(true);
		mMessage = getString(R.string.no_wifi_connection);
		SpannableString spannableString = new SpannableString(mMessage);
		spannableString.setSpan(new TextAppearanceSpan(getContext(), R.style.CustomDialogMessage), 0,
				mMessage.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
		dialog.setMessage(spannableString);
		dialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
			@Override
			public boolean onKey(final DialogInterface dialog, final int keyCode, final KeyEvent event) {
				if (keyCode == KeyEvent.KEYCODE_BACK) {
					//handled
					return true;
				}
				return false;
			}
		});

		return dialog;
	}

	private Dialog createCustomProgresstDialog() {
		final ProgressDialog dialog = new ProgressDialog(getActivity());
		dialog.setIndeterminate(true);
		SpannableString spannableString = new SpannableString(mMessage);
		spannableString.setSpan(new TextAppearanceSpan(getContext(), R.style.CustomDialogMessage), 0,
				mMessage.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
		dialog.setMessage(spannableString);
		dialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
			@Override
			public boolean onKey(final DialogInterface dialog, final int keyCode, final KeyEvent event) {
				if (keyCode == KeyEvent.KEYCODE_BACK) {
					//handled
					return true;
				}
				return false;
			}
		});

		return dialog;
	}

	private Dialog createCustomEditTextDialog() {
		AlertDialog.Builder dialog = new AlertDialog.Builder(getActivity());

		View password = LayoutInflater.from(getActivity()).inflate(R.layout.dialog_edit_text, null);
		dialog.setView(password);

		final TextView textView = (TextView) password.findViewById(R.id.custom_dialog_edit_text_title);
		mEditText = (EditText) password.findViewById(R.id.custom_dialog_edit_text);
		final CheckBox checkBox = (CheckBox) password.findViewById(R.id.custom_dialog_checkbox);

		textView.setText(mTitle);
		checkBox.setVisibility(mPassword ? View.VISIBLE : View.GONE);

		mEditText.setTransformationMethod((mPassword) ? PasswordTransformationMethod.getInstance() :
				HideReturnsTransformationMethod.getInstance());
		if (mPassword) {
			checkBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
				@Override
				public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
					mEditText.setTransformationMethod((!isChecked) ? PasswordTransformationMethod.getInstance() :
							HideReturnsTransformationMethod.getInstance());
					mEditText.setSelection(mEditText.length());
				}
			});
		}

		if (!Utils.isStringEmpty(mPositiveTitle)) {
			dialog.setPositiveButton(mPositiveTitle, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					if (mOnCustomDialogDismissedListener != null) {
						mOnCustomDialogDismissedListener.onPositiveButtonClicked(mTag);
					}
				}
			});
		}
		if (!Utils.isStringEmpty(mNegativeTitle)) {
			dialog.setNegativeButton(mNegativeTitle, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					if (mOnCustomDialogDismissedListener != null) {
						mOnCustomDialogDismissedListener.onNegativeButtonClicked(mTag);
					}
				}
			});
		}

		final AlertDialog editTextDialog = dialog.create();

		mEditText.addTextChangedListener(new TextWatcher() {
			@Override
			public void beforeTextChanged(CharSequence s, int start, int count, int after) {
			}

			@Override
			public void onTextChanged(CharSequence s, int start, int before, int count) {
				final Button positiveButton = editTextDialog.getButton(DialogInterface.BUTTON_POSITIVE);
				positiveButton.setEnabled(s.length() > 0);
			}

			@Override
			public void afterTextChanged(Editable s) {
			}
		});

		mEditText.setOnEditorActionListener(new TextView.OnEditorActionListener() {
			@Override
			public boolean onEditorAction(TextView view, int actionId, KeyEvent event) {
				if ((actionId == EditorInfo.IME_ACTION_DONE) && (mOnEditTextKeyClickedListener != null)) {
					mOnEditTextKeyClickedListener.onDoneClicked(mTag, mEditText);
					return true;
				}
				return false;
			}
		});

		editTextDialog.setOnShowListener(new DialogInterface.OnShowListener() {
			@Override
			public void onShow(DialogInterface dialog) {
				InputMethodManager imm = (InputMethodManager) getActivity().getSystemService(Context.INPUT_METHOD_SERVICE);
				imm.showSoftInput(mEditText, InputMethodManager.SHOW_IMPLICIT);
				final Button positiveButton = editTextDialog.getButton(DialogInterface.BUTTON_POSITIVE);
				positiveButton.setEnabled(false);
			}
		});

		return editTextDialog;
	}

	private Dialog createCustomAddWifiDialog() {
		AlertDialog.Builder dialog = new AlertDialog.Builder(getActivity());
		View addWifiView = LayoutInflater.from(getActivity()).inflate(R.layout.dialog_add_wifi, null);
		dialog.setView(addWifiView);

		mSSIDEditText = (EditText) addWifiView.findViewById(R.id.custom_dialog_add_wifi_ssid_edit_text);
		mSecuritySpinner = (Spinner) addWifiView.findViewById(R.id.custom_dialog_add_wifi_security_spinner);
		mEditText = (EditText) addWifiView.findViewById(R.id.custom_dialog_add_wifi_password_text);

		final TextView passwordText = (TextView) addWifiView.findViewById(R.id.custom_dialog_add_wifi_password_header);
		final CheckBox checkBox = (CheckBox) addWifiView.findViewById(R.id.custom_dialog_add_wifi_checkbox);

		SecurityAdapter adapter = new SecurityAdapter(getActivity());
		adapter.setData(getResources().getStringArray(R.array.network_security_array));
		mSecuritySpinner.setAdapter(adapter);

		mEditText.setTransformationMethod(PasswordTransformationMethod.getInstance());
		checkBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				mEditText.setTransformationMethod((!isChecked) ? PasswordTransformationMethod.getInstance() :
						HideReturnsTransformationMethod.getInstance());
				mEditText.setSelection(mEditText.length());
			}
		});

		dialog.setPositiveButton(getString(R.string.connect), new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				if (mOnCustomDialogDismissedListener != null) {
					mOnCustomDialogDismissedListener.onPositiveButtonClicked(mTag);
				}
			}
		});

		dialog.setNegativeButton(getString(R.string.cancel), new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				if (mOnCustomDialogDismissedListener != null) {
					mOnCustomDialogDismissedListener.onNegativeButtonClicked(mTag);
				}
			}
		});

		final AlertDialog addWifiDialog = dialog.create();

		mSecuritySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
			@Override
			public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
				updateAddWifiUI(addWifiDialog, mSecuritySpinner, passwordText, mEditText, checkBox);
			}

			@Override
			public void onNothingSelected(AdapterView<?> parent) {
			}
		});
		mSecuritySpinner.setSelection(1);

		mSSIDEditText.addTextChangedListener(new TextWatcher() {
			@Override
			public void beforeTextChanged(CharSequence s, int start, int count, int after) {}

			@Override
			public void onTextChanged(CharSequence s, int start, int before, int count) {
				updateAddWifiUI(addWifiDialog, mSecuritySpinner, passwordText, mEditText, checkBox);
			}

			@Override
			public void afterTextChanged(Editable s) {}
		});

		mSSIDEditText.setOnEditorActionListener(new TextView.OnEditorActionListener() {
			@Override
			public boolean onEditorAction(TextView view, int actionId, KeyEvent event) {
				if (actionId == EditorInfo.IME_ACTION_DONE) {
					boolean isOpen = getString(R.string.security_open).equals(mSecuritySpinner.getSelectedItem());

					if (isOpen) {
						if (mOnEditTextKeyClickedListener != null) {
							mOnEditTextKeyClickedListener.onDoneClicked(mTag, mEditText);
						}
					} else {
						mEditText.requestFocus();
					}

					return true;
				}
				return false;
			}
		});

		mEditText.addTextChangedListener(new TextWatcher() {
			@Override
			public void beforeTextChanged(CharSequence s, int start, int count, int after) {}

			@Override
			public void onTextChanged(CharSequence s, int start, int before, int count) {
				updateAddWifiUI(addWifiDialog, mSecuritySpinner, passwordText, mEditText, checkBox);
			}

			@Override
			public void afterTextChanged(Editable s) {}
		});

		mEditText.setOnEditorActionListener(new TextView.OnEditorActionListener() {
			@Override
			public boolean onEditorAction(TextView view, int actionId, KeyEvent event) {
				if ((actionId == EditorInfo.IME_ACTION_DONE) && (mOnEditTextKeyClickedListener != null)) {
					mOnEditTextKeyClickedListener.onDoneClicked(mTag, mEditText);
					return true;
				}
				return false;
			}
		});

		addWifiDialog.setOnShowListener(new DialogInterface.OnShowListener() {
			@Override
			public void onShow(DialogInterface dialog) {
				mSSIDEditText.requestFocus();
				InputMethodManager imm = (InputMethodManager) getActivity().getSystemService(Context.INPUT_METHOD_SERVICE);
				imm.showSoftInput(mSSIDEditText, InputMethodManager.SHOW_IMPLICIT);
				final Button positiveButton = addWifiDialog.getButton(DialogInterface.BUTTON_POSITIVE);
				positiveButton.setEnabled(false);
			}
		});

		return addWifiDialog;
	}

	private void updateAddWifiUI(final AlertDialog alertDialog, final Spinner spinner, final TextView textView,
								 final EditText editText, final CheckBox checkBox) {
		boolean isOpen = getString(R.string.security_open).equals(spinner.getSelectedItem());
		textView.setVisibility(isOpen ? View.GONE : View.VISIBLE);
		editText.setVisibility(isOpen ? View.GONE : View.VISIBLE);
		checkBox.setVisibility(isOpen ? View.GONE : View.VISIBLE);

		final Button positiveButton = alertDialog.getButton(DialogInterface.BUTTON_POSITIVE);
		positiveButton.setEnabled((isOpen) ? !mSSIDEditText.getText().toString().isEmpty() :
				(!mSSIDEditText.getText().toString().isEmpty() && !mEditText.getText().toString().isEmpty()));

	}

	public WifiCredential getWifiCredential() {
		if (mTypeDialog == CUSTOM_ADD_WIFI_DIALOG) {
			boolean isOpen = getString(R.string.security_open).equals(mSecuritySpinner.getSelectedItem());
			return new WifiCredential(mSSIDEditText.getText().toString(),
					(isOpen) ? null : mEditText.getText().toString(), isOpen);
		}
		return null;
	}

	public class WifiCredential {
		public String SSID;
		public String password;
		public boolean isOpen = false;

		public WifiCredential(final String SSID, final String password, final boolean isOpen) {
			this.SSID = SSID;
			this.password = password;
			this.isOpen = isOpen;
		}
	}

	public interface OnCustomDialogDismissedListener {
		void onPositiveButtonClicked(final String tag);
		void onNegativeButtonClicked(final String tag);
		void onDismissed(final String tag);
	}

	public interface OnEditTextKeyClickedListener {
		void onDoneClicked(final String tag, final EditText editText);
	}

	private class SecurityAdapter extends BaseAdapter {
		private String[] mSecurityList;
		private final LayoutInflater mInflater;

		public SecurityAdapter(final Context context) {
			mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		}

		public void setData(final String[] securityList) {
			mSecurityList = securityList;
			notifyDataSetChanged();
		}

		@Override
		public int getCount() {
			return mSecurityList.length;
		}

		@Override
		public Object getItem(int position) {
			return mSecurityList[position];
		}

		@Override
		public long getItemId(int position) {
			return position;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			final String security = mSecurityList[position];

			if (convertView == null) {
				convertView = mInflater.inflate(R.layout.list_item_security, parent, false);
			}
			TextView textView = (TextView)convertView.findViewById(R.id.list_item_security_text);
			if (textView != null) {
				textView.setText(security);
			}

			return convertView;
		}

	}
}
