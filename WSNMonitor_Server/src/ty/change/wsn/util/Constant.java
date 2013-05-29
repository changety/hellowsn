package ty.change.wsn.util;

public class Constant
{
	
	//无线传感器网络常量
	public final static String  IS_CHILD_YES= "Y";

	public final static String IS_CHILD_NO = "N";
	
	// 传感器种类
	public final static String TYPE_ENDDEVICE = "EndDevice";

	public final static String TYPE_ROUTER = "Router";

	public final static String TYPE_COOR = "Coordinator";
	
	public static String COOR_ADD = "0000";

	// 传感器节点 每次发来数据包的合法 头
	public final static String SENSOR_DATA_HEAD = "FE";
	
	// 路由器节点每次发来数据包的合法 头
	public final static String ROUTER_DATA_HEAD = "FD";
	
	// 协调器每次发来数据包的合法 头
	public final static String COOR_DATA_HEAD = "FC";
	
	// 节点 每次发来数据包的合法长度
	public final static int NODE_DATA_LENGTH = 46;
	
	// 路由器 每次发来数据包的合法长度
	public final static int ROUTER_DATA_LENGTH = 46;
	
	// 协调器初始化后第一次主动按键发送给服务平台的信息
	public final static int COOR_DATA_LENGTH = 26;
	
	//头消息
	public final static int DATA_HEAD_BEGIN = 0;
	
	public final static int DATA_HEAD_END = 2;
	
	// 传感数据netAddress数据起始位
	public final static int SENSORDATA_NETADDRESS_BEGIN = 8;
	// 传感数据netAddress数据结束位
	public final static int SENSORDATA_NETADDRESS_END = 12;

	// 传感数据tempreture数据起始位
	public final static int SENSORDATA_TEMPRETURE_BEGIN = 20;
	// 传感数据tempreture数据结束位
	public final static int SENSORDATA_TEMPRETURE_END = 22;
	
	// 传感数据voltage数据起始位
	public final static int SENSORDATA_VOLTAGE_BEGIN = 22;
	// 传感数据voltage数据结束位
	public final static int SENSORDATA_VOLTAGE_END = 24;
	
	// 传感数据pNetAddress数据起始位
	public final static int SENSORDATA_PNETADDRESS_BEGIN = 24;
	// 传感数据pNetAddress数据结束位
	public final static int SENSORDATA_PNETADDRESS_END = 28;
	
	// 传感数据IEEEAddress数据起始位
	public final static int SENSORDATA_IEEEADDRESS_BEGIN = 28;
	// 传感数据pNetAddress数据结束位
	public final static int SENSORDATA_IEEEADDRESS_END = 44;
	
	
	
	
	
	// 路由数据netAddress数据起始位
	public final static int ROUTERDATA_NETADDRESS_BEGIN = 8;
	// 路由数据netAddress数据结束位
	public final static int ROUTERDATA_NETADDRESS_END = 12;

	// 路由数据tempreture数据起始位
	public final static int ROUTERDATA_TEMPRETURE_BEGIN = 20;
	// 路由数据tempreture数据结束位
	public final static int ROUTERDATA_TEMPRETURE_END = 22;
	
	// 路由数据voltage数据起始位
	public final static int ROUTERDATA_VOLTAGE_BEGIN = 22;
	// 路由数据voltage数据结束位
	public final static int ROUTERDATA_VOLTAGE_END = 24;
	
	// 路由数据pNetAddress数据起始位
	public final static int ROUTERDATA_PNETADDRESS_BEGIN = 24;
	// 路由数据pNetAddress数据结束位
	public final static int ROUTERDATA_PNETADDRESS_END = 28;
	
	// 路由数据IEEEAddress数据起始位
	public final static int ROUTERDATA_IEEEADDRESS_BEGIN = 28;
	// 路由数据pNetAddress数据结束位
	public final static int ROUTERDATA_IEEEADDRESS_END = 44;
	
	
	// 协调器数据netAddress数据起始位
	public final static int COOR_NETADDRESS_BEGIN = 4;
	// 协调器数据netAddress数据结束位
	public final static int COOR_NETADDRESS_END = 8;

	// 协调器数据tempreture数据起始位
	public final static int COOR_IEEEADDRESS_BEGIN = 8;
	// 协调器数据tempreture数据结束位
	public final static int COOR_IEEEADDRESS_END = 24;
	
	public final static String COOR_PANID = "1234";
	
	
	
	
	
	
	
	//以下是交互常量

	public final static String USER_NOT_EXIST = "用户不存在";

	public final static String PASSWORD_WRONG = "密码错误";

	public final static String USER_RIGHT = "登入成功";

	public final static String USER_EXIST = "该用户名已存在，请重新输入";

	public final static String REGISTER_SUCCESS = "注册成功";

	// 用户admin权限
	public final static String USER_ADMIN = "admin";

	// 普通用户权限
	public final static String USER_NORMAL = "normal";

