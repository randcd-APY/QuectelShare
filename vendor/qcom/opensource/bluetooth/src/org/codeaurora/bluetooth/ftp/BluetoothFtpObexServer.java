/*
 * Copyright (c) 2008-2009, Motorola, Inc.
 * Copyright (c) 2010-2012, 2013 The Linux Foundation. All rights reserved.
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

package org.codeaurora.bluetooth.ftp;

import android.content.Context;
import android.os.Message;
import android.os.Handler;
import android.os.PowerManager;
import android.os.StatFs;
import android.text.TextUtils;
import android.util.Log;
import android.os.Bundle;
import android.webkit.MimeTypeMap;


import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;
import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.util.Date;
import java.text.SimpleDateFormat;
import java.util.List;
import java.lang.StringBuffer;

import javax.obex.ServerRequestHandler;
import javax.obex.ResponseCodes;
import javax.obex.ApplicationParameter;
import javax.obex.ServerOperation;
import javax.obex.Operation;
import javax.obex.HeaderSet;
import javax.obex.ObexHelper;
import android.os.Environment;
import android.os.storage.StorageManager;
import android.os.storage.StorageVolume;

public class BluetoothFtpObexServer extends ServerRequestHandler {

    private static final String TAG = "BluetoothFtpObexServer";

    private static final boolean D = BluetoothFtpService.DEBUG;

    private static final boolean V = Log.isLoggable(BluetoothFtpService.LOG_TAG, Log.VERBOSE) ? true : false;

    private static final int UUID_LENGTH = 16;

    /* To help parsing file attributes */
    private static final int INDEX_YEAR = 0;

    private static final int INDEX_MONTH = 1;

    private static final int INDEX_DATE = 2;

    private static final int INDEX_TIME = 3;

    private static final int INDEX_TIME_HOUR = 0;

    private static final int INDEX_TIME_MINUTE = 1;

    // type for list folder contents
    private static final String TYPE_LISTING = "x-obex/folder-listing";

    // record current path the client are browsing
    private String mCurrentPath = "";

    private long mConnectionId;

    private Handler mCallback = null;

    private Context mContext;

    private PowerManager.WakeLock mWakeLock = null;

    public static boolean sIsAborted = false;

    public static final String ROOT_FOLDER_PATH = "/sdcard";

    public static final String ENV_PRIMARY_EXTERNAL_STORAGE = "EXTERNAL_STORAGE";
    public static final String ENV_SECONDARY_EXTERNAL_STORAGE = "SECONDARY_STORAGE";
    private String rootPrimaryStoragePath = null;
    private String rootSecondaryStoragePath = null;
    private StorageManager mStorageManager;
    public static final String PRIMARY_INTERNAL_FOLDERNAME = "PHONE_MEMORY";
    public static final String SECONDARY_EXTERNAL_FOLDERNAME = "EXTERNAL_MEMORY";
    private static final String FOLDER_NAME_DOT = ".";

    private static final String FOLDER_NAME_DOTDOT = "..";

    List<String> filenames;

    List<String> types;

    // 128 bit UUID for FTP
    private static final byte[] FTP_TARGET = new byte[] {
            (byte)0xF9, (byte)0xEC, (byte)0x7B, (byte)0xC4, (byte)0x95,
            (byte)0x3c, (byte)0x11, (byte)0xD2, (byte)0x98, (byte)0x4E,
            (byte)0x52, (byte)0x54, (byte)0x00, (byte)0xDc, (byte)0x9E,
            (byte)0x09
    };

    private static final String[] LEGAL_PATH = {"/sdcard"};

    public BluetoothFtpObexServer(Handler callback, Context context) {
        super();
        mConnectionId = -1;
        mCallback = callback;
        mContext = context;
        // set initial value when ObexServer created
        if (D) Log.d(TAG, "Initialize FtpObexServer");
        filenames = new ArrayList<String>();
        types = new ArrayList<String>();
    }
    /**
    * onConnect
    *
    * Called when a CONNECT request is received.
    *
    * @param request contains the headers sent by the client;
    *        request will never be null
    * @param reply the headers that should be sent in the reply;
    *        reply will never be null
    * @return a response code defined in ResponseCodes that will
    *         be returned to the client; if an invalid response code is
    *         provided, the OBEX_HTTP_INTERNAL_ERROR response code
    *         will be used
    */
    @Override
    public int onConnect(final HeaderSet request, HeaderSet reply) {
        if (D) Log.d(TAG, "onConnect()+");
        acquireFtpWakeLock();
        int returnVal = onConnectInternal(request,  reply);
        releaseFtpWakeLock();
        if (D) Log.d(TAG, "onConnect()- returning " + returnVal);
        return returnVal;
    }

    private int onConnectInternal(final HeaderSet request, HeaderSet reply) {
        if (D) Log.d(TAG, "onConnectInternal()+");
        /* Extract the Target header */
        try {
            byte[] uuid = (byte[])request.getHeader(HeaderSet.TARGET);
            if (uuid == null) {
                return ResponseCodes.OBEX_HTTP_NOT_ACCEPTABLE;
            }
            if (D) Log.d(TAG, "onConnectInternal(): uuid=" + Arrays.toString(uuid));

            if (uuid.length != UUID_LENGTH) {
                Log.w(TAG, "Wrong UUID length");
                return ResponseCodes.OBEX_HTTP_NOT_ACCEPTABLE;
            }
            /* Compare the Uuid from target with FTP service uuid */
            for (int i = 0; i < UUID_LENGTH; i++) {
                if (uuid[i] != FTP_TARGET[i]) {
                    Log.w(TAG, "Wrong UUID");
                    return ResponseCodes.OBEX_HTTP_NOT_ACCEPTABLE;
                }
            }
            /* Add the uuid into the WHO header part of connect reply */
            reply.setHeader(HeaderSet.WHO, uuid);
        } catch (IOException e) {
            Log.e(TAG,"onConnectInternal "+ e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        /* Extract the remote WHO header and fill it in the Target header*/
        try {
            byte[] remote = (byte[])request.getHeader(HeaderSet.WHO);
            if (remote != null) {
                if (D) Log.d(TAG, "onConnectInternal(): remote=" +
                                                 Arrays.toString(remote));
                reply.setHeader(HeaderSet.TARGET, remote);
            }
        } catch (IOException e) {
            Log.e(TAG,"onConnectInternal "+ e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        if (V) Log.v(TAG, "onConnectInternal(): uuid is ok, will send out " +
                "MSG_SESSION_ESTABLISHED msg.");
        /* Notify the FTP service about the session establishment */
        Message msg = Message.obtain(mCallback);
        msg.what = BluetoothFtpService.MSG_SESSION_ESTABLISHED;
        msg.sendToTarget();
        mStorageManager = StorageManager.from(mContext);
        StorageVolume[] volumes = mStorageManager.getVolumeList();
        for (int i = 0; i < volumes.length; i++) {
            String path = volumes[i].getPath();
            if(V) Log.v(TAG, "Fetch storageManager volumeList : " + path + " :: isRemovable= " +
                volumes[i].isRemovable() + ", getDescription= " + volumes[i]
                    .getDescription(mContext.getApplicationContext()) + " , isEmulated= " +
                        volumes[i].isEmulated()+ ", isPrimary=" + volumes[i].isPrimary());
            /*Initialize the internal, external storage root paths from Enivronment*/
            if ( rootPrimaryStoragePath == null && volumes[i].isPrimary()) {
                rootPrimaryStoragePath = path;
            } else if (rootSecondaryStoragePath == null && volumes[i].isRemovable()) {
               //TODO: Support only first external SDCARD in list , in case of multiple
               rootSecondaryStoragePath = path;
            }
        }
        /*Initialize mCurrentPath to null to show Internal and External memory options*/
        mCurrentPath = null;
        if(D) Log.d(TAG,"ENV:  PRIMARY:  "+ rootPrimaryStoragePath +
            "\n SEC: "+ rootSecondaryStoragePath +"\n");


        if (D) Log.d(TAG, "onConnectInternal() -");
        return ResponseCodes.OBEX_HTTP_OK;
    }
    /**
    * onDisconnect
    *
    * Called when a DISCONNECT request is received.
    *
    * @param request contains the headers sent by the client;
    *        request will never be null
    * @param reply the headers that should be sent in the reply;
    *        reply will never be null
    */
    @Override
    public void onDisconnect(final HeaderSet req, final HeaderSet resp) {
        if (D) Log.d(TAG, "onDisconnect() +");

        acquireFtpWakeLock();
        resp.responseCode = ResponseCodes.OBEX_HTTP_OK;
        /* Send a message to the FTP service to close the Server session */
        if (mCallback != null) {
            Message msg = Message.obtain(mCallback);
            msg.what = BluetoothFtpService.MSG_SESSION_DISCONNECTED;
            msg.sendToTarget();
            if (V) Log.v(TAG,
                 "onDisconnect(): msg MSG_SESSION_DISCONNECTED sent out.");
        }
        releaseFtpWakeLock();
        if (D) Log.d(TAG, "onDisconnect() -");
    }
    /**
    * Called when a ABORT request is received.
    */
    @Override
    public int onAbort(HeaderSet request, HeaderSet reply) {
        if (D) Log.d(TAG, "onAbort() +");
        acquireFtpWakeLock();
        sIsAborted = true;
        if (D) Log.d(TAG, "onAbort() -");
        releaseFtpWakeLock();
        return ResponseCodes.OBEX_HTTP_OK;
    }

    /**
    * onDelete
    *
    * Called when a DELETE request is received.
    *
    * @param request contains the headers sent by the client;
    *        request will never be null
    * @param reply the headers that should be sent in the reply;
    *        reply will never be null
    * @return a response code defined in ResponseCodes that will
    *         be returned to the client; if an invalid response code is
    *         provided, the OBEX_HTTP_INTERNAL_ERROR response code
    *         will be used
    */
    @Override
    public int onDelete(HeaderSet request, HeaderSet reply) {
        if (D) Log.d(TAG, "onDelete()+");
        acquireFtpWakeLock();
        int returnVal = onDeleteInternal(request,  reply);
        releaseFtpWakeLock();
        if (D) Log.d(TAG, "onDelete()- returning " + returnVal);
        return returnVal;
    }

    private int onDeleteInternal(HeaderSet request, HeaderSet reply) {
        if (D) Log.d(TAG, "onDeleteInternal() +");
        String name = "";
        /* Check if Card is mounted */
        if(FileUtils.checkMountedState() == false) {
           Log.e(TAG,"SD card not Mounted");
           return ResponseCodes.OBEX_HTTP_NO_CONTENT;
        }
        /* 1. Extract the name header
         * 2. Check if the file exists by appending the name to current path
         * 3. Check if its read only
         * 4. Check if the directory is read only
         * 5. If 2 satisfies and 3 ,4 are not true proceed to delete the file
         */
        try{
           name = (String)request.getHeader(HeaderSet.NAME);
           /* Not allowed to delete a folder name with "." and ".." */
           if (TextUtils.equals(name, FOLDER_NAME_DOT) ||
               TextUtils.equals(name, FOLDER_NAME_DOTDOT) ) {
              if(D)  Log.d(TAG, "cannot delete the directory " + name);
              return ResponseCodes.OBEX_HTTP_UNAUTHORIZED;
           }
           if (D) Log.d(TAG,"onDeleteInternal File = " + name +
                                          "mCurrentPath = " + mCurrentPath );
           File deleteFile = new File(mCurrentPath + "/" + name);
           if(deleteFile.exists() == true){
               if (D) Log.d(TAG, "onDeleteInternal(): Found File" + name + "in folder "
                                                         + mCurrentPath);
               if(!deleteFile.canWrite()) {
                   return ResponseCodes.OBEX_HTTP_UNAUTHORIZED;
               }

               if(deleteFile.isDirectory()) {
                   if(!FileUtils.deleteDirectory(mCallback,deleteFile)) {
                       if (D) Log.d(TAG,"Directory  delete unsuccessful");
                       return ResponseCodes.OBEX_HTTP_UNAUTHORIZED;
                   }
               } else {
                   if(!deleteFile.delete()){
                       if (D) Log.d(TAG,"File delete unsuccessful");
                       return ResponseCodes.OBEX_HTTP_UNAUTHORIZED;
                   }
                   FileUtils.sendMessage(mCallback,BluetoothFtpService.MSG_FILE_DELETED,
                                             deleteFile.getAbsolutePath());
                }
           }
           else{
               if (D) Log.d(TAG,"File doesnot exist");
               return ResponseCodes.OBEX_HTTP_NOT_FOUND;
           }
        }catch (IOException e) {
            Log.e(TAG,"onDeleteInternal "+ e.toString());
            if (D) Log.d(TAG, "Delete operation failed");
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        if (D) Log.d(TAG, "onDeleteInternal() -");
        return ResponseCodes.OBEX_HTTP_OK;
    }
    /**
    * onPut
    *
    * Called when a PUT request is received.
    *
    * If an ABORT request is received during the processing of a PUT request,
    * op will be closed by the implementation.
    * @param operation contains the headers sent by the client and allows new
    *        headers to be sent in the reply; op will never be
    *        null
    * @return a response code defined in ResponseCodes that will
    *         be returned to the client; if an invalid response code is
    *         provided, the OBEX_HTTP_INTERNAL_ERROR response code
    *         will be used
    */
    @Override
    public int onPut(final Operation op) {
        if (D) Log.d(TAG, "onPut()+");
        acquireFtpWakeLock();
        int returnVal = onPutInternal(op);
        releaseFtpWakeLock();
        if (D) Log.d(TAG, "onPut()- returning " + returnVal);
        return returnVal;
    }

    private int onPutInternal(final Operation op) {
        if (D) Log.d(TAG, "onPutInternal() +");
        HeaderSet request = null;
        long length;
        String name = "";
        String filetype = "";
        int obexResponse = ResponseCodes.OBEX_HTTP_OK;

        if(FileUtils.checkMountedState() == false) {
            Log.e(TAG,"SD card not Mounted");
            return ResponseCodes.OBEX_HTTP_NO_CONTENT;
        }
        /* 1. Extract the name,length and type header from operation object
         *  2. check if name is null or empty
         *  3. Check if disk has available space for the length of file
         *  4. Open an input stream for the Bluetooth Socket and a file handle
         *     to the folder
         *  5. Check if the file exists and can be overwritten
         *  6. If 2,5 is false and 3 is satisfied proceed to read from the input
         *     stream and write to the new file
         */
        try {
            request = op.getReceivedHeader();
            length = extractLength(request);
            name = (String)request.getHeader(HeaderSet.NAME);
            /* Put with directory name "." and ".." is not allowed */
            if (TextUtils.equals(name, FOLDER_NAME_DOT) ||
                TextUtils.equals(name, FOLDER_NAME_DOTDOT) ) {
               if(D) Log.d(TAG, "cannot put the directory " + name);
               return ResponseCodes.OBEX_HTTP_NOT_ACCEPTABLE;
            }
            filetype = (String)request.getHeader(HeaderSet.TYPE);
            if (D) Log.d(TAG,"type = " + filetype + " name = " + name
                    + " Current Path = " + mCurrentPath + "length = " + length);

            if (length == 0) {
                if (D) Log.d(TAG, "length is 0,proceeding with the transfer");
            }
            if (name == null || name.equals("")) {
                if (D) Log.d(TAG, "name is null or empty, reject the transfer");
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            }
            if(FileUtils.checkAvailableSpace(length) == false) {
                if (D) Log.d(TAG,"No Space Available");
                return ResponseCodes.OBEX_HTTP_ENTITY_TOO_LARGE;
            }
            BufferedOutputStream buff_op_stream = null;
            InputStream in_stream = null;

            try {
                in_stream = op.openInputStream();
            } catch (IOException e1) {
                Log.e(TAG,"onPutInternal open input stream "+ e1.toString());
                if (D) Log.d(TAG, "Error while openInputStream");
                return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            }

            int positioninfile = 0;
            File fileinfo = new File(mCurrentPath+ "/" + name);
            File parentFile = fileinfo.getParentFile();
            if (parentFile != null) {
                if (parentFile.canWrite() == false) {
                    if (D) Log.d(TAG,"Dir "+ fileinfo.getParent() +"is read-only");
                    return ResponseCodes.OBEX_DATABASE_LOCKED;
                }
            } else {
                Log.e(TAG, "Error! Not able to get parent file name");
                return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            }
            /* If File exists we delete and proceed to take the rest of bytes */
            if(fileinfo.exists() == true) {
                if(fileinfo.canWrite()) {
                    fileinfo.delete();
                    fileinfo = null;
                    fileinfo = new File(mCurrentPath+ "/" + name);
                } else {
                    /* if Readonly reject the replace */
                    if (D) Log.d(TAG,"File is readonly");
                    return ResponseCodes.OBEX_DATABASE_LOCKED;
                }
            }

            FileOutputStream fileOutputStream = new FileOutputStream(fileinfo);
            buff_op_stream = new BufferedOutputStream(fileOutputStream, 0x4000);
            int outputBufferSize = op.getMaxPacketSize();
            byte[] buff = new byte[outputBufferSize];
            int readLength = 0;
            long timestamp = 0;
            long starttimestamp = System.currentTimeMillis();
            try {
                while ((positioninfile != length)) {
                    if (sIsAborted) {
                        ((ServerOperation)op).isAborted = true;
                        sIsAborted = false;
                        break;
                    }
                    timestamp = System.currentTimeMillis();
                    if (V) Log.v(TAG,"Read Socket >");
                    readLength = in_stream.read(buff);
                    if (V) Log.v(TAG,"Read Socket <");

                    if (readLength == -1) {
                        if (D) Log.d(TAG,"File reached end at position"
                                                             + positioninfile);
                        break;
                    }

                    buff_op_stream.write(buff, 0, readLength);
                    positioninfile += readLength;

                    if (V) {
                        Log.v(TAG, "Receive file position = " + positioninfile
                                  + " readLength "+ readLength + " bytes took "
                                  + (System.currentTimeMillis() - timestamp) +
                                  " ms");
                    }
                }
            }catch (IOException e1) {
                Log.e(TAG, "onPutInternal File receive"+ e1.toString());
                if (D) Log.d(TAG, "Error when receiving file");
                ((ServerOperation)op).isAborted = true;
                sIsAborted = true;
            }

            long finishtimestamp = System.currentTimeMillis();
            Log.i(TAG,"Put Request TP analysis : Received  "+ positioninfile +
                      " bytes in " + (finishtimestamp - starttimestamp)+"ms");
            if (buff_op_stream != null) {
                try {
                    buff_op_stream.close();
                 } catch (IOException e) {
                     Log.e(TAG,"onPutInternal close stream "+ e.toString());
                     if (D) Log.d(TAG, "Error when closing stream after send");
                     return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                }
            }
            if(D) Log.d(TAG,"close Stream >");
            if(sIsAborted == true){
                sIsAborted = false;
                /* If the transfer completed due to a
                 * abort from Ftp client, clean up the
                 * file in the Server
                 */
                fileinfo.delete();
                Log.e(TAG, " SET isAborted on excepton");
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            }
            if (!closeStream(in_stream, op)) {
                if (D) Log.d(TAG,"Failed to close Input stream");
                return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            }
            if (D) Log.d(TAG,"close Stream <");

            FileUtils.sendMessage(mCallback,BluetoothFtpService.MSG_FILE_RECEIVED,
                                                       fileinfo.getAbsolutePath());

        }catch (IOException e) {
            Log.e(TAG, "onPutInternal headers error "+ e.toString());
            if (D) Log.d(TAG, "request headers error");
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        if (D) Log.d(TAG, "onPutInternal() -");
        return ResponseCodes.OBEX_HTTP_OK;
    }
    /**
    * Called when a SETPATH request is received.
    *
    * @param request contains the headers sent by the client;
    *        request will never be null
    * @param reply the headers that should be sent in the reply;
    *        reply will never be null
    * @param backup true if the client requests that the server
    *        back up one directory before changing to the path described by
    *        name; false to apply the request to the
    *        present path
    * @param create true if the path should be created if it does
    *        not already exist; false if the path should not be
    *        created if it does not exist and an error code should be returned
    * @return a response code defined in ResponseCodes that will
    *         be returned to the client; if an invalid response code is
    *         provided, the OBEX_HTTP_INTERNAL_ERROR response code
    *         will be used
    */
    @Override
    public int onSetPath(final HeaderSet request, final HeaderSet reply,
            final boolean backup, final boolean create) {
        if (D) Log.d(TAG, "onSetPath()+");
        acquireFtpWakeLock();
        int returnVal = onSetPathInternal(request, reply, backup, create);
        releaseFtpWakeLock();
        if (D) Log.d(TAG, "onSetPath()- returning " + returnVal);
        return returnVal;
    }

    private int onSetPathInternal(final HeaderSet request, final HeaderSet reply,
            final boolean backup, final boolean create) {

        if (D) Log.d(TAG, "onSetPathInternal() +");

        String current_path_tmp = mCurrentPath;
        String tmp_path = null;
        /* Check if Card is mounted */
        if(FileUtils.checkMountedState() == false) {
           Log.e(TAG,"SD card not Mounted");
           return ResponseCodes.OBEX_HTTP_NO_CONTENT;
        }
        /* Extract the name header */
        try {
            tmp_path = (String)request.getHeader(HeaderSet.NAME);
        } catch (IOException e) {
            Log.e(TAG,"onSetPathInternal  get header"+ e.toString());
            if (D) Log.d(TAG, "Get name header fail");
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        if (D) Log.d(TAG, "backup=" + backup + " create=" + create +
                   " name=" + tmp_path +" mCurrentPath = " + mCurrentPath);

        /* If the name is "." or ".." do not allow to create or set the directory */
        if (TextUtils.equals(tmp_path, FOLDER_NAME_DOT) ||
            TextUtils.equals(tmp_path, FOLDER_NAME_DOTDOT)) {
           if(D) Log.d(TAG, "cannot create or set the directory to " + tmp_path);
           return ResponseCodes.OBEX_HTTP_NOT_ACCEPTABLE;
        }

        /* If backup flag is set then if the current path is not null then
         * remove the substring till '/' in the current path For ex. if current
         * path is "/sdcard/bluetooth" we will return a string "/sdcard" into
         * current_path_tmp
         *
         * else we append the name to the current path if not null or else
         * set the current path to ROOT Folder path
         */
        if (backup) {
            if (current_path_tmp != null) {
                if (D) Log.d(TAG, "current_tmp_path: " + current_path_tmp);
                if (current_path_tmp.length() != 0 &&
                    current_path_tmp.equals(rootPrimaryStoragePath)
                    == false && current_path_tmp.equals(rootSecondaryStoragePath) == false ) {
                    current_path_tmp = current_path_tmp.substring(0,
                            current_path_tmp.lastIndexOf("/"));
                } else if (current_path_tmp.equals(rootPrimaryStoragePath ) ||
                   current_path_tmp.equals(rootSecondaryStoragePath)){
                   current_path_tmp = null;
                }
            } else {
                /* We have already reached the root folder but user tries to press the
                 * back button
                 */
                Log.e(TAG, "current_path_tmp = null while backup");
                return ResponseCodes.OBEX_HTTP_NOT_ACCEPTABLE;
            }
        } else {
            //SetPath here comes into picture only when tmp_path not null.
            if (tmp_path == null ) {
                current_path_tmp = null;
            } else if ( tmp_path.equals(PRIMARY_INTERNAL_FOLDERNAME)
                && current_path_tmp == null) {
                current_path_tmp = rootPrimaryStoragePath;
            } else if ( tmp_path.equals(SECONDARY_EXTERNAL_FOLDERNAME)
                && current_path_tmp == null) {
                current_path_tmp = rootSecondaryStoragePath;
            } else if(current_path_tmp != null){
                //Allow only PRIMARY and SECONDARY FOLDERs at ROOT
                current_path_tmp = current_path_tmp + "/" + tmp_path;
            }
        }

        /* If the Path passed in the name doesnot exist and if the create flag
         * is set we proceed towards creating the folder in the current folder
         *
         * else if the path doesnot exist and the create flag is not set we
         * return ResponseCodes.OBEX_HTTP_NOT_FOUND
         */
        if ((current_path_tmp != null  && current_path_tmp.length() != 0)) {
            if (!FileUtils.doesPathExist(current_path_tmp)) {
                if (create) {
                    File filecreate = new File(current_path_tmp);
                    if (filecreate != null && !filecreate.mkdir()) {
                        Log.e(TAG, "Could not create " + tmp_path);
                        return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                    }
                } else {
                    Log.e(TAG, "path: " +  current_path_tmp + " not found");
                    return ResponseCodes.OBEX_HTTP_NOT_FOUND;
                }
            } else {
                /* Check if the folder to be created or set is not having same
                 * name as existing file in same location. If file with same name is
                 * present, return error to Client. */
                File file = new File(current_path_tmp);
                if (file.isFile()) {
                    if (create) {
                        Log.e(TAG, "File already exists with same name, " +
                            "can't create folder with same name");
                        return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                    } else {
                        Log.e(TAG, "Path is not valid, path being set is " +
                            "for file not folder");
                        return ResponseCodes.OBEX_HTTP_NOT_FOUND;
                    }
                }
            }
        } else if (current_path_tmp == null && tmp_path != null) {
            if (create) {
               // If path is null and create flag is set, new folder creation at PRIMARY
               // and SECONDARY FOLDERs, so return OBEX_HTTP_NOT_ACCEPTABLE
                Log.e(TAG, "creation of new folder not allowed at root folder");
               return ResponseCodes.OBEX_HTTP_NOT_ACCEPTABLE;
            } else if (!backup) {
                // If path is null and create flag is not set, set path to some folder other than
                // PRIMARY and SECONDARY FOLDERs so return OBEX_HTTP_NOT_FOUND
                Log.e(TAG, "cannot set path to " + tmp_path + " at root folder" +
                    " other than PHONE_MEMORY and EXTERNAL_MEMORY");
                return ResponseCodes.OBEX_HTTP_NOT_FOUND;
            }
        }

        mCurrentPath = current_path_tmp;
        if (V) Log.v(TAG, "after setPath, mCurrentPath ==  " + mCurrentPath);

        if (D) Log.d(TAG, "onSetPathInternal() -");

        return ResponseCodes.OBEX_HTTP_OK;
    }
    /**
    * Called when session is closed.
    */
    @Override
    public void onClose() {
        if (D) Log.d(TAG, "onClose() +");
        acquireFtpWakeLock();
        /* Send a message to the FTP service to close the Server session */
        if (mCallback != null) {
            Message msg = Message.obtain(mCallback);
            msg.what = BluetoothFtpService.MSG_SERVERSESSION_CLOSE;
            msg.sendToTarget();
            if (D) Log.d(TAG,
                       "onClose(): msg MSG_SERVERSESSION_CLOSE sent out.");
        }
        releaseFtpWakeLock();
        if (D) Log.d(TAG, "onClose() -");
    }

    @Override
    public int onGet(Operation op) {
        if (D) Log.d(TAG, "onGet()+");
        acquireFtpWakeLock();
        int returnVal = onGetInternal(op);
        releaseFtpWakeLock();
        if (D) Log.d(TAG, "onGet()- returning " + returnVal);
        return returnVal;
    }

    private int onGetInternal(Operation op) {
        if (D) Log.d(TAG, "onGetInternal() +");

        sIsAborted = false;
        HeaderSet request = null;
        String type = "";
        String name = "";
        /* Check if Card is mounted */
        if(FileUtils.checkMountedState() == false) {
           Log.e(TAG,"SD card not Mounted");
           return ResponseCodes.OBEX_HTTP_NO_CONTENT;
        }
        /*Extract the name and type header from operation object */
        try {
            request = op.getReceivedHeader();
            type = (String)request.getHeader(HeaderSet.TYPE);
            name = (String)request.getHeader(HeaderSet.NAME);
            /* Get with folder name "." and ".." is not allowed */
            if (TextUtils.equals(name, FOLDER_NAME_DOT) ||
                TextUtils.equals(name, FOLDER_NAME_DOTDOT) ) {
               if(D) Log.d(TAG, "cannot get the folder " + name);
               return ResponseCodes.OBEX_HTTP_NOT_ACCEPTABLE;
            }
        } catch (IOException e) {
            Log.e(TAG,"onGetInternal request headers "+ e.toString());
            if (D) Log.d(TAG, "request headers error");
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        if (D) Log.d(TAG,"type = " + type + " name = " + name +
                                       " Current Path = " + mCurrentPath);

        boolean validName = true;

        if (TextUtils.isEmpty(name)) {
            validName = false;
        }
        if (D) Log.d(TAG,"validName = " + validName);


        if(type != null) {
            /* If type is folder listing then invoke the routine to package
             * the folder listing contents in xml format
             *
             * Else call the routine to send the requested file names contents
             */
             if(type.equals(TYPE_LISTING)) {
                if(!validName || ( mCurrentPath!= null && ( mCurrentPath.equals(rootPrimaryStoragePath) ||
                    mCurrentPath.equals(rootSecondaryStoragePath)) ) ) {
                    if (D) Log.d(TAG,"Not having a name ");
                    if(!validName && mCurrentPath == null) {
                       //Show  BOTH Primary and Seconday Folders to Choose
                        List <String> storagePartitions = new ArrayList<String>();
                        storagePartitions.add(PRIMARY_INTERNAL_FOLDERNAME);
                        storagePartitions.add(SECONDARY_EXTERNAL_FOLDERNAME);
                        return folderListingXML(op, storagePartitions);
                    }
                    File rootfolder = new File(mCurrentPath);
                    File [] files = rootfolder.listFiles();
                    if (files != null) {
                        for(int i = 0; i < files.length; i++)
                            if (D) Log.d(TAG,"Folder listing =" + files[i] );
                        return sendFolderListingXml(0,op,files);
                    } else {
                        Log.e(TAG,"error in listing files");
                        return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                    }
                } else {
                    if (D) Log.d(TAG,"Non Root Folder");
                    if(type.equals(TYPE_LISTING) && mCurrentPath != null) {
                        File currentfolder = new File(mCurrentPath);
                        if (D) Log.d(TAG,"Current folder name = " +
                                                currentfolder.getName() +
                                          "Requested subFolder =" + name);
                        if(currentfolder.getName().compareTo(name) != 0) {
                            if (D) {
                                Log.d(TAG,"Not currently in this folder");
                            }
                            File subFolder = new File(mCurrentPath +"/"+ name);
                            if(subFolder.exists()) {
                                File [] files = subFolder.listFiles();
                                if (files != null) {
                                    return sendFolderListingXml(0,op,files);
                                } else {
                                    Log.e(TAG,"error in listing files");
                                    return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                                }
                            } else {
                                Log.e(TAG,
                                    "ResponseCodes.OBEX_HTTP_NO_CONTENT");
                                return ResponseCodes.OBEX_HTTP_NO_CONTENT;
                            }
                        }

                        File [] files = currentfolder.listFiles();
                        if (files != null) {
                            for(int i = 0; i < files.length; i++)
                               if (D) Log.d(TAG,"Non Root Folder listing =" + files[i] );
                            return sendFolderListingXml(0,op,files);
                        } else {
                            Log.e(TAG,"error in listing files");
                            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                        }
                    }
                }
            }
        } else {
            if (D) Log.d(TAG,"File get request");
            File fileinfo = new File (mCurrentPath + "/" + name);
            return sendFileContents(op,fileinfo);
        }
        if (D) Log.d(TAG, "onGetInternal() -");
        return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
    }

    /**
    * sendFileContents
    *
    * Called when a GET request to get the contents of a File is received
    *
    * @param op provides the handle to the current server operation
    * @param fileinfo provides the handle to the file to be sent
    * @return  a response code defined in ResponseCodes that will
    *         be returned to the client; if an invalid response code is
    *         provided, the OBEX_HTTP_INTERNAL_ERROR response code
    *         will be used
    */
    private final int sendFileContents(Operation op,File fileinfo){

        if (D) Log.d(TAG,"sendFile + = " + fileinfo.getName() );
        int position = 0;
        int readLength = 0;
        boolean isitokToProceed = false;
        int outputBufferSize = op.getMaxPacketSize();
        long timestamp = 0;
        int responseCode = -1;
        FileInputStream fileInputStream = null;
        OutputStream outputStream = null;
        BufferedInputStream bis;
        long finishtimestamp;
        long starttimestamp;
        long readbytesleft = 0;
        long filelength = fileinfo.length();

        byte[] buffer = new byte[outputBufferSize];

        if(fileinfo.exists() != true) {
          return ResponseCodes.OBEX_HTTP_NOT_FOUND;
        }

        try {
            fileInputStream = new FileInputStream(fileinfo);
            outputStream = op.openOutputStream();
        } catch(IOException e) {
            Log.e(TAG,"SendFilecontents open stream "+ e.toString());
            if(outputStream != null ) {
              closeStream(outputStream, op);
              outputStream = null;
            }
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        } finally {
            if (fileInputStream != null && outputStream == null) {
                try {
                    fileInputStream.close();
                } catch (IOException ei) {
                    Log.e(TAG, "Error while closing stream"+ ei.toString());
                }
                return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            }
        }
        bis = new BufferedInputStream(fileInputStream, 0x4000);
        starttimestamp = System.currentTimeMillis();
        /*
         * Some Devices expect Length, send Legth Header also
        */
        try {
            HeaderSet reply = new HeaderSet();
            if (reply != null) {
                reply.setHeader(HeaderSet.LENGTH, filelength);
                op.sendHeaders (reply);
            }
            while ((position != filelength)) {
                if (sIsAborted) {
                    ((ServerOperation)op).isAborted = true;
                    sIsAborted = false;
                    break;
                }
                timestamp = System.currentTimeMillis();

                readbytesleft = filelength - position;
                if(readbytesleft < outputBufferSize) {
                    outputBufferSize = (int) readbytesleft;
                }
                readLength = bis.read(buffer, 0, outputBufferSize);
                if (D) Log.d(TAG,"Read File");

                outputStream.write(buffer, 0, readLength);
                position += readLength;
                if (V) {
                    Log.v(TAG, "Sending file position = " + position
                       + " readLength " + readLength + " bytes took "
                       + (System.currentTimeMillis() - timestamp) + " ms");
                }
            }
        } catch (IOException e) {
            Log.e(TAG,"Write aborted " + e.toString());
            if (D) Log.d(TAG,"Write Abort Received");
            ((ServerOperation)op).isAborted = true;
            return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
        }
        finishtimestamp = System.currentTimeMillis();
        if (bis != null) {
            try {
                bis.close();
            } catch (IOException e) {
                Log.e(TAG,"input stream close" + e.toString());
                if (D) Log.d(TAG, "Error when closing stream after send");
                return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            }
        }

        if (!closeStream(outputStream, op)) {
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        if((position == filelength) || (((ServerOperation)op).isAborted == true)) {
            Log.i(TAG,"Get Request TP analysis : Transmitted "+ position +
                  " bytes in" + (finishtimestamp - starttimestamp)  + "ms");
            return ResponseCodes.OBEX_HTTP_OK;
        }else {
            return ResponseCodes.OBEX_HTTP_CONTINUE;
        }
    }

    /**
    * scanDirectory
    *
    * Scans a directory recursively for files and their mimetypes
    * and adds them into a global list of filenames and their
    * corresponding mime type list.
    *
    * @param dir File handle to file/folder
    * @return none
    */

    private final void scanDirectory(File dir) {
        Log.d(TAG,"scanDirectory Dest "+dir);
        if(dir.isFile()) {
            String mimeType = null;
            /* first we look for Mimetype in Android map */
            String extension = null, type = null;
            String name = dir.getAbsolutePath();
            int dotIndex = name.lastIndexOf(".");
            if (dotIndex < 0) {
                if (D) Log.d(TAG, "There is no file extension");
            } else {
                 extension = name.substring(dotIndex + 1).toLowerCase();
                 MimeTypeMap map = MimeTypeMap.getSingleton();
                 type = map.getMimeTypeFromExtension(extension);
                 if (V) Log.v(TAG, "Mimetype guessed from extension " + extension + " is " + type);
                 if (type != null) {
                     mimeType = type;
                 }
                 if (mimeType != null) {
                     mimeType = mimeType.toLowerCase();
                     if (D) Log.d(TAG, "Adding file path "+ dir.getAbsolutePath());
                     filenames.add( dir.getAbsolutePath());
                     if (D) Log.d(TAG, "Adding type" +mimeType);
                     types.add(mimeType);
                 }
            }
            return;
        }

        File [] files = dir.listFiles();
        if (files == null) return;
        for(int i = 0; i < files.length; i++) {
            if (D) Log.d(TAG,"Files =" + files[i]);
            if(files[i].isDirectory()) {
                scanDirectory(files[i]);
            } else if (files[i].isFile()) {
                String mimeType = null;
                /* first we look for Mimetype in Android map */
                String extension = null, type = null;
                String name = files[i].getAbsolutePath();
                int dotIndex = name.lastIndexOf(".");
                if (dotIndex < 0) {
                    if (D) Log.d(TAG, "There is no file extension");
                } else {
                    extension = name.substring(dotIndex + 1).toLowerCase();
                    MimeTypeMap map = MimeTypeMap.getSingleton();
                    type = map.getMimeTypeFromExtension(extension);
                    if (V) Log.v(TAG, "Mimetype guessed from extension " + extension + " is "
                                                                                           + type);
                    if (type != null) {
                        mimeType = type;
                    }
                    if (mimeType != null) {
                        mimeType = mimeType.toLowerCase();
                        if (D) Log.d(TAG, "Adding file path "+ files[i].getAbsolutePath());
                        filenames.add(files[i].getAbsolutePath());
                        if (D) Log.d(TAG, "Adding type" +mimeType);
                        types.add(mimeType);
                    }
                }
            }
        }
    }

    /* Extract the length from header */
    private final long extractLength(HeaderSet request) {
        long len = 0;
        if(request != null) {
            try {
               Object length = request.getHeader(HeaderSet.LENGTH);
               /* Ensure that the length is not null before
                * attempting a type cast to Long
                */
               if(length != null)
                  len = (Long)length;
           } catch(IOException e) {}
        }
        return len;
    }

    /** Function to send folder listing data to client */
    private final int pushBytes(Operation op, final String folderlistString) {
        if (D) Log.d(TAG,"pushBytes +");
        if (folderlistString == null) {
            if (D) Log.d(TAG, "folderlistString is null!");
            return ResponseCodes.OBEX_HTTP_OK;
        }

        byte [] folderListing = folderlistString.getBytes();
        int folderlistStringLen = folderListing.length;
        if (D) Log.d(TAG, "Send Data: len=" + folderlistStringLen);

        OutputStream outputStream = null;
        int pushResult = ResponseCodes.OBEX_HTTP_OK;
        try {
            outputStream = op.openOutputStream();
        } catch (IOException e) {
            Log.e(TAG, "open outputstrem failed" + e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        int position = 0;
        long timestamp = 0;
        int outputBufferSize = op.getMaxPacketSize();
        if (V) Log.v(TAG, "outputBufferSize = " + outputBufferSize);
        while (position != folderlistStringLen) {
            if (sIsAborted) {
                 ((ServerOperation)op).isAborted = true;
                 sIsAborted = false;
                 break;
            }
            if (V) timestamp = System.currentTimeMillis();
            int readLength = outputBufferSize;
            if (folderlistStringLen - position < outputBufferSize) {
                readLength = folderlistStringLen - position;
            }

            try {
                outputStream.write(folderListing, position, readLength);
            } catch (IOException e) {
                Log.e(TAG, "write outputstrem failed" + e.toString());
                pushResult = ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                break;
            }
            if (V) {
                if (D) Log.d(TAG, "Sending folderlist String position = " + position + " readLength "
                        + readLength + " bytes took " + (System.currentTimeMillis() - timestamp)
                        + " ms");
            }
            position += readLength;
        }

        if (V) Log.v(TAG, "Send Data complete!");

        if (!closeStream(outputStream, op)) {
            pushResult = ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        if (V) Log.v(TAG, "pushBytes - result = " + pushResult);
        return pushResult;
    }


    private final  int folderListingXML( Operation op, List<String> list) {
        if (V) Log.v(TAG, "FolderListingXml =" + list.size());

        String str = "<?xml version=\"1.0\"?><!DOCTYPE folder-listing SYSTEM \"obex-folder-listing.dtd\"><folder-listing version=\"1.0\">";
        for (String s : list) {
            str += "<folder name=\"";
            str += s;
            str += "\"/>";
        }

        str += "</folder-listing>";
        if (V) Log.v(TAG, "FolderListingXml -");
        return pushBytes(op, str);
    }


    /** Form and Send an XML format String to client for Folder listing */
    private final int sendFolderListingXml(final int type,Operation op,final File[] files) {
        if (V) Log.v(TAG, "sendFolderListingXml =" + files.length);

        StringBuilder result = new StringBuilder();
        result.append("<?xml version=\"1.0\"?>");
        result.append('\r');
        result.append('\n');
        result.append("<!DOCTYPE folder-listing SYSTEM \"obex-folder-listing.dtd\">");
        result.append('\r');
        result.append('\n');
        result.append("<folder-listing version=\"1.0\">");
        result.append('\r');
        result.append('\n');

        /* For the purpose of parsing file attributes and to maintain the standard,
         * enforce the format to be used
         */
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy MM dd HH:mm");

        String name = "";
        String permission  = "";
        for(int i =0; i < files.length; i++) {

            if (files[i].isDirectory()) {
               name = "folder name";
            } else {
               name = "file name";
            }

            if (files[i].canRead() && files[i].canWrite()) {
               permission = "RW";
            } else if(files[i].canRead()) {
               permission = "R";
            } else if(files[i].canWrite()) {
               permission = "W";
            }

            Date date = new Date(files[i].lastModified());
            String[] dateset = sdf.format(date).split(" ");

            StringBuffer xmldateformat = new StringBuffer(dateset[INDEX_YEAR]);
            xmldateformat.append(dateset[INDEX_MONTH]);
            xmldateformat.append(dateset[INDEX_DATE]);

            String[] timeset = dateset[INDEX_TIME].split(":");
            xmldateformat.append("T");
            xmldateformat.append(timeset[INDEX_TIME_HOUR]);
            xmldateformat.append(timeset[INDEX_TIME_MINUTE]);
            xmldateformat.append("00Z");

            if (D) Log.d(TAG, name +"=" + files[i].getName()+ " size="
                            + files[i].length() + " modified=" + date.toString()
                            + " dateformat to send=" + xmldateformat.toString());

            result.append("<" + name + "=\"" + files[i].getName()+ "\"" + " size=\"" +
                files[i].length() + "\"" + " user-perm=\"" + permission + "\"" +
                " modified=\"" + xmldateformat.toString()  + "\"" + "/>");
            result.append('\r');
            result.append('\n');
        }
        result.append("</folder-listing>");
        result.append('\r');
        result.append('\n');
        if (D) Log.d(TAG, "sendFolderListingXml -");
        return pushBytes(op, result.toString());
    }
    /* Close the output stream */
    public static boolean closeStream(final OutputStream out, final Operation op) {
        boolean returnvalue = true;
        if (D) Log.d(TAG, "closeoutStream +");
        try {
            if (out != null) {
                out.close();
            }
        } catch (IOException e) {
            Log.e(TAG, "outputStream close failed" + e.toString());
            returnvalue = false;
        }
        try {
            if (op != null) {
                op.close();
            }
        } catch (IOException e) {
            Log.e(TAG, "operation close failed" + e.toString());
            returnvalue = false;
        }

        if (D) Log.d(TAG, "closeoutStream -");
        return returnvalue;
    }
    /* Close the input stream */
    public static boolean closeStream(final InputStream in, final Operation op) {
        boolean returnvalue = true;
        if (D) Log.d(TAG, "closeinStream +");

        try {
            if (in != null) {
                in.close();
            }
        } catch (IOException e) {
            Log.e(TAG, "inputStream close failed" + e.toString());
            returnvalue = false;
        }
        try {
            if (op != null) {
                op.close();
            }
        } catch (IOException e) {
            Log.e(TAG, "operation close failed" + e.toString());
            returnvalue = false;
        }

        if (D) Log.d(TAG, "closeinStream -");

        return returnvalue;
    }

    private void acquireFtpWakeLock() {
        if (mWakeLock == null) {
            PowerManager pm = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
            mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,
                "StartingObexFtpTransaction");
            mWakeLock.setReferenceCounted(false);
            mWakeLock.acquire();
            if (V) Log.v(TAG, "mWakeLock acquired");
        }
        else
        {
            Log.e(TAG, "mWakeLock already acquired");
        }
    }

    private void releaseFtpWakeLock() {
        if (mWakeLock != null) {
            if (mWakeLock.isHeld()) {
                mWakeLock.release();
                if (V) Log.v(TAG, "mWakeLock released");
            } else {
                if (V) Log.v(TAG, "mWakeLock already released");
            }
            mWakeLock = null;
        }
    }
};
