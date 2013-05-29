package ty.change.wsn.activity;

import ty.change.wsn.R;
import android.app.ActivityGroup;
import android.content.Intent;
import android.os.Bundle;
import android.widget.TabHost;
import android.widget.TabHost.OnTabChangeListener;
import android.widget.TabHost.TabSpec;

/**
 * @author change_ty
 *
 */
public class NodeMainActivity extends ActivityGroup {
	public TabHost tab_host;
	private int tab;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.node_main_tabhost);
		tab = getIntent().getIntExtra("tab", 0);
		tab_host = (TabHost) findViewById(R.id.edit_item_tab_host);
		tab_host.setup(this.getLocalActivityManager());

		TabSpec tsNodeQuery = tab_host.newTabSpec("节点数据查询");
		tsNodeQuery.setIndicator("节点数据查询");
		tsNodeQuery.setContent(new Intent(this, NodeQueryActivity.class));
		tab_host.addTab(tsNodeQuery);

		TabSpec tsNodeNetQuery = tab_host.newTabSpec("节点网络查询");
		tsNodeNetQuery.setIndicator("节点网络查询");
		tsNodeNetQuery.setContent(new Intent(this, NodeNetActivity.class));
		tab_host.addTab(tsNodeNetQuery);

		TabSpec tsNodeControl = tab_host.newTabSpec("节点状态控制");
		tsNodeControl.setIndicator("节点状态控制");
		tsNodeControl.setContent(new Intent(this, NodeControlActivity.class));

		tab_host.addTab(tsNodeControl);
		tab_host.setCurrentTab(tab);

		tab_host.setOnTabChangedListener(changeLis);

	}

	OnTabChangeListener changeLis = new OnTabChangeListener() {
		@Override
		public void onTabChanged(String tabId) {
			setTitle(tabId);
		}
	};
}
