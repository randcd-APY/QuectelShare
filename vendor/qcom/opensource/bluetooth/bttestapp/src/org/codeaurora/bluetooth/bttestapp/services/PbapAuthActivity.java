/*
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *            notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *            notice, this list of conditions and the following disclaimer in the
 *            documentation and/or other materials provided with the distribution.
 *        * Neither the name of The Linux Foundation nor
 *            the names of its contributors may be used to endorse or promote
 *            products derived from this software without specific prior written
 *            permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.    IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package org.codeaurora.bluetooth.bttestapp.services;

import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.InputFilter;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.android.internal.app.AlertActivity;
import com.android.internal.app.AlertController;
import org.codeaurora.bluetooth.bttestapp.ProfileService;
import org.codeaurora.bluetooth.bttestapp.R;

public class PbapAuthActivity extends AlertActivity implements DialogInterface.OnClickListener,
        TextWatcher {

    private static final String TAG = "PbapAuthActivity";

    private static final int KEY_MAX_LENGTH = 16;

    private static final int TIMEOUT_MSG = 0;

    private static final int TIMEOUT_VALUE_MS = 2000;

    private static final String KEY_USER_TIMEOUT = "user_timeout";

    private BluetoothDevice mDevice;

    private Button mOkButton;

    private EditText mKeyEditText;

    private TextView mMessageTextView;

    private String mKey;

    private boolean mTimeout = false;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (ProfileService.PBAP_AUTH_ACTION_TIMEOUT.equals(intent.getAction())) {
                onTimeout();
            }
        }
    };

    private final Handler mTimeoutHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case TIMEOUT_MSG:
                    finish();
                    break;

                default:
                    break;
            }
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();

        if (ProfileService.PBAP_AUTH_ACTION_REQUEST.equals(intent.getAction())) {
            mDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            showAuthDialog();

            registerReceiver(mReceiver, new IntentFilter(ProfileService.PBAP_AUTH_ACTION_TIMEOUT));
        } else {
            Log.e(TAG, "This activity can be also started with AUTH_ACTION_REQUEST intent");
            finish();
        }
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                mKey = mKeyEditText.getText().toString();
                onPositive();
                break;

            case DialogInterface.BUTTON_NEGATIVE:
                onNegative();
                break;

            default:
                break;
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mReceiver);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);

        mTimeout = savedInstanceState.getBoolean(KEY_USER_TIMEOUT);

        if (mTimeout) {
            onTimeout();
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        outState.putBoolean(KEY_USER_TIMEOUT, mTimeout);
    }

    @Override
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {
    }

    @Override
    public void onTextChanged(CharSequence s, int start, int before, int count) {
    }

    @Override
    public void afterTextChanged(Editable s) {
        if (s.length() > 0) {
            mOkButton.setEnabled(true);
        }
    }

    private void showAuthDialog() {
        final AlertController.AlertParams p = mAlertParams;

        p.mIconId = android.R.drawable.ic_dialog_info;
        p.mTitle = getString(R.string.pbap_session_key_dialog_header);

        p.mPositiveButtonText = getString(android.R.string.ok);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonText = getString(android.R.string.cancel);
        p.mNegativeButtonListener = this;

        p.mView = createView();

        setupAlert();

        mOkButton = mAlert.getButton(DialogInterface.BUTTON_POSITIVE);
        mOkButton.setEnabled(false);
    }

    private View createView() {
        View view = getLayoutInflater().inflate(R.layout.pbap_auth, null);

        mMessageTextView = (TextView) view.findViewById(R.id.message);
        mMessageTextView.setText(getString(R.string.pbap_session_key_dialog_title,
                mDevice.getName()));

        mKeyEditText = (EditText) view.findViewById(R.id.text);
        mKeyEditText.addTextChangedListener(this);
        mKeyEditText.setFilters(new InputFilter[] {
                new InputFilter.LengthFilter(KEY_MAX_LENGTH)
        });

        return view;
    }

    private void onTimeout() {
        mTimeout = true;

        mMessageTextView.setText(getString(R.string.pbap_authentication_timeout_message,
                mDevice.getName()));

        mKeyEditText.setVisibility(View.GONE);
        mKeyEditText.clearFocus();
        mKeyEditText.removeTextChangedListener(PbapAuthActivity.this);

        mOkButton.setEnabled(true);

        mAlert.getButton(DialogInterface.BUTTON_NEGATIVE).setVisibility(View.GONE);

        mTimeoutHandler.sendMessageDelayed(mTimeoutHandler.obtainMessage(TIMEOUT_MSG),
                TIMEOUT_VALUE_MS);
    }

    private void onPositive() {
        if (!mTimeout) {
            Intent intent = new Intent(ProfileService.PBAP_AUTH_ACTION_RESPONSE);
            intent.putExtra(ProfileService.PBAP_AUTH_EXTRA_KEY, mKey);
            sendBroadcast(intent);

            mKeyEditText.removeTextChangedListener(this);
        }

        mTimeout = false;
        finish();
    }

    private void onNegative() {
        Intent intent = new Intent(ProfileService.PBAP_AUTH_ACTION_CANCEL);
        sendBroadcast(intent);

        mKeyEditText.removeTextChangedListener(this);

        finish();
    }
}
