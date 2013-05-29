package ty.change.wsn.entity;


public class Router extends ZigBeeNode
{
	private String voltage;
	private int id;
	public String getVoltage()
	{
		return voltage;
	}
	public void setVoltage(String voltage)
	{
		this.voltage = voltage;
	}
	public int getId()
	{
		return id;
	}
	public void setId(int id)
	{
		this.id = id;
	}

}
