package ty.change.wsn.service.impl;

import java.util.List;

import ty.change.wsn.dao.WSNDao;
import ty.change.wsn.entity.Coordinator;
import ty.change.wsn.entity.Router;
import ty.change.wsn.entity.EndDevice;
import ty.change.wsn.entity.ZigBeeNode;
import ty.change.wsn.service.WSNService;

public class WSNServiceImpl implements WSNService
{
	private WSNDao wsnDao;

	public WSNDao getWsnDao()
	{
		return wsnDao;
	}

	public void setWsnDao(WSNDao wsnDao)
	{
		this.wsnDao = wsnDao;
	}

	@Override
	public EndDevice findSensorData(String ieeeAddress)
	{
		return this.wsnDao.findSensorData(ieeeAddress);
	}

	@Override
	public List findDetailData(String type, String ieeeAddress, int time,
			int id, int page)
	{
		return this.wsnDao.findDetailData(type, ieeeAddress, time, id,
				page);
	}

	@Override
	public void addSensorData(EndDevice sensorData)
	{
		this.wsnDao.addSensorData(sensorData);
	}

	@Override
	public void addRouterData(Router routerData)
	{
		this.wsnDao.addRouterData(routerData);
	}

	@Override
	public void addCoordinatorData(Coordinator coordinatorData)
	{
		this.wsnDao.addCoordinatorData(coordinatorData);
	}

	@Override
	public void addZigbeeNTData(ZigBeeNode zigbeeNode)
	{
		this.wsnDao.addZigbeeNTData(zigbeeNode);
	}

	@Override
	public void updateZigbeeNTData(ZigBeeNode zigbeeNode)
	{
		this.wsnDao.updateZigbeeNTData(zigbeeNode);
	}

	// @Override
	// public void updateZigbeeNTDataIschild(String ieeeAddress, String ischild)
	// {
	// this.wsnDao.updateZigbeeNTData(ieeeAddress, ischild);
	// }

	@Override
	public List findAllZigbeeNTData()
	{
		return wsnDao.findAllZigBeeNTData();
	}

	@Override
	public List findSensorDatas(String ieeeAddress, int begin, int end)
	{
		return wsnDao.findSensorDatas(ieeeAddress, begin, end);
	}

	@Override
	public ZigBeeNode findParentByPIEEEAddress(String pieeeAddress)
	{
		return wsnDao.findParentByPIEEEAddress(pieeeAddress);
	}

	@Override
	public List findBrothersByPIEEEAddress(String pieeeAddress)
	{
		return wsnDao.findBrothersByPIEEEAddress(pieeeAddress);
	}

	@Override
	public ZigBeeNode findSelfByIEEEAddress(String ieeeAddress)
	{
		return wsnDao.findSelfByIEEEAddress(ieeeAddress);
	}

	@Override
	public ZigBeeNode findParentByPNetAddress(String pNetAddress)
	{
		return wsnDao.findParentByPNetAddress(pNetAddress);
	}
}
