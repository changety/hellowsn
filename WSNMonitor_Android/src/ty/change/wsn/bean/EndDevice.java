package ty.change.wsn.bean;


public class EndDevice extends ZigBeeNode{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private String temp; 
	private String voltage;
	public String getTemp() {
		return temp;
	}
	public void setTemp(String temp) {
		this.temp = temp;
	}
	public String getVoltage() {
		return voltage;
	}
	public void setVoltage(String voltage) {
		this.voltage = voltage;
	}


}
