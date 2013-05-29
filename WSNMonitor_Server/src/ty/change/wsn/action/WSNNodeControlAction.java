package ty.change.wsn.action;

import org.apache.struts2.ServletActionContext;

import ty.change.wsn.comm.SerialConnection;
import ty.change.wsn.comm.SerialConnectionException;
import ty.change.wsn.comm.SerialDemo;
import ty.change.wsn.dataparser.CommandGeneratorUtil;
import ty.change.wsn.entity.User;
import ty.change.wsn.entity.ZigBeeNode;
import ty.change.wsn.util.Constant;
import ty.change.wsn.util.ZigBeeNTUtil;

public class WSNNodeControlAction extends BaseAction
{

	private SerialDemo serialDemo;

	public SerialDemo getSerialDemo()
	{
		return serialDemo;
	}

	public void setSerialDemo(SerialDemo serialDemo)
	{
		this.serialDemo = serialDemo;
	}

	private SerialConnection serialConnection;

	private User user;
	private ZigBeeNode node4control;
	private String username, ieeeAddress, settingValue;
	private int command;
	private String commCommand;
	private String netAddress;
	private ZigBeeNTUtil zigbeeNTUtil = ZigBeeNTUtil.getInstance();

	@Override
	protected void init()
	{
		serialDemo = (SerialDemo) ServletActionContext.getServletContext()
				.getAttribute(Constant.SERIALAPP);
		serialConnection = serialDemo.getConnection();
		super.init();
	}

	public void wsnNodeControl() throws Exception
	{
		this.init();
		username = jsonFromReq.getString("username");
		command = jsonFromReq.getIntValue("command");
		ieeeAddress = jsonFromReq.getString("ieeeAddress");
		settingValue = jsonFromReq.getString("settingValue");
		// 得到user判断权限
		user = getUserService().getUserByUsername(username);
		node4control = getWsnService().findSelfByIEEEAddress(ieeeAddress);
		netAddress = node4control.getNetAddress();
		if (!Constant.USER_ADMIN.equals(user.getAuthority()))
		{
			response(Constant.AUTHORITY, Constant.AUTHORITY_REASON);
		}
		else
		{
			if (!serialConnection.isOpen())
			{
				response(Constant.SERIAL_COMM_STATE, Constant.COMM_STATE_CLOSED);
			}
			else
			{
				process(command);
			}
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
		case Constant.COMMAND_TURN_ON_LIGHT:
			node4control.setDeviceState(Constant.LIGHT_STATE_ON);
			break;
		// 关灯
		case Constant.COMMAND_TURN_OFF_LIGHT:
			node4control.setDeviceState(Constant.LIGHT_STATE_OFF);
			break;
		// 温度设置
		case Constant.COMMAND_TEMP_SETTING:
			node4control.setTempValue(settingValue);
			break;
		// 电压设置
		case Constant.COMMAND_VOLTAGE_SETTING:
			node4control.setVolValue(settingValue);
			break;
		case Constant.COMMAND_CYCLE_SETTING:
			node4control.setReportCycle(settingValue);
			break;
		default:
			break;
		}
		commCommand = CommandGeneratorUtil.CommandGenerator(settingValue,
				command, netAddress);
		serialConnection.writeToComm(commCommand.toString());
		zigbeeNTUtil.updateZigBeeNode(node4control);
		getWsnService().updateZigbeeNTData(node4control);
		response(command, "success");
		System.out.println(commCommand.toString());
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
		System.out.println(jsonToResp.toString());
		out.write(jsonToResp.toString());
		out.flush();
		out.close();
	}
}