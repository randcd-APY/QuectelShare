/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

package com.qualcomm.qti.avs.companion.activity;

import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import com.amazon.identity.auth.device.AuthError;
import com.amazon.identity.auth.device.api.Listener;
import com.amazon.identity.auth.device.api.authorization.AuthCancellation;
import com.amazon.identity.auth.device.api.authorization.AuthorizationManager;
import com.amazon.identity.auth.device.api.authorization.AuthorizeListener;
import com.amazon.identity.auth.device.api.authorization.AuthorizeRequest;
import com.amazon.identity.auth.device.api.authorization.AuthorizeResult;
import com.amazon.identity.auth.device.api.authorization.ScopeFactory;
import com.amazon.identity.auth.device.api.workflow.RequestContext;
import com.qualcomm.qti.avs.companion.R;
import com.qualcomm.qti.avs.companion.fragment.CustomDialogFragment;
import com.qualcomm.qti.avs.companion.fragment.SetupFragment;
import com.qualcomm.qti.avs.companion.fragment.SplashFragment;
import com.qualcomm.qti.avs.companion.fragment.ThingsToTryFragment;
import com.qualcomm.qti.avs.companion.util.DeviceSecureConnection;
import com.qualcomm.qti.avs.companion.util.FragmentManagerUtils;
import com.qualcomm.qti.avs.companion.util.RequestAsyncTask;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

import static com.qualcomm.qti.avs.companion.util.DeviceSecureConnection.OK;

public class AlexaSetupActivity extends BaseActivity implements SetupFragment.SetupFragmentListener {
	private static final String TAG = AlexaSetupActivity.class.getSimpleName();

	private static final String ALEXA_ALL_SCOPE = "alexa:all";
	public static final String DEVICE_SERIAL_NUMBER = "deviceSerialNumber";
	private static final String PRODUCT_INSTANCE_ATTRIBUTES = "productInstanceAttributes";
	public static final String PRODUCT_ID = "productID";
	public static final String CODE_CHALLENGE = "codeChallenge";
	public static final String CODE_CHALLENGE_METHOD = "codeChallengeMethod";
	public static final String HOST_ADDRESS = "hostAddress";

	protected Toolbar mToolbar = null;
	protected MenuItem mMenuItem = null;

	private FragmentManagerUtils mFragmentManagerUtils = null;

	private RequestContext mRequestContext;

	private static final String SPLASH_FRAGMENT = "SPLASH_FRAGMENT";
	private static final String THINGS_TO_TRY_FRAGMENT = "THINGS_TO_TRY_FRAGMENT";
	private static final String LOGIN_FAILED_TAG = "LoginFailedTag";

	private String mProductID;
	private String mCodeChallenge;
	private String mDSN;
	private String mCodeChallengeMethod;
	private String mHostAddress;

	private SendProvisionInfoTask mSendProvisionInfoTask = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Bundle bundle = getIntent().getExtras();
		if (bundle != null) {
			mProductID = bundle.getString(PRODUCT_ID);
			mCodeChallenge = bundle.getString(CODE_CHALLENGE);
			mCodeChallengeMethod = bundle.getString(CODE_CHALLENGE_METHOD);
			mDSN = bundle.getString(DEVICE_SERIAL_NUMBER);
			mHostAddress = bundle.getString(HOST_ADDRESS);
		}

		mRequestContext = RequestContext.create(this);
		mRequestContext.registerListener(new AuthorizeListenerImpl());

		setContentView(R.layout.activity_alexa);

		mToolbar = (Toolbar) findViewById(R.id.alexa_setup_tool_bar);
		mToolbar.setTitle(getString(R.string.alexa));
		setSupportActionBar(mToolbar);
		getSupportActionBar().setDisplayShowTitleEnabled(true);
		getSupportActionBar().setDisplayHomeAsUpEnabled(true);

