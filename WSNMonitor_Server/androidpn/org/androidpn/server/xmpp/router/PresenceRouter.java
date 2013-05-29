package org.androidpn.server.xmpp.router;

import org.androidpn.server.xmpp.handler.PresenceUpdateHandler;
import org.androidpn.server.xmpp.session.ClientSession;
import org.androidpn.server.xmpp.session.Session;
import org.androidpn.server.xmpp.session.SessionManager;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.xmpp.packet.JID;
import org.xmpp.packet.PacketError;
import org.xmpp.packet.Presence;

/** 
 * This class is to route Presence packets to their corresponding handler.
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class PresenceRouter {

    private final Log log = LogFactory.getLog(getClass());

    private SessionManager sessionManager;

    private PresenceUpdateHandler presenceUpdateHandler;

    /**
     * Constucts a packet router.
     */
    public PresenceRouter() {
        sessionManager = SessionManager.getInstance();
        presenceUpdateHandler = new PresenceUpdateHandler();
    }

    /**
     * Routes the Presence packet.
     * 
     * @param packet the packet to route
     */
    public void route(Presence packet) {
        if (packet == null) {
            throw new NullPointerException();
        }
        ClientSession session = sessionManager.getSession(packet.getFrom());

        if (session == null || session.getStatus() != Session.STATUS_CONNECTED) {
            handle(packet);
        } else {
            packet.setTo(session.getAddress());
            packet.setFrom((JID) null);
            packet.setError(PacketError.Condition.not_authorized);
            session.process(packet);
        }
    }

    private void handle(Presence packet) {
        try {
            Presence.Type type = packet.getType();
            // Presence updates (null == 'available')
            if (type == null || Presence.Type.unavailable == type) {
                presenceUpdateHandler.process(packet);
            } else {
                log.warn("Unknown presence type");
            }

        } catch (Exception e) {
            log.error("Could not route packet", e);
            Session session = sessionManager.getSession(packet.getFrom());
            if (session != null) {
                session.close();
            }
        }
    }

}
