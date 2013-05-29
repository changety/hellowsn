package ty.change.wsn.activity;

import java.util.ArrayList;
import java.util.HashMap;

import ty.change.wsn.R;
import ty.change.wsn.androidpn.client.ServiceManager;
import ty.change.wsn.util.Constants;
import ty.change.wsn.util.ViewUtility;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.GridView;
import android.widget.ProgressBar;
import android.widget.SimpleAdapter;
import android.widget.TextView;

public class MainActivity extends BaseActivity {

	private TextView titleTV;
	private ProgressBar mProgressbar;
	private GridView mgirdView;
	private ArrayList<HashMap<String, Object>> meumList;
	private final static int DATA_LOAD_ING = 0x001;
	private final static int DATA_LOAD_COMPLETE = 0x002;
	private String[] funtions;

	// 定义整型数组 即图片源
	private Integer[] icons = { R.drawable.squery, R.drawable.snet,
			R.drawable.smonitor, R.drawable.ssubscription, R.drawable.snoti,
			R.drawable.shistory, R.drawable.ssetting, R.drawable.sabout,
			R.drawable.sexit };

	/** Called when the activity is first created. */

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		System.out.println("onCreate");
		setContentView(R.layout.main_activity);
		initPage();
		registerConnectStatesReceiver(new XMPPConnectStatesReceiver());
		connectToPNServer();
	}

	@Override
	protected void initView() {
		funtions = getResources().getStringArray(R.array.FunctionList);

		// 取得GridView对象
		mgirdView = (GridView) findViewById(R.id.gridview);
		titleTV = (TextView) findViewById(R.id.gridviewTitle);
		mProgressbar = (ProgressBar) findViewById(R.id.gird_progress);
		// 添加元素给gridview
		meumList = new ArrayList<HashMap<String, Object>>();
		for (int i = 0; i < icons.length; i++) {
			HashMap<String, Object> map = new HashMap<String, Object>();
			map.put("ItemImage", icons[i]);
			map.put("ItemText", funtions[i]);
			meumList.add(map);
		}
		SimpleAdapter saItem = new SimpleAdapter(this, meumList, // 数据源
				R.layout.main_activity_item, // xml实现
				new String[] { "ItemImage", "ItemText" }, // 对应map的Key
				new int[] { R.id.ItemImage, R.id.ItemText }); // 对应R的Id
		// 添加Item到网格中
		mgirdView.setAdapter(saItem);
		// 事件监听
		mgirdView.setOnItemClickListener(new OnItemClickListener() {
			public void onItemClick(AdapterView<?> parent, View v,
					int position, long id) {
				switch (position) {
				case 0:
					ViewUtility.NavigateActivity(MainActivity.this,
							NodeMainActivity.class, 0);
					break;
				case 1:
					ViewUtility.NavigateActivity(MainActivity.this,
							NodeMainActivity.class, 1);
					break;
				case 2:
					ViewUtility.NavigateActivity(MainActivity.this,
							NodeMainActivity.class, 2);
					break;
				case 3:
					ViewUtility.NavigateActivity(MainActivity.this,
							SerialCommControlActivity.class);
					break;
				case 4:
					ViewUtility.NavigateActivity(MainActivity.this,
							NotificationsActivity.class);
					break;
				case 5:
					ViewUtility.NavigateActivity(MainActivity.this,
							NodeHistoryDataActivity.class);
					break;
				case 6:
					ViewUtility.NavigateActivity(MainActivity.this,
							NotificationSettingsActivity.class);

					break;
				case 7:
					ViewUtility.NavigateActivity(MainActivity.this,
							HelpActivity.class);
					break;
				case 8:
					MainActivity.this.finish();
					System.exit(0);
				default:
					break;
				}
			}
		});
	}

	@Override
	protected void initData() {
	}

	private void connectToPNServer() {
		xmppStateInfo(DATA_LOAD_ING);
		// Start the service
		ServiceManager serviceManager = new ServiceManager(this);
		serviceManager.setNotificationIcon(R.drawable.wsnnoti);
		serviceManager.startService();
	}

	private void xmppStateInfo(int type) {
		switch (type) {
		case DATA_LOAD_ING:
			titleTV.setText("连接推送服务器...请等待");
			mProgressbar.setVisibility(View.VISIBLE);
			break;
		case DATA_LOAD_COMPLETE:
			titleTV.setText("连接推送服务器成功！");
			mProgressbar.setVisibility(View.GONE);
			break;
		}
	}

	private class XMPPConnectStatesReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (action.equals(Constants.Connect_SUCCESS)) {
				xmppStateInfo(DATA_LOAD_COMPLETE);
			}
		}
	}

	private void registerConnectStatesReceiver(
			XMPPConnectStatesReceiver connectStatesReceiver) {
		IntentFilter filter = new IntentFilter();
		filter.addAction(Constants.Connect_SUCCESS);
		MainActivity.this.registerReceiver(connectStatesReceiver, filter);
	}

	@Override
	public void onBackPressed() {
		Intent MyIntent = new Intent(Intent.ACTION_MAIN);
		MyIntent.addCategory(Intent.CATEGORY_HOME);
		startActivity(MyIntent);
	}

}
