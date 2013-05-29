package ty.change.wsn.dao;

import java.util.Iterator;

import ty.change.wsn.bean.WSNNoti;

/**
 * @author change_ty
 * 推送消息增删改查
 */
public interface DaoHelper {
	
	public void addWSNNoti(WSNNoti noti);

	public int deleteWSNNoti(WSNNoti noti);
	
	public Iterator<WSNNoti> getAllNotices();

}
