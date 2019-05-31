/*
 * Copyright (c) 2011 The Linux Foundation. All rights reserved.
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

import android.os.Message;
import android.os.Handler;
import android.os.StatFs;
import android.os.Environment;
import android.util.Log;
import android.os.Bundle;
import android.webkit.MimeTypeMap;

import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.OutputStream;
import java.io.InputStream;
import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;

import javax.obex.ResponseCodes;
import javax.obex.Operation;

public class FileUtils {

    private static final String TAG = "FileUtils";

    private static final boolean D = BluetoothFtpService.DEBUG;

    private static final boolean V = BluetoothFtpService.VERBOSE;

    public static boolean interruptFileCopy = false;

    /**
    * deleteDirectory
    *
    * Called when a PUT request is received to delete a non empty folder
    *
    * @param mCallback handler for sending message
    * @param dir provides the handle to the directory to be deleted
    * @return a TRUE if operation was succesful or false otherwise
    */
    public static final boolean deleteDirectory(Handler mCallback,File dir) {
        if (D) Log.d(TAG, "deleteDirectory() +");
        if(dir.exists()) {
            File [] files = dir.listFiles();
            if (files == null) {
                Log.e(TAG, "error in listing directory ");
                return false;
            }
            for(int i = 0; i < files.length;i++) {
                if(files[i].isDirectory()) {
                    deleteDirectory(mCallback,files[i]);
                    if (D) Log.d(TAG,"Dir Delete =" + files[i].getName());
                } else {
                    if (D) Log.d(TAG,"File Delete =" + files[i].getName());
                    files[i].delete();
                    sendMessage(mCallback,BluetoothFtpService.MSG_FILE_DELETED,files[i].getAbsolutePath());
                }
            }
        }

        if (D) Log.d(TAG, "deleteDirectory() -");
        return( dir.delete() );
    }

    /**
    * copyFolders
    *
    * Called when a Copy action is to be performed from a source
    * folder to destination folder
    *
    * @param mCallback handler for sending message
    * @param src File handle to source directory
    * @param dest File handle to destination directory
    * @return a ResponseCodes.OBEX_HTTP_OK if operation was succesful
    *         or ResponseCodes.OBEX_HTTP_INTERNAL_ERROR otherwise
    */

    public static final int copyFolders(Handler mCallback,File src, File dest) {
         Log.d(TAG,"copyFolders src "+src+"dest "+dest);
         int ret = 0;
         dest.mkdir();
         File [] files = src.listFiles();
         if (files == null) {
             Log.e(TAG, "error in listing directory");
             return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
         }
         for(int i = 0; i < files.length; i++) {
             if (D) Log.d(TAG,"Files =" + files[i]);
             if(files[i].isDirectory()) {
                 File recdest = new File(dest.getAbsolutePath() + "/" + files[i].getName());
                 ret = copyFolders(mCallback,files[i],recdest);
                 if(ret != ResponseCodes.OBEX_HTTP_OK)
                     return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
             } else if(files[i].isFile()) {
                 File recdest = new File(dest.getAbsolutePath() + "/" + files[i].getName());
                 ret = copyFile(mCallback,files[i],recdest);
                 if(ret != ResponseCodes.OBEX_HTTP_OK)
                     return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
             }
        }
        return ResponseCodes.OBEX_HTTP_OK;
    }
    /**
    * copyFile
    *
    * Called when a Copy action is to be performed from a source
    * file to destination file
    *
    * @param mCallback handler for sending message
    * @param src File handle to source file
    * @param dest File handle to destination file
    * @return a ResponseCodes.OBEX_HTTP_OK if operation was succesful
    *         or ResponseCodes.OBEX_HTTP_INTERNAL_ERROR otherwise
    */

    public static final int copyFile(Handler mCallback,File src, File dest) {
        if (D) Log.d(TAG,"copyFile src "+ src +"dest "+dest);
        if (dest == null)
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        FileInputStream reader = null;
        FileOutputStream writer = null;
        interruptFileCopy = false;

        try {
            reader = new FileInputStream(src);
            writer = new FileOutputStream(dest);
        } catch(FileNotFoundException e) {
            Log.e(TAG,"copyFile file not found "+ e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        } finally {
            if (null != reader && null == writer) {
                try {
                    reader.close();
                } catch (IOException e) {
                    Log.e(TAG, "copyFile close stream failed"+ e.toString());
                    return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                }
            }
        }

        BufferedInputStream ins = new BufferedInputStream(reader, 0x40000);
        BufferedOutputStream os = new BufferedOutputStream(writer, 0x40000);
        byte[] buff = new byte[0x40000];
        long position = 0;
        int readLength = 0;
        long timestamp = System.currentTimeMillis();
        try {
            if(V) Log.v(TAG,"position = "+position + "src.filelength = "+src.length());
            while ((position != src.length()) && !interruptFileCopy) {
                if (BluetoothFtpObexServer.sIsAborted) {
                    BluetoothFtpObexServer.sIsAborted = false;
                    break;
                }

                readLength = ins.read(buff, 0, 0x40000);
                if (D) Log.d(TAG,"Read File");
                os.write(buff, 0, readLength);
                position += readLength;
                if (V) {
                    Log.v(TAG, "Copying file position = " + position
                       + " readLength " + readLength);
                }
            }
        } catch (IOException e) {
            Log.e(TAG,"copyFile "+ e.toString());
            if (D) Log.d(TAG, "File Copy failed");
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        if (ins != null) {
            try {
                ins.close();
                os.close();
            } catch (IOException e) {
                Log.e(TAG,"input/output stream close" + e.toString());
                if (D) Log.d(TAG, "Error when closing stream after send");
                return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            }
        }

        if (position != src.length()) {
            Log.i(TAG, "Copy is aborted ");
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        } else {
            Log.i(TAG,"copyFile completed in "+
                (System.currentTimeMillis() - timestamp) + "ms");
        }

        sendMessage(mCallback,BluetoothFtpService.MSG_FILE_RECEIVED,dest.getAbsolutePath());
        return ResponseCodes.OBEX_HTTP_OK;
    }

    /** check whether path is legal */
    public static final boolean doesPathExist(final String str) {
        if (D) Log.d(TAG,"doesPathExist + = " + str );
        File searchfolder = new File(str);
        if(searchfolder.exists())
            return true;
        return false;
    }

    /** Check the Mounted State of External Storage */
    public static final boolean checkMountedState() {
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            return true;
        } else {
            if (D) Log.d(TAG,"SD card Media not mounted");
            return false;
        }
    }

    /** Check the Available Space on External Storage */
    public static final boolean checkAvailableSpace(long filelength) {
        StatFs stat = new StatFs(BluetoothFtpObexServer.ROOT_FOLDER_PATH);
        if (D) Log.d(TAG,"stat.getAvailableBlocks() "+ stat.getAvailableBlocks());
        if (D) Log.d(TAG,"stat.getBlockSize() ="+ stat.getBlockSize());
        long availabledisksize = stat.getBlockSize() * ((long)stat.getAvailableBlocks() - 4);
        if (D) Log.d(TAG,"Disk size = " + availabledisksize + "File length = " + filelength);
        if (stat.getBlockSize() * ((long)stat.getAvailableBlocks() - 4) <  filelength) {
            if (D) Log.d(TAG,"Not Enough Space hence can't receive the file");
            return false;
        } else {
            return true;
        }
    }

    /* Send message to FTP Service */
    public static final void sendMessage(Handler mCallback,int msgtype, String name) {
        /* Send a message to the FTP service to initiate a Media scanner connection */
        if (mCallback != null) {
            Message msg = Message.obtain(mCallback);
            msg.what = msgtype;
            Bundle args = new Bundle();
            if(V) Log.e(TAG,"sendMessage "+name);
            String path = "/" + "mnt"+ name;
            String mimeType = null;

            /* first we look for Mimetype in Android map */
            String extension = null, type = null;
            int dotIndex = name.lastIndexOf(".");
            if (dotIndex < 0) {
                if (D) Log.d(TAG, "There is no file extension");
                return;
            } else {
                extension = name.substring(dotIndex + 1).toLowerCase();
                MimeTypeMap map = MimeTypeMap.getSingleton();
                type = map.getMimeTypeFromExtension(extension);
                if (V) Log.v(TAG, "Mimetype guessed from extension " + extension + " is " + type);
                if (type != null) {
                    mimeType = type;
                }
            }
            if (mimeType != null) {
                mimeType = mimeType.toLowerCase();
            } else {
                //Mimetype is unknown hence we dont need a media scan
                return;
            }

            args.putString("filepath", name);
            args.putString("mimetype", mimeType);
            msg.obj = args;
            msg.sendToTarget();
            if (V) Log.v(TAG,"msg" + msgtype  + "sent out.");
        }
    }

    /* Send custom message to FTP Service */
    public static final void sendCustomMessage(Handler mCallback,int msgtype, String[] files,
        String[] types) {
        /* Send a message to the FTP service to initiate a Media scanner connection */
        if (mCallback != null) {
            Message msg = Message.obtain(mCallback);
            msg.what = msgtype;
            Bundle args = new Bundle();
            Log.e(TAG,"sendCustomMessage ");

            args.putStringArray("filepaths", files);
            args.putStringArray("mimetypes", types);
            msg.obj = args;
            msg.sendToTarget();
            if (V) Log.v(TAG,"msg" + msgtype  + "sent out.");
        }
    }


};
