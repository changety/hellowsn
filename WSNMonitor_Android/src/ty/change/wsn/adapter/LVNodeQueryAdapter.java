package ty.change.wsn.adapter;

import java.util.List;

import com.nostra13.universalimageloader.core.ImageLoader;

import ty.change.wsn.R;
import ty.change.wsn.bean.ZigBeeNode;
import ty.change.wsn.util.Constants;
import ty.change.wsn.util.URL;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

/**
 * @author change_ty
 *
 */
public class LVNodeQueryAdapter extends BaseAdapter {
	private LayoutInflater mInflater;
	private List<ZigBeeNode> list;
	private int layoutID;
	private Context _context;
	
	private ImageLoader imageLoader;

	public LVNodeQueryAdapter(Context context, List<ZigBeeNode> list,
			int layoutID,ImageLoader imageLoader) {
		this._context = context;
		this.mInflater = LayoutInflater.from(_context);
		this.list = list;
		this.imageLoader = imageLoader;
		this.layoutID = layoutID;
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
		TextView nodeType, nodeContent, netAddress, ieeeAddress,
				nodeRefreshTime;
		ImageView nodeImage;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		if (convertView == null) {
			convertView = mInflater.inflate(layoutID, null);
			holder = new ViewHolder();
			holder.nodeType = (TextView) convertView
					.findViewById(R.id.node_listitem_nodetype);
			holder.nodeContent = (TextView) convertView
					.findViewById(R.id.node_listitem_content);
			holder.netAddress = (TextView) convertView
					.findViewById(R.id.node_listitem_netaddress);
			holder.ieeeAddress = (TextView) convertView
					.findViewById(R.id.node_listitem_ieeeaddress);
			holder.nodeRefreshTime = (TextView) convertView
					.findViewById(R.id.node_listitem_refreshtime);
			holder.nodeImage = (ImageView) convertView
					.findViewById(R.id.node_listitem_image);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
//		System.out.println("position:" + position);
		ZigBeeNode node = getItem(position);
		if(node.getType().equals(Constants.ENDDEVICE)){
			holder.nodeType.setText("终端节点");
		}else if (node.getType().equals(Constants.ROUTER)){
			holder.nodeType.setText("路由器节点");
		}else{
			holder.nodeType.setText("协调器节点");
		}
		if(Constants.ENDDEVICE.equals( node.getType())){
			imageLoader.displayImage(URL.ENDDEVICE_IMAGE_URL, holder.nodeImage);
		}else {
			imageLoader.displayImage(URL.ROUTER_IMAGE_URL, holder.nodeImage);
		}
		holder.nodeContent.setText("  ");
		holder.netAddress.setText("网络地址:" + node.getNetAddress());
		holder.ieeeAddress.setText("IEEE:" + node.getIeeeAddress());
		holder.nodeRefreshTime.setText(node.getUpdateTime().toLocaleString());
		// holder.nodeImage.setImageResource(R.drawable.node_image);
		return convertView;
	}
}
