package ty.change.wsn.dao;

import java.sql.SQLException;
import java.util.Iterator;

import ty.change.wsn.bean.WSNNoti;

import com.j256.ormlite.dao.Dao;
import com.j256.ormlite.dao.DaoManager;
import com.j256.ormlite.support.ConnectionSource;

/**
 * @author change_ty
 * use ormlite框架实现
 * 
 *
 */
public class DaoHelperImpl implements DaoHelper {

	private static DaoHelperImpl instance = null;
	private Dao<WSNNoti, Integer> notiDAO;

	public static synchronized DaoHelperImpl getInstance(ConnectionSource conn) {
		if (instance == null)
			instance = new DaoHelperImpl(conn);
		return instance;
	}

	private DaoHelperImpl(ConnectionSource conn) {
		try {
			notiDAO = DaoManager.createDao(conn, WSNNoti.class);
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void addWSNNoti(WSNNoti notice) {
		try {
			notiDAO.create(notice);
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}

	@Override
	public int deleteWSNNoti(WSNNoti notice) {
		int success = 0;
		try {
			notiDAO.delete(notice);
		} catch (java.sql.SQLException e) {
			e.printStackTrace();
		}
		return success;
	}

	@Override
	public Iterator<WSNNoti> getAllNotices() {
		try {
			return notiDAO.queryForAll().iterator();
		} catch (java.sql.SQLException e) {
			e.printStackTrace();
		}
		return null;
	}

}
