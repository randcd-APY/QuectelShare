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
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.app.FragmentTransaction;
import android.bluetooth.SdpMasRecord;
import android.content.ComponentName;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.AdapterContextMenuInfo;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.TimePicker;
import android.widget.Toast;
import android.widget.ViewFlipper;

import com.android.vcard.VCardConstants;
import com.android.vcard.VCardEntry;
import com.android.vcard.VCardProperty;
import org.codeaurora.bluetooth.bttestapp.R;
import android.bluetooth.client.map.BluetoothMapBmessage;
import android.bluetooth.client.map.BluetoothMapEventReport;
import android.bluetooth.client.map.BluetoothMapMessage;
import android.bluetooth.client.map.BluetoothMasClient;
import android.bluetooth.client.map.BluetoothMasClient.CharsetType;
import android.bluetooth.client.map.BluetoothMasClient.MessagesFilter;
import org.codeaurora.bluetooth.bttestapp.GetTextDialogFragment.GetTextDialogListener;
import org.codeaurora.bluetooth.bttestapp.R;
import org.codeaurora.bluetooth.bttestapp.StringListDialogFragment.StringListDialogListener;
import org.codeaurora.bluetooth.bttestapp.services.IMapServiceCallback;
import org.codeaurora.bluetooth.bttestapp.util.Logger;
import org.codeaurora.bluetooth.bttestapp.util.MonkeyEvent;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.List;

