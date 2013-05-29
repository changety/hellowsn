package ty.change.wsn.adapter;

import java.util.List;

import ty.change.wsn.R;
import ty.change.wsn.bean.WSNNoti;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

/**
 * @author change_ty
 *
 */
public class LVNotiAdapter extends BaseAdapter {
	private LayoutInflater mInflater;
	private List<WSNNoti> list;
	private int layoutID;
	private Context _context;

	public LVNotiAdapter(Context context,
			List<WSNNoti> list, int layoutID) {
		this._context = context;
		this.mInflater = LayoutInflater.from(_context);
		this.list = list;
		this.layoutID = layoutID;
	}

	@Override
	public int getCount() {
		return list.size();
	}

	@Override
	public WSNNoti getItem(int position) {
		return list.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	static class ViewHolder {
		TextView noti_listitem_title, noti_listitem_notidetail,
		noti_listitem_time;
	}
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		if (convertView == null) {
			convertView = mInflater.inflate(layoutID, null);
			holder = new ViewHolder();
			holder.noti_listitem_title = (TextView) convertView
					.findViewById(R.id.noti_listitem_title);
			holder.noti_listitem_notidetail = (TextView) convertView
					.findViewById(R.id.noti_listitem_notidetail);
			holder.noti_listitem_time = (TextView) convertView
					.findViewById(R.id.noti_listitem_time);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		WSNNoti noti = getItem(position);
		holder.noti_listitem_title.setText(noti.getTitle());
		holder.noti_listitem_notidetail.setText(noti.getMessage());
		holder.noti_listitem_time.setText(noti.getTime());
		return convertView;
	}
}
