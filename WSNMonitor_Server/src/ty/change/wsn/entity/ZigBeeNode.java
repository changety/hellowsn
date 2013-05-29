package ty.change.wsn.entity;

import java.io.Serializable;
import java.util.Date;

public class ZigBeeNode implements Serializable
{
	protected String ieeeAddress;
	protected String pieeeAddress;
	protected String netAddress;


	protected String pNetAddress;
	protected String type;
	protected String pType;
	protected Date joinTime;
	protected Date updateTime;

	private String tempValue = "30";
	private String reportCycle;
	private String volValue;
	private String isLeaf;
	private String deviceState;
	
	public String getIeeeAddress()
	{
		return ieeeAddress;
	}
	public void setIeeeAddress(String ieeeAddress)
	{
		this.ieeeAddress = ieeeAddress;
	}
	public String getPieeeAddress()
	{
		return pieeeAddress;
	}
	public void setPieeeAddress(String pieeeAddress)
	{
		this.pieeeAddress = pieeeAddress;
	}
	public String getNetAddress()
	{
		return netAddress;
	}
	public void setNetAddress(String netAddress)
	{
		this.netAddress = netAddress;
	}
	public String getpNetAddress()
	{
		return pNetAddress;
	}
	public void setpNetAddress(String pNetAddress)
	{
		this.pNetAddress = pNetAddress;
	}
	public String getType()
	{
		return type;
	}
	public void setType(String type)
	{
		this.type = type;
	}
	public String getpType()
	{
		return pType;
	}
	public void setpType(String pType)
	{
		this.pType = pType;
	}
	public Date getJoinTime()
	{
		return joinTime;
	}
	public void setJoinTime(Date joinTime)
	{
		this.joinTime = joinTime;
	}
	public Date getUpdateTime()
	{
		return updateTime;
	}
	public void setUpdateTime(Date updateTime)
	{
		this.updateTime = updateTime;
	}
	public String getTempValue()
	{
		return tempValue;
	}
	public void setTempValue(String tempValue)
	{
		this.tempValue = tempValue;
	}
	public String getReportCycle()
	{
		return reportCycle;
	}
	public void setReportCycle(String reportCycle)
	{
		this.reportCycle = reportCycle;
	}
	public String getVolValue()
	{
		return volValue;
	}
	public void setVolValue(String volValue)
	{
		this.volValue = volValue;
	}
	public String getIsLeaf()
	{
		return isLeaf;
	}
	public void setIsLeaf(String isLeaf)
	{
		this.isLeaf = isLeaf;
	}
	public String getDeviceState()
	{
		return deviceState;
	}
	public void setDeviceState(String deviceState)
	{
		this.deviceState = deviceState;
	}



}
