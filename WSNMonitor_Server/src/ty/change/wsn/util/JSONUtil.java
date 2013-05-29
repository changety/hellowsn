package ty.change.wsn.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;

import javax.servlet.ServletInputStream;
import javax.servlet.http.HttpServletRequest;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;

public class JSONUtil
{
	/**
	 * @param req
	 * @return 解析HttpPost过来的ServletInputStream 转化为JSONObject;
	 */
	public static JSONObject HttppostToJSONObject(HttpServletRequest req)
	{
		/* 读取数据 */
		JSONObject jsonForPost = null;
		BufferedReader br = null;
		String result = "";
		try
		{
			String temp;
			br = new BufferedReader(new InputStreamReader(
					(ServletInputStream) req.getInputStream(), "utf-8"));
			StringBuffer sb = new StringBuffer("");
			while ((temp = br.readLine()) != null)
			{
				sb.append(temp);
			}
			br.close();
			result = sb.toString();
			jsonForPost = JSON.parseObject(result);
		}
		catch (UnsupportedEncodingException e)
		{
			e.printStackTrace();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
		return jsonForPost;
	}

	/**
	 * @param req
	 * @return 解析HttpPost过来的ServletInputStream 转化为JSONArray;
	 */
	public static JSONArray HttppostToJSONArray(HttpServletRequest req)
	{
		/* 读取数据 */
		JSONArray jsonForPost = null;
		BufferedReader br = null;
		String result = "";
		try
		{
			String temp;
			br = new BufferedReader(new InputStreamReader(
					(ServletInputStream) req.getInputStream(), "UTF-8"));
			StringBuffer sb = new StringBuffer("");
			while ((temp = br.readLine()) != null)
			{
				sb.append(temp);
			}
			br.close();
			result = sb.toString();
			jsonForPost = JSON.parseArray(result);
		}
		catch (UnsupportedEncodingException e)
		{
			e.printStackTrace();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
		return jsonForPost;
	}
}
