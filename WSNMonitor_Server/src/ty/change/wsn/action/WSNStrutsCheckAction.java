package ty.change.wsn.action;

import java.util.List;

import ty.change.wsn.entity.User;
import ty.change.wsn.entity.ZigBeeNode;
import ty.change.wsn.exception.UserNotFoundException;
import ty.change.wsn.util.Constant;

import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;

public class WSNStrutsCheckAction extends BaseAction
{	
	@SuppressWarnings("unchecked")
	public void wsnStrutsCheck()
	{
		init();
		JSONArray jsonArrayToResp = new JSONArray();
		List<ZigBeeNode> wsnStructDataList = null;
		ZigBeeNode wsnStructData = null;
		String username = "";
		String command = "";
		User user = null;
		username = jsonFromReq.getString("username");
		command = jsonFromReq.getString("command");
		try
		{
			user = getUserService().getUserByUsername(username);
		}
		catch (UserNotFoundException e)
		{
			e.printStackTrace();
		}
		// 判断权限
		if (Constant.USER_ADMIN.equals(user.getAuthority()))
		{
			if (Constant.COMMAND_WSN_STRUCT_REFRESH.equals(command))
			{
				wsnStructDataList = getWsnService().findAllZigbeeNTData();
				for (int i = 0; i < wsnStructDataList.size(); i++)
				{
					wsnStructData = wsnStructDataList.get(i);
					jsonToResp = new JSONObject();
					jsonToResp.put("netAddress", wsnStructData.getNetAddress());
					jsonToResp.put("pNetAddress",
							wsnStructData.getpNetAddress());
					jsonToResp.put("ieeeAddress",
							wsnStructData.getIeeeAddress());
					jsonToResp.put("pieeeAddress",
							wsnStructData.getPieeeAddress());
					jsonToResp.put("type", wsnStructData.getType());
					jsonToResp.put("pType", wsnStructData.getpType());
					jsonToResp.put("isLeaf", wsnStructData.getIsLeaf());
					jsonToResp.put("tempValue", wsnStructData.getTempValue());
					jsonToResp.put("reportCycle",
							wsnStructData.getReportCycle());
					jsonToResp.put("volValue", wsnStructData.getVolValue());
					jsonToResp.put("deviceState",
							wsnStructData.getDeviceState());
					jsonToResp.put("updateTime", wsnStructData.getUpdateTime());
					jsonArrayToResp.add(i, jsonToResp);
				}
			}
		}
		// 没有权限
		else
		{
			jsonToResp = new JSONObject();
			jsonToResp.put("ActionResult", false);
			jsonToResp.put("Reason", Constant.AUTHORITY);
			jsonArrayToResp.add(0, jsonToResp);
		}
		System.out.println(jsonArrayToResp.toString());
		out.write(jsonArrayToResp.toString());
		out.flush();
		out.close();
	}
}