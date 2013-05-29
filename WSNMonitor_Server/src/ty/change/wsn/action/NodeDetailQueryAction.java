package ty.change.wsn.action;

import com.alibaba.fastjson.JSON;

public class NodeDetailQueryAction extends BaseAction
{
	private String type, ieeeAddress;
	private int time, id, page;
	private String queryDetail;

	public void queryDetail() throws Exception
	{
		init();
		ieeeAddress = jsonFromReq.getString("ieeeAddress");
		type = jsonFromReq.getString("type");
		time = jsonFromReq.getIntValue("time");
		id = jsonFromReq.getIntValue("id");
		page = jsonFromReq.getIntValue("page");
		queryDetail = JSON.toJSONString(this.getWsnService().findDetailData(
				type, ieeeAddress, time, id, page));
		
		System.out.println(queryDetail);
		out.write(queryDetail);
		out.flush();
		out.close();
	}
}