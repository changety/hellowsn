package ty.change.wsn.activity;

import ty.change.wsn.R;
import ty.change.wsn.app.AppException;
import ty.change.wsn.util.Constants;
import ty.change.wsn.util.ParamUtils;
import ty.change.wsn.util.URL;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ProgressBar;
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
public class SerialCommControlActivity extends BaseActivity {
	private static final String[] statusControl = { Constants.COMM_OPEN,
			Constants.COMM_CLOSE };
	private TextView status;
	private Spinner spinner;
	private ArrayAdapter<String> adapter;
	private Button statusCheckButton;
	private Button confirmButton;
	private String mUsername;
	private int mCommand;
	private ProgressBar mDialog_1;
	private JSONObject json4CommStateResult = null;
	private SharedPreferences sharedPreferences = null;
	private AsyncHttpClient httpClient;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.serial_comm_control);
		initPage();
	}

	@Override
	protected void initView() {
		status = (TextView) findViewById(R.id.monitorActivityStatusShowTextView);
		mDialog_1 = (ProgressBar) findViewById(R.id.serial_progress_1);
		status.setText("");
		spinner = (Spinner) findViewById(R.id.monitorActivityStatusControlSpinner);
		statusCheckButton = (Button) findViewById(R.id.monitorActivityControlStatusButton);
		confirmButton = (Button) findViewById(R.id.monitorActivityControlStatusConfirmButton);
		adapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_spinner_item, statusControl);
		adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		spinner.setAdapter(adapter);
		spinner.setVisibility(View.VISIBLE);
		statusCheckButton
				.setOnClickListener(new StatusCheckButtonClickListener());
		confirmButton.setOnClickListener(new ConfirmButtonClickListener());
	}

	@Override
	protected void initData() {
		sharedPreferences = this.getSharedPreferences(
				Constants.SHARED_PREFERENCE_NAME, Context.MODE_PRIVATE);
		mUsername = sharedPreferences.getString(Constants.XMPP_USERNAME, null);
	}

	class StatusCheckButtonClickListener implements OnClickListener {
		@Override
		public void onClick(View v) {
			mCommand = Constants.COMMAND_CHECK_COMM_STATE;
			serialControl(mUsername, mCommand);
		}
	}

	class ConfirmButtonClickListener implements OnClickListener {
		@Override
		public void onClick(View v) {
			if (Constants.COMM_OPEN
					.equals(spinner.getSelectedItem().toString())) {
				mCommand = Constants.COMMAND_COMM_OPEN;
			} else {
				mCommand = Constants.COMMAND_COMM_CLOSE;
			}
			serialControl(mUsername, mCommand);
		}
	}

	private void serialControl(final String mUsername, final int mCommand) {
		httpClient = new AsyncHttpClient();
		httpClient.post(SerialCommControlActivity.this,
				URL.SERIAL_COMM_CONTROL_URL,
				ParamUtils.SE4SerialCommContr(mUsername, mCommand), null,
				new AsyncHttpResponseHandler() {
					@Override
					public void onStart() {
						mDialog_1.setVisibility(View.VISIBLE);
						status.setText("操作中...");
					}

					public void onSuccess(String response) {
						int actionResult;
						String reason = "";
						json4CommStateResult = JSON.parseObject(response);
						actionResult = json4CommStateResult
								.getIntValue(Constants.RESULT);
						reason = json4CommStateResult
								.getString(Constants.REASON);
						Toast.makeText(getApplicationContext(), reason,
								Toast.LENGTH_SHORT).show();
						switch (actionResult) {
						case Constants.COMMAND_CHECK_COMM_STATE:
							status.setText(reason);
							break;
						case Constants.COMMAND_COMM_OPEN:
							status.setText("已打开");
							break;
						case Constants.COMMAND_COMM_CLOSE:
							status.setText("已关闭");
							break;
						default:
							break;
						}

					}

					@Override
					public void onFinish() {
						mDialog_1.setVisibility(View.GONE);
					}

					@Override
					public void onFailure(Throwable error, String content) {
						super.onFailure(error, content);
						(AppException.network((Exception) error))
								.makeToast(SerialCommControlActivity.this);
					}
				});

	}
}
