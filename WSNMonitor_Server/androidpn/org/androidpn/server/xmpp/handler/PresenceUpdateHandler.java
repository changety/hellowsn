package org.androidpn.server.xmpp.handler;

import org.androidpn.server.xmpp.router.PacketDeliverer;
import org.androidpn.server.xmpp.session.ClientSession;
import org.androidpn.server.xmpp.session.Session;
import org.androidpn.server.xmpp.session.SessionManager;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.xmpp.packet.JID;
import org.xmpp.packet.Packet;
import org.xmpp.packet.PacketError;
import org.xmpp.packet.Presence;

/** 
 * This class is to handle the presence protocol.
 *
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class PresenceUpdateHandler {

    protected final Log log = LogFactory.getLog(getClass());

    protected SessionManager sessionManager;

    /**
     * Constructor.
     */
    public PresenceUpdateHandler() {
        sessionManager = SessionManager.getInstance();
    }

    /**
     * Processes the presence packet.  处理存在packet
     * 
     * @param packet the packet
     */
    public void process(Packet packet) {
        ClientSession session = sessionManager.getSession(packet.getFrom());

        try {
            Presence presence = (Presence) packet;
            Presence.Type type = presence.getType();

            if (type == null) { // null == available
                if (session != null
                        && session.getStatus() == Session.STATUS_CLOSED) {
                    log.warn("Rejected available presence: " + presence + " - "
                            + session);
                    return;
                }

                if (session != null) {
                    session.setPresence(presence);
                    if (!session.isInitialized()) {
                        // initSession(session);
                        session.setInitialized(true);
                    }
                }

            } else if (Presence.Type.unavailable == type) {

                if (session != null) {
                    session.setPresence(presence);
                }

            } else {
                presence = presence.createCopy();
                if (session != null) {
                    presence.setFrom(new JID(null, session.getServerName(),
                            null, true));
                    presence.setTo(session.getAddress());
                } else {
                    JID sender = presence.getFrom();
                    presence.setFrom(presence.getTo());
                    presence.setTo(sender);
                }
                presence.setError(PacketError.Condition.bad_request);
                PacketDeliverer.deliver(presence);
            }

        } catch (Exception e) {
            log.error("Internal server error. Triggered by packet: " + packet,
                    e);
        }
    }

}