public class MapTestActivity extends MonkeyActivity implements GetTextDialogListener,
        StringListDialogListener {

    private final String TAG = "MapTestActivity";

    private final static short MAX_LIST_COUNT_DEFAULT = 0;
    private final static short LIST_START_OFFSET_DEFAULT = 0;
    private final static byte SUBJECT_LENGTH_DEFAULT = 0;
    private final static String MESSAGES_FILTER_DATE_FORMAT = "yyyy-MM-dd HH:mm:ss";

    private static final int REQUEST_CODE_GET_PARAMETERS = 0;

    private final String TAB_LIST = "List";
    private final String TAB_PREVIEW = "Preview";
    private final String TAB_EDIT = "Edit";

    private int mMasInstanceId = -1;

    private String mCurrentTab = TAB_LIST;

    private final String[] mActionBarTabsNames = {
            TAB_LIST, TAB_PREVIEW, TAB_EDIT
    };

    enum Job {
        IDLE,
        CONNECT,
        DISCONNECT,
        UPDATE_INBOX,
        SET_PATH,
        GET_MESSAGE_LISTING,
        GET_MESSAGE_LISTING_SIZE,
        GET_FOLDER_LISTING,
        GET_FOLDER_LISTING_SIZE,
        GET_MESSAGE,
        SET_STATUS_READ,
        SET_STATUS_UNREAD,
        DELETE_MESSAGE,
        PUSH_MESSAGE;
    }

    private Job mCurrentJob = Job.IDLE;

    private String mStartingGetMessageHandle = null;

    private String mPendingGetMessageHandle = null;

    private ArrayDeque<String> mSetPathQueue = null;

    private int mMessageListingParameters = 0;

    private ActionBar mActionBar = null;

    private ViewFlipper mViewFlipper = null;

    private String mPreviewMsgHandle = null;
    private BluetoothMapBmessage mMapBmessage = null;

    // UI on List tab
    private TextView mTextViewCurrentFolder = null;
    private Spinner mSpinnerFolders = null;
    private ArrayAdapter<String> mAdapterFolders = null;
    private EditText mEditTextMaxListCountFolders = null;
    private EditText mEditTextListStartOffsetFolders = null;
    private EditText mEditTextMaxListCountMessages = null;
    private EditText mEditTextListStartOffsetMessages = null;
    private ListView mListViewMessages = null;
    private EditText mEditTextSubjectLength = null;

    private List<BluetoothMapMessage> mModelMessages = null;
    private BluetoothMapMessageAdapter mAdapterMessages = null;

    // UI on Preview/Edit tab
    private EditText mViewBmsgHandle = null;
    private EditText mViewBmsgStatus = null;
    private EditText mViewBmsgType = null;
    private EditText mViewBmsgFolder = null;
    private EditText mViewBmsgEncoding = null;
    private EditText mViewBmsgCharset = null;
    private EditText mViewBmsgLanguage = null;
    private EditText mViewBmsgContents = null;
    private EditText mViewBmsgOrig = null;
    private EditText mViewBmsgRcpt = null;

    // MessagesFilter parameters
    private byte mMessageType = MessagesFilter.MESSAGE_TYPE_ALL;
    private Date mPeriodBegin = null;
    private Date mPeriodEnd = null;
    private byte mReadStatus = MessagesFilter.READ_STATUS_ANY;
    private String mRecipient = null;
    private String mOriginator = null;
    private byte mPriority = MessagesFilter.PRIORITY_ANY;

    private ArrayList<View> mListTouchables = null;

    private final SimpleDateFormat mSimpleDateFormat = new SimpleDateFormat(MESSAGES_FILTER_DATE_FORMAT);

    private ProfileService mProfileService = null;

    private final ArrayList<String> mEditOriginators = new ArrayList<String>();

    private final ArrayList<String> mEditRecipients = new ArrayList<String>();

    private final ServiceConnection mMapServiceConnection = new ServiceConnection() {

        private void shortToast(String s) {
            Toast.makeText(MapTestActivity.this, s, Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mProfileService = ((ProfileService.LocalBinder) service).getService();
            if(mProfileService != null) {
                BluetoothMasClient cli = mProfileService.getMapClient(mMasInstanceId);
                if (cli != null) {
                    SdpMasRecord masrec = cli.getInstanceData();
                    if(masrec != null)
                      MapTestActivity.this.getActionBar().setSubtitle(masrec.getServiceName());
                    }
            }
            mProfileService.setMapCallback(mMasInstanceId, new IMapServiceCallback() {

                @Override
                public void onConnect() {
                    goToState(Job.IDLE);
                    shortToast("MAS connect OK");
                }

                @Override
                public void onConnectError() {
                    goToState(Job.IDLE);
                    shortToast("MAS connect FAILED");
                }

                @Override
                public void onUpdateInbox() {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-updateinbox", true).send();
                    shortToast("UpdateInbox OK");
                }

                @Override
                public void onUpdateInboxError() {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-updateinbox", false).send();
                    shortToast("UpdateInbox FAILED");
                }

                @Override
                public void onSetPath(String path) {
                    if (mSetPathQueue != null && mSetPathQueue.size() > 0) {
                        String next = mSetPathQueue.removeFirst();
                        if(mProfileService != null &&
                           (mProfileService.getMapClient(mMasInstanceId)) != null ) {
                            mProfileService.getMapClient(mMasInstanceId).setFolderDown(next);
                        }
                    } else {
                        mTextViewCurrentFolder.setText(path);
                        clearFolderList();

                        goToState(Job.IDLE);
                        new MonkeyEvent("map-setpath", true).addReplyParam("path", path)
                                .send();
                        shortToast("SetPath OK: path=" + path);
                    }
                }

                @Override
                public void onSetPathError(String path) {
                    if (mSetPathQueue != null) {
                        mSetPathQueue = null;
                        mTextViewCurrentFolder.setText(path);
                        clearFolderList();
                    }

                    goToState(Job.IDLE);
                    new MonkeyEvent("map-setpath", false).addReplyParam("path", path)
                            .send();
                    shortToast("SetPath FAILED: path=" + path);
                }

                @Override
                public void onGetMessagesListing(ArrayList<BluetoothMapMessage> messages) {
                    mAdapterMessages.clear();
                    mAdapterMessages.addAll(messages);

                    updateListEmptyView(false);

                    goToState(Job.IDLE);
                    new MonkeyEvent("map-messageslisting", true)
                            .addReplyParam("size", messages.size()).addExtReply(messages).send();
                    shortToast("GetMessagesListing OK: size=" + messages.size());
                }

                @Override
                public void onGetMessagesListingError() {
                    updateListEmptyView(false);

                    goToState(Job.IDLE);
                    new MonkeyEvent("map-messageslisting", false).send();
                    shortToast("GetMessagesListing FAILED");
                }

                @Override
                public void onGetFolderListingSize(int size) {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-getfolderlisting-size", true)
                            .addReplyParam("size", size)
                            .send();
                    shortToast("GetFolderListing size OK: size=" + size);
                }

                @Override
                public void onGetFolderListingSizeError() {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-getfolderlisting-size", false).send();
                    shortToast("GetFolderListing size FAILED");
                }

                @Override
                public void onGetFolderListing(ArrayList<String> folders) {
                    clearFolderList();
                    mAdapterFolders.addAll(folders);

                    goToState(Job.IDLE);
                    new MonkeyEvent("map-getfolderlisting", true)
                            .addReplyParam("size", folders.size()).addExtReply(folders).send();
                    shortToast("GetFolderListing OK: size=" + folders.size());
                }

                @Override
                public void onGetFolderListingError() {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-getfolderlisting", false).send();
                    shortToast("GetFolderListing FAILED");
                }

                @Override
                public void onGetMessage(BluetoothMapBmessage message) {
                    updateMessage(message);

                    goToState(Job.IDLE);
                    new MonkeyEvent("map-getmessage", true)
                            .addExtReply(message.toString()).send();
                    shortToast("GetMessage OK");
                }

                @Override
                public void onGetMessageError() {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-getmessage", false).send();
                    shortToast("GetMessage FAILED");
                }

                @Override
                public void onSetMessageStatus() {
                    switch (mCurrentJob) {
                        case SET_STATUS_READ:
                            /* TODO: re-read message? check spec */
                            break;
                        case SET_STATUS_UNREAD:
                            /* TODO: re-read message? check spec */
                            break;
                        case DELETE_MESSAGE:
                            resetPreviewEditUi();
                            Toast.makeText(MapTestActivity.this, "Message deleted!",
                                    Toast.LENGTH_SHORT).show();
                            break;
                        default:
                            break;
                    }

                    goToState(Job.IDLE);
                    new MonkeyEvent("map-setmessagestatus", true).send();
                    shortToast("SetMessageStatus OK");
                }

                @Override
                public void onSetMessageStatusError() {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-setmessagestatus", false).send();
                    shortToast("SetMessageStatus FAILED");
                }

                @Override
                public void onPushMessage(String handle) {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-pushmessage", true).addReplyParam("handle", handle)
                            .send();
                    shortToast("PushMessage OK: handle=" + handle);
                }

                @Override
                public void onPushMessageError() {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-pushmessage", false).send();
                    shortToast("PushMessage FAILED");
                }

                @Override
                public void onGetMessagesListingSize(int size) {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-getmessageslisting-size", true)
                            .addReplyParam("size", size).send();
                    shortToast("GetMessagesListing size OK: size=" + size);
                }

                @Override
                public void onGetMessagesListingSizeError() {
                    goToState(Job.IDLE);
                    new MonkeyEvent("map-getmessageslisting-size", false).send();
                    shortToast("GetMessagesListing size FAILED");
                }

                @Override
                public void onEventReport(BluetoothMapEventReport eventReport) {
                    String msgType = (eventReport.getMsgType() != null) ?
                            eventReport.getMsgType().toString() : getString(R.string.blank);

                    new MonkeyEvent("map-eventreport", true)
                            .addReplyParam("type", eventReport.getType().toString())
                            .addReplyParam("handle", eventReport.getHandle())
                            .addReplyParam("folder", eventReport.getFolder())
                            .addReplyParam("old_folder", eventReport.getOldFolder())
                            .addReplyParam("msg_type", msgType)
                            .send();
                }
            });

            ProfileService.MapSessionData map = mProfileService.getMapSessionData(mMasInstanceId);
            if (map != null) {
                if (map.getFolderListing != null) {
                    clearFolderList();
                    mAdapterFolders.addAll(map.getFolderListing);
                }

                if (map.getMessagesListing != null) {
                    mAdapterMessages.clear();
                    mAdapterMessages.addAll(map.getMessagesListing);
                    updateListEmptyView(false);
                }

                if (map.getMessage != null) {
                    updateMessage(map.getMessage);
                }
            }

            if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
                mTextViewCurrentFolder.setText(mProfileService.getMapClient(mMasInstanceId)
                    .getCurrentPath());
            }
            updateUi(true);

            if (mStartingGetMessageHandle != null) {
                getMessage(mStartingGetMessageHandle, CharsetType.UTF_8, false);
                mStartingGetMessageHandle = null;
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            mProfileService = null;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();

        ActivityHelper.initialize(this, R.layout.activity_map_test);
        ActivityHelper.setActionBarTitle(this, R.string.title_map_test);

        mMasInstanceId = intent.getIntExtra(ProfileService.EXTRA_MAP_INSTANCE_ID, -1);

        if (ProfileService.ACTION_MAP_GET_MESSAGE.equals(intent.getAction())) {
            mStartingGetMessageHandle = intent
                    .getStringExtra(ProfileService.EXTRA_MAP_MESSAGE_HANDLE);
        }

        if (mMasInstanceId < 0) {
            Log.e(TAG, "Cannot start MAP activity without instance information");
            finish();
        }

        intent = new Intent(MapTestActivity.this, ProfileService.class);
        bindService(intent, mMapServiceConnection, BIND_AUTO_CREATE);

        mViewFlipper = (ViewFlipper) findViewById(R.id.maptest_viewflipper);
        mTextViewCurrentFolder = (TextView) findViewById(R.id.maptest_nav_current);
        mSpinnerFolders = (Spinner) findViewById(R.id.maptest_nav_folders);
        mAdapterFolders = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);
        mSpinnerFolders.setAdapter(mAdapterFolders);
        mEditTextMaxListCountFolders = (EditText) findViewById(R.id.maptest_nav_list_max);
        mEditTextMaxListCountFolders.setText(String.valueOf(MAX_LIST_COUNT_DEFAULT));
        mEditTextListStartOffsetFolders = (EditText) findViewById(R.id.maptest_nav_list_offset);
        mEditTextListStartOffsetFolders.setText(String.valueOf(LIST_START_OFFSET_DEFAULT));
        mEditTextMaxListCountMessages = (EditText) findViewById(R.id.maptest_msglist_max);
        mEditTextMaxListCountMessages.setText(String.valueOf(MAX_LIST_COUNT_DEFAULT));
        mEditTextListStartOffsetMessages = (EditText) findViewById(R.id.maptest_msglist_offset);
        mEditTextListStartOffsetMessages.setText(String.valueOf(LIST_START_OFFSET_DEFAULT));
        mListViewMessages = (ListView) findViewById(R.id.maptest_msglist_lv);
        mListViewMessages.setEmptyView(findViewById(R.id.maptest_msglist_empty));
        mModelMessages = new ArrayList<BluetoothMapMessage>();
        mAdapterMessages = new BluetoothMapMessageAdapter();
        mListViewMessages.setAdapter(mAdapterMessages);
        mListViewMessages.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String handle = mModelMessages.get(position).getHandle();

                getMessage(handle, CharsetType.UTF_8, false);
            }
        });
        registerForContextMenu(mListViewMessages);

        mEditTextSubjectLength = (EditText) findViewById(R.id.maptest_msglist_subject_len);
        mEditTextSubjectLength.setText(String.valueOf(SUBJECT_LENGTH_DEFAULT));
        mViewBmsgHandle = (EditText) findViewById(R.id.map_msg_handle);
        mViewBmsgStatus = (EditText) findViewById(R.id.maptest_bmsg_status);
        mViewBmsgType = (EditText) findViewById(R.id.maptest_bmsg_type);
        mViewBmsgFolder = (EditText) findViewById(R.id.maptest_bmsg_folder);
        mViewBmsgEncoding = (EditText) findViewById(R.id.maptest_bbody_encoding);
        mViewBmsgCharset = (EditText) findViewById(R.id.maptest_bbody_charset);
        mViewBmsgLanguage = (EditText) findViewById(R.id.maptest_bbody_language);
        mViewBmsgContents = (EditText) findViewById(R.id.maptest_message);
        mViewBmsgOrig = (EditText) findViewById(R.id.maptest_orig);
        mViewBmsgRcpt = (EditText) findViewById(R.id.maptest_rcpt);

        clearFolderList();
        updateListEmptyView(false);

        mActionBar = getActionBar();
        mActionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);

        ActionBar.TabListener tabListener = new ActionBar.TabListener() {
            @Override
            public void onTabUnselected(Tab tab, FragmentTransaction ft) {
            }

            @Override
            public void onTabSelected(Tab tab, FragmentTransaction ft) {
                mCurrentTab = tab.getText().toString();

                if (mCurrentTab.equals(TAB_LIST)) {
                    mViewFlipper.setDisplayedChild(0);
                } else if (mCurrentTab.equals(TAB_PREVIEW)) {
                    mViewFlipper.setDisplayedChild(1);
                } else if (mCurrentTab.equals(TAB_EDIT)) {
                    mViewFlipper.setDisplayedChild(2);
                }

                invalidateOptionsMenu();
            }

            @Override
            public void onTabReselected(Tab tab, FragmentTransaction ft) {
            }
        };

        for (String tab : mActionBarTabsNames) {
            mActionBar.addTab(
                    mActionBar.newTab()
                            .setText(tab)
                            .setTabListener(tabListener));

        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_CODE_GET_PARAMETERS) {
            if (resultCode == RESULT_OK) {
                mMessageListingParameters = (int) data.getLongExtra("result", 0);
            }
        }
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
        super.onDestroy();
        Logger.v(TAG, "onDestroy()");
        if (mProfileService != null) {
            mProfileService.setMapCallback(mMasInstanceId, null);
        }
        unbindService(mMapServiceConnection);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        mActionBarMenu = menu;

        getMenuInflater().inflate(R.menu.menu_map_test, menu);

        boolean connected = false;

        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ) {
            connected = (mProfileService.getMapClient(mMasInstanceId).getState() == BluetoothMasClient.ConnectionState.CONNECTED);
        }

        menu.findItem(R.id.menu_map_connect).setVisible(!connected);
        menu.findItem(R.id.menu_map_disconnect).setVisible(connected);
        menu.findItem(R.id.menu_map_update_inbox).setVisible(connected);

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_map_connect:
                onClickConnect();
                break;
            case R.id.menu_map_disconnect:
                onClickDisconnect();
                break;
            case R.id.menu_map_update_inbox:
                onClickUpdateInbox();
                break;
            case R.id.menu_map_goto_inbox:
                goToFolder("telecom/msg/inbox");
                break;
            case R.id.menu_map_goto_outbox:
                goToFolder("telecom/msg/outbox");
                break;
            case R.id.menu_map_goto_draft:
                goToFolder("telecom/msg/draft");
                break;
            default:
                Logger.w(TAG, "Unknown item selected.");
                break;
        }

        return super.onOptionsItemSelected(item);
    }

    private void clearFolderList() {
        mAdapterFolders.clear();
        mAdapterFolders.add(".");
    }

    private void onClickConnect() {
        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
            mProfileService.getMapClient(mMasInstanceId).connect();
            goToState(Job.CONNECT);
        }
    }

    private void onClickDisconnect() {
        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
            mProfileService.getMapClient(mMasInstanceId).disconnect();
            goToState(Job.DISCONNECT);
        }
    }

    private void onClickUpdateInbox() {
        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
            mProfileService.getMapClient(mMasInstanceId).updateInbox();
            goToState(Job.UPDATE_INBOX);
        }
    }

    private VCardEntry createVcard(BluetoothMapBmessage.Type type, String val) {
        VCardEntry vcard = new VCardEntry();
        VCardProperty prop = new VCardProperty();

        prop.setName(VCardConstants.PROPERTY_N);
        prop.setValues(val);
        vcard.addProperty(prop);

        if (BluetoothMapBmessage.Type.EMAIL.equals(type)) {
            prop.setName(VCardConstants.PROPERTY_EMAIL);
        } else {
            prop.setName(VCardConstants.PROPERTY_TEL);
        }
        vcard.addProperty(prop);

        return vcard;
    }

    public void onClickPushMessage(View v) {
        boolean transparent = ((CheckBox) findViewById(R.id.map_msg_push_transparent)).isChecked();
        boolean retry = ((CheckBox) findViewById(R.id.map_msg_push_retry)).isChecked();
        Spinner typeView = (Spinner) findViewById(R.id.bmsgedit_type);
        Spinner encView = (Spinner) findViewById(R.id.bmsgedit_encoding);
        EditText contentsView = (EditText) findViewById(R.id.bmsgedit_contents);
        int charset = ((RadioGroup) findViewById(R.id.map_msg_push_charset))
                .getCheckedRadioButtonId();

        BluetoothMapBmessage bmsg = new BluetoothMapBmessage();

        bmsg.setStatus(BluetoothMapBmessage.Status.UNREAD);

        for (BluetoothMapBmessage.Type t : BluetoothMapBmessage.Type.values()) {
            if (t.name().equals(typeView.getSelectedItem().toString())) {
                bmsg.setType(t);
                break;
            }
        }

        if (charset != R.id.map_msg_push_charset_native) {
            bmsg.setCharset("UTF-8");
        }

        if (encView.getSelectedItemPosition() > 0) {
            bmsg.setEncoding(encView.getSelectedItem().toString());
        }

        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ) {
            bmsg.setFolder(mProfileService.getMapClient(mMasInstanceId).getCurrentPath());
        }

        for (String rcpt : mEditOriginators) {
            bmsg.addOriginator(createVcard(bmsg.getType(), rcpt));
        }

        for (String orig : mEditRecipients) {
            bmsg.addRecipient(createVcard(bmsg.getType(), orig));
        }

        bmsg.setBodyContent(contentsView.getText().toString());

        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ) {
            mProfileService
                .getMapClient(mMasInstanceId)
                .pushMessage(
                        null,
                        bmsg,
                        charset == R.id.map_msg_push_charset_native ? BluetoothMasClient.CharsetType.NATIVE
                                : BluetoothMasClient.CharsetType.UTF_8, transparent, retry);

            goToState(Job.PUSH_MESSAGE);
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        int instanceId = intent.getIntExtra(ProfileService.EXTRA_MAP_INSTANCE_ID, -1);

        if (instanceId < 0) {
            // don't care if instance info is missing
            return;
        }

        // in case request is for different MAS instance than activity is
        // running, we'll just restart activity using the same intent to get
        // proper MAS instance
        if (instanceId != mMasInstanceId) {
            finish();
            overridePendingTransition(0, 0);
            startActivity(intent);
            overridePendingTransition(0, 0);
            return;
        }

        if (ProfileService.ACTION_MAP_GET_MESSAGE.equals(intent.getAction())) {
            String handle = intent.getStringExtra(ProfileService.EXTRA_MAP_MESSAGE_HANDLE);
            getMessage(handle, CharsetType.UTF_8, false);
        }
    }

    public void onClickDeleteMessage(View view) {
        if(mPreviewMsgHandle != null && mProfileService != null &&
            (mProfileService.getMapClient(mMasInstanceId)) != null ) {
            mProfileService.getMapClient(mMasInstanceId).setMessageDeletedStatus(mPreviewMsgHandle,
                    true);
            goToState(Job.DELETE_MESSAGE);
        }
    }

    public void onClickSetStatus(View view) {
        if (mMapBmessage == null || mPreviewMsgHandle == null) {
            return;
        }

        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ) {
            if (mMapBmessage.getStatus().equals(BluetoothMapBmessage.Status.READ)) {
                mProfileService.getMapClient(mMasInstanceId).setMessageReadStatus(mPreviewMsgHandle,
                    false);
                goToState(Job.SET_STATUS_UNREAD);
            } else {
                mProfileService.getMapClient(mMasInstanceId).setMessageReadStatus(mPreviewMsgHandle,
                    true);
                goToState(Job.SET_STATUS_READ);
            }
        }
    }

    public void onClickSetPathRoot(View view) {
        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
            mProfileService.getMapClient(mMasInstanceId).setFolderRoot();
            goToState(Job.SET_PATH);
        }
    }

    public void onClickSetPathUp(View view) {
        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
            mProfileService.getMapClient(mMasInstanceId).setFolderUp();
            goToState(Job.SET_PATH);
        }
    }

    public void onClickSetPathEnter(View view) {
        String folder = mSpinnerFolders.getSelectedItem().toString();

        // do not let go to current folder
        if (folder.equals(".")) {
            return;
        }

        mProfileService.getMapClient(mMasInstanceId).setFolderDown(folder);
        goToState(Job.SET_PATH);
    }

    public void onClickGetFolderListing(View view) {
        int count = 0;
        int offset = 0;

        try {
            count = Integer.parseInt(mEditTextMaxListCountFolders.getText().toString());
        } catch (NumberFormatException e) {
            Toast.makeText(this,
                   "Incorrect maxListCount. Default will be used",
                         Toast.LENGTH_LONG).show();
        }

        try {
            offset = Integer.parseInt(mEditTextListStartOffsetFolders.getText().toString());
        } catch (NumberFormatException e) {
            Toast.makeText(this,
                   "Incorrect listStartOffset. Default will be used",
                         Toast.LENGTH_LONG).show();
        }

        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
            try {
                mProfileService.getMapClient(mMasInstanceId).getFolderListing(count, offset);
                goToState(Job.GET_FOLDER_LISTING);
            } catch (IllegalArgumentException e) {
                Toast.makeText(this,
                    "GetFolderListing FAILED: illegal arguments (" + e.getMessage() + ")",
                    Toast.LENGTH_LONG).show();
            }
        }
    }

    public void onClickGetFolderListingSize(View view) {
        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
            mProfileService.getMapClient(mMasInstanceId).getFolderListingSize();
            goToState(Job.GET_FOLDER_LISTING_SIZE);
        }
    }

    public void onClickMessageParameters(View view) {
        Intent intent = new Intent(MapTestActivity.this, MessageFilterActivity.class);
        intent.putExtra("filter", (long) mMessageListingParameters);
        startActivityForResult(intent, REQUEST_CODE_GET_PARAMETERS);
    }

    public MessagesFilter getLocalMessageFilter() {
        MessagesFilter messageFilter = new MessagesFilter();
        messageFilter.setMessageType(mMessageType);
        messageFilter.setOriginator(mOriginator);
        messageFilter.setPeriod(mPeriodBegin, mPeriodEnd);
        messageFilter.setPriority(mPriority);
        messageFilter.setReadStatus(mReadStatus);
        messageFilter.setRecipient(mRecipient);
        return messageFilter;
    }

    public class MessageFilterDialogFragment extends DialogFragment {
        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            View dialogView = MapTestActivity.this.getLayoutInflater().inflate(
                    R.layout.messages_filter, null);

            final MessageFilterHolder holder = new MessageFilterHolder(dialogView);
            holder.populate();

            final AlertDialog.Builder alertBuilder = new AlertDialog.Builder(MapTestActivity.this);
            alertBuilder.setView(dialogView);
            alertBuilder
                    .setTitle(getResources().getString(R.string.dialog_title_create_msg_filter));
            alertBuilder.setPositiveButton(android.R.string.ok,
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            holder.save();
                        }
                    });
            alertBuilder.setNegativeButton(android.R.string.cancel, null);
            alertBuilder.setNeutralButton(getResources().getString(R.string.clear), null);

            // After click on button with assigned listener in function
            // setNeutralButton, the dialog automatically will close. To avoid
            // it,
            // onClickListener for this button have to be assigned manually.
            final AlertDialog alertDialog = alertBuilder.create();
            alertDialog.setOnShowListener(new DialogInterface.OnShowListener() {
                @Override
                public void onShow(DialogInterface dialog) {
                    Button clearButton = alertDialog.getButton(DialogInterface.BUTTON_NEUTRAL);
                    clearButton.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            holder.clear();
                        }
                    });
                }
            });

            return alertDialog;
        }
    }

    public void onClickGetMessagesFilter(View view) {
        new MessageFilterDialogFragment().show(getFragmentManager(), "msg_filter");
    }

    class MessageFilterHolder {
        CheckBox type_sms_gsm = null;
        CheckBox type_sms_cdma = null;
        CheckBox type_email = null;
        CheckBox type_mms = null;
        RadioButton status_read_all = null;
        RadioButton status_unread = null;
        RadioButton status_read = null;
        RadioButton priority_all = null;
        RadioButton priority_high = null;
        RadioButton priority_non_high = null;
        EditText period_begin = null;
        EditText period_end = null;
        EditText recipient = null;
        EditText originator = null;
        ImageButton pickPeriodBegin = null;
        ImageButton pickPeriodEnd = null;

        MessageFilterHolder(View row) {
            type_sms_gsm = (CheckBox) row.findViewById(R.id.map_filter_type_sms_gsm);
            type_sms_cdma = (CheckBox) row.findViewById(R.id.map_filter_type_sms_cdma);
            type_email = (CheckBox) row.findViewById(R.id.map_filter_type_email);
            type_mms = (CheckBox) row.findViewById(R.id.map_filter_type_mms);
            status_read_all = (RadioButton) row.findViewById(R.id.map_filter_read_status_all);
            status_unread = (RadioButton) row.findViewById(R.id.map_filter_read_status_unread);
            status_read = (RadioButton) row.findViewById(R.id.map_filter_read_status_read);
            priority_all = (RadioButton) row.findViewById(R.id.map_filter_priority_all);
            priority_high = (RadioButton) row.findViewById(R.id.map_filter_priority_high);
            priority_non_high = (RadioButton) row.findViewById(R.id.map_filter_status_non_high);
            period_begin = (EditText) row.findViewById(R.id.map_filter_period_begin);
            period_end = (EditText) row.findViewById(R.id.map_filter_period_end);
            recipient = (EditText) row.findViewById(R.id.map_filter_recipient);
            originator = (EditText) row.findViewById(R.id.map_filter_originator);
            pickPeriodBegin = (ImageButton) row.findViewById(R.id.map_pick_data_period_begin);
            pickPeriodBegin.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    new DateTimePicker(period_begin).pick();
                }
            });
            pickPeriodEnd = (ImageButton) row.findViewById(R.id.map_pick_data_period_end);
            pickPeriodEnd.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    new DateTimePicker(period_end).pick();
                }
            });
        }

        void populate() {
            type_sms_gsm.setChecked((mMessageType & MessagesFilter.MESSAGE_TYPE_SMS_GSM) != 0);
            type_sms_cdma.setChecked((mMessageType & MessagesFilter.MESSAGE_TYPE_SMS_CDMA) != 0);
            type_email.setChecked((mMessageType & MessagesFilter.MESSAGE_TYPE_EMAIL) != 0);
            type_mms.setChecked((mMessageType & MessagesFilter.MESSAGE_TYPE_MMS) != 0);
            status_read_all.setChecked(mReadStatus == MessagesFilter.READ_STATUS_ANY);
            status_read.setChecked(mReadStatus == MessagesFilter.READ_STATUS_READ);
            status_unread.setChecked(mReadStatus == MessagesFilter.READ_STATUS_UNREAD);
            priority_all.setChecked(mPriority == MessagesFilter.PRIORITY_ANY);
            priority_high.setChecked(mPriority == MessagesFilter.PRIORITY_HIGH);
            priority_non_high.setChecked(mPriority == MessagesFilter.PRIORITY_NON_HIGH);

            if (mPeriodBegin != null) {
                period_begin.setText(mSimpleDateFormat.format(mPeriodBegin));
            } else {
                period_begin.setText(null);
            }

            if (mPeriodEnd != null) {
                period_end.setText(mSimpleDateFormat.format(mPeriodEnd));
            } else {
                period_end.setText(null);
            }

            recipient.setText(mRecipient);
            originator.setText(mOriginator);
        }

        void clear() {
            mMessageType = MessagesFilter.MESSAGE_TYPE_ALL;
            mPeriodBegin = null;
            mPeriodEnd = null;
            mReadStatus = MessagesFilter.READ_STATUS_ANY;
            mRecipient = null;
            mOriginator = null;
            mPriority = MessagesFilter.PRIORITY_ANY;

            populate();
        }

        void save() {
            mMessageType = MessagesFilter.MESSAGE_TYPE_ALL;

            if (type_sms_gsm.isChecked()) {
                mMessageType |= MessagesFilter.MESSAGE_TYPE_SMS_GSM;
            }

            if (type_sms_cdma.isChecked()) {
                mMessageType |= MessagesFilter.MESSAGE_TYPE_SMS_CDMA;
            }

            if (type_email.isChecked()) {
                mMessageType |= MessagesFilter.MESSAGE_TYPE_EMAIL;
            }

            if (type_mms.isChecked()) {
                mMessageType |= MessagesFilter.MESSAGE_TYPE_MMS;
            }

            if (status_read.isChecked()) {
                mReadStatus = MessagesFilter.READ_STATUS_READ;
            } else if (status_unread.isChecked()) {
                mReadStatus = MessagesFilter.READ_STATUS_UNREAD;
            } else {
                mReadStatus = MessagesFilter.READ_STATUS_ANY;
            }

            if (priority_high.isChecked()) {
                mPriority = MessagesFilter.PRIORITY_HIGH;
            } else if (priority_non_high.isChecked()) {
                mPriority = MessagesFilter.PRIORITY_NON_HIGH;
            } else {
                mPriority = MessagesFilter.PRIORITY_ANY;
            }

            try {
                mPeriodBegin = mSimpleDateFormat.parse(period_begin.getText().toString());
            } catch (ParseException e) {
                mPeriodBegin = null;
                Logger.e(TAG, "Exception during parse begin period!");
            }

            try {
                mPeriodEnd = mSimpleDateFormat.parse(period_end.getText().toString());
            } catch (ParseException e) {
                mPeriodEnd = null;
                Logger.e(TAG, "Exception during parse end period!");
            }

            mRecipient = recipient.getText().toString();
            mOriginator = originator.getText().toString();
        }
    }

    public void onClickGetMessagesListing(View view) {
        String folder = mSpinnerFolders.getSelectedItem().toString();

        int maxListCount = 0;
        int listStartOffset = 0;
        int subjectLength = 0;

        try {
            maxListCount = Integer.parseInt(
                    mEditTextMaxListCountMessages.getText().toString());
        } catch (NumberFormatException e) {
            Toast.makeText(this,
                   "Incorrect maxListCount. Some defaults will be used",
                         Toast.LENGTH_LONG).show();
        }

        try {
            listStartOffset = Integer.parseInt(
                    mEditTextListStartOffsetMessages.getText().toString());
        } catch (NumberFormatException e) {
            Toast.makeText(this,
                   "Incorrect listStartOffset. Some defaults will be used",
                         Toast.LENGTH_LONG).show();
        }

        try {
            subjectLength = Integer.parseInt(
                    mEditTextSubjectLength.getText().toString());
        } catch (NumberFormatException e) {
            Toast.makeText(this,
                   "Incorrect subject lenght. Some defaults will be used",
                         Toast.LENGTH_LONG).show();
        }
        if (folder.equals(".")) {
            folder = "";
        }

        try {
            if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
                mProfileService.getMapClient(mMasInstanceId).getMessagesListing(folder,
                        mMessageListingParameters,
                        getLocalMessageFilter(), subjectLength, maxListCount, listStartOffset);
                goToState(Job.GET_MESSAGE_LISTING);
            }
            updateListEmptyView(true);
        } catch (IllegalArgumentException e) {
            Toast.makeText(this,
                    "GetMessagesListing FAILED: illegal arguments (" + e.getMessage() + ")",
                    Toast.LENGTH_LONG).show();
        }
    }

    public void updateListEmptyView(boolean working) {
        View progressBar = findViewById(R.id.maptest_msglist_progressbar);
        View textView = findViewById(R.id.maptest_msglist_empty);

        if (working) {
            textView.setVisibility(View.GONE);
            mListViewMessages.setEmptyView(progressBar);
        } else {
            progressBar.setVisibility(View.GONE);
            mListViewMessages.setEmptyView(textView);
        }
    }

    public void onClickGetMessageListingSize(View view) {
        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
            mProfileService.getMapClient(mMasInstanceId).getMessagesListingSize();
            goToState(Job.GET_MESSAGE_LISTING_SIZE);
        }
    }

    class BluetoothMapMessageAdapter extends ArrayAdapter<BluetoothMapMessage> {
        BluetoothMapMessageAdapter() {
            super(MapTestActivity.this, android.R.layout.simple_list_item_1, mModelMessages);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            View v = convertView;

            if (v == null) {
                v = getLayoutInflater().inflate(R.layout.message_row, parent, false);
            }

            BluetoothMapMessage msg = mModelMessages.get(position);

            ((TextView) v.findViewById(R.id.message_row_type)).setText(msg.getType().toString());
            ((TextView) v.findViewById(R.id.message_row_handle)).setText(msg.getHandle());

            ((TextView) v.findViewById(R.id.message_row_flag_text))
                    .setTextColor(msg.isText() ? Color.YELLOW : Color.DKGRAY);
            ((TextView) v.findViewById(R.id.message_row_flag_read))
                    .setTextColor(msg.isRead() ? Color.YELLOW : Color.DKGRAY);
            ((TextView) v.findViewById(R.id.message_row_flag_sent))
                    .setTextColor(msg.isSent() ? Color.YELLOW : Color.DKGRAY);
            ((TextView) v.findViewById(R.id.message_row_flag_drm))
                    .setTextColor(msg.isProtected() ? Color.YELLOW : Color.DKGRAY);
            ((TextView) v.findViewById(R.id.message_row_flag_prio))
                    .setTextColor(msg.isPriority() ? Color.YELLOW : Color.DKGRAY);

            ((TextView) v.findViewById(R.id.message_row_subject)).setText(msg.getSubject());
            Date dateTimeFormat = msg.getDateTime();
            if(dateTimeFormat != null ) {
                ((TextView) v.findViewById(R.id.message_row_date))
                        .setText(new SimpleDateFormat("dd/MM/yyyy HH:mm:ss").format(dateTimeFormat));
            }

            ((TextView) v.findViewById(R.id.message_row_from)).setText(msg.getSenderAddressing());
            if (msg.getSenderAddressing() != null && !msg.getSenderAddressing().isEmpty()) {
                ((TextView) v.findViewById(R.id.message_row_from_lbl))
                        .setVisibility(View.VISIBLE);
            } else {
                ((TextView) v.findViewById(R.id.message_row_from_lbl))
                        .setVisibility(View.INVISIBLE);
            }

            ((TextView) v.findViewById(R.id.message_row_to)).setText(msg.getRecipientAddressing());
            if (msg.getRecipientAddressing() != null && !msg.getRecipientAddressing().isEmpty()) {
                ((TextView) v.findViewById(R.id.message_row_to_lbl)).setVisibility(View.VISIBLE);
            } else {
                ((TextView) v.findViewById(R.id.message_row_to_lbl)).setVisibility(View.INVISIBLE);
            }

            return v;
        }
    }

    class DateTimePicker {
        View view = null;
        EditText editText = null;
        DatePicker dataPicker = null;
        TimePicker timePicker = null;

        DateTimePicker(EditText ref) {
            editText = ref;

            view = getLayoutInflater().inflate(R.layout.date_time_picker, null);

            dataPicker = (DatePicker) view.findViewById(R.id.date_picker);
            timePicker = (TimePicker) view.findViewById(R.id.time_picker);
            timePicker.setIs24HourView(true);

            String oldValue = editText.getText().toString();

            if (oldValue != null && !oldValue.isEmpty() && !oldValue.equals("")) {
                try {
                    Calendar cal = Calendar.getInstance();
                    cal.setTime(mSimpleDateFormat.parse(oldValue));
                    dataPicker.updateDate(cal.get(Calendar.YEAR),
                            cal.get(Calendar.MONTH), cal.get(Calendar.DAY_OF_MONTH));
                    timePicker.setCurrentHour(cal.get(Calendar.HOUR_OF_DAY));
                    timePicker.setCurrentMinute(cal.get(Calendar.MINUTE));
                } catch (ParseException e) {
                    Logger.e(TAG, "Parse exception in DataTimePicker!");
                }
            }
        }

        public void pick() {
            AlertDialog.Builder alertBuilder = new AlertDialog.Builder(MapTestActivity.this);
            alertBuilder.setView(view);
            alertBuilder.setTitle(getResources().getString(R.string.dialog_title_pick_date_time));
            alertBuilder.setPositiveButton(android.R.string.ok,
                    new DialogInterface.OnClickListener() {

                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            int year = dataPicker.getYear();
                            int month = dataPicker.getMonth();
                            int day = dataPicker.getDayOfMonth();
                            int hour = timePicker.getCurrentHour();
                            int minute = timePicker.getCurrentMinute();

                            editText.setText(mSimpleDateFormat.format(new GregorianCalendar(
                                    year, month, day, hour, minute, 0).getTime()));
                        }
                    });
            alertBuilder.setNegativeButton(android.R.string.cancel, null);
            alertBuilder.show();
        }
    }

    private void resetPreviewEditUi() {
        mViewBmsgHandle.setText("");

        mViewBmsgStatus.setText("");
        mViewBmsgType.setText("");
        mViewBmsgFolder.setText("");

        mViewBmsgEncoding.setText("");
        mViewBmsgCharset.setText("");
        mViewBmsgLanguage.setText("");

        mViewBmsgContents.setText("");

        mViewBmsgOrig.setText("");
        mViewBmsgRcpt.setText("");

        mPreviewMsgHandle = null;
        mMapBmessage = null;

        invalidateOptionsMenu();
    }

    private void updateUi(boolean invalidateOptionsMenu) {
        if (mListTouchables == null) {
            LinearLayout lay = (LinearLayout) findViewById(R.id.maptest_tab_list);
            mListTouchables = lay.getTouchables();
        }
        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
            for (View view : mListTouchables) {
                view.setEnabled(mProfileService.getMapClient(mMasInstanceId).getState() == BluetoothMasClient.ConnectionState.CONNECTED
                    && mCurrentJob == Job.IDLE);
            }
        }
        if (invalidateOptionsMenu) {
            invalidateOptionsMenu();
        }
    }

    private void goToState(Job job) {
        Logger.v(TAG, "Switch to " + job + " state from " + mCurrentJob.toString() + ".");
        mCurrentJob = job;
        updateUi(true);
    }

    private void updateMessage(BluetoothMapBmessage message) {
        StringBuilder sb;

        mPreviewMsgHandle = mPendingGetMessageHandle;
        mPendingGetMessageHandle = null;

        mMapBmessage = message;

        boolean isRead = mMapBmessage.getStatus().equals(BluetoothMapBmessage.Status.READ);
        ((Button) (findViewById(R.id.map_msg_set_status))).setText(
                isRead ? getString(R.string.map_set_unread) : getString(R.string.map_set_read));

        mActionBar.selectTab(mActionBar.getTabAt(1));

        mViewBmsgHandle.setText(mPreviewMsgHandle);

        mViewBmsgStatus.setText(message.getStatus().toString());
        mViewBmsgType.setText(message.getType().toString());
        mViewBmsgFolder.setText(message.getFolder());

        mViewBmsgEncoding.setText(message.getEncoding());
        mViewBmsgCharset.setText(message.getCharset());
        mViewBmsgLanguage.setText(message.getLanguage());

        mViewBmsgContents.setText(message.getBodyContent());

        sb = new StringBuilder();
        for (VCardEntry vcard : message.getOriginators()) {
            sb.append(vcard.getDisplayName());
            if (message.getType().equals(BluetoothMapBmessage.Type.EMAIL)) {
                if (vcard.getEmailList() != null && vcard.getEmailList().size() > 0) {
                    sb.append(" <").append(vcard.getEmailList().get(0).getAddress()).append(">");
                }
            } else {
                if (vcard.getPhoneList() != null && vcard.getPhoneList().size() > 0) {
                    sb.append(" <").append(vcard.getPhoneList().get(0).getNumber()).append(">");
                }
            }
            sb.append(", ");
        }
        mViewBmsgOrig.setText(sb.toString());

        sb = new StringBuilder();
        for (VCardEntry vcard : message.getRecipients()) {
            sb.append(vcard.getDisplayName());
            if (message.getType().equals(BluetoothMapBmessage.Type.EMAIL)) {
                if (vcard.getEmailList() != null && vcard.getEmailList().size() > 0) {
                    sb.append(" <").append(vcard.getEmailList().get(0).getAddress()).append(">");
                }
            } else {
                if (vcard.getPhoneList() != null && vcard.getPhoneList().size() > 0) {
                    sb.append(" <").append(vcard.getPhoneList().get(0).getNumber()).append(">");
                }
            }
            sb.append(", ");
        }
        mViewBmsgRcpt.setText(sb.toString());
    }

    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) {
        super.onCreateContextMenu(menu, v, menuInfo);

        menu.add(0, 1, 0, "status -> READ");
        menu.add(0, 2, 0, "status -> UNREAD");
        menu.add(0, 3, 0, "status -> DELETED");
        menu.add(0, 4, 0, "status -> UNDELETED");
        menu.add(0, 5, 0, "get in native charset");
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        AdapterContextMenuInfo info = (AdapterContextMenuInfo) item.getMenuInfo();

        BluetoothMapMessage bmsg = mAdapterMessages.getItem(info.position);

        switch (item.getItemId()) {
            case 1:
            case 2:
                if(mProfileService != null &&
                    (mProfileService.getMapClient(mMasInstanceId)) != null ) {
                        mProfileService.getMapClient(mMasInstanceId)
                                    .setMessageReadStatus(bmsg.getHandle(),item.getItemId() == 1);
                    goToState(Job.SET_STATUS_READ);
                }
                break;
            case 3:
            case 4:
                if(mProfileService != null &&
                    (mProfileService.getMapClient(mMasInstanceId)) != null ) {
                    mProfileService.getMapClient(mMasInstanceId).setMessageDeletedStatus(
                        bmsg.getHandle(), item.getItemId() == 3);
                    goToState(Job.DELETE_MESSAGE);
                }
                break;
            case 5:
                getMessage(bmsg.getHandle(), CharsetType.NATIVE, false);
                break;
        }

        return true;
    }

    public void onClickRemovePeople(View v) {
        if (v.getId() == R.id.bmsgedit_orig_del) {
            new StringListDialogFragment(mEditOriginators).show(getFragmentManager(), "orig_del");
        } else if (v.getId() == R.id.bmsgedit_rcpt_del) {
            new StringListDialogFragment(mEditRecipients).show(getFragmentManager(), "rcpt_del");
        }
    }

    public void onClickAddPerson(View v) {
        if (v.getId() == R.id.bmsgedit_rcpt_add) {
            new GetTextDialogFragment().show(getFragmentManager(), "rcpt_add");
        } else if (v.getId() == R.id.bmsgedit_orig_add) {
            new GetTextDialogFragment().show(getFragmentManager(), "orig_add");
        }
    }

    @Override
    public void onGetTextDialogPositive(DialogFragment dialog, String text) {
        if ("rcpt_add".equals(dialog.getTag())) {
            mEditRecipients.add(text);
        } else if ("orig_add".equals(dialog.getTag())) {
            mEditOriginators.add(text);
        }

        refreshEditPeople();
    }

    @Override
    public void onStringListDialogPositive(DialogFragment dialog, ArrayList<String> elements) {
        if ("rcpt_del".equals(dialog.getTag())) {
            mEditRecipients.clear();
            mEditRecipients.addAll(elements);
        } else if ("orig_del".equals(dialog.getTag())) {
            mEditOriginators.clear();
            mEditOriginators.addAll(elements);
        }

        refreshEditPeople();
    }

    private void refreshEditPeople() {
        StringBuilder sb;

        sb = new StringBuilder();
        for (String s : mEditOriginators) {
            sb.append(s).append(", ");
        }
        ((TextView) findViewById(R.id.bmsgedit_orig)).setText(sb.toString());

        sb = new StringBuilder();
        for (String s : mEditRecipients) {
            sb.append(s).append(", ");
        }
        ((TextView) findViewById(R.id.bmsgedit_rcpt)).setText(sb.toString());
    }

    public void onClickGetMessage(View v) {
        String handle = mViewBmsgHandle.getText().toString();
        int selCharset = ((RadioGroup) findViewById(R.id.map_msg_get_charset))
                .getCheckedRadioButtonId();
        boolean attachment = ((CheckBox) findViewById(R.id.map_msg_get_attachment)).isChecked();

        CharsetType charset = (selCharset == R.id.map_msg_get_charset_native ? CharsetType.NATIVE
                : CharsetType.UTF_8);

        if (!getMessage(handle, charset, attachment)) {
            Toast.makeText(MapTestActivity.this, "GetMessage rejected, check parameters",
                    Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickCopyMessage(View v) {
        String type = mViewBmsgType.getText().toString();
        Spinner typeView = (Spinner) findViewById(R.id.bmsgedit_type);

        for (int i = 0; i < typeView.getCount(); i++) {
            if (type.equals(typeView.getItemAtPosition(i))) {
                typeView.setSelection(i);
                break;
            }
        }

        String enc = mViewBmsgEncoding.getText().toString();
        Spinner encView = (Spinner) findViewById(R.id.bmsgedit_encoding);

        for (int i = 0; i < encView.getCount(); i++) {
            if (enc.equals(encView.getItemAtPosition(i))) {
                encView.setSelection(i);
                break;
            }
        }

        ((EditText) findViewById(R.id.bmsgedit_contents)).setText(mViewBmsgContents.getText());
    }

    private void goToFolder(String dst) {
        mSetPathQueue = new ArrayDeque<String>(Arrays.asList(dst.split("/")));

        if(mProfileService != null && (mProfileService.getMapClient(mMasInstanceId)) != null ){
            mProfileService.getMapClient(mMasInstanceId).setFolderRoot();
        }
    }

    private boolean getMessage(String handle, CharsetType charset, boolean attachments) {
        if (mProfileService == null || handle == null) {
            return false;
        }

        BluetoothMasClient cli = mProfileService.getMapClient(mMasInstanceId);
        if (cli == null) {
            return false;
        }

        if (!cli.getMessage(handle, attachments)) {
            return false;
        }

        goToState(Job.GET_MESSAGE);

        mPendingGetMessageHandle = handle;

        return true;
    }
}
