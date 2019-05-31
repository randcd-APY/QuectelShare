/*********************************************************************
*
* Copyright (c) 2015, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
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
*
************************************************************************/

package org.codeaurora.bluetooth.btlogsave;

import android.content.Context;
import android.widget.Toast;
import android.util.Log;
import android.os.Handler;
import android.widget.TextView;
import android.widget.LinearLayout;
import android.view.View;
import android.view.ViewGroup;
import android.content.Intent;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.lang.ProcessBuilder;
import java.lang.Exception;
import java.util.Scanner;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.lang.reflect.Field;
import java.lang.Runnable;

public class Utils{

    static {
        if (Main.mHasCriticalPermissions)
            new File("/sdcard/BtLogKit").mkdir();
        else
            Log.d(Main.TAG,"Don't have sdcard permission.");
    }

    private static Process process_logcat_start,process_logcat_clear,process_logcat_move;
    private static String filesDir;
    private static String timestamp;
    private static Thread thread;
    private static Context cxt;
    private static boolean logcat_clear = false;
    private static Handler handler;
    private static boolean running = true;
    private static Runnable runnable = new Runnable(){
        private long size = 0;
        public void run(){
            Log.d(Main.TAG,"Started thread");
            try{
                while(running){
                    File log_file = new File(cxt.getFilesDir().toString()+"/log.txt");
                    Thread.sleep(5000);
                    size = log_file.length()/(1024*1024);
                    if(size >= Main.log_file_size){
                        size=0;
                        String cmd_clear[] = {"/system/bin/sh", "-c", "logcat -c"};
                        process_logcat_clear = Runtime.getRuntime().exec(cmd_clear);
                        process_logcat_clear.waitFor();
                        process_logcat_clear.destroy();
                        logcat_clear = true;
                        stopLogging(cxt);
                        startLogging(cxt);
                        logcat_clear = false;
                        break;
                    }
                }
            }catch(IOException e){
                Log.e(Main.TAG,e.toString());
            }catch(InterruptedException e){
                Log.e(Main.TAG,e.toString());
            }
        }
    };

    public static void startLogging(Context context){
        Log.d(Main.TAG,"started logging");
        cxt = context;
        try{
            filesDir = context.getFilesDir().getAbsolutePath();
            if(!logcat_clear){
                String cmd_clear[] = {"/system/bin/sh", "-c", "logcat -c"};
                process_logcat_clear = Runtime.getRuntime().exec(cmd_clear);
                process_logcat_clear.waitFor();
                process_logcat_clear.destroy();
            }
            running = true;
            String cmd_start[] = {"/system/bin/sh", "-c", "logcat -v threadtime -f "+filesDir+"/log.txt"};
            process_logcat_start = Runtime.getRuntime().exec(cmd_start);
            Thread thread = new Thread(runnable);
            thread.start();

        }catch(IOException e){
            Log.e(Main.TAG,e.toString());
        }catch(InterruptedException e){
            Log.e(Main.TAG,e.toString());
        }
    }

    public static void stopLogging(Context context){
        Log.d(Main.TAG,"stopped logging");
        try{
            if(thread!=null){
                thread.interrupt();
            }
            filesDir = context.getFilesDir().getAbsolutePath();
            if(process_logcat_start!=null && !logcat_clear){
                process_logcat_start.destroy();
                showToast(context,"Log stored in /sdcard/BTLogKit/",Toast.LENGTH_LONG);
                running=false;
            }
            Calendar cal = Calendar.getInstance();
            DateFormat df = new SimpleDateFormat("MM-dd-HH-mm-ss-SSS");
            cal.setTimeInMillis(System.currentTimeMillis());
            timestamp = df.format(cal.getTime());
            process_logcat_move = Runtime.getRuntime().exec("rm -f /sdcard/BTLogKit/"+getLastModifiedFile());
            process_logcat_move.waitFor();
            process_logcat_move.destroy();
            process_logcat_move = Runtime.getRuntime().exec("mv "+filesDir+"/log.txt /sdcard/BTLogKit/"+timestamp+"_log.txt");
            process_logcat_move.waitFor();
            process_logcat_move.destroy();

            //updateUI(context);
        }catch (FileNotFoundException e) {
            Log.e(Main.TAG,e.toString());
        }catch(IOException e){
            Log.e(Main.TAG,e.toString());
        }catch(InterruptedException e){
            Log.e(Main.TAG,e.toString());
        }
    }

    public static String getLastModifiedFile(){
        File saveDir = new File("/sdcard/BTLogKit");
        File last_updated;
        long least_value = 0;
        File[] files = saveDir.listFiles();
        if (files != null)
        {
            if(files.length<4){
                return "nothing";
            }
            last_updated = files[0];
            least_value = last_updated.lastModified();
            for(File file : files){
                if(file.lastModified()<least_value){
                    last_updated = file;
                    least_value = file.lastModified();
                }
            }
            return last_updated.getName();
        }
        else
            return "nothing";
    }

    public static void showToast(final Context context,final String toast,final int length)
    {
        handler = new Handler(context.getMainLooper());

        runOnUiThread(new Runnable() {
            public void run()
            {
                Toast.makeText(context, toast, length).show();
            }

        });
    }

    public static void updateUI(final Context context) {

        handler = new Handler(context.getMainLooper());

        runOnUiThread(new Runnable() {
            public void run()
            {
                TextView tv;
                tv = new TextView(context);
                File file_new = new File(filesDir+"/"+timestamp+"_log.txt");
                long size_new = file_new.length();
                String value_in_MB = String.format("%.3f", ((float)size_new)/(1024.0*1024.0)) ;
                tv.setText(file_new.getName() + "-" + value_in_MB + "MB");
                tv.setTextColor(context.getResources().getColor(R.color.black));
                tv.setTextSize(context.getResources().getDimension(R.dimen.textsize_large));
                LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT ,LinearLayout.LayoutParams.WRAP_CONTENT);
                params.setMargins(10, 10, 10, 10);
                tv.setLayoutParams(params);
                Main.lv.addView(tv);
                View ruler = new View(context);
                ruler.setBackgroundColor(context.getResources().getColor(R.color.white));
                Main.lv.addView(ruler, new ViewGroup.LayoutParams( ViewGroup.LayoutParams.FILL_PARENT, 2));
            }
        });
    }

    private static void runOnUiThread(Runnable r) {
        handler.post(r);
    }

}
