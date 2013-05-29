package ty.change.wsn.dataparser;

import ty.change.wsn.util.Constant;

/**
 * @author Ruibron 完成接受对接受到的wsndata的校验，如果数据格式正确，则调用DataParseUtil类的方法进行解析拼装。
 */
public class CommandGeneratorUtil
{

	public static String CommandGenerator(String settingValue, int command,
			String netAddress)
	{

		StringBuffer commCommand = new StringBuffer();
		// FE
		commCommand.append(Constant.COMMAND_FIRST);
		// 04
		commCommand.append(Constant.COMMAND_LENGTH);
		// 0102
		commCommand.append(Constant.COMMAND_HEAD);
		// 网络地址
		commCommand.append(netAddress);

		switch (command)
		{
		//开灯
		case Constant.COMMAND_TURN_ON_LIGHT:
			// DA
			commCommand.append(Constant.COMMAND_LIGHT);
			// FB
			commCommand.append(Constant.COMMAND_LIGHT_ON);
			// FD
			commCommand.append(Constant.COMMAND_END);
			break;
		//关灯
		case Constant.COMMAND_TURN_OFF_LIGHT:
			// DA
			commCommand.append(Constant.COMMAND_LIGHT);
			// FA
			commCommand.append(Constant.COMMAND_LIGHT_OFF);
			// FE
			commCommand.append(Constant.COMMAND_END);
			break;
		//温度设置
		case Constant.COMMAND_TEMP_SETTING:
			String tempValue16 = DataParseUtil.getInstance()
					.parseTemperature10To16(settingValue);
			commCommand.append(Constant.COMMAND_TEMP);
			commCommand.append(tempValue16);
			commCommand.append(Constant.COMMAND_END);
			break;
		//电压设置
		case Constant.COMMAND_VOLTAGE_SETTING:
			String voltageValue16 = DataParseUtil.getInstance()
					.parseVoltage10To16(settingValue);
			commCommand.append(Constant.COMMAND_VOLTAGE);
			commCommand.append(voltageValue16);
			commCommand.append(Constant.COMMAND_END);
			break;
		//周期设置
		case Constant.COMMAND_CYCLE_SETTING:
			if(settingValue.equals("-1")){
				commCommand.append(Constant.COMMAND_REPORT);
				commCommand.append(Constant.COMMAND_REPORT_UNENABLE);
				commCommand.append(Constant.COMMAND_END);
			}else{
				commCommand.append(Constant.COMMAND_CYCLE);
				if(settingValue.equals("60")){
					commCommand.append("01");
				}else if(settingValue.equals("30")){
					commCommand.append("02");
				}else if(settingValue.equals("10")){
					commCommand.append("03");
				}
				commCommand.append(Constant.COMMAND_END);
			}
			break;
		default:
			break;
		}
		return commCommand.toString();
	}

}