package ty.change.wsn.dao;

import java.sql.SQLException;

import ty.change.wsn.bean.WSNNoti;
import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

import com.j256.ormlite.android.apptools.OrmLiteSqliteOpenHelper;
import com.j256.ormlite.dao.Dao;
import com.j256.ormlite.support.ConnectionSource;
import com.j256.ormlite.table.TableUtils;

/**
 * @author Change_Ty
 */
public class WSNDatabaseHelper extends OrmLiteSqliteOpenHelper {

	private static final String DATABASE_NAME = "wsn_monitor.db";
	private static final int DATABASE_VERSION = 1;


	private Dao<WSNNoti, Integer> notiDao = null;

	public WSNDatabaseHelper(Context context) {
		super(context, DATABASE_NAME, null, DATABASE_VERSION);
	}

	@Override
	public void onCreate(SQLiteDatabase db, ConnectionSource connectionSource) {
		try {
			long beginTime = System.currentTimeMillis();
			Log.i(WSNDatabaseHelper.class.getName(), "OnCreate");
			TableUtils.dropTable(connectionSource, WSNNoti.class, true);
			TableUtils.createTable(connectionSource, WSNNoti.class);
			long endTime = System.currentTimeMillis();
			System.out.println("建表时间" + (endTime - beginTime) / 1000f
					+ "秒");
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void onUpgrade(SQLiteDatabase db, ConnectionSource connectionSource,
			int oldVersion, int newVersion) {
		try {
			Log.i(WSNDatabaseHelper.class.getName(), "onUpgrade");
			TableUtils.dropTable(connectionSource, WSNNoti.class, true);
			onCreate(db, connectionSource);
		} catch (SQLException e) {
			Log.e(WSNDatabaseHelper.class.getName(), "Can't drop databases", e);
			throw new RuntimeException(e);
		}
	}

	public Dao<WSNNoti, Integer> getNotiDao() throws SQLException {
		if (notiDao == null) {
			notiDao = getDao(WSNNoti.class);
		}
		return notiDao;
	}

	/**
	 * { * Close the database connections and clear any cached DAOs.
	 */
	@Override
	public void close() {
		super.close();
		notiDao = null;
	}


}