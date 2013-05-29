package ty.change.wsn.adapter;

import java.util.List;

import com.nostra13.universalimageloader.core.ImageLoader;

import ty.change.wsn.R;
import ty.change.wsn.bean.Coordinator;
import ty.change.wsn.bean.Router;
import ty.change.wsn.bean.EndDevice;
import ty.change.wsn.bean.ZigBeeNode;
import ty.change.wsn.util.URL;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

/**
 * @author change_ty
 *
 */
public class LVNodeDetailAdapter extends BaseAdapter {
	private LayoutInflater mInflater;
	private List<? extends ZigBeeNode> list;
	private int layoutID;
	private Context _context;
	private ImageLoader imageLoader;

	public LVNodeDetailAdapter(Context context,
			List<? extends ZigBeeNode> list, int layoutID,ImageLoader imageLoader) {
		this._context = context;
		this.mInflater = LayoutInflater.from(_context);
		this.list = list;
		this.layoutID = layoutID;
		this.imageLoader = imageLoader;
	}

	@Override
	public int getCount() {
		return list.size();
	}

	@Override
	public ZigBeeNode getItem(int position) {
		return list.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	static class ViewHolder {
		TextView node_query_listitem_netaddress,
		node_query_listitem_ieeeaddress;
		ImageView node_detail_query_listitem_image, node_query_listitem_relay;
		RelativeLayout node_query_sensor_info_rl;
		TextView node_query_listitem_temp, node_query_listitem_lgiht,node_query_listitem_voltage;

		TextView node_query_reporttime, node_query_listitem_location;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		if (convertView == null) {
			convertView = mInflater.inflate(layoutID, null);
			holder = new ViewHolder();
			holder.node_query_listitem_netaddress = (TextView) convertView
					.findViewById(R.id.node_query_listitem_netaddress);
			holder.node_query_listitem_ieeeaddress = (TextView) convertView
					.findViewById(R.id.node_query_listitem_ieeeaddress);

			holder.node_detail_query_listitem_image = (ImageView) convertView
					.findViewById(R.id.node_detail_query_listitem_image);
			holder.node_query_listitem_relay = (ImageView) convertView
					.findViewById(R.id.node_query_listitem_relay);
			holder.node_query_listitem_location = (TextView) convertView
					.findViewById(R.id.node_query_listitem_location);
			holder.node_query_reporttime = (TextView) convertView
					.findViewById(R.id.node_query_reporttime);

			holder.node_query_sensor_info_rl = (RelativeLayout) convertView
					.findViewById(R.id.node_query_sensor_info_rl);

			holder.node_query_listitem_temp = (TextView) convertView
					.findViewById(R.id.node_query_listitem_temp);
			

			holder.node_query_listitem_voltage = (TextView) convertView
					.findViewById(R.id.node_query_listitem_voltage);

			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		ZigBeeNode node = getItem(position);
		if (node instanceof Coordinator) {
			holder.node_query_sensor_info_rl.setVisibility(View.GONE);
			holder.node_query_reporttime.setText((node.getJoinTime()).toLocaleString());
			imageLoader.displayImage(URL.ROUTER_IMAGE_URL, holder.node_detail_query_listitem_image);  
		} else if (node instanceof EndDevice) {
			holder.node_query_sensor_info_rl.setVisibility(View.VISIBLE);
			holder.node_query_listitem_temp.setText(((EndDevice) node)
					.getTemp()+"℃");
			holder.node_query_listitem_voltage.setText(((EndDevice) node)
					.getVoltage().substring(0, 3)+"V");
			holder.node_query_listitem_relay.setImageResource(R.drawable.bulb_off);
			
			holder.node_query_reporttime.setText((node.getUpdateTime()).toLocaleString());
			imageLoader.displayImage(URL.ENDDEVICE_IMAGE_URL, holder.node_detail_query_listitem_image);  
		} else if (node instanceof Router) {
			holder.node_query_sensor_info_rl.setVisibility(View.GONE);
			holder.node_query_reporttime.setText((node.getUpdateTime()).toLocaleString());
			imageLoader.displayImage(URL.ROUTER_IMAGE_URL, holder.node_detail_query_listitem_image);  

		}
		holder.node_query_listitem_netaddress.setText(node.getNetAddress());
		holder.node_query_listitem_ieeeaddress.setText(node.getIeeeAddress());
		
		holder.node_query_listitem_location.setText("  ");
		// holder.nodeImage.setImageResource(R.drawable.node_image);
		return convertView;
	}
}