	public final static int AUTHORITY = 100;
	
	public final static String AUTHORITY_REASON = "无此权限";
	
	
	// 查询服务器与串口通信的状态
	public final static int SERIAL_COMM_STATE = 200;
	
	
	// 查询服务器与串口通信的状态
	public final static int COMMAND_CHECK_COMM_STATE = 0;
	// 打开服务器与串口通信
	public final static int COMMAND_COMM_OPEN = 1;
	// 关闭服务器与串口通信
	public final static int COMMAND_COMM_CLOSE = 2;
	
	//开灯
	public final static int COMMAND_TURN_ON_LIGHT = 0;
	//关灯
	public final static int COMMAND_TURN_OFF_LIGHT = 1;
	//温度设置
	public final static int COMMAND_TEMP_SETTING = 2;
	//电压设置
	public final static int COMMAND_VOLTAGE_SETTING = 3;
	//周期设置
	public final static int COMMAND_CYCLE_SETTING = 4;
	//不报告
//	public final static int COMMAND_NODE_REPORT_SLEEP = 5;
	

	
	// 返回服务器与串口通信的开启状态
	public static final String COMM_STATE_OPENED = "通信已经开启";
	// 返回服务器与串口通信的关闭状态
	public static final String COMM_STATE_CLOSED = "通信已经关闭";
	
	public static final String LIGHT_STATE_ON = "ON";

	public static final String LIGHT_STATE_OFF= "OFF";
	
	public static final String NODE_REPORT_STATE_ENABLE = "ENABLE";

	public static final String NODE_REPORT_STATE_UNABLE = "UNABLE";
	
	public static final String COMMAND_TEMP_SETTING_SUCCESS = "TEMP_SETTING_SUCCESS";

	public static final String COMMAND_TEMP_SETTING_FAIL= "TEMP_SETTING_FAIL";
	
	public static final String COMMAND_VOLTAGE_SETTING_SUCCESS = "VOLTAGE_SETTING_SUCCESS";

	public static final String COMMAND_VOLTAGE_SETTING_FAIL= "VOLTAGE_SETTING_FAIL";
	
	public static final String COMMAND_REPORT_SETTING_SUCCESS = "REPORT_SETTING_SUCCESS";

	public static final String COMMAND_REPORT_SETTING_FAIL= "REPORT_SETTING_FAIL";

	public static final String COMMAND_WSN_STRUCT_REFRESH = "WSN_NT_REFRESH";
	
	public static final String COMMAND_FIRST = "FE";
	
	public static final String COMMAND_LENGTH = "04";
	
	public static final String COMMAND_HEAD = "0102";
	
	public static final String COMMAND_LIGHT = "DA";
	
	public static final String COMMAND_LIGHT_OFF = "FA";
	
	public static final String COMMAND_LIGHT_ON = "FB";
	
	
	public static final String COMMAND_TEMP = "DB";
	
	public static final String COMMAND_VOLTAGE = "DC";
	
	public static final String COMMAND_REPORT = "DD";
	
	public static final String COMMAND_CYCLE = "DE";
	

	
	public static final String COMMAND_REPORT_UNENABLE = "FD";
	
	
	public static final String COMMAND_END = "FD";
	
	
	
					

	public static final String NODE_1 = "No.1";// 对应下面6号

	public static final String NODE_2 = "No.2";// 对应下面7号

	public static final String NODE_3 = "No.3";// 对应下面3号

	public static final String NODE_4 = "No.4";// 对应下面1号

	public static final String NODE_5 = "No.5";// 对应下面5号

	public static final String NODE_IEEE0 = "00124B00010BD8C2";
	public static final String NODE_IEEE1 = "00124B00010BD8CE";
	public static final String NODE_IEEE2 = "00124B00010BD8C4";
	public static final String NODE_IEEE3 = "00124B00010BD8C9";
	public static final String NODE_IEEE4 = "00124B00010BD8CF";
	public static final String NODE_IEEE5 = "00124B00010BD8CB";
	// 飞比的三个
	public static final String NODE_IEEE6 = "00124B00017AEA7E";
	public static final String NODE_IEEE7 = "00124B00017AE30B";
	public static final String NODE_IEEE8 = "00124B00017AE184";

	public static final String NODE_Temperature = "温度";
	public static final String NODE_Humidity = "湿度";
	public static final String NODE_Light = "光强";
	public static final String NODE_Energy = "能量";
	public static final String NODE_CO2 = "CO2";
	public static final String NODE_CO = "CO";
	public static final String NODE_Pressure = "水压";
	public static final String NODE_Speed = "水流";

	public static final String NO = "N";
	public static final String YES = "Y";

	public static final String SERIALAPP = "serialDemo";

	public static final String TEMP_SETTING = "TEMP_SETTING";

	public static final String VOLTAGE_SETTING = "VOLTAGE_SETTING";

	public static final String RESULT = "RESULT";
	public static final String REASON = "REASON";

}
