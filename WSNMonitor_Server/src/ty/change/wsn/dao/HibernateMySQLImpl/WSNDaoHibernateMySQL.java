package ty.change.wsn.dao.HibernateMySQLImpl;

import java.sql.SQLException;
import java.util.List;

import org.hibernate.HibernateException;
import org.hibernate.Session;
import org.springframework.orm.hibernate3.HibernateCallback;
import org.springframework.orm.hibernate3.support.HibernateDaoSupport;

import ty.change.wsn.dao.WSNDao;
import ty.change.wsn.entity.Coordinator;
import ty.change.wsn.entity.EndDevice;
import ty.change.wsn.entity.Router;
import ty.change.wsn.entity.ZigBeeNode;
import ty.change.wsn.util.Constant;

public class WSNDaoHibernateMySQL extends HibernateDaoSupport implements WSNDao
{

	/**
	 * 根据SensorData的IEEEAddress查询最新[max(id)]的那条SensorData信息
	 */
	@Override
	public EndDevice findSensorData(String ieeeAddress)
	{
		final String sql = "select * from t_enddevice "
				+ "where (id = (select max(id) from t_enddevice where ieeeAddress= '"
				+ ieeeAddress + "' ))";
		List sensorList = getHibernateTemplate().executeFind(
				new HibernateCallback()
				{
					public Object doInHibernate(Session session)
							throws HibernateException, SQLException
					{
						return session.createSQLQuery(sql)
								.addEntity(EndDevice.class).list();
					}
				});
		// 必有
		return (EndDevice) sensorList.get(0);
	}

	/**
	 * 根据SensorData的IEEEAddress查询最新[max(id)]的那条SensorData信息
	 */
	@Override
	public List findDetailData(final String type ,String ieeeAddress, int time, int id,
			int page)
	{	
		String sql = "";
		String table = "";
		if(type.equals(Constant.TYPE_ENDDEVICE)){
			table = "t_enddevice";
		}else if(type.equals(Constant.TYPE_ROUTER)){
			table = "t_router";
		}else{
			table = "t_coordinator";
		}
		if (time == 0)
		{
			sql = "SELECT * FROM " + table + " WHERE ieeeAddress='"
					+ ieeeAddress
					+ "' and id <= (select max(id) from " + table + " where ieeeAddress='"
					+ ieeeAddress + "')" + " order by id DESC LIMIT " + page;
		}
		else
		{
			sql = "SELECT * FROM " + table + " WHERE ieeeAddress='" 
					+ ieeeAddress
					+ "' and id < " + id + " order by id DESC LIMIT " + page;
		}
		final String finalSQL = sql;
		System.out.println("finalSQL-------" + finalSQL);
		List sensorList = getHibernateTemplate().executeFind(
				new HibernateCallback()
				{
					public Object doInHibernate(Session session)
							throws HibernateException, SQLException
					{
						if(type.equals(Constant.TYPE_ENDDEVICE)){
							return session.createSQLQuery(finalSQL)
							.addEntity(EndDevice.class).list();
						}else if(type.equals(Constant.TYPE_ROUTER)){
							return session.createSQLQuery(finalSQL)
							.addEntity(Router.class).list();
						}else{
							return session.createSQLQuery(finalSQL)
							.addEntity(Coordinator.class).list();
						}
						
					}
				});
		// 必有
		return sensorList;
	}

	/**
	 * 添加一条sensorData的记录
	 */
	@Override
	public void addSensorData(EndDevice sensorData)
	{
		this.getHibernateTemplate().save(sensorData);
	}

	/**
	 * 添加一条routerData的记录
	 */
	@Override
	public void addRouterData(Router routerData)
	{
		this.getHibernateTemplate().save(routerData);
	}

	/**
	 * 添加一条coordinatorData的记录
	 */
	@Override
	public void addCoordinatorData(Coordinator coordinatorData)
	{
		this.getHibernateTemplate().save(coordinatorData);
	}

	/**
	 * 添加一条zigbeeNodeNT的记录
	 */
	@Override
	public void addZigbeeNTData(ZigBeeNode ZigBeeNode)
	{
		this.getHibernateTemplate().save(ZigBeeNode);
	}

	/**
	 * 根据子节点的pIEEEAddress查询出父节点的信息
	 */
	@Override
	public ZigBeeNode findParentByPIEEEAddress(String pieeeAddress)
	{
		List wsnNTList = this.getHibernateTemplate().find(
				"from ZigBeeNode z where z.ieeeAddress=?", pieeeAddress);
		return (ZigBeeNode) wsnNTList.get(0);
	}

	/**
	 * 根据节点的pIEEEAddress查询出 --该节点下的父节点是否有其他子节点
	 */
	@Override
	public List findBrothersByPIEEEAddress(String pieeeAddress)
	{
		List wsnNTList = this.getHibernateTemplate().find(
				"from ZigBeeNode z where z.pieeeAddress=?", pieeeAddress);
		return wsnNTList;
	}

	/**
	 * 根据节点的IEEEAddress的信息 查询 出该节点自己
	 */
	@Override
	public ZigBeeNode findSelfByIEEEAddress(String ieeeAddress)
	{
		List wsnNTList = this.getHibernateTemplate().find(
				"from ZigBeeNode z where z.ieeeAddress=?", ieeeAddress);
		return (ZigBeeNode) wsnNTList.get(0);
	}

	/**
	 * 根据节点的pNetAddress的信息 查询 出该节点的父节点,从而查出父节点的IEEEAddress
	 */
	@Override
	public ZigBeeNode findParentByPNetAddress(String pNetAddress)
	{
		// HQL 严格区别大小写
		List wsnNTList = this.getHibernateTemplate().find(
				"from ZigBeeNode z where z.netAddress=?", pNetAddress);
		return (ZigBeeNode) wsnNTList.get(0);
	}

	/**
	 * 更新一个节点的节点信息
	 */
	@Override
	public void updateZigbeeNTData(ZigBeeNode ZigBeeNode)
	{
		this.getHibernateTemplate().update(ZigBeeNode);
	}

	@Override
	public List findSensorDatas(String ieeeAddress, int begin, int end)
	{
		// final StringBuffer sbSql = new StringBuffer();
		// sbSql.append(
		// "select * from t_enddevice where ieeeAddress=" + ieeeAddress)
		// .append("order by id desc limit").append(begin + "," + end);
		final String sqlString = "select * from t_enddevice where ieeeAddress = '"
				+ ieeeAddress + "' order by id desc limit " + begin + "," + end;
		List sl = this.getHibernateTemplate().executeFind(
				new HibernateCallback()
				{
					public Object doInHibernate(Session session)
							throws HibernateException, SQLException
					{
						return session.createSQLQuery(sqlString)
								.addEntity(EndDevice.class).list();
					}
				});
		return sl;
	}

	@Override
	public List findAllZigBeeNTData()
	{
		List zigBeeNTlist = this.getHibernateTemplate().find("from ZigBeeNode");
		return zigBeeNTlist;
	}

}
