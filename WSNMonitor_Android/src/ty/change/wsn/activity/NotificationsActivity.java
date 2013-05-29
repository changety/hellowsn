package ty.change.wsn.activity;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import ty.change.wsn.R;
import ty.change.wsn.adapter.LVNotiAdapter;
import ty.change.wsn.bean.WSNNoti;
import ty.change.wsn.dao.DaoHelper;
import ty.change.wsn.dao.DaoHelperImpl;
import ty.change.wsn.util.ViewUtility;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Vibrator;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.ListView;
import android.widget.TextView;

/**
 * @author change_ty
 *
 */
public class NotificationsActivity extends
		ORMBaseActivity implements OnItemClickListener,
		OnItemLongClickListener {
	private TextView notiTitle;
	private ListView notiListview;
	private LVNotiAdapter notiLVadapter;
	private final List<WSNNoti> notilist = new ArrayList<WSNNoti>();
	private DaoHelper daoHelpImpl;
	private WSNNoti selectedWSNNoti;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.notification_activity);
		daoHelpImpl = DaoHelperImpl.getInstance(getConnectionSource());
		initPage();
	}
	@Override
	protected void initView() {
		notiTitle = (TextView) findViewById(R.id.noti_title);
		notiListview = (ListView) findViewById(R.id.noti_listview);
		notiListview.setOnItemLongClickListener(this);
		notiListview.setOnItemClickListener(this);
		notiLVadapter = new LVNotiAdapter(this, notilist,
				R.layout.notification_listitem);
		notiListview.setAdapter(notiLVadapter);

	}
	@Override
	protected void initData() {
		notilist.clear();
		for (Iterator<WSNNoti> iter = daoHelpImpl.getAllNotices(); iter
				.hasNext();) {
			WSNNoti noti = iter.next();
			notilist.add(noti);
		}
		if (notilist.size() == 0) {
			notiTitle.setText("暂无推送消息");
		} else {
			notiTitle.setText("有" + notilist.size() + "条推送消息");
		}
		notiLVadapter.notifyDataSetChanged();
	}

	private void freshData() {
		initData();
	}

	@Override
	public boolean onItemLongClick(AdapterView<?> parent, View view,
			int position, long id) {
		selectedWSNNoti = notiLVadapter.getItem(position);
		showDialog();
		return false;
	}

	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		selectedWSNNoti = notiLVadapter.getItem(position);
		ViewUtility.NavigateActivity(this, NotificationDetailsActivity.class,
				selectedWSNNoti);
	}

	private void showDialog() {
		Vibrator vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
		vibrator.vibrate(50);
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle("删除该条推送消息:");
		builder.setCancelable(true)
				.setPositiveButton("删除", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						daoHelpImpl.deleteWSNNoti(selectedWSNNoti);
						freshData();
					}
				})
				.setNegativeButton("取消", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						dialog.cancel();
					}
				});
		builder.show();
	}

}
