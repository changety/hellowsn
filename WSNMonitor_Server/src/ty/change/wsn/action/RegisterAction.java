package ty.change.wsn.action;

import ty.change.wsn.entity.User;
import ty.change.wsn.util.Constant;

import com.alibaba.fastjson.JSON;

public class RegisterAction extends BaseAction
{

	public void register() throws Exception
	{

		init();
		String username = "";
		username = jsonFromReq.getString("username");
		User user = getUserService().findUserByUsername(username);
		if (null == user)
		{
			user = JSON.parseObject(jsonFromReq.toString(), User.class);
			System.out.println(user.getEmail());
			getUserService().saveUser(user);
			jsonToResp.put("ActionResult", true);
			jsonToResp.put("Reason", Constant.REGISTER_SUCCESS);
		}
		else
		{
			jsonToResp.put("ActionResult", false);
			jsonToResp.put("Reason", Constant.USER_EXIST);
		}
		System.out.println(jsonToResp.toString());
		out.write(jsonToResp.toString());
		out.flush();
		out.close();
	}
}
