package ty.change.wsn.bean;


public class Router extends ZigBeeNode {
	/**
	 * 
	 */
	private static final long serialVersionUID = 5689620418145491177L;
	/**
	 * 
	 */
	private String voltage;
	public String getVoltage()
	{
		return voltage;
	}
	public void setVoltage(String voltage)
	{
		this.voltage = voltage;
	}
}
