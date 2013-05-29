package ty.change.wsn.dataparser;

import java.util.Date;

import org.androidpn.server.util.Config;
import org.androidpn.server.xmpp.push.NotificationManager;

import ty.change.wsn.entity.Coordinator;
import ty.change.wsn.entity.EndDevice;
import ty.change.wsn.entity.Router;
import ty.change.wsn.entity.ZigBeeNode;
import ty.change.wsn.service.WSNService;
import ty.change.wsn.util.ApplicationContextHelper;
import ty.change.wsn.util.Constant;
import ty.change.wsn.util.ZigBeeNTUtil;

public class DataParseUtil
{
	public static DataParseUtil instance = null;

	private NotificationManager notificationManager;

	private DataParseUtil()
	{
		notificationManager = new NotificationManager();
	}

	public synchronized static DataParseUtil getInstance()
	{
		if (instance == null)
		{
			instance = new DataParseUtil();
		}
		return instance;
	}

	WSNService wsnDaoService = (WSNService) ApplicationContextHelper
			.getBean("wsnService");
	ZigBeeNTUtil wsnStructUtil = ZigBeeNTUtil.getInstance();
	ZigBeeNode zbParentNodeInDB = null;
	EndDevice sensorData = null;
	Router routerData = null;
	Coordinator coordinatorData = null;
	boolean flag = true;
	String temp;
	String voltage;
	String pIEEEAddress;

	/**
	 * @param tempRawData
	 *            解析sensorData里的温度值 ,16进制转化为十进制
	 * @return
	 */
	public String parseTemperature16To10(String tempRawData)
	{
		return Integer.parseInt(tempRawData, 16) + "";
	}

	/**
	 * @param voltageRawData
	 *            解析sensorData里的电压值 ,16进制转化为十进制
	 * @return
	 */
	public String parseVoltage16To10(String voltageRawData)
	{
		return Integer.parseInt(voltageRawData, 16) * 0.1 + "";
	}

	/**
	 * @param tempSettingData
	 *            ,10进制转化为16进制
	 * @return
	 */
	public String parseTemperature10To16(String tempSettingData)
	{
		Integer temp = new Integer(tempSettingData);
		return Integer.toHexString(temp);
	}

	/**
	 * @param voltageSettingData
	 *            ,10进制转化为16进制
	 * @return
	 */
	public String parseVoltage10To16(String voltageSettingData)
	{
		Float volatge = new Float(voltageSettingData);

		return Integer.toHexString((int) (volatge * 10));

	}

	/**
	 * @param commTxData
	 *            将要发送的字符串指令转为byte数组,以便串口发送给协调器。
	 * @return
	 */
	public byte[] parseCommTxData(String commTxData)
	{
		int commTxDataLength = commTxData.length() / 2;
		int newData = 0;
		Integer integer;
		byte[] theCommTxData = new byte[commTxDataLength];
		byte eachCommTxData;
		for (int i = 0; i < commTxDataLength; i++)
		{
			newData = Integer.parseInt(
					commTxData.substring(2 * i, 2 * (i + 1)), 16);
			integer = new Integer(newData);
			eachCommTxData = integer.byteValue();
			if (newData < 127)
			{
				eachCommTxData &= 0x7F;
			}
			theCommTxData[i] = eachCommTxData;
		}
		return theCommTxData;
	}

	/**
	 * @param wsnData
	 *            将DataCheckUtil检查过来的合法的wsnData 解析出来
	 *            然后拼装成SensorData，并且持久化到sensorData数据库表里
	 * @return
	 */
	public EndDevice assemblySensorData(String wsnData)
	{
		sensorData = new EndDevice();
		// 参见Constant类
		sensorData.setNetAddress(wsnData.substring(
				Constant.SENSORDATA_NETADDRESS_BEGIN,
				Constant.SENSORDATA_NETADDRESS_END));
		temp = parseTemperature16To10(wsnData.substring(
				Constant.SENSORDATA_TEMPRETURE_BEGIN,
				Constant.SENSORDATA_TEMPRETURE_END));
		voltage = parseVoltage16To10(wsnData.substring(
				Constant.SENSORDATA_VOLTAGE_BEGIN,
				Constant.SENSORDATA_VOLTAGE_END));
		sensorData.setTemp(temp);
		sensorData.setVoltage(voltage);
		sensorData.setType(Constant.TYPE_ENDDEVICE);
		if (Constant.COOR_ADD.equals(wsnData.substring(
				Constant.SENSORDATA_PNETADDRESS_BEGIN,
				Constant.SENSORDATA_PNETADDRESS_END)))
		{
			sensorData.setpType(Constant.TYPE_COOR);
		}
		else
		{
			sensorData.setpType(Constant.TYPE_ROUTER);
		}
		sensorData.setpNetAddress(wsnData.substring(
				Constant.SENSORDATA_PNETADDRESS_BEGIN,
				Constant.SENSORDATA_PNETADDRESS_END));
		sensorData.setIeeeAddress(wsnData.substring(
				Constant.SENSORDATA_IEEEADDRESS_BEGIN,
				Constant.SENSORDATA_IEEEADDRESS_END));
		// 根据sensorData的父节点网络地址，从数据库里，查出父节点 对象,从而得到pIEEE地址
		zbParentNodeInDB = wsnDaoService.findParentByPNetAddress(sensorData
				.getpNetAddress());
		sensorData.setPieeeAddress(zbParentNodeInDB.getIeeeAddress());
		sensorData.setUpdateTime(new Date());
		// 持久化到sensorData数据库表里
		wsnDaoService.addSensorData(sensorData);
		pushCheck(sensorData);
		return sensorData;
	}

