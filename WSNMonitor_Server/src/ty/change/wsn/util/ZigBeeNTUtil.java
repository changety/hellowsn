package ty.change.wsn.util;

import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import ty.change.wsn.entity.Coordinator;
import ty.change.wsn.entity.EndDevice;
import ty.change.wsn.entity.Router;
import ty.change.wsn.entity.ZigBeeNode;
import ty.change.wsn.service.WSNService;

public class ZigBeeNTUtil
{
	public static ZigBeeNTUtil instance = null;
	// 用于维护网络拓扑结构的Map,IEEEAddress作key,该IEEEAddress的ZigbeeNode作为Value
//	public static Map<String, ZigBeeNode> zigbeeNTMap = new HashMap<String, ZigBeeNode>();
	private  Map<String, ZigBeeNode> zigbeeNTMap = new ConcurrentHashMap<String, ZigBeeNode>();
	ZigBeeNode zbNodeInMapSelf = null;
	ZigBeeNode zbNodeInDBSelf = null;
	ZigBeeNode zbNodeInMapParent = null;
	ZigBeeNode zbNodeInDBParent = null;
	List brothersList = null;
	// Spring容器中拿wsnDaoService
	WSNService wsnDaoService = (WSNService) ApplicationContextHelper
			.getBean("wsnService");

