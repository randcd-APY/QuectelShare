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

import android.bluetooth.client.map.BluetoothMapBmessage;
import android.bluetooth.client.map.BluetoothMapEventReport;
import android.bluetooth.client.map.BluetoothMapMessage;

import java.util.ArrayList;

public interface IMapServiceCallback {

    public void onConnect();

    public void onConnectError();

    public void onUpdateInbox();

    public void onUpdateInboxError();

    public void onSetPath(String currentPath);

    public void onSetPathError(String currentPath);

    public void onGetFolderListing(ArrayList<String> folders);

    public void onGetFolderListingError();

    public void onGetFolderListingSize(int size);

    public void onGetFolderListingSizeError();

    public void onGetMessagesListing(ArrayList<BluetoothMapMessage> messages);

    public void onGetMessagesListingError();

    public void onGetMessage(BluetoothMapBmessage message);

    public void onGetMessageError();

    public void onSetMessageStatus();

    public void onSetMessageStatusError();

    public void onPushMessage(String handle);

    public void onPushMessageError();

    public void onGetMessagesListingSize(int size);

    public void onGetMessagesListingSizeError();

    public void onEventReport(BluetoothMapEventReport eventReport);
}
