package ty.change.wsn.activity;

import ty.change.wsn.R;
import android.app.Activity;
import android.os.Bundle;

/**
 * @author change_ty
 *
 */
public abstract class BaseActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.help_activity);
	}
	
	/**
	 * 页面初始化
	 */
	protected void initPage() {
		//载入日期控件
		initView();
		initData();
	}
	/**
	 * 组件初始化
	 */
	protected abstract void initView();
	/**
	 * 数据初始化
	 */
	protected abstract void initData();
	
}
