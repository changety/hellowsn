package ty.change.wsn.activity;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import ty.change.wsn.R;
import ty.change.wsn.app.AppException;
import ty.change.wsn.bean.ZigBeeNode;
import ty.change.wsn.util.Constants;
import ty.change.wsn.util.ParamUtils;
import ty.change.wsn.util.URL;
import ty.change.wsn.util.ViewUtility;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Bundle;
import android.os.Vibrator;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.alibaba.fastjson.JSON;
import com.loopj.android.http.AsyncHttpClient;
import com.loopj.android.http.AsyncHttpResponseHandler;

/**
 * @author change_ty
 *
 */
public class NodeNetActivity extends BaseActivity {

	private TextView node_net_hint;
	private String mUsername, mCommand;
	private SharedPreferences sharedPreferences = null;
	private ZigBeeNode data4NT;
	private SurfaceView sv;
	private SurfaceHolder holder;
	private ProgressBar mProgressbar;
	private ProgressDialog mDialog;
	private AsyncHttpClient httpClient;

	// intent传递过来的所有节点信息
	private final ArrayList<ZigBeeNode> wsnNTChangingList = new ArrayList<ZigBeeNode>();
	private ZigBeeNode data5NT;
	private ZigBeeNode coorNode;
	private ZigBeeNode touchedNode;
	// 层次
	private int level;
	private int nodeCount;
	// 最大层的节点个数
	private int maxCount;
	// 每层多少个
	private Map<Integer, Integer> eachLayerCount = new LinkedHashMap<Integer, Integer>();
	// 用于盛放每一个层的Map集合 key是level层次，value是每层的所有节点

	private Map<Integer, ArrayList<ZigBeeNode>> allLayersMap = new LinkedHashMap<Integer, ArrayList<ZigBeeNode>>();
	// 一层下面的所有子节点
	private ArrayList<ZigBeeNode> allChidrenList;
	// 一层当中某个节点下面的所有子节点
	private ArrayList<ZigBeeNode> oneNextList;

	private float radius = 0;

