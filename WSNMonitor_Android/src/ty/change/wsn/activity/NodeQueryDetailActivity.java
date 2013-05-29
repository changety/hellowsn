package ty.change.wsn.activity;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import ty.change.wsn.R;
import ty.change.wsn.adapter.LVNodeDetailAdapter;
import ty.change.wsn.app.AppException;
import ty.change.wsn.bean.Coordinator;
import ty.change.wsn.bean.Router;
import ty.change.wsn.bean.EndDevice;
import ty.change.wsn.bean.ZigBeeNode;
import ty.change.wsn.uicomponent.PullToRefreshListView;
import ty.change.wsn.uicomponent.PullToRefreshListView.LvState;
import ty.change.wsn.util.Constants;
import ty.change.wsn.util.ParamUtils;
import ty.change.wsn.util.URL;
import ty.change.wsn.util.ViewUtility;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Vibrator;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.alibaba.fastjson.JSON;
import com.loopj.android.http.AsyncHttpClient;
import com.loopj.android.http.AsyncHttpResponseHandler;

/**
 * @author change_ty
 */
public class NodeQueryDetailActivity extends ImageBaseActivity implements
		OnClickListener,OnItemClickListener {

	private Context thisContext;
	private AsyncHttpClient httpClient;

	// head
	private FrameLayout mHeader;
	private ImageView mBack;
	private ImageView mControl;
	private TextView mHeadTitle;
	private ProgressBar mHeadProgressbar;

	// pull
	private PullToRefreshListView mLvNodeDetail;
	private LVNodeDetailAdapter lvNodeDetailAdapter;
	private final List<ZigBeeNode> lvNodeDetailData = new ArrayList<ZigBeeNode>();
	private List<? extends ZigBeeNode> requestNodeDetailData;
	// lvfooter
	private LinearLayout lvNodeDetail_footer;
	private TextView lvNodeDetail_foot_more;
	private ProgressBar lvNodeDetail_foot_progress;

	private LvState curLvDataState = LvState.LISTVIEW_DATA_MORE;
	private final static int DATA_LOAD_ING = 0x001;
	private final static int DATA_LOAD_FINISH = 0x002;

	private ZigBeeNode selectedNode;
	private SharedPreferences sharedPreferences = null;
	private String username;
	private String ieeeAddress;
	private String nodeType;
	private int id = -1;
	private int requestTime = 0;
	private int pageNum = 10;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.node_query_detail_activity);
		initPage();

	}

	// 初始化视图控件
	protected void initView() {
		thisContext = NodeQueryDetailActivity.this;
		sharedPreferences = this.getSharedPreferences(
				Constants.SHARED_PREFERENCE_NAME, Context.MODE_PRIVATE);
		username = sharedPreferences.getString(Constants.XMPP_USERNAME, null);
		selectedNode = (ZigBeeNode) getIntent().getSerializableExtra(
				Constants.ZIGBEE_NODE);
		ieeeAddress = selectedNode.getIeeeAddress();
		nodeType = selectedNode.getType();
		System.out.println(nodeType);

		// 标题栏
		mHeader = (FrameLayout) findViewById(R.id.node_detail_header_fl);
		mBack = (ImageView) findViewById(R.id.node_detail_back);
		mControl = (ImageView) findViewById(R.id.node_detail_control);
		mHeadTitle = (TextView) findViewById(R.id.node_detail_head_title);
		mBack.setOnClickListener(this);
		mControl.setOnClickListener(this);

		mHeadProgressbar = (ProgressBar) findViewById(R.id.node_detail_head_progress);

		lvNodeDetail_footer = (LinearLayout) getLayoutInflater().inflate(
				R.layout.pull_listview_footer, null);
		lvNodeDetail_foot_more = (TextView) lvNodeDetail_footer
				.findViewById(R.id.listview_foot_more);
		lvNodeDetail_foot_progress = (ProgressBar) lvNodeDetail_footer
				.findViewById(R.id.listview_foot_progress);

		lvNodeDetailAdapter = new LVNodeDetailAdapter(thisContext,
				lvNodeDetailData, R.layout.node_detail_query_listitem,
				imageLoader);

		mLvNodeDetail = (PullToRefreshListView) findViewById(R.id.frame_listview_nodesdetail);
		mLvNodeDetail.setOnItemClickListener(this);
		mLvNodeDetail.addFooterView(lvNodeDetail_footer);// 添加底部视图
		mLvNodeDetail.setAdapter(lvNodeDetailAdapter);
		mLvNodeDetail
				.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
					@Override
					public boolean onItemLongClick(AdapterView<?> parent,
							View view, int position, long id) {
						showDialog();
						return true;
					}
				});
		mLvNodeDetail.setOnScrollListener(new AbsListView.OnScrollListener() {
			public void onScrollStateChanged(AbsListView view, int scrollState) {
				mLvNodeDetail.onScrollStateChanged(view, scrollState);

				// 数据为空--不用继续下面代码了
				if (lvNodeDetailData.size() == 0)
					return;

				// 判断是否滚动到底部
				boolean scrollEnd = false;
				try {
					if (view.getPositionForView(lvNodeDetail_footer) == view
							.getLastVisiblePosition())
						scrollEnd = true;
				} catch (Exception e) {
					scrollEnd = false;
				}
				if (scrollEnd && curLvDataState == LvState.LISTVIEW_DATA_MORE) {
					lvNodeDetail_foot_more.setText("加载中...");
					lvNodeDetail_foot_progress.setVisibility(View.VISIBLE);
					loadLvNodeDetail();
				}
			}

			public void onScroll(AbsListView view, int firstVisibleItem,
					int visibleItemCount, int totalItemCount) {
				mLvNodeDetail.onScroll(view, firstVisibleItem,
						visibleItemCount, totalItemCount);
			}
		});
		mLvNodeDetail
				.setOnRefreshListener(new PullToRefreshListView.OnRefreshListener() {
					public void onRefresh() {
						// 复位
						id = -1;
						pageNum = 10;
						requestTime = 0;
						loadLvNodeDetail();
					}
				});

	}

	// 初始化评论数据
	protected void initData() {
		if (nodeType.equals(Constants.ENDDEVICE)) {
			mHeadTitle.setText("终端节点");
		} else if (nodeType.equals(Constants.ROUTER)) {
			mHeadTitle.setText("路由器节点");
		} else {
			mHeadTitle.setText("协调器节点");
		}
		loadLvNodeDetail();
	}

	private void headButton(int type) {
		switch (type) {
		case DATA_LOAD_ING:
			mControl.setVisibility(View.GONE);
			mHeadProgressbar.setVisibility(View.VISIBLE);
			break;
		case DATA_LOAD_FINISH:
			mControl.setVisibility(View.VISIBLE);
			mHeadProgressbar.setVisibility(View.GONE);
			break;
		}
	}

	private void lvFooter(int size) {
		if (size == pageNum) {
			curLvDataState = LvState.LISTVIEW_DATA_MORE;
			lvNodeDetail_foot_more.setText(R.string.load_more);
		} else if (size < pageNum && size > 0) {
			curLvDataState = LvState.LISTVIEW_DATA_FULL;
			lvNodeDetail_foot_more.setText(R.string.load_full);
		} else if (size == 0) {
			curLvDataState = LvState.LISTVIEW_DATA_EMPTY;
			lvNodeDetail_foot_more.setText(R.string.load_empty);
		} else if (size == -1) {
			curLvDataState = LvState.LISTVIEW_DATA_MORE;
			lvNodeDetail_foot_more.setText(R.string.load_error);
		}
		lvNodeDetail_foot_progress.setVisibility(View.GONE);
	}

	private void loadLvNodeDetail() {
		httpClient = new AsyncHttpClient();
		httpClient.post(NodeQueryDetailActivity.this,
				URL.NODE_DETAIL_QUERY_URL, ParamUtils.SE4NodeDetailQuery(
						username, ieeeAddress, nodeType, id, pageNum,
						requestTime), null, new AsyncHttpResponseHandler() {
					@Override
					public void onStart() {
						headButton(DATA_LOAD_ING);
					}
					@Override
					public void onSuccess(String content) {
						if (++requestTime == 1) {
							lvNodeDetailData.clear();
							mLvNodeDetail.setSelection(0);
						}
						if (nodeType.equals(Constants.ENDDEVICE)) {
							requestNodeDetailData = JSON.parseArray(content,
									EndDevice.class);
						} else if (nodeType.equals(Constants.ROUTER)) {
							requestNodeDetailData = JSON.parseArray(content,
									Router.class);
						} else {
							requestNodeDetailData = JSON.parseArray(content,
									Coordinator.class);
						}
						id = requestNodeDetailData.get(
								requestNodeDetailData.size() - 1).getId();
						lvNodeDetailData.addAll(requestNodeDetailData);
						lvFooter(requestNodeDetailData.size());
						lvNodeDetailAdapter.notifyDataSetChanged();
					}

					@Override
					public void onFailure(Throwable error, String content) {
						super.onFailure(error, content);
						lvFooter(-1);
						(AppException.network((Exception) error))
								.makeToast(NodeQueryDetailActivity.this);
					}

					@Override
					public void onFinish() {
						super.onFinish();
						mLvNodeDetail
								.onRefreshComplete(getString(R.string.pull_to_refresh_update)
										+ new Date().toLocaleString());
						headButton(DATA_LOAD_FINISH);
					}
				});
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.node_detail_back:
			ViewUtility.NavigateActivity(thisContext, NodeQueryActivity.class,
					true);
			break;
		case R.id.node_detail_control:
			showDialog();
			break;
		default:
			break;
		}
	}

	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		showDialog();
	}
	private void showDialog() {
		Vibrator vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
		vibrator.vibrate(50);
		AlertDialog.Builder builder = new AlertDialog.Builder(
				NodeQueryDetailActivity.this);
		builder.setTitle("节点基本信息 :");
		builder.setMessage(
				"节点类型:" + selectedNode.getType() + "\n" + "网络地址:"
						+ selectedNode.getNetAddress())
				.setCancelable(false)
				.setPositiveButton("确定", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						dialog.cancel();
					}
				})
				.setNegativeButton("节点控制",
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
								ViewUtility.NavigateActivity(
										NodeQueryDetailActivity.this,
										NodeControlActivity.class,
										selectedNode.getIeeeAddress());
							}
						});
		builder.show();
	}
}
