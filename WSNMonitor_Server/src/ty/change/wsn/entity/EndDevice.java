package ty.change.wsn.entity;



public class EndDevice extends ZigBeeNode
{
	private String temp; 
	private String voltage;
	private int id;

	public String getTemp()
	{
		return temp;
	}

	public void setTemp(String temp)
	{
		this.temp = temp;
	}

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
