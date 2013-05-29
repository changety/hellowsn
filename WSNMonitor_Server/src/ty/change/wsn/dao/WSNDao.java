package ty.change.wsn.dao;

import java.util.List;

import ty.change.wsn.entity.Coordinator;
import ty.change.wsn.entity.Router;
import ty.change.wsn.entity.EndDevice;
import ty.change.wsn.entity.User;
import ty.change.wsn.entity.ZigBeeNode;

public interface WSNDao
{

	public EndDevice findSensorData(String IEEEAddress);

	public void addSensorData(EndDevice sensorData);

	public void addRouterData(Router routerData);

	public void addCoordinatorData(Coordinator coordinatorData);

	public void addZigbeeNTData(ZigBeeNode zigbeeNode);

	public void updateZigbeeNTData(ZigBeeNode zigbeeNode);

	public List findSensorDatas(String IEEEAddress, int begin, int end);

	public ZigBeeNode findParentByPIEEEAddress(String pIEEEAddress);

	public List findBrothersByPIEEEAddress(String pIEEEAddress);

	public ZigBeeNode findSelfByIEEEAddress(String IEEEAddress);

	public ZigBeeNode findParentByPNetAddress(String pNetAddress);

	public List findAllZigBeeNTData();

	public List findDetailData(String type, String iEEEAddress, int time,
			int id, int page);

}
