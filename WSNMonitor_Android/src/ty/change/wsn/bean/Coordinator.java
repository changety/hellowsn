package ty.change.wsn.bean;

public class Coordinator extends ZigBeeNode {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private String panid;
	public String getPanid()
	{
		return panid;
	}
	public void setPanid(String panid)
	{
		this.panid = panid;
	}
	
}
