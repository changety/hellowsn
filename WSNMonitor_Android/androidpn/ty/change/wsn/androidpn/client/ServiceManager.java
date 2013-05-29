/*
 * Copyright (C) 2010 Moduad Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package ty.change.wsn.androidpn.client;

import java.util.Properties;

import ty.change.wsn.activity.NotificationSettingsActivity;
import ty.change.wsn.util.Constants;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;

/** 
 * This class is to manage the notificatin service and to load the configuration.
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public final class ServiceManager {

    private static final String LOGTAG = LogUtil
            .makeLogTag(ServiceManager.class);

    private Context context;

    private SharedPreferences sharedPrefs;

    private Properties props;

    private String version = "0.5.0";

    private String apiKey;

    private String xmppHost;

    private String xmppPort;

    private String callbackActivityPackageName;

    private String callbackActivityClassName;

    public ServiceManager(Context context) {
        this.context = context;

        if (context instanceof Activity) {
            Log.i(LOGTAG, "Callback Activity...");
            Activity callbackActivity = (Activity) context;
            //org.androidpn.demoapp
            callbackActivityPackageName = callbackActivity.getPackageName();
            System.out.println(callbackActivityPackageName);
            //org.androidpn.demoapp.DemoAppActivity
            callbackActivityClassName = callbackActivity.getClass().getName();
            System.out.println(callbackActivityClassName);
        }

        props = loadProperties();
        apiKey = props.getProperty("apiKey", "1234567890");
        xmppHost = props.getProperty("xmppHost", Constants.XMPP_IP);
        xmppPort = props.getProperty("xmppPort", "5222");
        Log.i(LOGTAG, "apiKey=" + apiKey);
        Log.i(LOGTAG, "xmppHost=" + xmppHost);
        Log.i(LOGTAG, "xmppPort=" + xmppPort);

        sharedPrefs = context.getSharedPreferences(
                Constants.SHARED_PREFERENCE_NAME, Context.MODE_PRIVATE);
        Editor editor = sharedPrefs.edit();
        editor.putString(Constants.API_KEY, apiKey);
        editor.putString(Constants.VERSION, version);
        editor.putString(Constants.XMPP_HOST, xmppHost);
        editor.putInt(Constants.XMPP_PORT, Integer.parseInt(xmppPort));
        editor.putString(Constants.CALLBACK_ACTIVITY_PACKAGE_NAME,
                callbackActivityPackageName);
        editor.putString(Constants.CALLBACK_ACTIVITY_CLASS_NAME,
                callbackActivityClassName);
        editor.commit();
        
        Log.i(LOGTAG, "sharedPrefs=" + sharedPrefs.toString());
    }

    public void startService() {
        Thread serviceThread = new Thread(new Runnable() {
            @Override
            public void run() {
                Intent intent = NotificationService.getIntent();
                context.startService(intent);
            }
        });
        serviceThread.start();
    }

    public void stopService() {
        Intent intent = NotificationService.getIntent();
        context.stopService(intent);
    }

     
    /**
     * 读取androidpn.properties文件
     */
    private Properties loadProperties() {
 
        Properties props = new Properties();
        try {
            int id = context.getResources().getIdentifier("androidpn", "raw",
                    context.getPackageName());
            props.load(context.getResources().openRawResource(id));
        } catch (Exception e) {
            Log.e(LOGTAG, "Could not find the properties file.", e);
            // e.printStackTrace();
        }
        return props;
    }

    
    public void setNotificationIcon(int iconId) {
        Editor editor = sharedPrefs.edit();
        editor.putInt(Constants.NOTIFICATION_ICON, iconId);
        editor.commit();
    }


    public static void viewNotificationSettings(Context context) {
        Intent intent = new Intent().setClass(context,
                NotificationSettingsActivity.class);
        context.startActivity(intent);
    }

}
