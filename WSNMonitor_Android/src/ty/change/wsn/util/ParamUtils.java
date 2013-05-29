package ty.change.wsn.util;

import java.io.UnsupportedEncodingException;

import org.apache.http.entity.StringEntity;
import org.apache.http.protocol.HTTP;

import ty.change.wsn.bean.User;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

public class ParamUtils {
	/**
	 * @param username
	 * @param password
	 *            用于构建发送到服务器端的登入交换信息格式， JSONObject
	 * @return
	 */

	public static StringEntity SE4Login(String username, String pwd) {
		StringEntity se = null;
		JSONObject json4Login = new JSONObject();
		json4Login.put("username", username);
		json4Login.put("password", pwd);
		try {
			se = new StringEntity(json4Login.toString(), HTTP.UTF_8);
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return se;
	}

	public static StringEntity SE4Register(User user) {
		StringEntity se = null;
		try {
			se = new StringEntity(JSON.toJSONString(user), HTTP.UTF_8);
			System.out.println(JSON.toJSONString(user));
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return se;
	}

	/**
	 * @param username
	 * @param command
	 *            用于构建发送到服务器端的节点控制json格式的信息， JSONObject
	 * @return
	 */
	public static StringEntity SE4NodeQuery(String username, String command) {
		StringEntity se = null;
		JSONObject json4NTRefresh = new JSONObject();
		json4NTRefresh.put("username", username);
		json4NTRefresh.put("command", command);
		try {
			se = new StringEntity(json4NTRefresh.toString(), HTTP.UTF_8);
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return se;
	}

	public static StringEntity SE4NodeDetailQuery(String username,
			String ieeeAddress, String type, int id, int page, int time) {
		StringEntity se = null;
		JSONObject json4NodeDetailQuery = new JSONObject();
		json4NodeDetailQuery.put("username", username);
		json4NodeDetailQuery.put("ieeeAddress", ieeeAddress);
		json4NodeDetailQuery.put("type", type);
		json4NodeDetailQuery.put("id", id);
		json4NodeDetailQuery.put("page", page);
		json4NodeDetailQuery.put("time", time);
		try {
			se = new StringEntity(json4NodeDetailQuery.toJSONString(),
					HTTP.UTF_8);
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return se;
	}

	/**
	 * 用于构建发送到服务器端的节点控制json格式的信息， JSONObject
	 * 
	 * @param username
	 * @param command
	 * @param ieeeAddress
	 * @param settingValue
	 * @return
	 */
	public static StringEntity SE4NodeControl(String username, int command,
			String ieeeAddress, String settingValue) {
		StringEntity se = null;
		JSONObject json4NodeControl = new JSONObject();
		json4NodeControl.put("username", username);
		json4NodeControl.put("command", command);
		json4NodeControl.put("ieeeAddress", ieeeAddress);
		json4NodeControl.put("settingValue", settingValue);
		System.out.println(json4NodeControl.toString());
		try {
			se = new StringEntity(json4NodeControl.toJSONString(), HTTP.UTF_8);
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return se;
	}

	/**
	 * @param username
	 * @param password
	 * @param authority
	 *            用于构建发送到服务器端的注册信息格式， JSONObject
	 * @return
	 */
	public static String JSON4Register(String username, String password,
			String email, String authority) {
		JSONObject json4Register = new JSONObject();
		json4Register.put("username", username);
		json4Register.put("password", password);
		json4Register.put("email", email);
		json4Register.put("authority", authority);
		return json4Register.toString();
	}

	/**
	 * 用于构建发送到服务器端的节点控制json格式的信息， JSONObject
	 * 
	 * @param username
	 * @param command
	 * @param ieeeAddress
	 * @param tempSettingValue
	 * @param voltageSettingValue
	 * @return
	 */
	public static StringEntity SE4SerialCommContr(String username, int command) {
		StringEntity se = null;
		JSONObject json4SerialCommControl = new JSONObject();
		json4SerialCommControl.put("username", username);
		json4SerialCommControl.put("command", command);
		try {
			se = new StringEntity(json4SerialCommControl.toString(), HTTP.UTF_8);
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return se;

	}

}
