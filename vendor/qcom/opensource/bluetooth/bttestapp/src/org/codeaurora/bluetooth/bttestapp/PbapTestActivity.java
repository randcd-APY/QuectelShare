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

package org.codeaurora.bluetooth.bttestapp;

import android.app.ActionBar;
import android.app.ActionBar.Tab;
import android.app.ActionBar.TabListener;
import android.app.FragmentTransaction;
import android.bluetooth.BluetoothDevice;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.IBinder;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;

import com.android.vcard.VCardEntry;
import android.bluetooth.client.pbap.BluetoothPbapCard;
import android.bluetooth.client.pbap.BluetoothPbapClient;
import org.codeaurora.bluetooth.bttestapp.R;
import org.codeaurora.bluetooth.bttestapp.services.IPbapServiceCallback;
import org.codeaurora.bluetooth.bttestapp.util.Logger;
import org.codeaurora.bluetooth.bttestapp.util.MonkeyEvent;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;

public class PbapTestActivity extends MonkeyActivity implements IBluetoothConnectionObserver {

    private final String TAG = "PbapTestActivity";

    /*
     * Class constants.
     */
    private static final int REQUEST_CODE_GET_FILTER_FOR_DOWNLOAD_TAB = 0;
    private static final int REQUEST_CODE_GET_FILTER_FOR_VCARD_TAB = 1;

    private ArrayDeque<String> mSetPathQueue = null;

    private static final short MAX_COUNT_DEFAULT_VALUE = 0;
    private static final short OFFSET_DEFAULT_VALUE = 0;
    private static final String HANDLE_DEFAULT_VALUE = "0.vcf";

    /*
     * Common UI.
     */
    private ActionBar mActionBar = null;
    private ViewFlipper mViewFlipper = null;
    /*
     * Download functionality UI.
     */
    private Spinner mDownloadSpinner = null;
    private RadioButton mRadioButtonVCard21 = null;
    private RadioButton mRadioButtonVCard30 = null;
    private EditText mEditTextDownloadMaxListCount = null;
    private EditText mEditTextDownloadOffsetValue = null;
    private Button mButtonFilter = null;
    private Button mButtonDownloadSearch = null;

    private ProgressBar mDownloadProgressBar = null;
    private TextView mTextViewDownloadNothingFound = null;
    private ListView mListViewDownloadContacts = null;

    private RadioButton mRadioButtonSearchName = null;
    private RadioButton mRadioButtonSearchNumber = null;
    private RadioButton mRadioButtonSearchSound = null;
    private EditText mEditTextSearchValue = null;
    private EditText mEditTextBrowseMaxListCount = null;
    private EditText mEditTextBrowseOffsetValue = null;
    private RadioButton mRadioButtonOrderUnordered = null;
    private RadioButton mRadioButtonOrderAlphabetical = null;
    private RadioButton mRadioButtonOrderIndexed = null;
    private RadioButton mRadioButtonOrderPhonetic = null;
    private Button mButtonBrowseSearch = null;

    private ProgressBar mBrowseProgressBar = null;
    private TextView mTextViewBrowseNothingFound = null;
    private ListView mListViewBrowseContacts = null;

    /*
     * vCard Details functionality.
     */
    private Spinner mVcardSpinner = null;
    private VcardView mVcardView = null;
    private EditText mEditTextHandleValue = null;

    /*
     * Download local variables.
     */
    private long mDownloadValueFilter = 0;
    private byte mDownloadValueCardType = BluetoothPbapClient.VCARD_TYPE_21;
    private int mDownloadValueMaxCount = MAX_COUNT_DEFAULT_VALUE;
    private int mDownloadValueOffset = OFFSET_DEFAULT_VALUE;

    /*
     * Browse local variables.
     */
    private byte mBrowseValueOrder = BluetoothPbapClient.ORDER_BY_DEFAULT;
    private byte mBrowseValueSearchAttr = BluetoothPbapClient.SEARCH_ATTR_NAME;
    private int mBrowseValueMaxCount = MAX_COUNT_DEFAULT_VALUE;
    private int mBrowseValueOffset = OFFSET_DEFAULT_VALUE;

    /*
     * Action bar tabs.
     */
    private ActionBar.Tab mDownloadTab = null;
    private ActionBar.Tab mBrowseTab = null;
    private ActionBar.Tab mVcardTab = null;