	// 判断该层的节点 下面是否还有子节点
	private boolean hasNodes;
	private int flag = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.node_net_activity);
		initView();
		flag = 1;
	}

	@Override
	protected void initView() {
		mProgressbar = (ProgressBar) findViewById(R.id.grapf_progress);
		node_net_hint = (TextView) findViewById(R.id.node_net_hint);
		node_net_hint.setVisibility(View.VISIBLE);
		sv = (SurfaceView) findViewById(R.id.wsnntsurfaceview);
		holder = sv.getHolder();
		//这边有点奇怪 ... 用AysnTask 来实现 不需要 回调
		holder.addCallback(new Callback() {
			@Override
			public void surfaceDestroyed(SurfaceHolder holder) {
			}
			@Override
			public void surfaceCreated(SurfaceHolder holder) {
				initData();
			}
			@Override
			public void surfaceChanged(SurfaceHolder holder, int format, int width,
					int height) {
			}
		});
		
	}

	@Override
	protected void initData() {
		sharedPreferences = this.getSharedPreferences(
				Constants.SHARED_PREFERENCE_NAME, Context.MODE_PRIVATE);
		mUsername = sharedPreferences.getString(Constants.XMPP_USERNAME, null);
		mCommand = Constants.COMMAND_WSN_STRUCT_REFRESH;
		drawNodeNet(mUsername, mCommand);
	}

	@Override
	protected void onPause() {
		super.onPause();
		flag = 0;
	}

	@Override
	protected void onResume() {
		super.onResume();
		if (flag == 0) {
			drawNodeNet(mUsername, mCommand);
		}
	}

	private void drawNodeNet(final String mUsername, final String mCommand) {
		mDialog = new ProgressDialog(this);
		mDialog.setTitle("控制中");
		mDialog.setMessage("请稍后...");
		mDialog.setCancelable(true);
		httpClient = new AsyncHttpClient();
		httpClient.post(NodeNetActivity.this, URL.WSN_STRUCT_REFRESH_URL,
				ParamUtils.SE4NodeQuery(mUsername, mCommand), null,
				new AsyncHttpResponseHandler() {
					@Override
					public void onStart() {
						mProgressbar.setVisibility(View.VISIBLE);
						node_net_hint.setText("网络拓扑图读取中...");
					}
					@Override
					public void onSuccess(String response) {
						System.out.println("onsuccess" + Thread.currentThread().toString());
						wsnNTChangingList.clear();
						wsnNTChangingList.addAll(JSON.parseArray(response,
								ZigBeeNode.class));
						drawNet();
						node_net_hint.setText("查询完成,最后更新于 ："
								+ "2013-3-6  下午2:58:39");
						mProgressbar.setVisibility(View.GONE);
					}

					@Override
					public void onFinish() {
						System.out.println("onfinish" + Thread.currentThread().toString());
						mDialog.dismiss();
					}

					@Override
					public void onFailure(Throwable error, String content) {
						super.onFailure(error, content);
						(AppException.network((Exception) error))
								.makeToast(NodeNetActivity.this);
					}
				});
	}

	public String formateTime(long time) {
		Calendar calendar = Calendar.getInstance();
		calendar.setTimeInMillis(time);
		// 显示为24小时格式
		SimpleDateFormat simpleDF = new SimpleDateFormat("yyyy MMdd-HH:mm:ss");
		String logTime = simpleDF.format(calendar.getTime());
		return logTime;
	}

	@Override
	public void onBackPressed() {
		super.onBackPressed();
	}

	public void drawNet() {
		sv.setVisibility(View.VISIBLE);
		level = 0;
		nodeCount = 0;
		// 最大层的节点个数
		maxCount = 0;
		// 每层多少个
		eachLayerCount = new LinkedHashMap<Integer, Integer>();

		allLayersMap = new LinkedHashMap<Integer, ArrayList<ZigBeeNode>>();
		// 一层下面的所有子节点
		allChidrenList = new ArrayList<ZigBeeNode>();

		oneNextList = new ArrayList<ZigBeeNode>();

		sv.clearAnimation();
		for (int i = 0; i < wsnNTChangingList.size(); i++) {
			if (("0000").equals(wsnNTChangingList.get(i).getNetAddress())) {
				coorNode = wsnNTChangingList.get(i);
				allChidrenList = new ArrayList<ZigBeeNode>();
				allChidrenList.add(coorNode);
				break;
			}
		}
		// 将该list放到 map的第一个层中
		allLayersMap.put(level, allChidrenList);
		eachLayerCount.put(level, allChidrenList.size());
		// 将其他每一层的节点也放到map中,从协调器那层
		getLayerMap(wsnNTChangingList, allLayersMap.get(0));
		maxCount = getMaxCountInMap(eachLayerCount);
		wsnNTChangingList.clear();
//		holder = sv.getHolder();
		sv.setOnTouchListener(new View.OnTouchListener() {

			public boolean onTouch(View v, MotionEvent event) {
				Vibrator vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
				float x = event.getX();
				float y = event.getY();
				switch (event.getAction()) {
				case MotionEvent.ACTION_UP:
					touchedNode = getTheTouchedNode(x, y);
					if (touchedNode != null) {
						vibrator.vibrate(50);
						AlertDialog.Builder builder = new AlertDialog.Builder(
								NodeNetActivity.this);
						builder.setTitle("节点基本信息 :");
						builder.setMessage(
								"节点类型:" + touchedNode.getType() + "\n"
										+ "网络地址:" + touchedNode.getNetAddress())
								.setCancelable(false)
								.setPositiveButton("确定",
										new DialogInterface.OnClickListener() {
											public void onClick(
													DialogInterface dialog,
													int id) {
												dialog.cancel();
											}
										})
								.setNegativeButton("节点控制",
										new DialogInterface.OnClickListener() {
											public void onClick(
													DialogInterface dialog,
													int id) {
												ViewUtility
														.NavigateActivity(
																NodeNetActivity.this,
																NodeControlActivity.class,
																touchedNode
																		.getIeeeAddress());
											}
										});
						builder.show();
					}
					break;
				default:
					break;
				}
				return true;
			}
		});
		Canvas canvas = holder.lockCanvas();// 获取画布

		float radius1 = canvas.getWidth() / (4 * (level + 1));
		// 根据横坐标设
		float radius2 = canvas.getHeight() / (2 * (2 * maxCount + 1));

		// 设置最大半径,
		if (radius1 > radius2) {
			radius = radius2;
		} else {
			radius = radius1;
		}
		// coorNode.setRadius(radius);

		// wsnNTChangingList.add(coorNode);

		// 背景设置
		canvas.drawColor(Color.LTGRAY);
		// 取得画笔
		Paint nodePaint = new Paint(Paint.ANTI_ALIAS_FLAG);

		for (int levelth = 0; levelth < allLayersMap.size(); levelth++) {

			allChidrenList = allLayersMap.get(levelth);
			for (int location = 0; location < allChidrenList.size(); location++) {
				data5NT = allChidrenList.get(location);

				drawNode(canvas, nodePaint, data5NT, location, levelth, radius);
			}
		}
		holder.unlockCanvasAndPost(canvas);// 解锁画布，提交画好的图像
	}

	public void drawNode(Canvas canvas, Paint nodePaint, ZigBeeNode data5NT,
			int location, int levelth, float radius) {

		// x * (2 * location + 1)
		float layerWitdh = canvas.getWidth() * (2 * location + 1);
		int xfactor = 2 * allLayersMap.get(levelth).size();

		// y*(2level + 1)
		float layerHeight = canvas.getHeight() * (2 * levelth + 1);
		int yfactor = 2 * (level + 1);
		// x轴坐标
		float x = layerWitdh / xfactor;
		// y轴坐标
		float y = layerHeight / yfactor;

		data5NT.setX(x);
		data5NT.setY(y);
		data5NT.setRadius(radius);
		wsnNTChangingList.add(data5NT);
		if (Constants.COORDINATOR.equals(data5NT.getType())) {
			nodePaint.setColor(Color.RED);
			data5NT.setRadius((float) (data5NT.getRadius() * 1.3));
		} else if (Constants.ENDDEVICE.equals(data5NT.getType())) {
			nodePaint.setColor(Color.BLUE);
			data5NT.setRadius((float) (data5NT.getRadius() * 1.1));
		} else {
			nodePaint.setColor(Color.GREEN);
		}
		canvas.drawCircle(data5NT.getX(), data5NT.getY(), data5NT.getRadius(),
				nodePaint);
		nodePaint.setColor(Color.BLACK);
		if (data5NT.getFatherNode() != null) {
			canvas.drawLine(x, y, data5NT.getFatherNode().getX(), data5NT
					.getFatherNode().getY(), nodePaint);
		}
	}

	/**
	 * 抽取出整个节点结构，将 除了协调器之外的每一个层分出来 放到一个map里，key是 层树， value是层的节点list
	 * 
	 * @param wsnNTChangingList
	 * @param fatherLayerList
	 * @return
	 */
	public Map<Integer, ArrayList<ZigBeeNode>> getLayerMap(
			ArrayList<ZigBeeNode> wsnNTChangingList,
			ArrayList<ZigBeeNode> fatherLayerList) {
		level++;
		hasNodes = true;
		allChidrenList = new ArrayList<ZigBeeNode>();
		// 遍历该层每一个子节点
		for (int i = 0; i < fatherLayerList.size(); i++) {
			// 如果不是叶子节点的话
			if (Constants.NO.equals(fatherLayerList.get(i).getIsLeaf())) {
				hasNodes = false;
				// 找到该节点下的所有子节点
				oneNextList = getListForPNet(wsnNTChangingList,
						fatherLayerList.get(i));
				// 添加到这一层的allChidrenList当中
				allChidrenList.addAll(oneNextList);
			}
		}
		if (allChidrenList.size() > 0) {
			allLayersMap.put(level, allChidrenList);
			eachLayerCount.put(level, allChidrenList.size());
		}
		if (!hasNodes) {
			getLayerMap(wsnNTChangingList, allChidrenList);
		}
		return allLayersMap;
	}

	/**
	 * 在整个wsnNTChangingList里 寻找 父节点地址为fatherNode.pNetAddress的所有子节点
	 * 
	 * @param wsnNTChangingList
	 * @param fatherNode
	 * @return
	 */
	public ArrayList<ZigBeeNode> getListForPNet(
			List<ZigBeeNode> wsnNTChangingList, ZigBeeNode fatherNode) {
		ArrayList<ZigBeeNode> childrenList = new ArrayList<ZigBeeNode>();
		for (Iterator<ZigBeeNode> iter = wsnNTChangingList.iterator(); iter
				.hasNext();) {
			data5NT = iter.next();
			if (data5NT.getpNetAddress() != null) {
				if (fatherNode.getNetAddress().trim()
						.equals((data5NT.getpNetAddress().trim()))) {
					data5NT.setFatherNode(fatherNode);
					childrenList.add(data5NT);
					fatherNode.setChildrenList(childrenList);
				}
			}
		}
		// 将这部分节点从wsnNTChangingList中移除
		wsnNTChangingList.removeAll(childrenList);
		return childrenList;
	}

	/**
	 * 根据eachLayerCount的记载，得到层中 节点个数最大的数字
	 * 
	 * @param eachLayerCount
	 *            记录了每层的节点个数
	 * @return
	 */
	public int getMaxCountInMap(Map<Integer, Integer> eachLayerCount) {
		int maxCount = eachLayerCount.get(0);
		for (int i = 1; i < eachLayerCount.size(); i++) {
			if (eachLayerCount.get(i) > maxCount)
				maxCount = eachLayerCount.get(i);
		}
		return maxCount;
	}

	/**
	 * 得到touch位置的那个节点信息，根据触碰点的位置与 节点所在的圆心的距离判断，如果小于节点的半径 就一定是该节点
	 * 
	 * @param x
	 * @param y
	 * @return
	 */
	public ZigBeeNode getTheTouchedNode(float x, float y) {
		ZigBeeNode touchedNode = null;
		for (int i = 0; i < wsnNTChangingList.size(); i++) {
			ZigBeeNode node = wsnNTChangingList.get(i);
			float xNode = node.getX();
			float yNode = node.getY();
			float locationX = xNode - x;
			float locationY = yNode - y;
			float distance = locationX * locationX + locationY * locationY;
			float dis = (float) java.lang.Math.sqrt(distance);
			if (dis <= radius) {
				touchedNode = node;
				break;
			}
		}
		return touchedNode;
	}

	// 在整个wsnNTList里 寻找 父节点地址为pNetAddress的所有子节点
	public ArrayList<ZigBeeNode> getListForPNet(
			List<ZigBeeNode> wsnNTChangingList, String pNetAddress) {
		ArrayList<ZigBeeNode> childrenList = new ArrayList<ZigBeeNode>();
		for (Iterator<ZigBeeNode> iter = wsnNTChangingList.iterator(); iter
				.hasNext();) {
			data4NT = iter.next();
			// 如果是该pNetAddress的子节点那么添加到里面
			if (data4NT.getpNetAddress() != null
					&& (pNetAddress.trim().equals((data4NT.getpNetAddress()
							.trim())))) {
				childrenList.add(data4NT);
			}
		}
		return childrenList;
	}
}
