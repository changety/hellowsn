package ty.change.wsn.dataparser;

import ty.change.wsn.entity.Coordinator;
import ty.change.wsn.entity.Router;
import ty.change.wsn.entity.EndDevice;
import ty.change.wsn.util.Constant;
import ty.change.wsn.util.ZigBeeNTUtil;

/**
 * @author Ruibron 完成接受对接受到的wsndata的校验，如果数据格式正确，则调用DataParseUtil类的方法进行解析拼装。
 */
public class DataCheckUtil
{
	public static DataCheckUtil instance = null;

	private DataCheckUtil()
	{

	}

	public synchronized static DataCheckUtil getInstance()
	{
		if (instance == null)
		{
			instance = new DataCheckUtil();
		}
		return instance;
	}

	EndDevice sensorData = null;
	Router routerData = null;
	Coordinator coordinatorData = null;

	/**
	 * @param wsnData
	 *            检查wsnData属于哪个网络设备发送来的
	 */
	public void commDataCheck(String wsnData)
	{
		// 判断Node校验头
		if (Constant.SENSOR_DATA_HEAD.equals(wsnData.substring(
				Constant.DATA_HEAD_BEGIN, Constant.DATA_HEAD_END)))
		{
			// 如果是NODE的FE，再判断sensorData长度
			if (wsnData.length() == Constant.NODE_DATA_LENGTH)// 判断字符串长度是否合法
			{	
				// 将wsnData解析成sensorData
				sensorData = DataParseUtil.getInstance().assemblySensorData(
						wsnData);
				ZigBeeNTUtil.getInstance().transformToZigbeeNode(sensorData);
			}
		}
		// 判断Router校验头
		else if (Constant.ROUTER_DATA_HEAD.equals(wsnData.substring(
				Constant.DATA_HEAD_BEGIN, Constant.DATA_HEAD_END)))
		{
			// 如果是Router的FD，再判断routerData的长度
			if (wsnData.length() == Constant.ROUTER_DATA_LENGTH)// 判断字符串长度是否合法
			{	
				// 将wsnData解析成routerData
				routerData = DataParseUtil.getInstance().assemblyRouterData(
						wsnData);
				ZigBeeNTUtil.getInstance().transformToZigbeeNode(routerData);
			}
		}
		// 判断Coor校验头
		else if (Constant.COOR_DATA_HEAD.equals(wsnData.substring(
				Constant.DATA_HEAD_BEGIN, Constant.DATA_HEAD_END)))
		{
			// 如果是Coor的FC，再判断CoorData的长度
			if (wsnData.length() == Constant.COOR_DATA_LENGTH)// 判断字符串长度是否合法
			{	
				// 将wsnData解析成CoorData
				coordinatorData = DataParseUtil.getInstance()
						.assemblyCoordinatorData(wsnData);
				ZigBeeNTUtil.getInstance().transformToZigbeeNode(coordinatorData);
			}
		}
	}
}