		mToolbar.setNavigationOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				onBackPressed();
			}
		});

		mFragmentManagerUtils = new FragmentManagerUtils(getSupportFragmentManager(), R.id.alexa_setup_frame_layout);
		mFragmentManagerUtils.startFragment(SplashFragment.newInstance(SPLASH_FRAGMENT), SPLASH_FRAGMENT, false);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		mMenuItem = menu.getItem(0);
		if (SPLASH_FRAGMENT.equals(mFragmentManagerUtils.getCurrentFragmentTag())) {
			mMenuItem.setTitle(getString(R.string.skip));
		} else if (THINGS_TO_TRY_FRAGMENT.equals(mFragmentManagerUtils.getCurrentFragmentTag())) {
			mMenuItem.setTitle(getString(R.string.done));
		}
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if (item.getItemId() == R.id.btn_action_bar) {
			finish();
		}
		return super.onOptionsItemSelected(item);
	}

	@Override
	protected void onResume() {
		super.onResume();
		mRequestContext.onResume();
	}

	@Override
	public void onBackPressed() {
		if (SPLASH_FRAGMENT.equals(mFragmentManagerUtils.getCurrentFragmentTag())) {
			super.onBackPressed();
		} else if (THINGS_TO_TRY_FRAGMENT.equals(mFragmentManagerUtils.getCurrentFragmentTag())) {
			finish();
		}
	}

	@Override
	public void onTopButtonClicked(String tag) {}

	@Override
	public void onMiddleButtonClicked(String tag) {}

	@Override
	public void onBottomButtonClicked(String tag) {
		if (SPLASH_FRAGMENT.equals(tag)) {
			final JSONObject scopeData = new JSONObject();
			final JSONObject productInstanceAttributes = new JSONObject();

			try {
				productInstanceAttributes.put(DEVICE_SERIAL_NUMBER, mDSN);
				scopeData.put(PRODUCT_INSTANCE_ATTRIBUTES, productInstanceAttributes);
				scopeData.put(PRODUCT_ID, mProductID);

				AuthorizationManager.authorize(new AuthorizeRequest.Builder(mRequestContext)
						.addScope(ScopeFactory.scopeNamed(ALEXA_ALL_SCOPE, scopeData))
						.forGrantType(AuthorizeRequest.GrantType.AUTHORIZATION_CODE)
						.withProofKeyParameters(mCodeChallenge, mCodeChallengeMethod)
						.build());
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}
	}

	private void showThingsToTry() {
		mFragmentManagerUtils.startFragment(ThingsToTryFragment.newInstance(THINGS_TO_TRY_FRAGMENT), THINGS_TO_TRY_FRAGMENT, true);
		mToolbar.setTitle(getString(R.string.things_to_try));
		if (mMenuItem != null) {
			mMenuItem.setTitle(getString(R.string.done));
		}
		getSupportActionBar().setDisplayHomeAsUpEnabled(false);
	}

	private void sendProvisionInfo(final String authorizationCode, final String redirectUri, final String clientID) {
		if (mSendProvisionInfoTask == null) {
			mSendProvisionInfoTask = new SendProvisionInfoTask(authorizationCode, redirectUri, clientID);
			mSendProvisionInfoTask.execute();
		}
	}

	private class AuthorizeListenerImpl extends AuthorizeListener {
		@Override
		public void onSuccess(final AuthorizeResult authorizeResult) {
			final String authorizationCode = authorizeResult.getAuthorizationCode();
			final String redirectUri = authorizeResult.getRedirectURI();
			final String clientId = authorizeResult.getClientId();

			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					sendProvisionInfo(authorizationCode, redirectUri, clientId);
				}
			});
		}

		@Override
		public void onError(final AuthError authError) {
			CustomDialogFragment customDialogFragment = CustomDialogFragment
					.newDialog(LOGIN_FAILED_TAG, getString(R.string.login_error),
							getString(R.string.auth_error_is, authError.toString()), getString(R.string.ok), null);
			getCustomDialogManager().showDialog(customDialogFragment, LOGIN_FAILED_TAG);
		}

		@Override
		public void onCancel(final AuthCancellation authCancellation) {
			Log.e(TAG, "[onCancel] User cancelled authorization");
		}
	}

	private class SendProvisionInfoTask extends RequestAsyncTask {
		private String mAuthorizationCode;
		private String mRedirectUri;
		private String mClientID;

		private static final String AVS_AUTH = "avsAuth";
		private static final String AVS_CLIENT_ID = "avsClientId";
		private static final String AVS_REDIRECT_URI = "avsRedirectUri";
		private static final String SET_AUTH = "/config/avs/set_auth";

		private boolean mSuccess = false;

		public SendProvisionInfoTask(final String authorizationCode, final String redirectUri, final String clientID) {
			super(AlexaSetupActivity.this, getString(R.string.connecting), true);
			mAuthorizationCode = authorizationCode;
			mRedirectUri = redirectUri;
			mClientID = clientID;
		}

		@Override
		protected Void doInBackground(Void... params) {
			Map<String, String> data = new HashMap<>();
			data.put(AVS_AUTH, mAuthorizationCode);
			data.put(AVS_CLIENT_ID, mClientID);
			data.put(AVS_REDIRECT_URI, mRedirectUri);

			String hostName = mHostAddress;
			String url = "https://" + hostName + SET_AUTH;
			DeviceSecureConnection deviceSecureConnection = new DeviceSecureConnection(url, data);
			DeviceSecureConnection.ConnectionResult result = deviceSecureConnection.doRequest();
			if (result.resultCode == OK) {
				mSuccess = true;
			}
			return null;
		}

		@Override
		protected void onPostExecute(final Void param) {
			super.onPostExecute(param);
			mSendProvisionInfoTask = null;
			if (mSuccess) {
				showThingsToTry();
			}
		}
	}
}