	public  Map<String, ZigBeeNode> getZigbeeNTMap()
	{
		return zigbeeNTMap;
	}
	public  void setZigbeeNTMap(Map<String, ZigBeeNode> zigbeeNTMap)
	{
		this.zigbeeNTMap = zigbeeNTMap;
	}
	@SuppressWarnings("unchecked")
	private ZigBeeNTUtil()
	{	
		List<ZigBeeNode> list = (List<ZigBeeNode>)wsnDaoService.findAllZigbeeNTData();
		for(ZigBeeNode node :list)
		{
			zigbeeNTMap.clear();
			zigbeeNTMap.put(node.getIeeeAddress(),node);
		}
	}
	public synchronized static ZigBeeNTUtil getInstance()
	{
		if (instance == null)
		{
			instance = new ZigBeeNTUtil();
		}
		return instance;
	}
	/**
	 * 维护网络拓扑结构的主方法
	 * @param zbNode
	 */
	private synchronized void NTMaintain(ZigBeeNode zbNode)
	{
		if (zbNode.getType().equals(Constant.TYPE_COOR))
		{
			//先设为Y，后面根据其他节点的加入再更改
			 zbNode.setIsLeaf("Y");
//			 协调器直接加到MAP中
			zigbeeNTMap.put(zbNode.getIeeeAddress(), zbNode);
			wsnDaoService.addZigbeeNTData(zbNode);
		}
		else 
		{	
			//sensor
			if (zbNode.getType().equals(Constant.TYPE_ENDDEVICE))
			{
				// 直接设置为子节点
				zbNode.setIsLeaf("Y");
			}
			//router
			else 
			{
				//当路由器节点的时候， 先看看 表里 有没有父节点是该路由器节点的记录， 如果>=1 则说明有。
				brothersList = wsnDaoService.findBrothersByPIEEEAddress(zbNode.getIeeeAddress());
				if(brothersList.size() >= 1)
				{
					zbNode.setIsLeaf("N");
				}
				else
				{
					zbNode.setIsLeaf("Y");
				}
			}
	// 查找是否有该IEEE地址的wsnStructData 存在在这个map里，如果有的话返回这个，没有的话返回NULL
		zbNodeInMapSelf = zigbeeNTMap.get(zbNode.getIeeeAddress());
			if (null == zbNodeInMapSelf)
			{				
				// 查询该节点父节点的信息,并且根据需要修改  "from ZigBeeNode z where z.IEEEAddress=?"
				zbNodeInDBParent = wsnDaoService.findParentByPIEEEAddress(zbNode.getPieeeAddress());
				if (zbNodeInDBParent.getIsLeaf().equals("Y"))
				{
					// 将该父节点设置为非叶子节点
					zbNodeInDBParent.setIsLeaf("N");
					// 并将更新过的父节点 放进Map
					zigbeeNTMap.put(
							zbNodeInDBParent.getIeeeAddress(), zbNodeInDBParent);
					// 并父节点更改数据库信息
					wsnDaoService.updateZigbeeNTData(zbNodeInDBParent);
				}
				//将节点放到map里
				zigbeeNTMap.put(zbNode.getIeeeAddress(), zbNode);
				//将子节点放进去
				wsnDaoService.addZigbeeNTData(zbNode);
			}
			//如果map里已经有该节点信息
			else
			{
				//再判断节点的 父节点IEEE信息  父节点Pnet信息有没有改变过,
				if ((!zbNodeInMapSelf.getPieeeAddress().equals(
						zbNode.getPieeeAddress()))
						|| (!zbNodeInMapSelf.getpNetAddress().equals(
								zbNode.getpNetAddress())))
				{
					//如果有一个改变过的话,看该父节点下面是否还有其他子节点
					brothersList = wsnDaoService
							.findBrothersByPIEEEAddress(zbNodeInMapSelf
									.getPieeeAddress());
					//brothersList.size() == 1 意味着只是自己
					if (brothersList.size() == 1)
					{
						zbNodeInDBParent = wsnDaoService
								.findParentByPIEEEAddress(zbNodeInMapSelf
										.getPieeeAddress());
						zbNodeInDBParent.setIsLeaf("Y");
						zigbeeNTMap.put(zbNodeInDBParent.getIeeeAddress(),
								zbNodeInDBParent);
						wsnDaoService.updateZigbeeNTData(zbNodeInDBParent);
					}
					// 改变过的话， 就替换原来的
					zigbeeNTMap.put(zbNode.getIeeeAddress(),
							zbNode);
					// 并且修改数据库里自己的信息
					wsnDaoService.updateZigbeeNTData(zbNode);
					// 修改父节点的信息
				}
				else if (!zbNodeInMapSelf.getNetAddress().equals(
						zbNode.getNetAddress()))
				{
					// 改变的过的话， 就替换原来的
					zigbeeNTMap.put(zbNode.getIeeeAddress(),
							zbNode);
					// 并且修改数据库里自己的信息
					wsnDaoService.updateZigbeeNTData(zbNode);
					// 修改父节点的信息
				}
			}
		}
	}
	/**
	 * 将SensorData RouterData CoorData转化为ZigbeeNodeNT
	 * @param zbNode
	 */
	public synchronized void transformToZigbeeNode(Object zbNode)
	{
		ZigBeeNode zbData = new ZigBeeNode();
		if (zbNode instanceof EndDevice)
		{
			zbData.setIeeeAddress(((EndDevice) zbNode).getIeeeAddress());
			zbData.setPieeeAddress(((EndDevice) zbNode).getPieeeAddress());
			zbData.setNetAddress(((EndDevice) zbNode).getNetAddress());
			zbData.setpNetAddress(((EndDevice) zbNode).getpNetAddress());
			zbData.setType(((EndDevice) zbNode).getType());
			zbData.setpType(((EndDevice) zbNode).getpType());
			zbData.setUpdateTime(((EndDevice) zbNode).getUpdateTime());
//			zbData.setTempValue("25");
			zbData.setReportCycle("30");
			zbData.setVolValue("3.3");
			zbData.setDeviceState("off");

		}
		else if (zbNode instanceof Router)
		{
			zbData.setIeeeAddress(((Router) zbNode).getIeeeAddress());
			zbData.setPieeeAddress(((Router) zbNode).getPieeeAddress());
			zbData.setNetAddress(((Router) zbNode).getNetAddress());
			zbData.setpNetAddress(((Router) zbNode).getpNetAddress());
			zbData.setType(((Router) zbNode).getType());
			zbData.setpType(((Router) zbNode).getpType());
			zbData.setUpdateTime(((Router) zbNode).getUpdateTime());
			zbData.setVolValue("3.3");
			zbData.setReportCycle("60");
		}
		else
		{
			zbData.setIeeeAddress(((Coordinator) zbNode).getIeeeAddress());
			zbData.setPieeeAddress(((Coordinator) zbNode).getPieeeAddress());
			zbData.setNetAddress(((Coordinator) zbNode).getNetAddress());
			zbData.setpNetAddress(((Coordinator) zbNode).getpNetAddress());
			zbData.setType(((Coordinator) zbNode).getType());
			zbData.setpType(((Coordinator) zbNode).getpType());
			zbData.setUpdateTime(((Coordinator) zbNode).getJoinTime());
		}
		NTMaintain(zbData);
	}
	
	
	public void updateZigBeeNode(ZigBeeNode zigBeeNode){
		zigbeeNTMap.put(zigBeeNode.getIeeeAddress(),zigBeeNode);
	}
}