	/**
	 * @param wsnData
	 *            将DataCheckUtil检查过来的合法的wsnData 解析出来
	 *            然后拼装成RouterData，并且持久化到RouterData数据库表里
	 * @return
	 */
	public Router assemblyRouterData(String wsnData)
	{
		routerData = new Router();
		routerData.setNetAddress(wsnData.substring(
				Constant.ROUTERDATA_NETADDRESS_BEGIN,
				Constant.ROUTERDATA_NETADDRESS_END));
		// 根据不同的父节点的网络地址 设置不同的pType类型
		if (Constant.COOR_ADD.equals(wsnData.substring(
				Constant.ROUTERDATA_PNETADDRESS_BEGIN,
				Constant.ROUTERDATA_PNETADDRESS_END)))
		{
			routerData.setpType(Constant.TYPE_COOR);
		}
		else
		{
			routerData.setpType(Constant.TYPE_ROUTER);
		}
		routerData.setType(Constant.TYPE_ROUTER);
		routerData.setpNetAddress(wsnData.substring(
				Constant.ROUTERDATA_PNETADDRESS_BEGIN,
				Constant.ROUTERDATA_PNETADDRESS_END));
		routerData.setIeeeAddress(wsnData.substring(
				Constant.ROUTERDATA_IEEEADDRESS_BEGIN,
				Constant.ROUTERDATA_IEEEADDRESS_END));
		// 根据routerData的父节点网络地址，从数据库里，查出父节点 对象,从而得到pIEEE地址
		zbParentNodeInDB = wsnDaoService.findParentByPNetAddress(routerData
				.getpNetAddress());
		// 将父节点的IEEE地址set进去
		routerData.setPieeeAddress(zbParentNodeInDB.getIeeeAddress());
		routerData.setUpdateTime(new Date());
		wsnDaoService.addRouterData(routerData);
		return routerData;
	}

	/**
	 * @param wsnData
	 *            将DataCheckUtil检查过来的合法的wsnData 解析出来 然后拼装成CoordinatorData，
	 *            并且持久化到CoordinatorData数据库表里
	 * @return
	 */
	public Coordinator assemblyCoordinatorData(String wsnData)
	{
		coordinatorData = new Coordinator();
		coordinatorData.setNetAddress(wsnData.substring(
				Constant.COOR_NETADDRESS_BEGIN, Constant.COOR_NETADDRESS_END));
		coordinatorData.setType(Constant.TYPE_COOR);
		coordinatorData
				.setIeeeAddress(wsnData.substring(
						Constant.COOR_IEEEADDRESS_BEGIN,
						Constant.COOR_IEEEADDRESS_END));
		coordinatorData.setJoinTime(new Date());
		coordinatorData.setPanid(Constant.COOR_PANID);
		wsnDaoService.addCoordinatorData(coordinatorData);
		return coordinatorData;
	}

	private synchronized void pushCheck(ZigBeeNode node)
	{
		ZigBeeNode nodeInMap = wsnStructUtil.getZigbeeNTMap().get(
				node.getIeeeAddress());
		if (nodeInMap != null)
		{
			if (node instanceof EndDevice)
			{
				Integer temp = new Integer(((EndDevice) node).getTemp());
				if (temp.intValue() >= new Integer(nodeInMap.getTempValue())
						.intValue())
				{
					String message = "节点：" + node.getIeeeAddress()
							+ ",当前实时温度为:" + temp.intValue() + ",温度超过设定的:"
							+ nodeInMap.getTempValue() + "!请注意!";

					notificationManager.sendBroadcast(
							Config.getString("apiKey", ""), "WSN监控平台", message,
							"");
				}
			}
		}
	}

}
