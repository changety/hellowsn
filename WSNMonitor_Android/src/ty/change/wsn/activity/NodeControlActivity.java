package ty.change.wsn.activity;

import java.util.ArrayList;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import ty.change.wsn.R;
import ty.change.wsn.app.AppException;
import ty.change.wsn.bean.ZigBeeNode;
import ty.change.wsn.util.Constants;
import ty.change.wsn.util.ParamUtils;
import ty.change.wsn.util.URL;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.loopj.android.http.AsyncHttpClient;
import com.loopj.android.http.AsyncHttpResponseHandler;

/**
 * @author change_ty
 *
 */
public class NodeControlActivity extends Activity implements OnClickListener,
		OnItemSelectedListener {

	private String[] statusControl;
	private String[] reportRate = new String[] { "10秒", "30秒", "60秒", "休眠" };
	private ArrayAdapter<String> nodeControlAdapter, nodeReportAdapter;
	private String username, ieeeAddress, command;
	private String lightSetting = "FF";
	private String tempSetting = "23";
	private String cycleSetting = "60";
	private JSONObject json4NodeControlResult;
	private SharedPreferences sharedPreferences = null;
	private Intent intent;
	private ZigBeeNode selectedNode;

	private final int LOADING_INIT = 0, LOADING_INIT_END = -1,
			LOADING_ENDDEVICE = 1, LOADING_ROUTER = 2, LOADING_COOR = 3;

	private RelativeLayout node_control_spinner_rl,
			node_control_reportenable_rl, node_control_light_rl,
			node_control_temp_rl;

	private TextView nodeControlSpinnerTV, nodeControlLightTV,
			nodeControlReportTV, node_control_temp_setting_tv;
	private Spinner nodeControlSpinner, node_control_report_rate_spinner;
	private Button node_temp_control_button, node_control_report_rate_button,nodeControlLightToggleButton;
	private ProgressBar node_control_light_progress,
			node_control_report_proress, node_control_temp_proress,
			node_control_spinner_progress;
	private EditText node_control_temp_et;

	// intent传递过来的所有节点信息
	private final ArrayList<ZigBeeNode> wsnNTChangingList = new ArrayList<ZigBeeNode>();

	private final Map<String, ZigBeeNode> wsnNTChangingMap = new ConcurrentHashMap<String, ZigBeeNode>();

	private ImageView node_control_light_iv;
	private AsyncHttpClient httpClient;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.node_control_activity);
		initView();
		initData();
	}

	/**
	 * activity组件初始化方法.
	 */
	private void initView() {
		sharedPreferences = this.getSharedPreferences(
				Constants.SHARED_PREFERENCE_NAME, Context.MODE_PRIVATE);
		username = sharedPreferences.getString(Constants.XMPP_USERNAME, null);
		command = Constants.COMMAND_WSN_STRUCT_REFRESH;
		ieeeAddress = getIntent().getStringExtra(Constants.SELECTED_NODE);
		node_control_spinner_rl = (RelativeLayout) findViewById(R.id.node_control_spinner_rl);
		node_control_reportenable_rl = (RelativeLayout) findViewById(R.id.node_control_reportenable_rl);
		node_control_temp_rl = (RelativeLayout) findViewById(R.id.node_control_temp_rl);
		node_control_light_rl = (RelativeLayout) findViewById(R.id.node_control_light_rl);
		nodeControlSpinnerTV = (TextView) findViewById(R.id.nodeControlSpinnerTV);
		nodeControlLightTV = (TextView) findViewById(R.id.nodeControlLightTV);
		nodeControlReportTV = (TextView) findViewById(R.id.nodeControlReportTV);
		node_control_temp_setting_tv = (TextView) findViewById(R.id.node_control_temp_setting_tv);
		nodeControlLightToggleButton = (Button) findViewById(R.id.nodeControlLightToggleButton);
		node_control_light_progress = (ProgressBar) findViewById(R.id.node_control_light_progress);
		node_control_report_proress = (ProgressBar) findViewById(R.id.node_control_report_proress);
		node_control_temp_proress = (ProgressBar) findViewById(R.id.node_control_temp_proress);
		node_control_spinner_progress = (ProgressBar) findViewById(R.id.node_control_spinner_progress);
		node_control_temp_et = (EditText) findViewById(R.id.node_control_temp_et);
		node_temp_control_button = (Button) findViewById(R.id.node_temp_control_button);
		node_control_report_rate_button = (Button) findViewById(R.id.node_control_report_rate_button);
		node_temp_control_button.setOnClickListener(this);
		node_control_report_rate_button.setOnClickListener(this);
		nodeControlLightToggleButton.setOnClickListener(this);
		node_control_light_iv = (ImageView) findViewById(R.id.node_control_light_iv);

		node_control_report_rate_spinner = (Spinner) findViewById(R.id.node_control_report_rate_spinner);
		nodeReportAdapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_spinner_item, reportRate);
		nodeReportAdapter
				.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		node_control_report_rate_spinner.setAdapter(nodeReportAdapter);
		node_control_report_rate_spinner.setOnItemSelectedListener(this);
	}

	private void initData() {
		httpClient = new AsyncHttpClient();
		httpClient.post(this, URL.WSN_STRUCT_REFRESH_URL,
				ParamUtils.SE4NodeQuery(username, command), null,
				new AsyncHttpResponseHandler() {
					@Override
					public void onStart() {
						progressLoading(LOADING_INIT);
					}

					public void onSuccess(String response) {
						wsnNTChangingList.clear();
						wsnNTChangingMap.clear();
						wsnNTChangingList.addAll(JSON.parseArray(response,
								ZigBeeNode.class));
						for (ZigBeeNode node : wsnNTChangingList) {
							wsnNTChangingMap.put(node.getIeeeAddress(), node);
						}
						initNodesSpinner();
					}

					@Override
					public void onFinish() {
						progressLoading(LOADING_INIT_END);
					}

					@Override
					public void onFailure(Throwable error, String content) {
						super.onFailure(error, content);
						(AppException.network((Exception) error))
								.makeToast(NodeControlActivity.this);
					}
				});
	}

	protected void initNodesSpinner() {
		statusControl = new String[wsnNTChangingList.size()];
		for (int i = 0; i < wsnNTChangingList.size(); i++) {
			ZigBeeNode node = wsnNTChangingList.get(i);
			if (node.getType().equals(Constants.COORDINATOR)) {
				statusControl[i] = "协调器:" + node.getIeeeAddress();
			} else if (node.getType().equals(Constants.ROUTER)) {
				statusControl[i] = "路由器:" + node.getIeeeAddress();
			} else {
				statusControl[i] = "终端节点:" + node.getIeeeAddress();
			}
		}
		nodeControlSpinner = (Spinner) findViewById(R.id.nodeControlSpinner);

		nodeControlAdapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_spinner_item, statusControl);
		nodeControlAdapter
				.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		nodeControlSpinner.setAdapter(nodeControlAdapter);
		nodeControlSpinner.setOnItemSelectedListener(this);
		if (ieeeAddress != null) {
			selectedNode = getTheSelectedNode(ieeeAddress);
			int position = 0;
			for (int i = 0; i < statusControl.length; i++) {
				if (selectedNode.getIeeeAddress()
						.equals(statusControl[i].substring(statusControl[i]
								.length() - 16))) {
					position = i;
				}
			}
			nodeControlSpinner.setSelection(position);
		} else {
			nodeControlSpinner.setSelection(0);
			selectedNode = getTheSelectedNode(statusControl[0]
					.substring(statusControl[0].length() - 16));
		}
		refreshView();
	}

	/**
	 * 弹出对话框
	 */
	private void showDialog(String title, String message) {
		AlertDialog.Builder builder = new AlertDialog.Builder(
				NodeControlActivity.this);
		builder.setTitle(title);
		builder.setMessage(message)
				.setCancelable(false)
				.setPositiveButton("YES",
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
								intent = new Intent();
								intent.setClass(NodeControlActivity.this,
										SerialCommControlActivity.class);
								startActivity(intent);
							}
						})
				.setNegativeButton("NO", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						dialog.cancel();
					}
				});
		builder.show();
	}

	// 用来验证用户名 密码的合法性的正则表达式
	private boolean checkTempValue(String s) {
		return s.matches("([0-9])|([1-9][0-9])|100");
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.nodeControlLightToggleButton:
			if (selectedNode.getDeviceState().equals("ON")) {
				nodeControl(username, Constants.COMMAND_TURN_OFF_LIGHT,
						selectedNode.getIeeeAddress(), lightSetting);
			} else {
				nodeControl(username, Constants.COMMAND_TURN_ON_LIGHT,
						selectedNode.getIeeeAddress(), lightSetting);
			}
			break;
		case R.id.node_temp_control_button:
			if (checkTempValue(node_control_temp_et.getText().toString())) {
				tempSetting = node_control_temp_et.getText().toString();
				nodeControl(username, Constants.COMMAND_TEMP_SETTING,
						selectedNode.getIeeeAddress(), tempSetting);
			} else {
				Toast.makeText(this, "请输入合法温度0-100度，如20", Toast.LENGTH_SHORT)
						.show();
//				node_control_temp_et.setText("20");
			}
			break;
		case R.id.node_control_report_rate_button:
			switch (node_control_report_rate_spinner.getSelectedItemPosition()) {
			case 0:
				cycleSetting = "10";
				break;
			case 1:
				cycleSetting = "30";
				break;
			case 2:
				cycleSetting = "60";
				break;
			case 3:
				cycleSetting = "-1";
				break;
			default:
				break;
			}
			nodeControl(username, Constants.COMMAND_CYCLE_SETTING,
					selectedNode.getIeeeAddress(), cycleSetting);
			break;
		default:
			break;
		}

	}

	private void nodeControl(final String username, final int command,
			final String ieeeAddress, final String settingValue) {
		httpClient = new AsyncHttpClient();
		httpClient.post(this, URL.WSN_NODE_CONTROL_URL, ParamUtils
				.SE4NodeControl(username, command, ieeeAddress, settingValue),
				null, new AsyncHttpResponseHandler() {
					@Override
					public void onStart() {
						switch (command) {
						case Constants.COMMAND_TURN_ON_LIGHT:
							node_control_light_progress
									.setVisibility(View.VISIBLE);
							break;
						case Constants.COMMAND_TURN_OFF_LIGHT:
							node_control_light_progress
									.setVisibility(View.VISIBLE);
							break;
						case Constants.COMMAND_TEMP_SETTING:
							node_control_temp_proress
									.setVisibility(View.VISIBLE);
							break;
						case Constants.COMMAND_VOLTAGE_SETTING:
							break;
						case Constants.COMMAND_CYCLE_SETTING:
							node_control_report_proress
									.setVisibility(View.VISIBLE);
							break;
						default:
							break;
						}
					}

					public void onSuccess(String result) {
						json4NodeControlResult = JSON.parseObject(result);
						int actionResult = json4NodeControlResult
								.getIntValue(Constants.RESULT);
						String reason = json4NodeControlResult
								.getString(Constants.REASON);
						switch (actionResult) {
						case Constants.COMMAND_AUTHORITY:
							Toast.makeText(NodeControlActivity.this, reason,
									Toast.LENGTH_SHORT).show();
							break;
						case Constants.SERIAL_COMM_STATE:
							showDialog(reason, "需要开启通信吗?");
							break;
						case Constants.COMMAND_TURN_ON_LIGHT:
							selectedNode.setDeviceState("ON");
							node_control_light_iv
									.setImageResource(R.drawable.bulb_on);
							nodeControlLightTV.setText("灯的状态:"
									+ selectedNode.getDeviceState());
							nodeControlLightToggleButton
									.setText("关    闭");
							break;
						// 关灯
						case Constants.COMMAND_TURN_OFF_LIGHT:
							selectedNode.setDeviceState("OFF");
							node_control_light_iv
									.setImageResource(R.drawable.bulb_off);
							nodeControlLightTV.setText("灯的状态:"
									+ selectedNode.getDeviceState());
							nodeControlLightToggleButton
									.setText("打    开");
							break;
						// 温度设置
						case Constants.COMMAND_TEMP_SETTING:
							selectedNode.setTempValue(tempSetting);
							node_control_temp_setting_tv.setText("当前温度设定:"
									+ selectedNode.getTempValue() + "℃");
							node_control_temp_et.setText(selectedNode
									.getTempValue());
							break;
							// 周期设置
						case Constants.COMMAND_CYCLE_SETTING:
							selectedNode.setReportCycle(cycleSetting);
							nodeControlReportTV.setText("报告周期:"
									+ selectedNode.getReportCycle());
							break;

						// 电压设置
						// case Constants.COMMAND_VOLTAGE_SETTING:
						// selectedNode.setVoltageSetting(reason);
						// settingTV.setText("当前电压设定:"
						// + selectedNode.getVoltageSetting() + "v");
						// settingET.setText(selectedNode.getVoltageSetting());
						// break;
						default:
							break;
						}
						wsnNTChangingMap.put(selectedNode.getIeeeAddress(),
								selectedNode);
						Toast.makeText(NodeControlActivity.this, "设置成功",
								Toast.LENGTH_SHORT).show();
					}

					@Override
					public void onFinish() {
						progressLoading(LOADING_INIT_END);
					}

					@Override
					public void onFailure(Throwable error, String content) {
						super.onFailure(error, content);
						(AppException.network((Exception) error))
								.makeToast(NodeControlActivity.this);
					}
				});
	}

	@Override
	public void onNothingSelected(AdapterView<?> parent) {
	}

	@Override
	public void onItemSelected(AdapterView<?> parent, View view, int position,
			long id) {
		switch (parent.getId()) {
		case R.id.node_control_report_rate_spinner:
			System.out.println(reportRate[position]);
			break;
		case R.id.nodeControlSpinner:
			ieeeAddress = statusControl[position]
					.substring(statusControl[position].length() - 16);
			nodeControlSpinnerTV.setText(statusControl[position]);
			selectedNode = getTheSelectedNode(ieeeAddress);
			refreshView();
			break;
		default:
			break;
		}
	}

	private void refreshView() {
		if (selectedNode.getType().equals(Constants.ENDDEVICE)) {
			node_control_spinner_rl.setVisibility(View.VISIBLE);
			node_control_reportenable_rl.setVisibility(View.VISIBLE);
			node_control_temp_rl.setVisibility(View.VISIBLE);
			node_control_light_rl.setVisibility(View.VISIBLE);
			if (selectedNode.getDeviceState().equalsIgnoreCase(
					Constants.LIGHT_STATE_ON)) {
				nodeControlLightTV.setText("目前继电器外设状态:"
						+ selectedNode.getDeviceState() + ",点击按钮控制关闭");
				node_control_light_iv.setImageResource(R.drawable.bulb_on);
			} else {
				nodeControlLightTV.setText("目前继电器外设状态:"
						+ selectedNode.getDeviceState() + ",点击按钮控制打开");
				node_control_light_iv.setImageResource(R.drawable.bulb_off);
			}
			if (selectedNode.getReportCycle().equals(
					Constants.NODE_REPORT_CYCLE_FREQUENCE)) {
				nodeControlReportTV.setText("目前该节点传感器每10秒采集一次");
				node_control_report_rate_spinner.setSelection(0);
			} else if (selectedNode.getReportCycle().equals(
					Constants.NODE_REPORT_CYCLE_NORMAL)) {
				nodeControlReportTV.setText("目前该节点传感器每30秒采集一次");
				node_control_report_rate_spinner.setSelection(1);
			} else if (selectedNode.getReportCycle().equals(
					Constants.NODE_REPORT_CYCLE_SLOW)) {
				nodeControlReportTV.setText("目前该节点传感器每60秒采集一次");
				node_control_report_rate_spinner.setSelection(2);
			} else if (selectedNode.getReportCycle().equals(
					Constants.NODE_REPORT_STATE_SLEEP)) {
				nodeControlReportTV.setText("目前该节点处于休眠状态");
				node_control_report_rate_spinner.setSelection(3);
			}
			node_control_temp_setting_tv.setText("当前报警温度设定:"
					+ selectedNode.getTempValue() + "℃,输入报警温度值：");
			node_control_temp_et.setText(selectedNode.getTempValue());
		} else if (selectedNode.getType().equals(Constants.ROUTER)) {
			node_control_spinner_rl.setVisibility(View.VISIBLE);
			node_control_reportenable_rl.setVisibility(View.VISIBLE);
			node_control_temp_rl.setVisibility(View.GONE);
			node_control_light_rl.setVisibility(View.GONE);
			if (selectedNode.getReportCycle().equals(
					Constants.NODE_REPORT_CYCLE_FREQUENCE)) {
				nodeControlReportTV.setText("目前该节点每10秒报告一次");
				node_control_report_rate_spinner.setSelection(0);
			} else if (selectedNode.getReportCycle().equals(
					Constants.NODE_REPORT_CYCLE_NORMAL)) {
				nodeControlReportTV.setText("目前该节点每30秒报告一次");
				node_control_report_rate_spinner.setSelection(1);
			} else if (selectedNode.getReportCycle().equals(
					Constants.NODE_REPORT_CYCLE_SLOW)) {
				nodeControlReportTV.setText("目前该节点每60秒报告一次");
				node_control_report_rate_spinner.setSelection(2);
			} else if (selectedNode.getReportCycle().equals(
					Constants.NODE_REPORT_STATE_SLEEP)) {
				nodeControlReportTV.setText("目前该节点处于休眠状态");
				node_control_report_rate_spinner.setSelection(3);
			}
		} else {
			node_control_spinner_rl.setVisibility(View.VISIBLE);
			node_control_light_rl.setVisibility(View.GONE);
			node_control_reportenable_rl.setVisibility(View.GONE);
			node_control_temp_rl.setVisibility(View.GONE);
		}
	}

	private void progressLoading(int i) {
		switch (i) {
		case LOADING_INIT:
			node_control_light_progress.setVisibility(View.VISIBLE);
			node_control_report_proress.setVisibility(View.VISIBLE);
			node_control_temp_proress.setVisibility(View.VISIBLE);
			node_control_spinner_progress.setVisibility(View.VISIBLE);
			break;
		case LOADING_ENDDEVICE:
			node_control_spinner_progress.setVisibility(View.GONE);
			node_control_light_progress.setVisibility(View.VISIBLE);
			node_control_report_proress.setVisibility(View.VISIBLE);
			node_control_temp_proress.setVisibility(View.VISIBLE);
			break;
		case LOADING_ROUTER:
			node_control_spinner_progress.setVisibility(View.GONE);
			node_control_light_progress.setVisibility(View.GONE);
			node_control_report_proress.setVisibility(View.VISIBLE);
			node_control_temp_proress.setVisibility(View.GONE);
			break;
		case LOADING_COOR:
			node_control_spinner_progress.setVisibility(View.GONE);
			node_control_light_progress.setVisibility(View.GONE);
			node_control_report_proress.setVisibility(View.GONE);
			node_control_temp_proress.setVisibility(View.GONE);
			break;
		case LOADING_INIT_END:
			node_control_spinner_progress.setVisibility(View.GONE);
			node_control_light_progress.setVisibility(View.GONE);
			node_control_report_proress.setVisibility(View.GONE);
			node_control_temp_proress.setVisibility(View.GONE);
			break;

		default:
			break;
		}
	}

	/**
	 * @param netAddress
	 * @return 返回这个netAddress的节点对象
	 */
	private ZigBeeNode getTheSelectedNode(String ieeeAddress) {
		return wsnNTChangingMap.get(ieeeAddress);
	}

	// private ZigBeeNode getTheSelectedNode(String ieeeAddress) {
	// ZigBeeNode selectedNode = null;
	// for (int i = 0; i < wsnNTChangingList.size(); i++) {
	// if (wsnNTChangingList.get(i).getIeeeAddress().equals(ieeeAddress)) {
	// selectedNode = wsnNTChangingList.get(i);
	// break;
	// }
	// }
	// return selectedNode;
	// }

}