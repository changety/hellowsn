package org.androidpn.server.xmpp.router;

import org.androidpn.server.xmpp.PacketException;
import org.androidpn.server.xmpp.session.ClientSession;
import org.androidpn.server.xmpp.session.SessionManager;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.xmpp.packet.JID;
import org.xmpp.packet.Packet;

/** 
 * This class is to deliver the packets to the connected sessions. 第一个deliver 
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class PacketDeliverer {

    private static final Log log = LogFactory.getLog(PacketDeliverer.class);

    /**
     * Delivers the packet to the packet recipient.    把packet 发送给 接收器
     * 
     * @param packet the packet to deliver
     * @throws PacketException if the packet is null or the recipient was not found.
     */
    public static void deliver(Packet packet) throws PacketException {
        if (packet == null) {
            throw new PacketException("Packet was null");
        }

        try {
        	// getTo() the packet is addressed to  
            JID recipient = packet.getTo();
            if (recipient != null) {
                ClientSession clientSession = SessionManager.getInstance()
                        .getSession(recipient);
                if (clientSession != null) {
                    clientSession.deliver(packet);
                }
            }
        } catch (Exception e) {
            log.error("Could not deliver packet: " + packet.toString(), e);
        }
    }
}
