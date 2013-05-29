package ty.change.wsn.service;

import java.util.List;

import ty.change.wsn.entity.Coordinator;
import ty.change.wsn.entity.Router;
import ty.change.wsn.entity.EndDevice;
import ty.change.wsn.entity.ZigBeeNode;

public interface WSNService
{	
	public EndDevice findSensorData(String ieeeAddress);

	public void addSensorData(EndDevice sensorData);

	public void addRouterData(Router routerData);

	public void addCoordinatorData(Coordinator coordinatorData);

	public void addZigbeeNTData(ZigBeeNode zigbeeNode);

	public void updateZigbeeNTData(ZigBeeNode zigbeeNode);

	public List findAllZigbeeNTData();

	public List findSensorDatas(String ieeeAddress, int begin, int end);

	public ZigBeeNode findParentByPIEEEAddress(String pieeeAddress);

	public List findBrothersByPIEEEAddress(String pieeeAddress);

	public ZigBeeNode findSelfByIEEEAddress(String ieeeAddress);

	public ZigBeeNode findParentByPNetAddress(String pNetAddress);

	public List findDetailData(String type ,String ieeeAddress, int time, int id, int page);
	

}