    /*
     * vCard local variables.
     */
    private String mVcardHandleValue = HANDLE_DEFAULT_VALUE;
    private long mVcardValueFilter = 0;
    private byte mVcardValueCardType = BluetoothPbapClient.VCARD_TYPE_21;

    /*
     * Download adapter.
     */
    private VCardEntryAdapter mVCardEntryAdapter = null;

    OnItemClickListener mOnDownloadItemClickListener = new OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            VCardEntry vcard = (VCardEntry) parent.getAdapter().getItem(position);
            mVcardView.setVCardEntry(vcard);
            mActionBar.selectTab(mVcardTab);
        }
    };

    /*
     * Browse adapter.
     */
    private BluetoothPbapCardAdapter mBluetoothPbapCardAdapter = null;

    OnItemClickListener mOnBrowseItemClickListener = new OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            BluetoothPbapCard pbacpCard = (BluetoothPbapCard) parent.getAdapter().getItem(position);
            if ((mProfileService != null) && (mProfileService.getPbapClient() != null)) {
                mProfileService.getPbapClient().pullVcardEntry(pbacpCard.handle);
            }
        }
    };

    /*
     * PBAP Service.
     */
    private ProfileService mProfileService = null;
    private final IPbapServiceCallback mPbapServiceCallback = new IPbapServiceCallback() {

        @Override
        public void onSetPhoneBookDone() {
            if (mSetPathQueue != null && mSetPathQueue.size() > 0) {
                String next = mSetPathQueue.removeFirst();

                setPhonebookFolder(next);
            } else {
                new MonkeyEvent("pbap-setphonebook", true).send();
            }
        }

        @Override
        public void onPullPhoneBookDone(ArrayList<VCardEntry> list, int missedCalls) {
            mVCardEntryAdapter.clear();
            mVCardEntryAdapter.addAll(list);

            stopProgressBarDownload();

            /* send event for monkeyrunner */
            MonkeyEvent evt = new MonkeyEvent("pbap-pullphonebook", true);
            evt.addReplyParam("size", list.size());
            for (VCardEntry card : list) {
                evt.addExtReply(BluetoothPbapCard.jsonifyVcardEntry(card));
            }
            evt.send();

            Toast.makeText(PbapTestActivity.this, "Missed calls=" + missedCalls, Toast.LENGTH_SHORT)
                    .show();
        }

        @Override
        public void onPullVcardListingDone(ArrayList<BluetoothPbapCard> list, int missedCalls) {
            mBluetoothPbapCardAdapter.clear();
            mBluetoothPbapCardAdapter.addAll(list);
            stopProgressBarBrowse();

            /* send event for monkeyrunner */
            new MonkeyEvent("pbap-pullvcardlisting", true)
                    .addReplyParam("size", list.size())
                    .addExtReply(list)
                    .send();

            Toast.makeText(PbapTestActivity.this, "Missed calls=" + missedCalls, Toast.LENGTH_SHORT)
                    .show();
        }

        @Override
        public void onPullVcardEntryDone(VCardEntry vcard) {
            Logger.v(TAG, "onReceivedPbapPullVcardEntryDone()");

            /* send event for monkeyrunner */
            new MonkeyEvent("pbap-pullvcardentry", true)
                    .addExtReply(BluetoothPbapCard.jsonifyVcardEntry(vcard))
                    .send();

            mVcardView.setVCardEntry(vcard);
            mActionBar.selectTab(mVcardTab);
        }

        @Override
        public void onPullPhoneBookSizeDone(int size, int type) {
            /* send event for monkeyrunner */
            if (type == 0) {
                new MonkeyEvent("pbap-pullphonebook-size", true)
                        .addReplyParam("size", size)
                        .send();
            } else {
                new MonkeyEvent("pbap-pullvcardlisting-size", true)
                        .addReplyParam("size", size)
                        .send();
            }

            Toast.makeText(PbapTestActivity.this, "size=" + size, Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onSetPhoneBookError() {
            Logger.e(TAG, "Received from PBAP set phone book error.");

            mSetPathQueue = null;

            new MonkeyEvent("pbap-setphonebook", false).send();
        }

        @Override
        public void onPullPhoneBookError() {
            Logger.e(TAG, "Received from PBAP pull phone book error.");
            stopProgressBarDownload();
            mListViewDownloadContacts.setVisibility(View.GONE);
            Toast.makeText(PbapTestActivity.this, "PullPhoneBook FAILED", Toast.LENGTH_LONG).show();
            new MonkeyEvent("pbap-pullphonebook", false).send();
        }

        @Override
        public void onPullVcardListingError() {
            Logger.e(TAG, "Received from PBAP listing error.");
            stopProgressBarBrowse();
            mListViewBrowseContacts.setVisibility(View.GONE);
            Toast.makeText(PbapTestActivity.this, "PullvCardListing FAILED", Toast.LENGTH_LONG)
                    .show();
            new MonkeyEvent("pbap-pullvcardlisting", false).send();
        }

        @Override
        public void onPullVcardEntryError() {
            Logger.e(TAG, "Received from PBAP vCard entry error.");
            Toast.makeText(PbapTestActivity.this, "PullvCardEntry FAILED", Toast.LENGTH_LONG)
                    .show();
            new MonkeyEvent("pbap-pullvcardentry", false).send();
        }

        @Override
        public void onPullPhoneBookSizeError() {
            Logger.e(TAG, "Received from PBAP pull phone book size error.");
            Toast.makeText(PbapTestActivity.this, "PullPhoneBook size FAILED", Toast.LENGTH_LONG)
                    .show();
            new MonkeyEvent("pbap-pullphonebook-size", false).send();
        }

        @Override
        public void onPullVcardListingSizeError() {
            Logger.e(TAG, "Received from PBAP pull vCard listing sizeerror.");
            Toast.makeText(PbapTestActivity.this, "PullvCardListing size FAILED", Toast.LENGTH_LONG)
                    .show();
            new MonkeyEvent("pbap-pullvcardlisting-size", false).send();
        }

        @Override
        public void onSessionConnected() {
            Toast.makeText(PbapTestActivity.this, "PBAP session connected", Toast.LENGTH_SHORT)
                    .show();
            invalidateOptionsMenu();
            setButtonsVisible(true);
        }

        @Override
        public void onSessionDisconnected() {
            Toast.makeText(PbapTestActivity.this, "PBAP session disconnected", Toast.LENGTH_SHORT)
                    .show();
            invalidateOptionsMenu();
            setButtonsVisible(false);
        }
    };

    /*
     * PBAP Service Connection.
     */
    private final ServiceConnection mPbapServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Logger.v(TAG, "onServiceConnected()");
            mProfileService = ((ProfileService.LocalBinder) service).getService();

            if (mProfileService != null) {
                mProfileService.setPbapCallback(mPbapServiceCallback);
                if (mProfileService.getPbapClient() != null) {
                    if (mProfileService.getPbapClient().getState() == BluetoothPbapClient.ConnectionState.CONNECTED)
                        setButtonsVisible(true);
                }
            }

            ProfileService.PbapSessionData pbap = mProfileService.getPbapSessionData();

            if (pbap.pullPhoneBook != null) {
                mVCardEntryAdapter.clear();
                mVCardEntryAdapter.addAll(pbap.pullPhoneBook);
                stopProgressBarDownload();
            }

            if (pbap.pullVcardListing != null) {
                mBluetoothPbapCardAdapter.clear();
                mBluetoothPbapCardAdapter.addAll(pbap.pullVcardListing);
                stopProgressBarBrowse();
            }

            if (pbap.pullVcardEntry != null) {
                mVcardView.setVCardEntry(pbap.pullVcardEntry);
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            Logger.v(TAG, "onServiceDisconnected()");
            mProfileService = null;
        }
    };

    private void enableButton(int id, boolean enable) {
        Button button = (Button) findViewById(id);
        button.setEnabled(enable);
    }

    private void setButtonsVisible (boolean visible) {
        enableButton(R.id.pbap_download_filter_button, visible);
        enableButton(R.id.pbap_download_search, visible);
        enableButton(R.id.pbap_browse_search, visible);
        enableButton(R.id.pbap_download_getsize, visible);
        enableButton(R.id.pbap_download_abort, visible);
        enableButton(R.id.pbap_browse_getsize, visible);
        enableButton(R.id.pbap_browse_abort, visible);
        enableButton(R.id.pbap_browse_set_phonebook, visible);
        enableButton(R.id.pbap_vcard_set_phonebook, visible);
        enableButton(R.id.pbap_vcard_get_vcard, visible);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Logger.v(TAG, "onCreate()");
        ActivityHelper.initialize(this, R.layout.activity_pbap_test);
        ActivityHelper.setActionBarTitle(this, R.string.title_pbap_test);

        prepareActionBar();
        prepareUserInterfaceDownload();
        prepareUserInterfaceBrowse();
        prepareUserInterfaceVcard();
        setButtonsVisible (false);

        mBluetoothPbapCardAdapter = new BluetoothPbapCardAdapter();
        mListViewBrowseContacts.setAdapter(mBluetoothPbapCardAdapter);

        mVCardEntryAdapter = new VCardEntryAdapter();
        mListViewDownloadContacts.setAdapter(mVCardEntryAdapter);

        BluetoothConnectionReceiver.registerObserver(this);

        // bind to PBAP service
        Intent intent = new Intent(this, ProfileService.class);
        bindService(intent, mPbapServiceConnection, BIND_AUTO_CREATE);
    }

    @Override
    protected void onStart() {
        super.onStart();
        Logger.v(TAG, "onStart()");
    }

    @Override
    protected void onStop() {
        super.onStop();
        Logger.v(TAG, "onStop()");
    }

    @Override
    protected void onDestroy() {
    /*
         * Going to call abort if any pending request is ongoing,
         * checks for the same are handled internally
        */
        if ((mProfileService != null) && (mProfileService.getPbapClient() != null)) {
            mProfileService.getPbapClient().abort();
        } else {
            Logger.e(TAG, "Not able to ABORT");
        }

        super.onDestroy();
        Logger.v(TAG, "onDestroy()");

        mProfileService.setPbapCallback(null);

        unbindService(mPbapServiceConnection);
        BluetoothConnectionReceiver.removeObserver(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Logger.v(TAG, "onResume()");
    }

    @Override
    protected void onPause() {
        super.onPause();
        Logger.v(TAG, "onPause()");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        mActionBarMenu = menu;

        getMenuInflater().inflate(R.menu.menu_pbap_test, menu);

        if (mProfileService == null)
        {
            Logger.e(TAG, "mProfileSevice is null");
            return false;
        }

        if (mProfileService.getPbapClient() != null) {
            if (mProfileService.getPbapClient().getState() != BluetoothPbapClient.ConnectionState.DISCONNECTED) {
                menu.findItem(R.id.menu_pbap_disconnect).setVisible(true);
            } else {
                menu.findItem(R.id.menu_pbap_connect).setVisible(true);
            }
        }

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_pbap_connect:
                if ((mProfileService != null) && (mProfileService.getPbapClient() != null))
                    mProfileService.getPbapClient().connect();
                break;
            case R.id.menu_pbap_disconnect:
                if ((mProfileService != null) && (mProfileService.getPbapClient() != null))
                    mProfileService.getPbapClient().disconnect();
                break;
            default:
                Logger.w(TAG, "Unknown item selected.");
                break;
        }
        return true;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Logger.v(TAG, "onActivityResult()");

        if (resultCode != RESULT_OK) {
            Logger.w(TAG, "Result code is not ok for req: " + requestCode + ".");
            return;
        }

        if (requestCode == REQUEST_CODE_GET_FILTER_FOR_DOWNLOAD_TAB) {
            long result = data.getLongExtra("result", -1);

            if (result != -1) {
                Logger.d(TAG, "Received download tab filter " + result + ".");
                mDownloadValueFilter = result;
            } else {
                Logger.w(TAG, "Somethings go wrong here!");
            }
        }

        if (requestCode == REQUEST_CODE_GET_FILTER_FOR_VCARD_TAB) {
            long result = data.getLongExtra("result", -1);

            if (result != -1) {
                Logger.d(TAG, "Received vcard tab filter " + result + ".");
                mVcardValueFilter = result;
            } else {
                Logger.w(TAG, "Somethings go wrong here!");
            }
        }
    }

    private void prepareUserInterfaceDownload() {
        mDownloadSpinner = (Spinner) findViewById(R.id.pbap_download_spinner);
        mButtonFilter = (Button) findViewById(R.id.pbap_download_filter_button);
        mButtonFilter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mButtonFilter.onClick()");

                Intent intent = new Intent(PbapTestActivity.this, VcardFilterActivity.class);
                intent.putExtra("filter", mDownloadValueFilter);
                intent.putExtra("type", mDownloadValueCardType);
                startActivityForResult(intent, REQUEST_CODE_GET_FILTER_FOR_DOWNLOAD_TAB);
            }
        });

        mEditTextDownloadMaxListCount = (EditText) findViewById(R.id.pbap_download_max_list_count);
        mEditTextDownloadMaxListCount.setText(String.valueOf(mDownloadValueMaxCount));

        mEditTextDownloadOffsetValue = (EditText) findViewById(R.id.pbap_download_offset_value);
        mEditTextDownloadOffsetValue.setText(String.valueOf(mDownloadValueOffset));

        mRadioButtonVCard21 = (RadioButton) findViewById(R.id.pbap_download_vcard21);
        mRadioButtonVCard21.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mRadioButonVCard21.onClick()");
                mDownloadValueCardType = BluetoothPbapClient.VCARD_TYPE_21;
            }
        });

        mRadioButtonVCard30 = (RadioButton) findViewById(R.id.pbap_download_vcard30);
        mRadioButtonVCard30.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mRadioButonVCard30.onClick()");
                mDownloadValueCardType = BluetoothPbapClient.VCARD_TYPE_30;
            }
        });

        mButtonDownloadSearch = (Button) findViewById(R.id.pbap_download_search);
        mButtonDownloadSearch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mButtonDownloadSearch.onClick()");
                runPbapTestDownload();
            }
        });

        mDownloadProgressBar = (ProgressBar) findViewById(R.id.pbap_download_progress_bar);
        mTextViewDownloadNothingFound = (TextView) findViewById(R.id.pbap_download_test_list_empty);
        mListViewDownloadContacts = (ListView) findViewById(R.id.pbap_download_listview_contacts_list);
        mListViewDownloadContacts.setOnItemClickListener(mOnDownloadItemClickListener);

        mDownloadProgressBar.setVisibility(View.GONE);
        mListViewDownloadContacts.setVisibility(View.GONE);
    }

    private void prepareUserInterfaceBrowse() {
        mRadioButtonSearchName = (RadioButton) findViewById(R.id.pbap_browse_name);
        mRadioButtonSearchName.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mRadioButtonSearchName.onClick()");
                mBrowseValueSearchAttr = BluetoothPbapClient.SEARCH_ATTR_NAME;
            }
        });

        mRadioButtonSearchNumber = (RadioButton) findViewById(R.id.pbap_browse_number);
        mRadioButtonSearchNumber.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mRadioButtonSearchNumber.onClick()");
                mBrowseValueSearchAttr = BluetoothPbapClient.SEARCH_ATTR_NUMBER;
            }
        });

        mRadioButtonSearchSound = (RadioButton) findViewById(R.id.pbap_browse_sound);
        mRadioButtonSearchSound.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mRadioButtonSearchSound.onClick()");
                mBrowseValueSearchAttr = BluetoothPbapClient.SEARCH_ATTR_SOUND;
            }
        });

        mEditTextSearchValue = (EditText) findViewById(R.id.pbap_browse_search_value);

        mEditTextBrowseMaxListCount = (EditText) findViewById(R.id.pbap_browse_max_list_count);
        mEditTextBrowseMaxListCount.setText(String.valueOf(mBrowseValueMaxCount));

        mEditTextBrowseOffsetValue = (EditText) findViewById(R.id.pbap_browse_offset_value);
        mEditTextBrowseOffsetValue.setText(String.valueOf(mBrowseValueOffset));

        mRadioButtonOrderUnordered = (RadioButton) findViewById(R.id.pbap_browse_unordered);
        mRadioButtonOrderUnordered.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mRadioButtonOrderUnordered.onClick()");
                mBrowseValueOrder = BluetoothPbapClient.ORDER_BY_DEFAULT;
            }
        });

        mRadioButtonOrderAlphabetical = (RadioButton) findViewById(R.id.pbap_browse_alphabetical);
        mRadioButtonOrderAlphabetical.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mRadioButtonOrderAlphabetical.onClick()");
                mBrowseValueOrder = BluetoothPbapClient.ORDER_BY_ALPHABETICAL;
            }
        });

        mRadioButtonOrderIndexed = (RadioButton) findViewById(R.id.pbap_browse_indexed);
        mRadioButtonOrderIndexed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mRadioButtonOrderIndexed.onClick()");
                mBrowseValueOrder = BluetoothPbapClient.ORDER_BY_INDEXED;
            }
        });

        mRadioButtonOrderPhonetic = (RadioButton) findViewById(R.id.pbap_browse_phonetic);
        mRadioButtonOrderPhonetic.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mRadioButtonOrderPhonetic.onClick()");
                mBrowseValueOrder = BluetoothPbapClient.ORDER_BY_PHONETIC;
            }
        });

        mButtonBrowseSearch = (Button) findViewById(R.id.pbap_browse_search);
        mButtonBrowseSearch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Logger.v(TAG, "mButtonBrowseSearch.onClick()");
                runPbapTestBrowse();
            }
        });

        mBrowseProgressBar = (ProgressBar) findViewById(R.id.pbap_browse_progress_bar);
        mTextViewBrowseNothingFound = (TextView) findViewById(R.id.pbap_browse_test_list_empty);
        mListViewBrowseContacts = (ListView) findViewById(R.id.pbap_browse_listview_contacts_list);
        mListViewBrowseContacts.setOnItemClickListener(mOnBrowseItemClickListener);

        mBrowseProgressBar.setVisibility(View.GONE);
        mListViewBrowseContacts.setVisibility(View.GONE);
    }

    private void prepareUserInterfaceVcard() {
        mVcardSpinner = (Spinner) findViewById(R.id.pbap_vcard_spinner);
        mVcardView = (VcardView) findViewById(R.id.pbap_vcard_vcardview);

        mEditTextHandleValue = (EditText) findViewById(R.id.pbap_vcard_header);
        mEditTextHandleValue.setText(mVcardHandleValue);
    }

    private void prepareActionBar() {
        mActionBar = getActionBar();

        if (mActionBar != null) {
            mActionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);
            mDownloadTab = mActionBar.newTab().setText(R.string.download);
            mBrowseTab = mActionBar.newTab().setText(R.string.browse);
            mVcardTab = mActionBar.newTab().setText(R.string.vcard);

            mViewFlipper = (ViewFlipper) findViewById(R.id.pbap_test_viewflipper);

            mDownloadTab.setTabListener(new TabListener() {
                @Override
                public void onTabUnselected(Tab tab, FragmentTransaction ft) {
                    /* NoP */
                }

                @Override
                public void onTabSelected(Tab tab, FragmentTransaction ft) {
                    mViewFlipper.setDisplayedChild(0);
                }

                @Override
                public void onTabReselected(Tab tab, FragmentTransaction ft) {
                    /* NoP */
                }
            });

            mBrowseTab.setTabListener(new TabListener() {
                @Override
                public void onTabUnselected(Tab tab, FragmentTransaction ft) {
                    /* NoP */
                }

                @Override
                public void onTabSelected(Tab tab, FragmentTransaction ft) {
                    mViewFlipper.setDisplayedChild(1);
                }

                @Override
                public void onTabReselected(Tab tab, FragmentTransaction ft) {
                    /* NoP */
                }
            });

            mVcardTab.setTabListener(new TabListener() {

                @Override
                public void onTabUnselected(Tab tab, FragmentTransaction ft) {
                    /* NoP */
                }

                @Override
                public void onTabSelected(Tab tab, FragmentTransaction ft) {
                    mViewFlipper.setDisplayedChild(2);
                }

                @Override
                public void onTabReselected(Tab tab, FragmentTransaction ft) {
                    /* NoP */
                }
            });

            mActionBar.addTab(mDownloadTab);
            mActionBar.addTab(mBrowseTab);
            mActionBar.addTab(mVcardTab);
        }
    }

    private void runPbapTestDownload() {
        Logger.v(TAG, "runPbapTestDownload()");

        try {
            mDownloadValueMaxCount = Integer.parseInt(mEditTextDownloadMaxListCount.getText()
                    .toString());
        } catch (NumberFormatException e) {
            mDownloadValueMaxCount = MAX_COUNT_DEFAULT_VALUE;
        }

        try {
            mDownloadValueOffset = Integer.parseInt(mEditTextDownloadOffsetValue.getText()
                    .toString());
        } catch (NumberFormatException e) {
            mDownloadValueOffset = OFFSET_DEFAULT_VALUE;
        }

        // Refresh UI EditTexts value if some are blank after edit.
        mEditTextDownloadMaxListCount.setText(String.valueOf(mDownloadValueMaxCount));
        mEditTextDownloadOffsetValue.setText(String.valueOf(mDownloadValueOffset));

        try {
            if ((mProfileService != null) && (mProfileService.getPbapClient() != null)) {
                if (mProfileService.getPbapClient().pullPhoneBook(
                        mDownloadSpinner.getSelectedItem().toString(), mDownloadValueFilter,
                        mDownloadValueCardType, mDownloadValueMaxCount, mDownloadValueOffset))
                    startProgressBarDownload();
            } else {
                    Toast.makeText(this, "PullPhoneBook FAILED", Toast.LENGTH_LONG).show();
            }
        } catch (IllegalArgumentException e) {
            Toast.makeText(this,
                    "PullPhoneBook FAILED: illegal arguments (" + e.getMessage() + ")",
                    Toast.LENGTH_LONG).show();
        }
    }

    private void startProgressBarDownload() {
        mDownloadProgressBar.setVisibility(View.VISIBLE);
        mListViewDownloadContacts.setVisibility(View.GONE);
        mTextViewDownloadNothingFound.setVisibility(View.GONE);
    }

    private void stopProgressBarDownload() {
        mDownloadProgressBar.setVisibility(View.GONE);
        if (mVCardEntryAdapter.getCount() == 0) {
            mTextViewDownloadNothingFound.setVisibility(View.VISIBLE);
            mListViewDownloadContacts.setVisibility(View.GONE);
        } else {
            mTextViewDownloadNothingFound.setVisibility(View.GONE);
            mListViewDownloadContacts.setVisibility(View.VISIBLE);
        }
    }

    private void runPbapTestBrowse() {
        Logger.v(TAG, "runPbapTestBrowse()");

        byte order = mBrowseValueOrder;
        String searchValue = mEditTextSearchValue.getText().toString();

        try {
            mBrowseValueMaxCount = Integer.parseInt(mEditTextBrowseMaxListCount.getText()
                    .toString());
        } catch (NumberFormatException e) {
            mBrowseValueMaxCount = MAX_COUNT_DEFAULT_VALUE;
        }

        try {
            mBrowseValueOffset = Integer.parseInt(mEditTextBrowseOffsetValue.getText().toString());
        } catch (NumberFormatException e) {
            mBrowseValueOffset = OFFSET_DEFAULT_VALUE;
        }

        // Refresh UI EditTexts value if some are blank after edit.
        mEditTextBrowseMaxListCount.setText(String.valueOf(mBrowseValueMaxCount));
        mEditTextBrowseOffsetValue.setText(String.valueOf(mBrowseValueOffset));

        try {
            boolean started = false;
            if ((mProfileService != null) && (mProfileService.getPbapClient() != null)) {
                if (searchValue != "" && !searchValue.isEmpty()) {
                    started = mProfileService.getPbapClient().pullVcardListing(null, order,
                        mBrowseValueSearchAttr, searchValue, mBrowseValueMaxCount,
                        mBrowseValueOffset);
                } else {
                    started = mProfileService.getPbapClient().pullVcardListing(null, order,
                        mBrowseValueMaxCount, mBrowseValueOffset);
                }
            }

            if (started) {
                startProgressBarBrowse();
            } else {
                Toast.makeText(this, "PullvCardListing FAILED", Toast.LENGTH_LONG).show();
            }
        } catch (IllegalArgumentException e) {
            Toast.makeText(this,
                    "PullvCardListing FAILED: illegal arguments (" + e.getMessage() + ")",
                    Toast.LENGTH_LONG).show();
        }
    }

    private void startProgressBarBrowse() {
        mBrowseProgressBar.setVisibility(View.VISIBLE);
        mListViewBrowseContacts.setVisibility(View.GONE);
        mTextViewBrowseNothingFound.setVisibility(View.GONE);
    }

    private void stopProgressBarBrowse() {
        mBrowseProgressBar.setVisibility(View.GONE);
        if (mBluetoothPbapCardAdapter.getCount() == 0) {
            mTextViewBrowseNothingFound.setVisibility(View.VISIBLE);
            mListViewBrowseContacts.setVisibility(View.GONE);
        } else {
            mTextViewBrowseNothingFound.setVisibility(View.GONE);
            mListViewBrowseContacts.setVisibility(View.VISIBLE);
        }
    }

    class VCardEntryAdapter extends ArrayAdapter<VCardEntry> {
        VCardEntryAdapter() {
            super(PbapTestActivity.this, android.R.layout.simple_list_item_1);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            View row = convertView;
            boolean defaultPhoto = true;

            if (row == null) {
                LayoutInflater inflater = getLayoutInflater();
                row = inflater.inflate(R.layout.vcard_row, null);
            }

            VCardEntry tmp = getItem(position);

            ((TextView) row.findViewById(R.id.vcard_title)).setText(tmp.getDisplayName());

            if (tmp.getPhotoList() != null && tmp.getPhotoList().size() > 0) {
                byte[] data = tmp.getPhotoList().get(0).getBytes();
                try {
                    if (data != null) {
                        Bitmap bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
                        ((ImageView) row.findViewById(R.id.vcard_photo)).setImageBitmap(bmp);
                        defaultPhoto = false;
                    }
                } catch (IllegalArgumentException e) {
                }
            }

            if (defaultPhoto) {
                ((ImageView) row.findViewById(R.id.vcard_photo))
                        .setImageResource(R.drawable.ic_contact_picture);
            }

            return row;
        }
    }

    class BluetoothPbapCardAdapter extends ArrayAdapter<BluetoothPbapCard> {
        BluetoothPbapCardAdapter() {
            super(PbapTestActivity.this, android.R.layout.simple_list_item_1);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            View row = convertView;

            if (row == null) {
                LayoutInflater inflater = getLayoutInflater();
                row = inflater.inflate(R.layout.vcard_row, null);
            }

            BluetoothPbapCard tmp = getItem(position);

            StringBuilder sb = new StringBuilder();

            if (tmp.firstName != null) {
                sb.append(tmp.firstName).append(" ");
            }
            sb.append(tmp.lastName);

            ((TextView) row.findViewById(R.id.vcard_title)).setText(sb.toString());
            return row;
        }
    }

    @Override
    public void onDeviceChanged(BluetoothDevice device) {
        Logger.v(TAG, "onDeviceChanged()");
        /* NoP */
    }

    @Override
    public void onDeviceDisconected() {
        Logger.e(TAG, "BT device disconnected!");
    }

    public void onClick_download_getsize(View v) {
        if ((mProfileService != null) && (mProfileService.getPbapClient() != null)) {
            mProfileService.getPbapClient().pullPhoneBookSize(
                mDownloadSpinner.getSelectedItem().toString());
        }
    }

    public void onClick_browse_getsize(View v) {
        if ((mProfileService != null) && (mProfileService.getPbapClient() != null)) {
            mProfileService.getPbapClient().pullVcardListingSize("");
        }
    }

    public void onClickVcardFilterAttributes(View v) {
        Intent intent = new Intent(PbapTestActivity.this, VcardFilterActivity.class);
        intent.putExtra("filter", mVcardValueFilter);
        intent.putExtra("type", mVcardValueCardType);
        startActivityForResult(intent, REQUEST_CODE_GET_FILTER_FOR_VCARD_TAB);
    }

    public void onClickRadioButtonVcardType21(View v) {
        mVcardValueCardType = BluetoothPbapClient.VCARD_TYPE_21;
    }

    public void onClickRadioButtonVcardType30(View v) {
        mVcardValueCardType = BluetoothPbapClient.VCARD_TYPE_30;
    }

    public void onClick_setPhonebook(View v) {
        int id = getResources().getIdentifier(v.getTag().toString(), "id", getPackageName());

        Spinner spinner = (Spinner) findViewById(id);

        if (spinner != null) {
            setPhonebook(spinner.getSelectedItem().toString());
        }
    }

    public void onClickVcardButtonGet(View v) {
        mVcardHandleValue = mEditTextHandleValue.getText().toString();

        if (mVcardHandleValue.isEmpty() || mVcardHandleValue == null) {
            mVcardHandleValue = HANDLE_DEFAULT_VALUE;
            mEditTextHandleValue.setText(mVcardHandleValue);
        }

        if ((mProfileService != null) && (mProfileService.getPbapClient() != null)) {
            mProfileService.getPbapClient().pullVcardEntry(mVcardHandleValue, mVcardValueFilter,
                mVcardValueCardType);
        }
    }

    private void setPhonebook(String dst) {
        dst = dst.replaceFirst("\\.vcf$", "");

        mSetPathQueue = new ArrayDeque<String>(Arrays.asList(dst.split("/")));

        if ((mProfileService != null) && (mProfileService.getPbapClient() != null)) {
            mProfileService.getPbapClient().setPhoneBookFolderRoot();
        }
    }

    private void setPhonebookFolder(String folder) {
        if ((mProfileService != null) && (mProfileService.getPbapClient() != null)) {
            mProfileService.getPbapClient().setPhoneBookFolderDown(folder);
        }
    }

    public void onClick_abort(View v) {
        /*
         * Going to call abort if any pending request is ongoing,
         * checks for the same are handled internally
         */
        if ((mProfileService != null) && (mProfileService.getPbapClient() != null)) {
            mProfileService.getPbapClient().abort();
        }
    }
}
