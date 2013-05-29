package ty.change.wsn.action;

import org.apache.struts2.ServletActionContext;

import ty.change.wsn.comm.SerialConnection;
import ty.change.wsn.comm.SerialConnectionException;
import ty.change.wsn.comm.SerialDemo;
import ty.change.wsn.entity.User;
import ty.change.wsn.util.Constant;

public class SerialCommControlAction extends BaseAction
{
	private SerialConnection serialConnection;
	private User user;
	private String username;
	private int command;
	private SerialDemo serialDemo;
	
	public SerialDemo getSerialDemo()
	{
		return serialDemo;
	}

	public void setSerialDemo(SerialDemo serialDemo)
	{
		this.serialDemo = serialDemo;
	}

	
	@Override
	protected void init()
	{
		serialDemo = (SerialDemo) ServletActionContext.getServletContext()
				.getAttribute(Constant.SERIALAPP);
		serialConnection = serialDemo.getConnection();
		super.init();
	}

	public void serialCommControl() throws Exception
	{
		this.init();
		username = jsonFromReq.getString("username");
		command = jsonFromReq.getIntValue("command");
		// 得到user判断权限
		user = getUserService().getUserByUsername(username);
		if (!Constant.USER_ADMIN.equals(user.getAuthority()))
		{
			response(Constant.AUTHORITY, Constant.AUTHORITY_REASON);
		}
		else
		{
			process(command);
		}
	}

	/**
	 * 处理方法,直接处理那些不用往串口写东西的command,如果需要往串口写东西，交给nodeControl
	 * 
	 * @param command
	 * @throws SerialConnectionException
	 */
	private void process(int command) throws SerialConnectionException
	{
		switch (command)
		{
		// 查看是否开启串口通信
		case Constant.COMMAND_CHECK_COMM_STATE:
			if (serialConnection.isOpen())
			{
				response(Constant.COMMAND_CHECK_COMM_STATE,
						Constant.COMM_STATE_OPENED);
			}
			else
			{
				response(Constant.COMMAND_CHECK_COMM_STATE,
						Constant.COMM_STATE_CLOSED);
			}
			break;
		// 打开串口
		case Constant.COMMAND_COMM_OPEN:
			if (!serialConnection.isOpen())
			{
				serialDemo.openComm();
			}
			response(Constant.COMMAND_COMM_OPEN, Constant.COMM_STATE_OPENED);
			break;
		// 关闭串口
		case Constant.COMMAND_COMM_CLOSE:
			if (serialConnection.isOpen())
			{
				serialDemo.portClosed();
			}
			response(Constant.COMMAND_COMM_CLOSE, Constant.COMM_STATE_CLOSED);
			break;
		default:
			break;
		}
	}

	/**
	 * 拼装返回的字符串
	 * 
	 * @param actionResult
	 * @param reason
	 */
	private void response(int actionResult, String reason)
	{
		jsonToResp.put(Constant.RESULT, actionResult);
		jsonToResp.put(Constant.REASON, reason);

		out.write(jsonToResp.toString());
		out.flush();
		out.close();
	}
}
