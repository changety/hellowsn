package ty.change.wsn.androidpn.client;

import java.util.Date;

import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.packet.Packet;

import ty.change.wsn.bean.WSNNoti;
import ty.change.wsn.dao.DaoHelper;
import ty.change.wsn.dao.DaoHelperImpl;
import ty.change.wsn.util.Constants;
import android.content.Intent;
import android.util.Log;

import com.j256.ormlite.support.ConnectionSource;

/**
 * This class notifies the receiver of incoming notifcation packets
 * asynchronously. 2
 * 
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class NotificationPacketListener implements PacketListener {

	private static final String LOGTAG = LogUtil
			.makeLogTag(NotificationPacketListener.class);

	private final XmppManager xmppManager;
	private DaoHelper daoHelpImpl;

	public NotificationPacketListener(XmppManager xmppManager,ConnectionSource conn) {
		this.xmppManager = xmppManager;
		daoHelpImpl = DaoHelperImpl.getInstance(conn);
	}

	@Override
	public void processPacket(Packet packet) {
		Log.d(LOGTAG, "NotificationPacketListener.processPacket()...");
		Log.d(LOGTAG, "packet.toXML()=" + packet.toXML());

		if (packet instanceof NotificationIQ) {

			NotificationIQ notification = (NotificationIQ) packet;
			if (notification.getChildElementXML().contains(
					"androidpn:iq:notification")) {

				String notificationId = notification.getId();
				String notificationApiKey = notification.getApiKey();
				String notificationTitle = notification.getTitle();
				String notificationMessage = notification.getMessage();
				String notificationUri = notification.getUri();
				
				WSNNoti noti = new WSNNoti();
				noti.setMessage(notification.getMessage());
				noti.setTime(new Date().toLocaleString());
				noti.setTitle(notification.getTitle());
				daoHelpImpl.addWSNNoti(noti);

				Intent intent = new Intent(Constants.ACTION_SHOW_NOTIFICATION);

				intent.putExtra(Constants.NOTIFICATION_ID, notificationId);
				intent.putExtra(Constants.NOTIFICATION_API_KEY,
						notificationApiKey);
				intent.putExtra(Constants.NOTIFICATION_TITLE, notificationTitle);
				intent.putExtra(Constants.NOTIFICATION_MESSAGE,
						notificationMessage);
				intent.putExtra(Constants.NOTIFICATION_URI, notificationUri);

				xmppManager.getContext().sendBroadcast(intent);
			}
		}

	}

}
