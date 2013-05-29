package ty.change.wsn.activity;

import java.util.Date;

import org.apache.http.entity.StringEntity;

import ty.change.wsn.R;
import ty.change.wsn.app.AppException;
import ty.change.wsn.bean.User;
import ty.change.wsn.util.Constants;
import ty.change.wsn.util.ParamUtils;
import ty.change.wsn.util.URL;
import ty.change.wsn.util.ViewUtility;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
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
public class UserRegisterActivity extends BaseActivity implements
		OnClickListener {
	private AsyncHttpClient httpClient;
	private Dialog m_Dialog;
	private Spinner spinner;
	private ArrayAdapter<String> adapter;
	private Button submit;
	private Button reset;
	private Button reback;
	private EditText username, password, repassword, email;
	private String Username, Password, Email, Authority;
	private boolean isRegistering;
	private TextView registerTitle, usernameInfo, passwordInfo, repasswordInfo,
			emailInfo;
	private SharedPreferences sharedPreferences = null;
	private JSONObject json4UserRegusterResult = null;
	private static final String[] authorityChoice = { "管理员", "普通用户" };
	private User newUser;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.register);
		initPage();
	}

	@Override
	protected void initView() {
		username = (EditText) findViewById(R.id.registerusername);
		password = (EditText) findViewById(R.id.registerpassword);
		repassword = (EditText) findViewById(R.id.registerrepassword);
		email = (EditText) findViewById(R.id.registeremailet);
		registerTitle = (TextView) findViewById(R.id.registertitle);
		usernameInfo = (TextView) findViewById(R.id.registerusernameinfo);
		passwordInfo = (TextView) findViewById(R.id.registerpasswordinfo);
		repasswordInfo = (TextView) findViewById(R.id.registerrepasswordinfo);
		emailInfo = (TextView) findViewById(R.id.registeremailinfo);
		spinner = (Spinner) findViewById(R.id.usertype);
		submit = (Button) findViewById(R.id.submitbutton);
		reset = (Button) findViewById(R.id.resetbutton);
		reback = (Button) findViewById(R.id.rebackbutton);
		adapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_spinner_item, authorityChoice);
		adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		spinner.setAdapter(adapter);
		spinner.setVisibility(View.VISIBLE);
		submit.setOnClickListener(this);
		reset.setOnClickListener(this);
		reback.setOnClickListener(this);
		username.setOnFocusChangeListener(new OnUsernameChangeListener());
		password.setOnFocusChangeListener(new OnPasswordChangeListener());
		repassword.setOnFocusChangeListener(new OnRepasswordChangeListener());
		email.setOnFocusChangeListener(new OnEmailChangeListener());
	}

	@Override
	protected void initData() {
		sharedPreferences = this.getSharedPreferences(
				Constants.SHARED_PREFERENCE_NAME, Context.MODE_PRIVATE);
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.submitbutton:
			checkAndRegister();
			break;
		case R.id.resetbutton:
			username.setText("");
			password.setText("");
			repassword.setText("");
			email.setText("");
			break;
		case R.id.rebackbutton:
			ViewUtility.NavigateActivity(UserRegisterActivity.this,
					UserLoginActivity.class, true);
			break;
		default:
			break;
		}
	}

	private void checkAndRegister() {
		if (!checkUsernameOrPassword(username.getText().toString())) {
			username.setText("");
			Toast.makeText(getApplicationContext(), "用户名长度为3-6位,且只能包含字母数字",
					Toast.LENGTH_SHORT).show();
		} else if (!checkUsernameOrPassword(password.getText().toString())) {
			password.setText("");
			repassword.setText("");
			Toast.makeText(getApplicationContext(), "密码长度为3-6位,且只能包含字母数字",
					Toast.LENGTH_SHORT).show();
		} else if (!password.getText().toString()
				.equals(repassword.getText().toString())) {
			repassword.setText("");
			Toast.makeText(getApplicationContext(), "两次密码不一样",
					Toast.LENGTH_SHORT).show();
		} else if (!checkEmail(email.getText().toString())) {
			email.setText("");
			Toast.makeText(getApplicationContext(), "请输入合法邮箱",
					Toast.LENGTH_SHORT).show();
		} else {
			Username = username.getText().toString();
			Password = password.getText().toString();
			Email = email.getText().toString();
			Authority = authorityChoice[spinner.getSelectedItemPosition()];
			if ("管理员".equals(Authority)) {
				Authority = "admin";
			} else {
				Authority = "normal";
			}
			newUser = new User(Username,Password,Email,Authority);
			register(ParamUtils.SE4Register(newUser));
		}

	}

	private void register(StringEntity registerPrama) {
		m_Dialog = ProgressDialog.show(UserRegisterActivity.this, "请等待...",
				"正在注册...", true);
		m_Dialog.setCancelable(true);
		httpClient = new AsyncHttpClient();
		httpClient.post(UserRegisterActivity.this, URL.REGISTER_URL,
				registerPrama, null, new AsyncHttpResponseHandler() {
					@Override
					public void onStart() {
						isRegistering = true;
						registerTitle.setText("用户信息注册中...");
						m_Dialog.show();
					}

					public void onSuccess(String response) {
						json4UserRegusterResult = JSON.parseObject(response);
						System.out.println(json4UserRegusterResult.toString());
						boolean flags = json4UserRegusterResult
								.getBoolean("ActionResult");
						if (flags) {
							Toast.makeText(UserRegisterActivity.this, "注册成功",
									Toast.LENGTH_SHORT).show();
							SharedPreferences.Editor editor = sharedPreferences
									.edit();
							// 将登入信息 写到sharedPreferences里.
							editor.putString("lastLogin",
									new Date().toLocaleString())
									.putString(Constants.XMPP_USERNAME,
											Username)
									.putString(Constants.XMPP_PASSWORD,
											Password)
									.putString(Constants.XMPP_EMAIL, Email)
									.putString(Constants.XMPP_AUTHORITY,
											Authority)
									.putString(Constants.XMPP_REGISTER_TIME,
											new Date().toLocaleString())
									.putString(Constants.XMPP_LASTLOGIN_TIME,
											new Date().toLocaleString())
									.commit();
							ViewUtility.NavigateActivity(
									UserRegisterActivity.this,
									MainActivity.class, true);
						} else {
							Toast.makeText(UserRegisterActivity.this,
									"用户名重复，请重新注册", Toast.LENGTH_SHORT).show();
						}
					}

					@Override
					public void onFinish() {
						isRegistering = false;
						m_Dialog.dismiss();
					}

					@Override
					public void onFailure(Throwable error, String content) {
						super.onFailure(error, content);
						(AppException.network((Exception) error))
								.makeToast(UserRegisterActivity.this);
					}
				});

	}

	// 用来验证用户名 密码的合法性的正则表达式
	private boolean checkUsernameOrPassword(String s) {
		return s.matches("[a-z0-9A-Z]{3,6}");
	}

	// 验证邮箱是否合法
	private boolean checkEmail(String s) {
		return s.matches("[\\w[.-]]+@[\\w[.-]]+\\.[\\w]+");
	}

	class OnUsernameChangeListener implements OnFocusChangeListener {
		@Override
		public void onFocusChange(View v, boolean hasFocus) {
			if (hasFocus) {
				usernameInfo.setVisibility(0);
			} else {
				usernameInfo.setVisibility(4);
			}
		}
	}

	class OnPasswordChangeListener implements OnFocusChangeListener {
		@Override
		public void onFocusChange(View v, boolean hasFocus) {
			if (hasFocus) {
				passwordInfo.setVisibility(0);
			} else {
				passwordInfo.setVisibility(4);
			}
		}
	}

	class OnRepasswordChangeListener implements OnFocusChangeListener {
		@Override
		public void onFocusChange(View v, boolean hasFocus) {
			if (hasFocus) {
				repasswordInfo.setVisibility(0);
			} else {
				repasswordInfo.setVisibility(4);
			}
		}
	}

	class OnEmailChangeListener implements OnFocusChangeListener {
		@Override
		public void onFocusChange(View v, boolean hasFocus) {
			if (hasFocus) {
				emailInfo.setVisibility(0);
			} else {
				emailInfo.setVisibility(4);
			}
		}
	}

	@Override
	public void onBackPressed() {
		if (isRegistering) {
			m_Dialog.dismiss();
			httpClient.cancelRequests(UserRegisterActivity.this, true);
		} else {
			ViewUtility.NavigateActivity(UserRegisterActivity.this,
					UserLoginActivity.class, true);
		}
	}
}
