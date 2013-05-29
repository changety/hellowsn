package ty.change.wsn.androidpn.client;

import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import ty.change.wsn.dao.WSNDatabaseHelper;
import ty.change.wsn.util.Constants;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.IBinder;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.j256.ormlite.android.apptools.OrmLiteBaseService;
import com.j256.ormlite.support.ConnectionSource;

/**
 * Service that continues to run in background and respond to the push 
 * notification events from the server. This should be registered as service
 * in AndroidManifest.xml. 
 * 
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class NotificationService extends OrmLiteBaseService<WSNDatabaseHelper> {

    private static final String LOGTAG = LogUtil
            .makeLogTag(NotificationService.class);

    public static final String SERVICE_NAME = "ty.change.wsn.androidpn.client.NotificationService";

    private TelephonyManager telephonyManager;

    //    private WifiManager wifiManager;
    //
    //    private ConnectivityManager connectivityManager;

    private BroadcastReceiver notificationReceiver;

    private BroadcastReceiver connectivityReceiver;

    private PhoneStateListener phoneStateListener;

    private ExecutorService executorService;

    private TaskSubmitter taskSubmitter;

    private TaskTracker taskTracker;

    private XmppManager xmppManager;

    private SharedPreferences sharedPrefs;

    private String deviceId;
    private ConnectionSource conn;
    
    public NotificationService() {
    	System.out.println("NotificationService init");
        notificationReceiver = new NotificationReceiver();
        connectivityReceiver = new ConnectivityReceiver(this);
        phoneStateListener = new PhoneStateChangeListener(this);
        executorService = Executors.newSingleThreadExecutor();
        taskSubmitter = new TaskSubmitter(this);
        taskTracker = new TaskTracker(this);
    }

    @Override
    public void onCreate() {
        Log.d(LOGTAG, "onCreate()...");
        super.onCreate();
		conn = getConnectionSource();
        //得到设备
        telephonyManager = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        // wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        // connectivityManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);

        sharedPrefs = getSharedPreferences(Constants.SHARED_PREFERENCE_NAME,
                Context.MODE_PRIVATE);

        // Get deviceId
        deviceId = telephonyManager.getDeviceId();
        // Log.d(LOGTAG, "deviceId=" + deviceId);
        Editor editor = sharedPrefs.edit();
        editor.putString(Constants.DEVICE_ID, deviceId);
        editor.commit();

        // If running on an emulator       如果运行在仿真器
        if (deviceId == null || deviceId.trim().length() == 0
                || deviceId.matches("0+")) {
            if (sharedPrefs.contains("EMULATOR_DEVICE_ID")) {
                deviceId = sharedPrefs.getString(Constants.EMULATOR_DEVICE_ID,
                        "");
            } else {
                deviceId = (new StringBuilder("EMU")).append(
                        (new Random(System.currentTimeMillis())).nextLong())
                        .toString();
                editor.putString(Constants.EMULATOR_DEVICE_ID, deviceId);
                editor.commit();
            }
        }
        Log.d(LOGTAG, "deviceId=" + deviceId);

        xmppManager = new XmppManager(this,conn);

        taskSubmitter.submit(new Runnable() {
            public void run() {
                NotificationService.this.start();
            }
        });
    }

    @Override
    public void onStart(Intent intent, int startId) {
        Log.d(LOGTAG, "onStart()...");
    }

    @Override
    public void onDestroy() {
        Log.d(LOGTAG, "onDestroy()...");
        stop();
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.d(LOGTAG, "onBind()...");
        return null;
    }

    @Override
    public void onRebind(Intent intent) {
        Log.d(LOGTAG, "onRebind()...");
    }

    @Override
    public boolean onUnbind(Intent intent) {
        Log.d(LOGTAG, "onUnbind()...");
        return true;
    }
    //由此拿到配置文件中的 NotificationService
    public static Intent getIntent() {
        return new Intent(SERVICE_NAME);
    }

    public ExecutorService getExecutorService() {
        return executorService;
    }

    public TaskSubmitter getTaskSubmitter() {
        return taskSubmitter;
    }

    public TaskTracker getTaskTracker() {
        return taskTracker;
    }

    public XmppManager getXmppManager() {
        return xmppManager;
    }

    public SharedPreferences getSharedPreferences() {
        return sharedPrefs;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void connect() {
        Log.d(LOGTAG, "connect()...");
        taskSubmitter.submit(new Runnable() {
            public void run() {
                NotificationService.this.getXmppManager().connect();
            }
        });
    }

    public void disconnect() {
        Log.d(LOGTAG, "disconnect()...");
        taskSubmitter.submit(new Runnable() {
            public void run() {
                NotificationService.this.getXmppManager().disconnect();
            }
        });
    }

    private void registerNotificationReceiver() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(Constants.ACTION_SHOW_NOTIFICATION);
        filter.addAction(Constants.ACTION_NOTIFICATION_CLICKED);
        filter.addAction(Constants.ACTION_NOTIFICATION_CLEARED);
        registerReceiver(notificationReceiver, filter);
    }

    private void unregisterNotificationReceiver() {
        unregisterReceiver(notificationReceiver);
    }

    private void registerConnectivityReceiver() {
        Log.d(LOGTAG, "registerConnectivityReceiver()...");
        telephonyManager.listen(phoneStateListener,
                PhoneStateListener.LISTEN_DATA_CONNECTION_STATE);
        IntentFilter filter = new IntentFilter();
        // filter.addAction(android.net.wifi.WifiManager.NETWORK_STATE_CHANGED_ACTION);
        filter.addAction(android.net.ConnectivityManager.CONNECTIVITY_ACTION);
        registerReceiver(connectivityReceiver, filter);
    }

    private void unregisterConnectivityReceiver() {
        Log.d(LOGTAG, "unregisterConnectivityReceiver()...");
        telephonyManager.listen(phoneStateListener,
                PhoneStateListener.LISTEN_NONE);
        unregisterReceiver(connectivityReceiver);
    }

    private void start() {
        Log.d(LOGTAG, "start()...");
        registerNotificationReceiver();
        registerConnectivityReceiver();
        // Intent intent = getIntent();
        // startService(intent);
        xmppManager.connect();
    }

    private void stop() {
        Log.d(LOGTAG, "stop()...");
        unregisterNotificationReceiver();
        unregisterConnectivityReceiver();
        xmppManager.disconnect();
        executorService.shutdown();
    }

    /**
     * Class for summiting a new runnable task.
     */
    public class TaskSubmitter {

        final NotificationService notificationService;

        public TaskSubmitter(NotificationService notificationService) {
            this.notificationService = notificationService;
        }

        @SuppressWarnings("rawtypes")
		public Future submit(Runnable task) {
            Future result = null;
            if (!notificationService.getExecutorService().isTerminated()
                    && !notificationService.getExecutorService().isShutdown()
                    && task != null) {
            	//submit有返回值，而execute没有  
                result = notificationService.getExecutorService().submit(task);
            }
            return result;
        }

    }

    /**
     * Class for monitoring the running task count.
     */
    public class TaskTracker {

        final NotificationService notificationService;

        public int count;

        public TaskTracker(NotificationService notificationService) {
            this.notificationService = notificationService;
            this.count = 0;
        }

        public void increase() {
            synchronized (notificationService.getTaskTracker()) {
                notificationService.getTaskTracker().count++;
                Log.d(LOGTAG, "Incremented task count to " + count);
            }
        }

        public void decrease() {
            synchronized (notificationService.getTaskTracker()) {
                notificationService.getTaskTracker().count--;
                Log.d(LOGTAG, "Decremented task count to " + count);
            }
        }

    }

}
