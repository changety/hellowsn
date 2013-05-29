package ty.change.wsn.util;

import ty.change.wsn.bean.WSNNoti;
import ty.change.wsn.bean.ZigBeeNode;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;

public class ViewUtility {
	// Activity跳转辅助类
	public static void NavigateActivity(Context context, Class<?> cls) {
		Intent intent = new Intent();
		intent.setClass(context, cls);
		((Activity) context).startActivity(intent);
	}

	public static void NavigateActivity(Context context, Class<?> cls, int tab) {
		Intent intent = new Intent();
		intent.setClass(context, cls);
		intent.putExtra("tab", tab);
		((Activity) context).startActivity(intent);
	}

	public static void NavigateActivity(Context context, Class<?> cls,
			WSNNoti noti) {
		Intent intent = new Intent();
		intent.setClass(context, cls);
		intent.putExtra(Constants.WSN_NOTI, noti);
		((Activity) context).startActivity(intent);
	}

	public static void NavigateActivity(Context context, Class<?> cls,
			boolean finish) {
		Intent intent = new Intent();
		intent.setClass(context, cls);
		((Activity) context).startActivity(intent);
		if (finish) {
			((Activity) context).finish();
		}
	}

	public static void NavigateActivity(Context context, Class<?> cls,
			ZigBeeNode node) {
		Intent intent = new Intent();
		intent.putExtra(Constants.SELECTED_NODE, node);
		intent.setClass(context, cls);
		((Activity) context).startActivity(intent);
	}

	public static void NavigateActivity(Context context, Class<?> cls,
			String iEEEAddress) {
		Intent intent = new Intent();
		intent.putExtra(Constants.SELECTED_NODE, iEEEAddress);
		intent.setClass(context, cls);
		((Activity) context).startActivity(intent);
	}

	/**
	 * 从ServiceInfo 跳到 ServiceDetail
	 * 
	 * @param context
	 * @param cls
	 * @param serviceItem
	 */
	public static void NavigateActivity(Context context, Class<?> cls,
			ZigBeeNode zigBeeNode, boolean finish) {
		Intent intent = new Intent();
		intent.putExtra(Constants.ZIGBEE_NODE, zigBeeNode);
		intent.setClass(context, cls);
		((Activity) context).startActivity(intent);
		if (finish) {
			((Activity) context).finish();
		}
	}

	public static void NavigateActivity(Context context, Class<?> cls,
			String netAddress, boolean finish) {
		Intent intent = new Intent();
		intent.putExtra("touchedNode", netAddress);
		intent.setClass(context, cls);
		((Activity) context).startActivity(intent);
		if (finish) {
			((Activity) context).finish();
		}
	}

}
