package ty.change.wsn.activity;

import ty.change.wsn.dao.WSNDatabaseHelper;
import android.os.Bundle;

import com.j256.ormlite.android.apptools.OpenHelperManager;
import com.j256.ormlite.support.ConnectionSource;

/**
 * @author change_ty
 *
 */
public class ORMBaseActivity extends BaseActivity {
	protected WSNDatabaseHelper databaseHelper = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	}

	@Override
	protected void initView() {
		// TODO Auto-generated method stub

	}

	@Override
	protected void initData() {
		// TODO Auto-generated method stub

	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		/*
		 * You'll need this in your class to release the helper when done.
		 */
		if (databaseHelper != null) {
			OpenHelperManager.releaseHelper();
			databaseHelper = null;
		}
	}

	/**
	 * You'll need this in your class to get the helper from the manager once
	 * per class.
	 */
	private WSNDatabaseHelper getHelper() {
		if (databaseHelper == null) {
			databaseHelper = OpenHelperManager.getHelper(this,
					WSNDatabaseHelper.class);
		}
		return databaseHelper;
	}

	/**
	 * Get a connection source for this action.
	 */
	protected ConnectionSource getConnectionSource() {
		return getHelper().getConnectionSource();
	}

}
