package ty.change.wsn.entity;



public class Coordinator extends ZigBeeNode
{	
	private String panid;
	private int id;
	public String getPanid()
	{
		return panid;
	}
	public void setPanid(String panid)
	{
		this.panid = panid;
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
