package ty.change.wsn.action;

import java.io.IOException;
import java.io.PrintWriter;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts2.ServletActionContext;

import ty.change.wsn.service.UserService;
import ty.change.wsn.service.WSNService;
import ty.change.wsn.util.JSONUtil;

import com.alibaba.fastjson.JSONObject;
import com.opensymphony.xwork2.ActionSupport;

public class BaseAction extends ActionSupport
{
	private WSNService wsnService;
	private UserService userService;
	HttpServletResponse resp;
	HttpServletRequest req;
	JSONObject jsonFromReq;
	JSONObject jsonToResp = new JSONObject();

	PrintWriter out;

	protected void init()
	{
		resp = ServletActionContext.getResponse();
		req = ServletActionContext.getRequest();
		resp.setContentType("application/json");
		resp.setCharacterEncoding("UTF-8");
		try
		{
			out = resp.getWriter();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
		jsonFromReq = JSONUtil.HttppostToJSONObject(req);
		System.out.println(jsonFromReq.toString());
	}

	public UserService getUserService()
	{
		return userService;
	}

	public void setUserService(UserService userService)
	{
		this.userService = userService;
	}

	public WSNService getWsnService()
	{
		return wsnService;
	}

	public void setWsnService(WSNService wsnService)
	{
		this.wsnService = wsnService;
	}
}
