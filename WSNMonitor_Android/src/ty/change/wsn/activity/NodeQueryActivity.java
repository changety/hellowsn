package ty.change.wsn.activity;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import ty.change.wsn.R;
import ty.change.wsn.adapter.LVNodeQueryAdapter;
import ty.change.wsn.app.AppException;
import ty.change.wsn.bean.ZigBeeNode;
import ty.change.wsn.uicomponent.PullToRefreshListView;
import ty.change.wsn.util.Constants;
import ty.change.wsn.util.ParamUtils;
import ty.change.wsn.util.URL;
import ty.change.wsn.util.ViewUtility;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;

import com.alibaba.fastjson.JSON;
import com.loopj.android.http.AsyncHttpClient;
import com.loopj.android.http.AsyncHttpResponseHandler;
/**
 * @author change_ty
 *
 */
public class NodeQueryActivity extends ImageBaseActivity implements
		OnClickListener {

	private PullToRefreshListView mLvNodeInfo;
	private final List<ZigBeeNode> nodesDataList = new ArrayList<ZigBeeNode>();
	private LVNodeQueryAdapter lvNodeQueryAdapter;
	private SharedPreferences sharedPreferences = null;
	private AsyncHttpClient httpClient;

	private String username, command;

	private Button frame_btn_allnodes, frame_btn_coordinator, frame_btn_router,
			frame_btn_enddevice;

	private int curLvDataState;

	private ZigBeeNode selectedNode;

	private final static int DATA_LOAD_ING = 0x001;
	private final static int DATA_LOAD_COMPLETE = 0x002;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.node_query_activity);
		initPage();
	}

	// 初始化视图控件
	protected void initView() {
		sharedPreferences = this.getSharedPreferences(
				Constants.SHARED_PREFERENCE_NAME, Context.MODE_PRIVATE);
		username = sharedPreferences.getString(Constants.XMPP_USERNAME, null);
		command = Constants.COMMAND_WSN_STRUCT_REFRESH;

		frame_btn_allnodes = (Button) findViewById(R.id.frame_btn_allnodes);
		frame_btn_coordinator = (Button) findViewById(R.id.frame_btn_coordinator);
		frame_btn_router = (Button) findViewById(R.id.frame_btn_router);
		frame_btn_enddevice = (Button) findViewById(R.id.frame_btn_enddevice);
		frame_btn_allnodes.setOnClickListener(this);
		frame_btn_coordinator.setOnClickListener(this);
		frame_btn_router.setOnClickListener(this);
		frame_btn_enddevice.setOnClickListener(this);

		mLvNodeInfo = (PullToRefreshListView) findViewById(R.id.frame_listview_nodes);
		lvNodeQueryAdapter = new LVNodeQueryAdapter(this, nodesDataList,
				R.layout.node_query_listitem, imageLoader);
		mLvNodeInfo.setAdapter(lvNodeQueryAdapter);

		mLvNodeInfo.setOnScrollListener(new AbsListView.OnScrollListener() {
			public void onScrollStateChanged(AbsListView view, int scrollState) {
				mLvNodeInfo.onScrollStateChanged(view, scrollState);
			}

			public void onScroll(AbsListView view, int firstVisibleItem,
					int visibleItemCount, int totalItemCount) {
				mLvNodeInfo.onScroll(view, firstVisibleItem, visibleItemCount,
						totalItemCount);
			}
		});
		mLvNodeInfo
				.setOnRefreshListener(new PullToRefreshListView.OnRefreshListener() {
					public void onRefresh() {
						loadNodesData();
					}
				});
		mLvNodeInfo.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				System.out.println("position:" + position);
				if (position == 0)
					return;
				selectedNode = lvNodeQueryAdapter.getItem(position - 1);
				ViewUtility.NavigateActivity(NodeQueryActivity.this,
						NodeQueryDetailActivity.class, selectedNode, false);
			}
		});
	}

	// 初始化控件数据
	protected void initData() {
		loadNodesData();
	}

	private void loadNodesData() {
		httpClient = new AsyncHttpClient();
		httpClient.post(this, URL.WSN_STRUCT_REFRESH_URL,
				ParamUtils.SE4NodeQuery(username, command), null,
				new AsyncHttpResponseHandler() {
					@Override
					public void onStart() {
					}
					public void onSuccess(String response) {
						nodesDataList.clear();
						nodesDataList.addAll(JSON.parseArray(response,
								ZigBeeNode.class));
						lvNodeQueryAdapter.notifyDataSetChanged();
					}
					@Override
					public void onFinish() {
						mLvNodeInfo
								.onRefreshComplete(getString(R.string.pull_to_refresh_update)
										+ new Date().toLocaleString());
						mLvNodeInfo.setSelection(0);
					}

					@Override
					public void onFailure(Throwable error, String content) {
						super.onFailure(error, content);
						(AppException.network((Exception) error))
								.makeToast(NodeQueryActivity.this);
					}
				});
	}
	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.frame_btn_allnodes:
			//TODO
			break;
		case R.id.frame_btn_coordinator:
			//TODO
			break;
		case R.id.frame_btn_router:
			//TODO
			break;
		case R.id.frame_btn_enddevice:
			//TODO
			break;
		default:
			break;
		}
	}
}