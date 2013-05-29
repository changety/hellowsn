package ty.change.wsn.util;

public class Constants {

	public static int NUM = 5;

	public static final String NO = "N";
	public static final String YES = "Y";

	public static final String ENDDEVICE = "EndDevice";

	public static final String ROUTER = "Router";

	public static final String COORDINATOR = "Coordinator";

	public static final String ACTIVITY = "ACTIVITY";

	public static final String SLEEP = "SLEEP";

	public static final String COORDINATOR_NETADDRESS = "0000";

	public static String WSN_PATH = "wsn/";
	// 虚拟机调试
	 public static String WSN_IP = "http://10.0.2.2:8080/";
	// 手机调试,无限局域网
//	public static String WSN_IP = "http://192.168.1.110:8080/";

	public static String XMPP_IP = "";

	// // 手机调试,无限局域网
	// public static String WSN_IP = "http://192.168.3.163:8080/";

	// 花生壳调试
	// public static String WSN_IP = "http://hellowsn.vicp.cc/";

	public static String SMART_HOME_TV = "电视";

	public static final String NODE_Temperature = "温度";
	public static final String NODE_Enagery = "能量";

	// 查询服务器与串口通信的状态
	public final static int COMMAND_CHECK_COMM_STATE = 0;
	// 打开服务器与串口通信
	public final static int COMMAND_COMM_OPEN = 1;
	// 关闭服务器与串口通信
	public final static int COMMAND_COMM_CLOSE = 2;
	// 开灯
	public final static int COMMAND_TURN_ON_LIGHT = 0;
	// 关灯
	public final static int COMMAND_TURN_OFF_LIGHT = 1;
	// 温度设置
	public final static int COMMAND_TEMP_SETTING = 2;
	// 电压设置
	public final static int COMMAND_VOLTAGE_SETTING = 3;
	// 周期设置
	public final static int COMMAND_CYCLE_SETTING = 4;
	// 不报告
	public final static int COMMAND_NODE_REPORT_SLEEP = 5;

	public final static int COMMAND_AUTHORITY = 100;
	// 查询服务器与串口通信的状态
	public final static int SERIAL_COMM_STATE = 200;

	public final static int ENDDEVICE_SELECTED = 0;

	public final static int ROUTER_SELECTED = 1;

	public final static int COORDINATOR_SELECTED = 2;

	public static final String COMM_OPEN = "开启通信";
	public static final String COMM_CLOSE = "关闭通信";
	public static final String COMM_OPENED = "通信已经开启";
	public static final String COMM_CLOSED = "通信已经终端";

	public static final String COMMAND_WSN_STRUCT_REFRESH = "WSN_NT_REFRESH";

	public static final String NODE_REPORT_STATE_SLEEP = "-1";

	public static final String NODE_REPORT_CYCLE_SLOW = "60";
	public static final String NODE_REPORT_CYCLE_NORMAL = "30";
	public static final String NODE_REPORT_CYCLE_FREQUENCE = "10";

	public static final String NODE_REPORT_STATE_UNABLE = "UNABLE";

	public static final String LIGHT_STATE_ON = "ON";

	public static final String LIGHT_STATE_OFF = "OFF";

	public static final String RESULT = "RESULT";
	public static final String REASON = "REASON";

	// 推送有关

	public static final String SHARED_PREFERENCE_NAME = "client_preferences";

	// PREFERENCE KEYS

	public static final String CALLBACK_ACTIVITY_PACKAGE_NAME = "CALLBACK_ACTIVITY_PACKAGE_NAME";

	public static final String CALLBACK_ACTIVITY_CLASS_NAME = "CALLBACK_ACTIVITY_CLASS_NAME";

	public static final String API_KEY = "API_KEY";

	public static final String VERSION = "VERSION";

	public static final String XMPP_HOST = "XMPP_HOST";

	public static final String XMPP_PORT = "XMPP_PORT";

	public static final String XMPP_USERNAME = "XMPP_USERNAME";

	public static final String XMPP_PASSWORD = "XMPP_PASSWORD";

	public static final String XMPP_EMAIL = "XMPP_EMAIL";

	public static final String XMPP_AUTHORITY = "XMPP_AUTHORITY";

	public static final String XMPP_LASTLOGIN_TIME = "XMPP_LASTLOGIN_TIME";

	public static final String XMPP_REGISTER_TIME = "XMPP_REGISTER_TIME";

	// public static final String USER_KEY = "USER_KEY";

	public static final String DEVICE_ID = "DEVICE_ID";

	public static final String EMULATOR_DEVICE_ID = "EMULATOR_DEVICE_ID";

	public static final String NOTIFICATION_ICON = "NOTIFICATION_ICON";

	public static final String SETTINGS_NOTIFICATION_ENABLED = "SETTINGS_NOTIFICATION_ENABLED";

	public static final String SETTINGS_SOUND_ENABLED = "SETTINGS_SOUND_ENABLED";

	public static final String SETTINGS_VIBRATE_ENABLED = "SETTINGS_VIBRATE_ENABLED";

	public static final String SETTINGS_TOAST_ENABLED = "SETTINGS_TOAST_ENABLED";

	// NOTIFICATION FIELDS

	public static final String NOTIFICATION_ID = "NOTIFICATION_ID";

	public static final String NOTIFICATION_API_KEY = "NOTIFICATION_API_KEY";

	public static final String NOTIFICATION_TITLE = "NOTIFICATION_TITLE";

	public static final String NOTIFICATION_MESSAGE = "NOTIFICATION_MESSAGE";

	public static final String NOTIFICATION_URI = "NOTIFICATION_URI";

	public static final String NOTIFICATION_TIME = "NOTIFICATION_TIME";

	// INTENT ACTIONS

	public static final String ACTION_SHOW_NOTIFICATION = "org.androidpn.client.SHOW_NOTIFICATION";

	public static final String ACTION_NOTIFICATION_CLICKED = "org.androidpn.client.NOTIFICATION_CLICKED";

	public static final String ACTION_NOTIFICATION_CLEARED = "org.androidpn.client.NOTIFICATION_CLEARED";

	public static final String Connect_SUCCESS = "Connect_SUCCESS";

	public static final String ZIGBEE_NODE = "ZIGBEE_NODE";
	public static final String SELECTED_NODE = "SELECTED_NODE";
	public static final String WSN_NOTI = "WSN_NOTI";

}
