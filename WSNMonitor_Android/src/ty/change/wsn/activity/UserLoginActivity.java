package ty.change.wsn.activity;

import java.util.Date;

import ty.change.wsn.R;
import ty.change.wsn.app.AppException;
import ty.change.wsn.util.Constants;
import ty.change.wsn.util.ParamUtils;
import ty.change.wsn.util.URL;
import ty.change.wsn.util.ViewUtility;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
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
public class UserLoginActivity extends BaseActivity implements OnClickListener {

	private Button loginButton, exitButton, registerButton;
	private EditText mUsername, mPwd, mIP;
	private TextView logintv;
	private ProgressDialog m_Dialog;
	private JSONObject json4login = null;
	private AsyncHttpClient httpClient;
	private SharedPreferences sharedPreferences = null;
	private boolean isLogning = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.login_activity);
		initPage();
	}

	@Override
	protected void initView() {
		logintv = (TextView) findViewById(R.id.logintv);
		mUsername = (EditText) findViewById(R.id.login_username_edit);
		mPwd = (EditText) findViewById(R.id.login_password_edit);
		mIP = (EditText) findViewById(R.id.login_ip_edit);
		loginButton = (Button) findViewById(R.id.loginlogin);
		exitButton = (Button) findViewById(R.id.loginexit);
		registerButton = (Button) findViewById(R.id.loginregister);
		loginButton.setOnClickListener(this);
		exitButton.setOnClickListener(this);
		registerButton.setOnClickListener(this);
	}

	@Override
	protected void initData() {
		sharedPreferences = this.getSharedPreferences(
				Constants.SHARED_PREFERENCE_NAME, Context.MODE_PRIVATE);
		String spUsername = sharedPreferences.getString(
				Constants.XMPP_USERNAME, null);
		String lastLogin = sharedPreferences.getString(
				Constants.XMPP_LASTLOGIN_TIME, null);
		if (lastLogin == null) {
			logintv.setText("欢迎您，您是第一次使用该软件，请注册账号");
		} else {
			logintv.setText("欢迎您:" + spUsername + "您上次的登入时间是:" + lastLogin);
			mUsername.setText(spUsername);
		}
		
		mIP.setText("10.0.2.2");
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.loginlogin:
			Constants.WSN_IP = "http://" + mIP.getText().toString() + ":8080/";
			Constants.XMPP_IP = mIP.getText().toString();
			String username = mUsername.getText().toString();
			String pwd = mPwd.getText().toString();
			userLogin(username, pwd);
			break;
		case R.id.loginregister:
			ViewUtility.NavigateActivity(UserLoginActivity.this,
					UserRegisterActivity.class, true);
			break;
		case R.id.loginexit:
			System.exit(0);
			break;
		default:
			break;
		}
	}

	/**
	 * 一.发送http请求，并传递相应的参数；
	 * 二.获取http返回的状态，根据返回的状态，如404错误，500错误，连接超时，请求异常等，并在界面提示相关状态；
	 * 三.web服务器端封装数据并返回一定格式的数据对象，例如封装json对象； 四.http状态返回正常，取出正确的参数并解析，如解析json对象；
	 * 五.解析服务器端返回的数据后显示在android相应的控件或存储本地数据，提示操作完成等。
	 * 
	 * @see http://notfatboy.iteye.com/blog/686260 http具体执行过程 java native method
	 *      api HttpURLConnection
	 * @see http
	 *      ://developer.android.com/reference/java/net/HttpURLConnection.html
	 * @param username
	 * @param pwd
	 */
	private void userLogin(final String username, final String pwd) {
		m_Dialog = new ProgressDialog(this);
		m_Dialog.setTitle("登入中");
		m_Dialog.setMessage("请稍后...");
		m_Dialog.setCancelable(true);

		httpClient = new AsyncHttpClient();
		httpClient.post(UserLoginActivity.this, URL.LOGIN_URL,
				ParamUtils.SE4Login(username, pwd), null,
				new AsyncHttpResponseHandler() {
					@Override
					public void onStart() {
						isLogning = true;
						m_Dialog.show();
					}

					public void onSuccess(String response) {
						json4login = JSON.parseObject(response);
						boolean flag = json4login.getBoolean("ActionResult");
						String reason = json4login.getString("Reason");
						Toast.makeText(UserLoginActivity.this, reason,
								Toast.LENGTH_SHORT).show();
						if (flag) {
							SharedPreferences.Editor editor = sharedPreferences
									.edit();
							// 将登入信息 写到sharedPreferences里.
							editor.putString(Constants.XMPP_LASTLOGIN_TIME,
									new Date().toLocaleString())
									.putString(Constants.XMPP_USERNAME,
											username)
									.putString(Constants.XMPP_PASSWORD, pwd)
									.commit();
							ViewUtility.NavigateActivity(
									UserLoginActivity.this, MainActivity.class);
							UserLoginActivity.this.finish();
						}
					}

					@Override
					public void onFinish() {
						isLogning = false;
						m_Dialog.dismiss();
					}

					@Override
					public void onFailure(Throwable error, String content) {
						super.onFailure(error, content);
						isLogning = false;
						(AppException.network((Exception) error))
								.makeToast(UserLoginActivity.this);
					}
				});

	}

	@Override
	public void onBackPressed() {
		if (isLogning) {
			m_Dialog.dismiss();
			httpClient.cancelRequests(UserLoginActivity.this, true);
		} else {
			super.onBackPressed();
		}
	}

}
