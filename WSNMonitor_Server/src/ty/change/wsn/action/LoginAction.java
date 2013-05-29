package ty.change.wsn.action;

import ty.change.wsn.entity.User;
import ty.change.wsn.exception.UserNotFoundException;
import ty.change.wsn.util.Constant;

public class LoginAction extends BaseAction
{

	public void login()
	{
		init();
		String username = "";
		String password = "";
		username = jsonFromReq.getString("username");
		password = jsonFromReq.getString("password");
		User user = null;
		try
		{
			user = this.getUserService().getUserByUsername(username);
		}
		catch (UserNotFoundException e)
		{
			e.printStackTrace();
		}
		if (null == user)
		{
			jsonToResp.put("ActionResult", false);
			jsonToResp.put("Reason", Constant.USER_NOT_EXIST);
		}
		else if (!password.equals(user.getPassword()))
		{

			jsonToResp.put("ActionResult", false);
			jsonToResp.put("Reason", Constant.PASSWORD_WRONG);
		}
		else
		{
			jsonToResp.put("ActionResult", true);
			jsonToResp.put("Reason", Constant.USER_RIGHT);
		}
		System.out.println(jsonToResp.toString());
		out.write(jsonToResp.toString());
		out.flush();
		out.close();
	}
}