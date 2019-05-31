/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package org.codeaurora.bluetooth.hidtestapp;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.IBluetooth;
import android.bluetooth.IBluetoothInputDevice;
import android.bluetooth.BluetoothInputDevice;
import android.bluetooth.BluetoothProfile.ServiceListener;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class HidTestApp extends Activity implements OnClickListener {
	private static final String TAG = "HidTestApp";
	private static BluetoothInputDevice mService = null;
	private BluetoothAdapter mAdapter;
	private BluetoothDevice mRemoteDevice;
	private Context mContext;

	private EditText mEtBtAddress, mEtBtCommand;
	private Button mBtnExecute;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.hidtestapp);
		Log.i(TAG, "On Create ");
		mEtBtAddress = (EditText) findViewById(R.id.id_et_btaddress);
		mEtBtCommand = (EditText) findViewById(R.id.id_et_btcommand);
		mBtnExecute = (Button) findViewById(R.id.id_btn_execute);
		mBtnExecute.setOnClickListener(this);
		mContext = getApplicationContext();
		mAdapter = BluetoothAdapter.getDefaultAdapter();
		init();
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		deinit();
	}

	@Override
	public void onClick(View v) {
		if (v == mBtnExecute) {
			executeCommand();
		}

	}

	private boolean executeCommand() {

		boolean isValid = true;
		String address = mEtBtAddress.getText().toString().trim();
		String command = mEtBtCommand.getText().toString().trim();
		Log.e(TAG, "address = " + address);
		Log.e(TAG, "command = " + command);
		if (address.length() != 17) {
			mEtBtAddress.setError("Enter Proper Address");
			isValid = false;
		} else if (command.length() == 0) {
			mEtBtCommand.setError("Enter proper command");
			isValid = false;
		}

		if (isValid) {
		    /* Make sure that all the address bytes are in upper case */
			String addSplit[] = address.split(":");
			address = addSplit[0].toUpperCase() + ":" + addSplit[1].toUpperCase() + ":" +
				addSplit[2].toUpperCase() + ":" + addSplit[3].toUpperCase() + ":" +
				addSplit[4].toUpperCase() + ":" + addSplit[5].toUpperCase();
			Log.e(TAG, "address = " + address);
			try {
				mRemoteDevice = mAdapter.getRemoteDevice(address);
			} catch (IllegalArgumentException e) {
				Toast.makeText(mContext, "Invalid BD Address", Toast.LENGTH_LONG).show();
				return false;
			}
			String[] separated = command.split(" ");
			int len = separated.length;

			if (len > 0) {
				if (separated[0].equals("getreport")) {
					if (len != 4) {
						Toast.makeText(mContext, "Invalid getreport command", Toast.LENGTH_LONG).show();
						isValid = false;
					} else {
						try {
							int reportType = Integer.parseInt(separated[1]);
							int reportId = Integer.parseInt(separated[2]);
							int bufferSize = Integer.parseInt(separated[3]);
							Log.v(TAG," reportType " + reportType);
							Log.v(TAG," reportId " + reportId);
							Log.v(TAG," bufferSize " + bufferSize);
							getReport(mRemoteDevice, (byte)reportType, (byte)reportId, bufferSize);
							Toast.makeText(mContext, "getreport command sent", Toast.LENGTH_LONG).show();
						} catch(NumberFormatException e) {
							Toast.makeText(mContext, "getreport input parameters exception",
                                                               Toast.LENGTH_LONG).show();
						}
					}
				} else if (separated[0].equals("setreport")) {
					if (len != 3) {
						Toast.makeText(mContext, "Invalid setreport command", Toast.LENGTH_LONG).show();
						isValid = false;
					} else {
						try {
							int reportType = Integer.parseInt(separated[1]);
							Log.v(TAG," reportType " + reportType);
							Log.v(TAG," report " + separated[2]);
							setReport(mRemoteDevice, (byte)reportType, separated[2]);
							Toast.makeText(mContext, "setreport command sent", Toast.LENGTH_LONG).show();
						} catch(NumberFormatException e) {
							Toast.makeText(mContext, "setreport input parameters exception",
                                                               Toast.LENGTH_LONG).show();
						}
					}
				}  else if (separated[0].equals("virtualunplug")) {
					if (len != 1) {
						Toast.makeText(mContext, "Invalid virtualunplug command", Toast.LENGTH_LONG).show();
						isValid = false;
					} else {
					    virtualUnplug(mRemoteDevice);
						Toast.makeText(mContext, "virtualunplug command sent", Toast.LENGTH_LONG).show();
					}
				} else if (separated[0].equals("getprotocolmode")) {
					if (len != 1) {
						Toast.makeText(mContext, "Invalid getprotocolmode command", Toast.LENGTH_LONG).show();
						isValid = false;
					} else {
					    getProtocolMode(mRemoteDevice);
						Toast.makeText(mContext, "getprotocolmode command sent", Toast.LENGTH_LONG).show();
					}
				} else if (separated[0].equals("setprotocolmode")) {
					if (len != 2) {
						Toast.makeText(mContext, "Invalid setprotocolmode command", Toast.LENGTH_LONG).show();
						isValid = false;
					} else {
						try {
							int mode = Integer.parseInt(separated[1]);
							Log.v(TAG," mode " + mode);
							setProtocolMode(mRemoteDevice, mode);
							Toast.makeText(mContext, "setprotocolmode command sent", Toast.LENGTH_LONG).show();
						} catch(NumberFormatException e) {
							Toast.makeText(mContext, "setprotocolmode input parameters exception",
                                                               Toast.LENGTH_LONG).show();
						}
					}
				} else if (separated[0].equals("getidle")) {
					if (len != 1) {
						Toast.makeText(mContext, "Invalid getidle command", Toast.LENGTH_LONG).show();
						isValid = false;
					} else {
					    getIdleTime(mRemoteDevice);
						Toast.makeText(mContext, "getidle command sent", Toast.LENGTH_LONG).show();
					}
				} else if (separated[0].equals("setidle")) {
					if (len != 2) {
						Toast.makeText(mContext, "Invalid setidle command", Toast.LENGTH_LONG).show();
						isValid = false;
					} else {
						try {
							int idleTime = Integer.parseInt(separated[1]);
							Log.v(TAG," idleTime " + idleTime);
							setIdleTime(mRemoteDevice, (byte)idleTime);
							Toast.makeText(mContext, "setidle command sent", Toast.LENGTH_LONG).show();
						} catch(NumberFormatException e) {
							Toast.makeText(mContext, "setidle input parameters exception",
                                                               Toast.LENGTH_LONG).show();
						}
					}
				} else if (separated[0].equals("senddata")) {
					if (len != 2) {
						Toast.makeText(mContext, "Invalid senddata command", Toast.LENGTH_LONG).show();
						isValid = false;
					} else {
						Log.v(TAG," report " + separated[1]);
						sendData(mRemoteDevice, separated[1]);
						Toast.makeText(mContext, "senddata command sent", Toast.LENGTH_LONG).show();
					}
				} else {
					Toast.makeText(mContext, "Invalid command", Toast.LENGTH_LONG).show();
					Log.v(TAG," Invalid command " + separated[0]);
					isValid = false;
				}
			}
		}
		return isValid;

	}

    public void init() {
        if (!mAdapter.getProfileProxy(mContext, mServiceListener,
                BluetoothProfile.INPUT_DEVICE)) {
            Log.w(TAG, "Cannot obtain profile proxy");
            return;
        }
    }

    public void deinit() {
        if (mService != null) {
            mAdapter.closeProfileProxy(BluetoothProfile.INPUT_DEVICE, mService);
        }
    }

    boolean getProtocolMode(BluetoothDevice device) {
        if (mService == null) return false;
        return mService.getProtocolMode(device);
    }

    boolean setProtocolMode(BluetoothDevice device, int mode) {
        if (mService == null) return false;
        return mService.setProtocolMode(device, mode);
    }

    boolean virtualUnplug(BluetoothDevice device) {
        if (mService == null) return false;
        return mService.virtualUnplug(device);
    }

    boolean getReport(BluetoothDevice device, byte reportType, byte reportId, int bufferSize) {
        if (mService == null) return false;
        return mService.getReport(device, reportType, reportId, bufferSize);
    }

    boolean setReport(BluetoothDevice device, byte reportType, String report) {
        if (mService == null) return false;
        return mService.setReport(device, reportType, report);
    }

    boolean sendData(BluetoothDevice device, String report) {
        if (mService == null) return false;
        return mService.sendData(device, report);
    }

    boolean getIdleTime(BluetoothDevice device) {
        if (mService == null) return false;
        return mService.getIdleTime(device);
    }

    boolean setIdleTime(BluetoothDevice device, byte idleTime) {
        if (mService == null) return false;
        return mService.setIdleTime(device, idleTime);
    }

    private final ServiceListener mServiceListener = new ServiceListener() {

        @Override
        public void onServiceConnected(int profile, BluetoothProfile proxy) {
            if (profile != BluetoothProfile.INPUT_DEVICE) return;

            Log.i(TAG, "Profile proxy connected");

            mService = (BluetoothInputDevice) proxy;

        }

        @Override
        public void onServiceDisconnected(int profile) {
            if (profile != BluetoothProfile.INPUT_DEVICE) return;

            Log.i(TAG, "Profile proxy disconnected");

            mService = null;
        }
    };
}
