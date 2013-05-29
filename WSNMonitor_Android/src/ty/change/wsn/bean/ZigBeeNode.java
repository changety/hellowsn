package ty.change.wsn.bean;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Date;

import com.alibaba.fastjson.annotation.JSONField;

public class ZigBeeNode implements Serializable {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	protected int id;
	protected String ieeeAddress;
	protected String pieeeAddress;
	protected String netAddress;
	protected String pNetAddress;
	protected String type;
	protected String pType;
	protected Date joinTime;
	protected Date updateTime;
	
	private String tempValue;
	private String reportCycle;
	private String volValue;
	private String isLeaf;
	private String deviceState;
	
	private float x;
	private float y;
	private float radius;
	private ZigBeeNode fatherNode;
	private ArrayList<ZigBeeNode> childrenList;
	
	

	public String getType() {
		return type;
	}

	@JSONField(name = "type")
	public void setType(String type) {
		this.type = type;
	}

	public String getpType() {
		return pType;
	}

	@JSONField(name = "pType")
	public void setpType(String pType) {
		this.pType = pType;
	}

	public String getNetAddress() {
		return netAddress;
	}

	@JSONField(name = "netAddress")
	public void setNetAddress(String netAddress) {
		this.netAddress = netAddress;
	}

	public String getpNetAddress() {
		return pNetAddress;
	}

	@JSONField(name = "pNetAddress")
	public void setpNetAddress(String pNetAddress) {
		this.pNetAddress = pNetAddress;
	}

//	public String getiEEEAddress() {
//		return iEEEAddress;
//	}
//
//	@JSONField(name = "iEEEAddress")
//	public void setiEEEAddress(String iEEEAddress) {
//		this.iEEEAddress = iEEEAddress;
//	}
//
//	public String getpIEEEAddress() {
//		return pIEEEAddress;
//	}
//
//	@JSONField(name = "pIEEEAddress")
//	public void setpIEEEAddress(String pIEEEAddress) {
//		this.pIEEEAddress = pIEEEAddress;
//	}


	public int getId() {
		return id;
	}

	@JSONField(name = "id")
	public void setId(int id) {
		this.id = id;
	}

	public Date getJoinTime() {
		return joinTime;
	}

	@JSONField(name = "joinTime")
	public void setJoinTime(Date joinTime) {
		this.joinTime = joinTime;
	}


	public String getIeeeAddress() {
		return ieeeAddress;
	}

	public void setIeeeAddress(String ieeeAddress) {
		this.ieeeAddress = ieeeAddress;
	}

	public String getPieeeAddress() {
		return pieeeAddress;
	}

	public void setPieeeAddress(String pieeeAddress) {
		this.pieeeAddress = pieeeAddress;
	}

	public Date getUpdateTime() {
		return updateTime;
	}

	public void setUpdateTime(Date updateTime) {
		this.updateTime = updateTime;
	}

	public String getTempValue() {
		return tempValue;
	}

	public void setTempValue(String tempValue) {
		this.tempValue = tempValue;
	}


	public String getVolValue() {
		return volValue;
	}

	public void setVolValue(String volValue) {
		this.volValue = volValue;
	}

	public String getIsLeaf() {
		return isLeaf;
	}

	public void setIsLeaf(String isLeaf) {
		this.isLeaf = isLeaf;
	}

	public String getDeviceState() {
		return deviceState;
	}

	public void setDeviceState(String deviceState) {
		this.deviceState = deviceState;
	}

	public float getX() {
		return x;
	}

	public void setX(float x) {
		this.x = x;
	}

	public float getY() {
		return y;
	}

	public void setY(float y) {
		this.y = y;
	}

	public float getRadius() {
		return radius;
	}

	public void setRadius(float radius) {
		this.radius = radius;
	}

	public ZigBeeNode getFatherNode() {
		return fatherNode;
	}

	public void setFatherNode(ZigBeeNode fatherNode) {
		this.fatherNode = fatherNode;
	}

	public ArrayList<ZigBeeNode> getChildrenList() {
		return childrenList;
	}

	public void setChildrenList(ArrayList<ZigBeeNode> childrenList) {
		this.childrenList = childrenList;
	}

	public String getReportCycle() {
		return reportCycle;
	}

	public void setReportCycle(String reportCycle) {
		this.reportCycle = reportCycle;
	}

}
